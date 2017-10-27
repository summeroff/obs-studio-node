import { StatefulService } from './stateful-service';

// Extends StatefulService with code that will persist the
// state across executions of the application.

export abstract class PersistentStatefulService<TState extends object> extends StatefulService<TState> {

  // This is the default state if the state is not found
  // in local storage.
  static defaultState = {};

  static get initialState() {
    const persisted = JSON.parse(localStorage.getItem(this.localStorageKey)) || {};

    return {
      ...this.defaultState,
      ...persisted
    };
  }

  static get localStorageKey() {
    return `PersistentStatefulService-${this.name}`;
  }

  init() {
    this.store.watch(
      () => {
        return JSON.stringify(this.state);
      },
      val => {
        localStorage.setItem((this.constructor as typeof PersistentStatefulService).localStorageKey, val);
      }
    );
  }

}