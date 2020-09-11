// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_CALLBACK_FUNCTION_BASE_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_CALLBACK_FUNCTION_BASE_H_

#include "base/callback.h"
#include "third_party/blink/renderer/platform/bindings/name_client.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_v8_reference.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

// CallbackFunctionBase is the common base class of all the callback function
// classes. Most importantly this class provides a way of type dispatching (e.g.
// overload resolutions, SFINAE technique, etc.) so that it's possible to
// distinguish callback functions from anything else. Also it provides a common
// implementation of callback functions.
//
// As the signatures of callback functions vary, this class does not implement
// |Invoke| member function that performs "invoke" steps. Subclasses will
// implement it.
class PLATFORM_EXPORT CallbackFunctionBase
    : public GarbageCollectedFinalized<CallbackFunctionBase>,
      public NameClient {
 public:
  virtual ~CallbackFunctionBase() = default;

  virtual void Trace(blink::Visitor* visitor);

  v8::Local<v8::Object> CallbackObject() {
    return callback_function_.NewLocal(GetIsolate());
  }

  v8::Isolate* GetIsolate() const {
    return incumbent_script_state_->GetIsolate();
  }

  // Returns the ScriptState of the relevant realm of the callback object.
  //
  // NOTE: This function must be used only when it's pretty sure that the
  // callcack object is the same origin-domain. Otherwise,
  // |CallbackRelevantScriptStateOrReportError| or
  // |CallbackRelevantScriptStateOrThrowException| must be used instead.
  ScriptState* CallbackRelevantScriptState() {
    DCHECK(callback_relevant_script_state_);
    return callback_relevant_script_state_;
  }

  // Returns the ScriptState of the relevant realm of the callback object iff
  // the callback is the same origin-domain. Otherwise, reports an error and
  // returns nullptr.
  ScriptState* CallbackRelevantScriptStateOrReportError(const char* interface,
                                                        const char* operation);

  // Returns the ScriptState of the relevant realm of the callback object iff
  // the callback is the same origin-domain. Otherwise, throws an exception and
  // returns nullptr.
  ScriptState* CallbackRelevantScriptStateOrThrowException(
      const char* interface,
      const char* operation);

  DOMWrapperWorld& GetWorld() const { return incumbent_script_state_->World(); }

  // Returns true if the ES function has a [[Construct]] internal method.
  bool IsConstructor() const { return CallbackFunction()->IsConstructor(); }

  // Evaluates the given |closure| as part of the IDL callback function, i.e.
  // in the relevant realm of the callback object with the callback context as
  // the incumbent realm.
  //
  // NOTE: Do not abuse this function.  Let |Invoke| method defined in a
  // subclass do the right thing.  This function is rarely needed.
  void EvaluateAsPartOfCallback(base::OnceCallback<void()> closure);

  // Makes the underlying V8 function collectable by V8 Scavenger GC.  Do not
  // use this function unless you really need a hacky performance optimization.
  // The V8 function is collectable by V8 Full GC whenever this instance is no
  // longer referenced, so there is no need to call this function unless you
  // really need V8 *Scavenger* GC to collect the V8 function before V8 Full GC
  // runs.
  void DisposeV8FunctionImmediatelyToReduceMemoryFootprint() {
    callback_function_.Clear();
  }

 protected:
  explicit CallbackFunctionBase(v8::Local<v8::Object>);

  v8::Local<v8::Function> CallbackFunction() const {
    return callback_function_.NewLocal(GetIsolate()).As<v8::Function>();
  }

  ScriptState* IncumbentScriptState() { return incumbent_script_state_; }

 private:
  // The "callback function type" value.
  // Use v8::Object instead of v8::Function in order to handle
  // [TreatNonObjectAsNull].
  TraceWrapperV8Reference<v8::Object> callback_function_;
  // The associated Realm of the callback function type value iff it's the same
  // origin-domain. Otherwise, nullptr.
  Member<ScriptState> callback_relevant_script_state_;
  // The callback context, i.e. the incumbent Realm when an ECMAScript value is
  // converted to an IDL value.
  // https://heycam.github.io/webidl/#dfn-callback-context
  Member<ScriptState> incumbent_script_state_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_CALLBACK_FUNCTION_BASE_H_
