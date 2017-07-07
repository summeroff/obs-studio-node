#include "ISource.h"
#include "Data.h"
#include "Properties.h"
#include "Common.h"

/* FIXME or NOTE:
	Need to make sure there's no side effects of passing
	back an interface like this.
 */

namespace osn {

obs::source* ISource::GetHandle(v8::Local<v8::Object> object)
{
    ISourceHandle* source = Nan::ObjectWrap::Unwrap<ISource>(object);
    return source->GetHandle();
}

NAN_MODULE_INIT(ISource::Init)
{
    auto locProto = Nan::New<v8::FunctionTemplate>();
    locProto->SetClassName(FIELD_NAME("Source"));
    Nan::SetAccessor(locProto->InstanceTemplate(), FIELD_NAME("name"), name, name);
    Nan::SetAccessor(locProto->InstanceTemplate(), FIELD_NAME("type"), type);
    Nan::SetAccessor(locProto->InstanceTemplate(), FIELD_NAME("flags"), flags, flags);
    Nan::SetAccessor(locProto->InstanceTemplate(), FIELD_NAME("width"), width);
    Nan::SetAccessor(locProto->InstanceTemplate(), FIELD_NAME("height"), height);
    Nan::SetAccessor(locProto->InstanceTemplate(), FIELD_NAME("name"), name, name);
    Nan::SetAccessor(locProto->InstanceTemplate(), FIELD_NAME("status"), status);
    Nan::SetAccessor(locProto->InstanceTemplate(), FIELD_NAME("id"), id);
    Nan::SetAccessor(locProto->InstanceTemplate(), FIELD_NAME("configurable"), configurable);
    Nan::SetAccessor(locProto->InstanceTemplate(), FIELD_NAME("properties"), properties);
    Nan::SetAccessor(locProto->InstanceTemplate(), FIELD_NAME("settings"), settings);

    Nan::SetMethod(locProto->PrototypeTemplate(), "release", release);
    Nan::SetMethod(locProto->PrototypeTemplate(), "remove", remove);
    Nan::SetMethod(locProto->PrototypeTemplate(), "update", update);

    Nan::Set(target, FIELD_NAME("Source"), locProto->GetFunction());
    prototype.Reset(locProto);
}

NAN_METHOD(ISource::release)
{
    obs::source *handle = ISource::GetHandle(info.Holder());

    handle->release();
}

NAN_METHOD(ISource::remove)
{
    obs::source *handle = ISource::GetHandle(info.Holder());

    handle->remove();
}

NAN_GETTER(ISource::type)
{
    obs::source *handle = ISource::GetHandle(info.Holder());

    info.GetReturnValue().Set(handle->type());
}

NAN_GETTER(ISource::name)
{
    obs::source *handle = ISource::GetHandle(info.Holder());

    info.GetReturnValue().Set(Nan::New(handle->name()).ToLocalChecked());
}

NAN_SETTER(ISource::name)
{
    obs::source *handle = ISource::GetHandle(info.Holder());

    if (!value->IsString()) {
        Nan::ThrowTypeError("Expected string");
        return;
    }

    Nan::Utf8String name(value);
    handle->name(*name);
}

NAN_GETTER(ISource::flags)
{
    obs::source *handle = ISource::GetHandle(info.Holder());

    info.GetReturnValue().Set(handle->flags());
}

NAN_SETTER(ISource::flags)
{
    obs::source *handle = ISource::GetHandle(info.Holder());

    if (!value->IsUint32()) {
        Nan::ThrowTypeError("Expected unsigned 32-bit integer");
        return;
    }

    uint32_t flags = Nan::To<uint32_t>(value).FromJust();
    handle->flags(flags);
}

NAN_GETTER(ISource::status)
{
    obs::source *handle = ISource::GetHandle(info.Holder());

    info.GetReturnValue().Set(handle->status());
}

NAN_GETTER(ISource::id)
{
    obs::source *handle = ISource::GetHandle(info.Holder());

    info.GetReturnValue().Set(Nan::New(handle->id()).ToLocalChecked());
}

NAN_GETTER(ISource::configurable)
{
    obs::source *handle = ISource::GetHandle(info.Holder());

    info.GetReturnValue().Set(handle->configurable());
}

NAN_GETTER(ISource::properties)
{
    obs::source *handle = ISource::GetHandle(info.Holder());

    Properties *bindings = new Properties(handle->properties());
    auto object = Properties::Object::GenerateObject(bindings);

    info.GetReturnValue().Set(object);
}

NAN_GETTER(ISource::settings)
{
    obs::source *handle = ISource::GetHandle(info.Holder());

    obs_data_t *data = handle->settings();
    auto result = FromDataToObject(data);

    info.GetReturnValue().Set(result);
}

NAN_METHOD(ISource::update)
{
    obs::source *handle = ISource::GetHandle(info.Holder());

    if (!info[0]->IsObject()) {
        Nan::ThrowError("Expected object");
        return;
    }

    auto object = Nan::To<v8::Object>(info[0]).ToLocalChecked();

    obs_data_t *data = FromObjectToData(object);
    handle->update(data);
    obs_data_release(data);
}

NAN_GETTER(ISource::width)
{
    obs::source *handle = ISource::GetHandle(info.Holder());

    info.GetReturnValue().Set(handle->width());
}

NAN_GETTER(ISource::height)
{
    obs::source *handle = ISource::GetHandle(info.Holder());

    info.GetReturnValue().Set(handle->height());
}

Nan::Persistent<v8::FunctionTemplate> ISource::prototype = Nan::Persistent<v8::FunctionTemplate>();

}