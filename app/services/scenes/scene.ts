import { times } from 'lodash';
import { Mutator, mutation } from '../stateful-service';
import { ScenesService } from './scenes';
import { ISourceCreateOptions, SourcesService, TSourceType } from '../sources';
import { ISceneItem, SceneItem } from './scene-item';
import { ConfigFileService } from '../config-file';
import Utils from '../utils';
import { nodeObs, ObsScene, ObsSceneItem } from '../obs-api';
import electron from '../../vendor/electron';

const { ipcRenderer } = electron;


export interface IScene {
  id: string;
  name: string;
  activeItemId: string;
  items: ISceneItem[];
}

// TODO: delete these options after we will handle the config loading on the frontend side
export interface ISceneItemAddOptions {
  obsSceneItemIsAlreadyExist: boolean;
  obsSceneItemId: number;
}


@Mutator()
export class Scene implements IScene {
  id: string;
  name: string;
  activeItemId: string;
  items: ISceneItem[];

  private scenesService: ScenesService = ScenesService.instance;
  private sourcesService: SourcesService = SourcesService.instance;
  private configFileService: ConfigFileService = ConfigFileService.instance;
  private sceneState: IScene;

  constructor(sceneId: string) {
    this.sceneState = this.scenesService.state.scenes[sceneId];
    Utils.applyProxy(this, this.sceneState);
  }


  getObsScene(): ObsScene {
    return ObsScene.fromName(this.name);
  }


  getItem(sceneItemId: string): SceneItem {
    const sceneItemModel = this.sceneState.items.find(sceneItemModel => sceneItemModel.sceneItemId === sceneItemId);
    return sceneItemModel ?
      new SceneItem(this.id, sceneItemModel.sceneItemId, sceneItemModel.sourceId) :
      null;
  }


  getItems(options = { showHidden: false }): SceneItem[] {
    const sources = this.sceneState.items.map(sourceModel => {
      return this.getItem(sourceModel.sceneItemId);
    });

    return options.showHidden ? sources : sources.filter(source => !source.isHidden);
  }


  get inactiveSources(): SceneItem[] {
    return this.sceneState.items.filter(sourceModel => {
      return sourceModel.sceneItemId !== this.activeItemId;
    }).map(source => {
      return this.getItem(source.sceneItemId);
    });
  }


  get activeItem(): SceneItem {
    return this.getItem(this.activeItemId);
  }


  loadConfig() {
    this.getObsScene().getItems().forEach(obsSceneItem => {
      const obsSource = obsSceneItem.source;

      // some sources could be already added from another .loadConfig call
      // because several scenes can reference to one source
      let source = this.sourcesService.getSourceByName(obsSource.name);
      if (!source) {
        source = this.sourcesService.createSource(
          obsSource.name,
          obsSource.id as TSourceType,
          { obsSourceIsAlreadyExist: true, }
        );
      }

      this.addSource(
        source.sourceId,
        { obsSceneItemIsAlreadyExist: true, obsSceneItemId: obsSceneItem.id }
      );

    });
  }


  createAndAddSource(sourceName: string, type: TSourceType): SceneItem {
    const source = this.sourcesService.createSource(sourceName, type);
    return this.addSource(source.sourceId);
  }


  addSource(sourceId: string, options?: ISceneItemAddOptions): SceneItem {
    const source = this.sourcesService.getSource(sourceId);
    const sceneItemId = ipcRenderer.sendSync('getUniqueId');

    let obsSceneItem: ObsSceneItem;
    if (options && options.obsSceneItemIsAlreadyExist) {
      obsSceneItem = this.getObsScene().findItem(options.obsSceneItemId);
    } else {
      obsSceneItem = this.getObsScene().add(source.getObsInput());
    }

    this.ADD_SOURCE_TO_SCENE(sceneItemId, source.sourceId, obsSceneItem.id);
    const sceneItem = this.getItem(sceneItemId);

    // Newly added sources are immediately active
    this.makeItemActive(sceneItemId);

    sceneItem.loadAttributes();

    this.configFileService.save();
    this.scenesService.sourceAdded.next(sceneItem.sceneItemState);
    return sceneItem;
  }


  removeItem(sceneItemId: string) {
    const sceneItem = this.getItem(sceneItemId);
    sceneItem.getObsSceneItem().remove();
    this.REMOVE_SOURCE_FROM_SCENE(sceneItemId);
    this.scenesService.sourceRemoved.next(sceneItem.sceneItemState);
  }


  makeItemActive(sceneItemId: string) {
    const selectedItem = this.getItem(sceneItemId);
    this.getObsScene().getItems().forEach(obsSceneItem => {
      if (!selectedItem || selectedItem.obsSceneItemId !== obsSceneItem.id) {
        obsSceneItem.selected = false;
        return;
      }
      obsSceneItem.selected = true;
    });

    this.MAKE_SOURCE_ACTIVE(sceneItemId);
  }


  setSourceOrder(sourceId: string, positionDelta: number, order: string[]) {
    let operation: 'move_down' | 'move_up';

    if (positionDelta > 0) {
      operation = 'move_down';
    } else {
      operation = 'move_up';
    }

    const source = this.getItem(sourceId);

    times(Math.abs(positionDelta), () => {
      // This should operate on a specific scene rather
      // than just the active scene.
      nodeObs.OBS_content_setSourceOrder(source.name, operation);
    });

    const hiddenSourcesOrder = this.getItems({ showHidden: true })
      .filter(item => item.isHidden)
      .map(item => item.sceneItemId);

    order.unshift(...hiddenSourcesOrder);

    this.SET_SOURCE_ORDER(order);
  }

  @mutation()
  private MAKE_SOURCE_ACTIVE(sceneItemId: string) {
    this.sceneState.activeItemId = sceneItemId;
  }

  @mutation()
  private ADD_SOURCE_TO_SCENE(sceneItemId: string, sourceId: string, obsSceneItemId: number) {
    this.sceneState.items.unshift({
      // This is information that belongs to a scene/source pair

      // The id of the source
      sceneItemId,
      sourceId,
      obsSceneItemId,

      // Position in video space
      x: 0,
      y: 0,

      // Scale between 0 and 1
      scaleX: 1.0,
      scaleY: 1.0,

      visible: true,

      crop: {
        top: 0,
        bottom: 0,
        left: 0,
        right: 0
      }
    });
  }

  @mutation()
  private REMOVE_SOURCE_FROM_SCENE(sceneItemId: string) {

    if (this.sceneState.activeItemId === sceneItemId) {
      this.sceneState.activeItemId = null;
    }

    this.sceneState.items = this.sceneState.items.filter(source => {
      return source.sceneItemId !== sceneItemId;
    });
  }

  @mutation()
  private SET_SOURCE_ORDER(order: string[]) {

    // TODO: This is O(n^2)
    this.sceneState.items = order.map(id => {
      return this.sceneState.items.find(source => {
        return source.sceneItemId === id;
      });
    });
  }

}