/**
 * TitaniumKit
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _TITANIUM_EXAMPLES_NATIVEAPIEXAMPLE_HPP_
#define _TITANIUM_EXAMPLES_NATIVEAPIEXAMPLE_HPP_

#include "Titanium/API.hpp"

using namespace HAL;

/*!
 @class
 
 @discussion This is an example of how to implement Titanium::API
 for a native platform.
 */
class NativeAPIExample final : public Titanium::API, public JSExport<NativeAPIExample>
{
public:
	NativeAPIExample(const JSContext&) TITANIUM_NOEXCEPT;

	virtual ~NativeAPIExample() TITANIUM_NOEXCEPT;  //= default;
	NativeAPIExample(const NativeAPIExample&) = default;
	NativeAPIExample& operator=(const NativeAPIExample&) = default;
#ifdef TITANIUM_MOVE_CTOR_AND_ASSIGN_DEFAULT_ENABLE
	NativeAPIExample(NativeAPIExample&&) = default;
	NativeAPIExample& operator=(NativeAPIExample&&) = default;
#endif

	static void JSExportInitialize();

protected:
	virtual void log(const std::string& message) const TITANIUM_NOEXCEPT override final;
};

#endif  // _TITANIUM_EXAMPLES_NATIVEAPIEXAMPLE_HPP_
