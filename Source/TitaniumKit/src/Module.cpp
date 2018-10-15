/**
 * TitaniumKit
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "Titanium/Module.hpp"
#include "Titanium/detail/TiImpl.hpp"
#include <cstdint>
#include "Titanium/UI/Window.hpp"
#include "Titanium/UI/TabGroup.hpp"

namespace Titanium
{
	Module::Module(const JSContext& js_context, const std::string& apiName) TITANIUM_NOEXCEPT
	    : JSExportObject(js_context)
	    , apiName__(apiName)
	    , ctorProperties__(js_context.CreateObject())
	{
	}

	void Module::postCallAsConstructor(const JSContext& js_context, const std::vector<JSValue>& arguments) {
		if (arguments.size() >= 1) {
			const auto _0 = arguments.at(0);
			if (_0.IsObject()) {
				JSObject properties = static_cast<JSObject>(_0);
				TITANIUM_LOG_DEBUG(apiName__, " ctor has ", properties.GetPropertyNames().GetCount(), " properties for ", this);
			}
		}
	}

	Module::~Module() TITANIUM_NOEXCEPT
	{
	}

	TITANIUM_PROPERTY_READWRITE(Module, bool, bubbleParent)
	TITANIUM_PROPERTY_READWRITE(Module, std::string, apiName)

	void Module::addEventListener(const std::string& name, JSObject& callback, JSObject& this_object) TITANIUM_NOEXCEPT
	{
		if (!callback.IsFunction()) {
			TITANIUM_LOG_WARN(apiName__, " addEventListener: Listener is not a function for event '", name, "'");
			return;
		}

		std::vector<JSObject> event_listener_list;
		if (event_listener_map__.find(name) != event_listener_map__.end()) {
			event_listener_list = event_listener_map__.at(name);
		}

		JSObject callback_payload = get_context().CreateObject();
		callback_payload.SetProperty("callback", callback);
		callback_payload.SetProperty("this_object", this_object);

		// Precondition
		const auto event_listener_index = eventListenerIndex(event_listener_list, name, callback_payload);
		if (event_listener_index > 0) {
			TITANIUM_LOG_WARN(apiName__, " addEventListener: event listener already added at index ", event_listener_index, " for event '", name, "'");
		} else {
			const auto callback_list_index = event_listener_list.size();
			if (callback_list_index == 0) {
				// This is the first event listener for this event name, so signal
				// module subclasses that callbacks have been registered for this event.
				enableEvent(name);
			}
			TITANIUM_LOG_DEBUG(apiName__, " addEventListener: add listener at index ", callback_list_index, " for event '", name, "' for ", this);
			event_listener_list.push_back(callback_payload);
		}
		event_listener_map__[name] = event_listener_list;
	}

	void Module::removeEventListener(const std::string& name, JSObject& callback, JSObject& this_object) TITANIUM_NOEXCEPT
	{
		if (!callback.IsFunction()) {
			TITANIUM_LOG_WARN(apiName__, " removeEventListener: callback is not a function for event '", name, "'");
			return;
		}

		if (event_listener_map__.find(name) == event_listener_map__.end()) {
			TITANIUM_LOG_WARN(apiName__, " removeEventListener: No event listener's for event '", name, "'");
			return;
		}

		std::vector<JSObject> event_listener_list = event_listener_map__.at(name);

		JSObject callback_payload = get_context().CreateObject();
		callback_payload.SetProperty("callback", callback);
		callback_payload.SetProperty("this_object", this_object);

		const auto event_listener_count = event_listener_list.size();
		if (event_listener_count > 0) {
			const auto event_listener_index = eventListenerIndex(event_listener_list, name, callback_payload);

			if (event_listener_count == 1) {
				// This is the last event listener for this event name, and it is about
				// to be removed, so signal module subclasses that there are no more
				// callbacks registered for this event.
				disableEvent(name);
			}

			TITANIUM_LOG_DEBUG(apiName__, " removeEventListener: remove listener at index ", event_listener_index, " for event '", name, "' for ", this);

			// postcondition
			event_listener_list.erase(event_listener_list.begin() +  event_listener_index);
			//if (!deleted) {
				// This is a TitaniumKit logic error.
				//TITANIUM_LOG_ERROR(apiName__, " removeEventListener: event listener at index ", event_listener_index, " for event '", name, "' was not removed");
				//TITANIUM_ASSERT(deleted);
			//}
		} else {
			TITANIUM_LOG_WARN(apiName__, " removeEventListener: listener does not exist for event '", name, "'");
		}
		event_listener_map__[name] = event_listener_list;
	}

	void Module::applyProperties(const JSObject& props, JSObject& this_object) TITANIUM_NOEXCEPT
	{
		const auto properties = props.GetProperties();
		for (const auto pair : properties) {
			this_object.SetProperty(pair.first, pair.second);
		}

		const auto module = this_object.GetPrivate<Titanium::Module>();
		if (module && module->propertiesSet__ == false) {
			module->ctorProperties__ = props;
			module->propertiesSet__ = true;
			module->afterPropertiesSet();
		}
	}

	void Module::afterPropertiesSet() TITANIUM_NOEXCEPT
	{
		TITANIUM_LOG_DEBUG(apiName__, " afterPropertiesSet for ", this);
	}

	void Module::ShowRedScreenOfDeath(const JSContext& ctx, const HAL::detail::js_runtime_error& e) TITANIUM_NOEXCEPT
	{
		try {
			auto js_error = ctx.CreateError();
			js_error.SetProperty("message", ctx.CreateString(e.js_message()));
			js_error.SetProperty("name", ctx.CreateString(e.js_name()));
			js_error.SetProperty("fileName", ctx.CreateString(e.js_filename()));
			js_error.SetProperty("stack", ctx.CreateString(e.js_stack()));
			js_error.SetProperty("nativeStack", ctx.CreateString(e.js_nativeStack()));
			js_error.SetProperty("lineNumber", ctx.CreateNumber(e.js_linenumber()));

			const auto rsod = ctx.get_global_object().GetProperty("Titanium_RedScreenOfDeath");
			auto rsod_func = static_cast<JSObject>(rsod);
			const std::vector<JSValue> args = { js_error };
			rsod_func(args, rsod_func);
			HAL::JSError::ClearNativeStack();
		}
		catch (...) {
			// Just to make sure we don't throw another exception :(
		}

	}

	void Module::ShowRedScreenOfDeath(const JSContext& ctx, const std::string& message) TITANIUM_NOEXCEPT
	{
		try {
			auto js_error = ctx.CreateError();
			js_error.SetProperty("message", ctx.CreateString(message));
			js_error.SetProperty("nativeStack", ctx.CreateString(HAL::JSError::GetNativeStack()));

			const auto rsod = ctx.get_global_object().GetProperty("Titanium_RedScreenOfDeath");
			auto rsod_func = static_cast<JSObject>(rsod);
			const std::vector<JSValue> args = { js_error };
			rsod_func(args, rsod_func);
			HAL::JSError::ClearNativeStack();
		} catch (...) {
			// Just to make sure we don't throw another exception :(
		}
	}

	bool Module::hasEventListener(const std::string& event_name) TITANIUM_NOEXCEPT
	{
		return event_listener_map__.find(event_name) != event_listener_map__.end() && event_listener_map__.at(event_name).size() > 0;
	}

	void Module::fireEvent(const std::string& name) TITANIUM_NOEXCEPT
	{
		fireEvent(name, get_context().CreateObject());
	}

	void Module::fireEvent(const std::string& name, const JSObject& event, const bool& propagate) TITANIUM_NOEXCEPT
	{
		if (!enableEvents__) {
			TITANIUM_LOG_WARN(apiName__, " fireEvent: Stopped firing '", name, "'");
			return;
		}

		// bubbleParent:
		// 1. if bubbleParent=true and parent listens to the event, invoke event for both
		// 2. if bubbleParent=false and parent listens to the event, don't fire parent event

		// if source is not 'this' module and its bubleParent=false, leave it to the source
		const auto source = event.GetProperty("source");
		if (source.IsObject()) {
			const auto sourceObj = static_cast<JSObject>(source).GetPrivate<Module>();
			if (sourceObj && !sourceObj->get_bubbleParent()) {
				return;
			}
		}

		// if this module does not listen to it
		if (event_listener_map__.find(name) == event_listener_map__.end()) {
			// bubbleParent=true and parent listens to it? then invoke parent event
			const auto parent = get_object().GetProperty("parent");
			if (propagate && get_bubbleParent() && parent.IsObject()) {
				const auto parentObj = static_cast<JSObject>(parent).GetPrivate<Module>();
				if (parentObj) {
					parentObj->fireEvent(name, event);
				}
			}
			return;
		}

		std::vector<JSObject> event_listener_list = event_listener_map__.at(name);
		const auto event_listener_count = event_listener_list.size();

		if (event_listener_count == 0) {
			TITANIUM_LOG_WARN(apiName__, " fireEvent: No listeners for event '", name, "'");
			return;
		}

		auto event_copy = event;
		if (!event_copy.HasProperty("source")) {
			event_copy.SetProperty("source", get_object());
		}
		if (!event_copy.HasProperty("bubbles")) {
			event_copy.SetProperty("bubbles", get_context().CreateBoolean(get_bubbleParent()));
		}

		event_copy.SetProperty("type", event.get_context().CreateString(name));

		TITANIUM_EXCEPTION_CATCH_START {
			for (size_t i = 0; i < event_listener_count; ++i) {
				JSObject callback_payload = event_listener_list.at(i);

				JSValue callback_property = callback_payload.GetProperty("callback");

				// Precondition
				TITANIUM_ASSERT(callback_property.IsObject());
				JSObject callback = static_cast<JSObject>(callback_property);

				// Precondition
				TITANIUM_ASSERT(callback.IsFunction());

				JSValue this_object_property = callback_payload.GetProperty("this_object");

				// Precondition
				TITANIUM_ASSERT(this_object_property.IsObject());
				JSObject this_object = static_cast<JSObject>(this_object_property);

				TITANIUM_LOG_DEBUG(apiName__, " fireEvent: name = '", name, "' for listener at index ", i, " for ", this);

				//
				// Note: Currently there's no way to access "arguments.callee" inside HAL JSExport callback.
				// We cheat here, by adding callback as an argument
				//
				callback({ static_cast<JSValue>(event_copy), callback }, this_object);
			}
		} TITANIUM_EXCEPTION_CATCH_END
	}

	void Module::enableEvent(const std::string& event_name) TITANIUM_NOEXCEPT
	{
		TITANIUM_LOG_WARN(apiName__, " enableEvent: Unimplemented (event name '", event_name, "' for ", this);
	}

	void Module::disableEvent(const std::string& event_name) TITANIUM_NOEXCEPT
	{
		TITANIUM_LOG_WARN(apiName__, " disableEvent: Unimplemented (event name '", event_name, "' for ", this);
	}

	unsigned Module::eventListenerIndex(const std::vector<JSObject>& event_listener_list, const std::string& name, JSObject& callback) TITANIUM_NOEXCEPT
	{
		unsigned index = 0;
		bool found = false;
		const auto event_listener_count = event_listener_list.size();
		for (unsigned i = 0; i < event_listener_count; ++i) {
			JSObject callback_payload = event_listener_list.at(i);

			JSValue callback_property = callback_payload.GetProperty("callback");
			if (callback == callback_property) {
				// precondition
				if (found) {
					// This is a TitaniumKit logic error.
					TITANIUM_LOG_ERROR("Module::eventListenerIndex: multiple identical listeners found for event = '", name, "'");
				}

				index = i;
				found = true;
				break;
			}
		}

		TITANIUM_LOG_DEBUG("Module::eventListenerIndex: index = ", index, " for event '", name, "'");
		return index;
	}

	void Module::JSExportInitialize()
	{
		JSExport<Module>::SetClassVersion(1);
		JSExport<Module>::SetParent(JSExport<JSExportObject>::Class());
		TITANIUM_ADD_PROPERTY(Module, bubbleParent);
		TITANIUM_ADD_CONSTANT_PROPERTY(Module, apiName);
		TITANIUM_ADD_PROPERTY(Module, lifecycleContainer);

		TITANIUM_ADD_FUNCTION(Module, addEventListener);
		TITANIUM_ADD_FUNCTION(Module, removeEventListener);
		TITANIUM_ADD_FUNCTION(Module, applyProperties);
		TITANIUM_ADD_FUNCTION(Module, fireEvent);
		TITANIUM_ADD_FUNCTION(Module, getBubbleParent);
		TITANIUM_ADD_FUNCTION(Module, setBubbleParent);
		TITANIUM_ADD_FUNCTION(Module, getApiName);
		TITANIUM_ADD_FUNCTION(Module, getLifecycleContainer);
	}

	TITANIUM_PROPERTY_GETTER_BOOL(Module, bubbleParent)
	TITANIUM_PROPERTY_SETTER_BOOL(Module, bubbleParent)
	TITANIUM_PROPERTY_GETTER_STRING(Module, apiName)

	TITANIUM_PROPERTY_GETTER(Module, lifecycleContainer)
	{
		if (lifecycleContainerWindow__) {
			return lifecycleContainerWindow__->get_object();
		} else if (lifecycleContainerTabGroup__) {
			return lifecycleContainerTabGroup__->get_object();
		} else {
			return get_context().CreateNull();
		}
	}

	TITANIUM_PROPERTY_SETTER(Module, lifecycleContainer)
	{
		if (argument.IsObject()) {
			const auto object = static_cast<JSObject>(argument);
			lifecycleContainerWindow__   = object.GetPrivate<Titanium::UI::Window>();
			lifecycleContainerTabGroup__ = object.GetPrivate<Titanium::UI::TabGroup>();
		} else {
			lifecycleContainerWindow__   = nullptr;
			lifecycleContainerTabGroup__ = nullptr;
		}
		return true;
	}

	TITANIUM_FUNCTION(Module, addEventListener)
	{
		ENSURE_ARGUMENT_INDEX(1);
		ENSURE_STRING_AT_INDEX(name, 0);
		ENSURE_OBJECT_AT_INDEX(callback, 1);

		TITANIUM_ASSERT(callback.IsFunction());
		addEventListener(name, callback, this_object);
		return get_context().CreateUndefined();
	}

	TITANIUM_FUNCTION(Module, removeEventListener)
	{
		ENSURE_ARGUMENT_INDEX(1);
		ENSURE_STRING_AT_INDEX(name, 0);
		ENSURE_OBJECT_AT_INDEX(callback, 1);

		TITANIUM_ASSERT(callback.IsFunction());
		removeEventListener(name, callback, this_object);
		return get_context().CreateUndefined();
	}

	TITANIUM_FUNCTION(Module, applyProperties)
	{
		ENSURE_OBJECT_AT_INDEX(props, 0);

		applyProperties(props, this_object);
		return get_context().CreateUndefined();
	}

	TITANIUM_FUNCTION(Module, fireEvent)
	{
		ENSURE_STRING_AT_INDEX(name, 0);
		ENSURE_OPTIONAL_OBJECT_AT_INDEX(param, 1);

		fireEvent(name, param);
		return get_context().CreateUndefined();
	}

	TITANIUM_FUNCTION_AS_GETTER(Module, getBubbleParent, bubbleParent)
	TITANIUM_FUNCTION_AS_SETTER(Module, setBubbleParent, bubbleParent)
	TITANIUM_FUNCTION_AS_GETTER(Module, getApiName, apiName)
	TITANIUM_FUNCTION_AS_GETTER(Module, getLifecycleContainer, lifecycleContainer)
	TITANIUM_FUNCTION_AS_SETTER(Module, setLifecycleContainer, lifecycleContainer)

}  // namespace Titanium {
