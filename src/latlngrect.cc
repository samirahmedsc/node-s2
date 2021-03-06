#include <node.h>
#include <nan.h>
#include "node_object_wrap.h"           // for ObjectWrap
#include "v8.h"                         // for Handle, String, Integer, etc

#include "s2.h"
#include "s2cell.h"
#include "s2latlngrect.h"
#include "latlngrect.h"
#include "latlng.h"
#include "cell.h"
#include "cap.h"
#include "cellid.h"

using namespace v8;

Persistent<FunctionTemplate> LatLngRect::constructor;

void LatLngRect::Init(Handle<Object> target) {
    NanScope();

    constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(LatLngRect::New));
    Local<String> name = String::NewSymbol("S2LatLngRect");

    constructor->InstanceTemplate()->SetInternalFieldCount(1);
    constructor->SetClassName(name);

    NODE_SET_PROTOTYPE_METHOD(constructor, "center", GetCenter);
    NODE_SET_PROTOTYPE_METHOD(constructor, "size", GetSize);
    NODE_SET_PROTOTYPE_METHOD(constructor, "area", GetArea);
    NODE_SET_PROTOTYPE_METHOD(constructor, "getVertex", GetVertex);
    NODE_SET_PROTOTYPE_METHOD(constructor, "getCapBound", GetCapBound);
    NODE_SET_PROTOTYPE_METHOD(constructor, "contains", Contains);
    NODE_SET_PROTOTYPE_METHOD(constructor, "isValid", IsValid);
    NODE_SET_PROTOTYPE_METHOD(constructor, "isEmpty", IsEmpty);
    NODE_SET_PROTOTYPE_METHOD(constructor, "isPoint", IsPoint);
    NODE_SET_PROTOTYPE_METHOD(constructor, "union", Union);
    NODE_SET_PROTOTYPE_METHOD(constructor, "intersection", Intersection);
    NODE_SET_PROTOTYPE_METHOD(constructor, "approxEquals", ApproxEquals);

    target->Set(name, constructor->GetFunction());
}

LatLngRect::LatLngRect()
    : ObjectWrap(),
      this_() {}

Handle<Value> LatLngRect::New(const Arguments& args) {
    NanScope();

    if (!args.IsConstructCall()) {
        return NanThrowError("Use the new operator to create instances of this object.");
    }

    if (args[0]->IsExternal()) {
        Local<External> ext = Local<External>::Cast(args[0]);
        void* ptr = ext->Value();
        LatLngRect* ll = static_cast<LatLngRect*>(ptr);
        ll->Wrap(args.This());
        return args.This();
    }

    if (args.Length() == 0) {
        return NanThrowError("(latlng) or (latlng, latlng) required");
    }

    LatLngRect* obj = new LatLngRect();

    obj->Wrap(args.This());


    Handle<Object> ll = args[0]->ToObject();

    if (!NanHasInstance(LatLng::constructor, ll)) {
        return NanThrowError("(latlng) required");
    }

    if (args.Length() == 1) {
        obj->this_ = S2LatLngRect(
            S2LatLngRect::FromPoint(node::ObjectWrap::Unwrap<LatLng>(ll)->get()));
    } else if (args.Length() == 2) {
        Handle<Object> llb = args[1]->ToObject();

        if (!NanHasInstance(LatLng::constructor, llb)) {
            return NanThrowError("(latlng, latlng) required");
        }

        obj->this_ = S2LatLngRect(
            S2LatLngRect::FromPointPair(
                node::ObjectWrap::Unwrap<LatLng>(ll)->get(),
                node::ObjectWrap::Unwrap<LatLng>(llb)->get()));
    }

    return args.This();
}

Handle<Value> LatLngRect::New(S2LatLngRect s2cell) {
    NanScope();
    LatLngRect* obj = new LatLngRect();
    obj->this_ = s2cell;
    Handle<Value> ext = External::New(obj);
    Handle<Object> handleObject = constructor->GetFunction()->NewInstance(1, &ext);
    return scope.Close(handleObject);
}

NAN_METHOD(LatLngRect::GetCenter) {
    NanScope();
    LatLngRect* latlngrect = node::ObjectWrap::Unwrap<LatLngRect>(args.This());
    NanReturnValue(LatLng::New(latlngrect->this_.GetCenter()));
}

NAN_METHOD(LatLngRect::GetCapBound) {
    NanScope();
    LatLngRect* latlngrect = node::ObjectWrap::Unwrap<LatLngRect>(args.This());
    NanReturnValue(Cap::New(latlngrect->this_.GetCapBound()));
}

NAN_METHOD(LatLngRect::GetSize) {
    NanScope();
    LatLngRect* latlngrect = node::ObjectWrap::Unwrap<LatLngRect>(args.This());
    NanReturnValue(LatLng::New(latlngrect->this_.GetSize()));
}

NAN_METHOD(LatLngRect::GetArea) {
    NanScope();
    LatLngRect* latlngrect = node::ObjectWrap::Unwrap<LatLngRect>(args.This());
    NanReturnValue(NanNew<Number>(latlngrect->this_.Area()));
}

NAN_METHOD(LatLngRect::GetVertex) {
    NanScope();
    LatLngRect* latlngrect = node::ObjectWrap::Unwrap<LatLngRect>(args.This());
    NanReturnValue(LatLng::New(latlngrect->this_.GetVertex(args[0]->ToNumber()->Value())));
}

NAN_METHOD(LatLngRect::Contains) {
    NanScope();
    LatLngRect* latlngrect = node::ObjectWrap::Unwrap<LatLngRect>(args.This());
    S2LatLng other = node::ObjectWrap::Unwrap<LatLng>(args[0]->ToObject())->get();
    NanReturnValue(NanNew<Boolean>(latlngrect->this_.Contains(other)));
}

NAN_METHOD(LatLngRect::IsValid) {
    NanScope();
    LatLngRect* latlngrect = node::ObjectWrap::Unwrap<LatLngRect>(args.This());
    NanReturnValue(NanNew<Boolean>(latlngrect->this_.is_valid()));
}

NAN_METHOD(LatLngRect::IsEmpty) {
    NanScope();
    LatLngRect* latlngrect = node::ObjectWrap::Unwrap<LatLngRect>(args.This());
    NanReturnValue(NanNew<Boolean>(latlngrect->this_.is_empty()));
}

NAN_METHOD(LatLngRect::IsPoint) {
    NanScope();
    LatLngRect* latlngrect = node::ObjectWrap::Unwrap<LatLngRect>(args.This());
    NanReturnValue(NanNew<Boolean>(latlngrect->this_.is_point()));
}

NAN_METHOD(LatLngRect::Union) {
    NanScope();
    LatLngRect* latlngrect = node::ObjectWrap::Unwrap<LatLngRect>(args.This());
    S2LatLngRect other = node::ObjectWrap::Unwrap<LatLngRect>(args[0]->ToObject())->get();
    NanReturnValue(LatLngRect::New(latlngrect->this_.Union(other)));
}

NAN_METHOD(LatLngRect::Intersection) {
    NanScope();
    LatLngRect* latlngrect = node::ObjectWrap::Unwrap<LatLngRect>(args.This());
    S2LatLngRect other = node::ObjectWrap::Unwrap<LatLngRect>(args[0]->ToObject())->get();
    NanReturnValue(LatLngRect::New(latlngrect->this_.Intersection(other)));
}

NAN_METHOD(LatLngRect::ApproxEquals) {
    NanScope();
    LatLngRect* latlngrect = node::ObjectWrap::Unwrap<LatLngRect>(args.This());
    S2LatLngRect other = node::ObjectWrap::Unwrap<LatLngRect>(args[0]->ToObject())->get();
    NanReturnValue(NanNew<Boolean>(latlngrect->this_.ApproxEquals(other)));
}