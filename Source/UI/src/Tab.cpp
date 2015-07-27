/**
 * Titanium.UI.Tab for Windows
 *
 * Copyright (c) 2015 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "TitaniumWindows/UI/Tab.hpp"
#include "TitaniumWindows/UI/Window.hpp"
#include "TitaniumWindows/Utility.hpp"

namespace TitaniumWindows
{
	namespace UI
	{

		using namespace Windows::UI::Xaml;
		using namespace Windows::UI::Xaml::Controls;

		Tab::Tab(const JSContext& js_context) TITANIUM_NOEXCEPT
			: Titanium::UI::Tab(js_context)
		{
		}

		void Tab::postCallAsConstructor(const JSContext& js_context, const std::vector<JSValue>& arguments)
		{
			Titanium::UI::Tab::postCallAsConstructor(js_context, arguments);	

#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
			pivotItem__ = ref new PivotItem();

			Titanium::UI::Tab::setLayoutDelegate<WindowsViewLayoutDelegate>();
			getViewLayoutDelegate<WindowsViewLayoutDelegate>()->setComponent(pivotItem__);
#else
			grid__ = ref new Grid();
			Titanium::UI::Tab::setLayoutDelegate<WindowsViewLayoutDelegate>();
			getViewLayoutDelegate<WindowsViewLayoutDelegate>()->setComponent(grid__);
#endif
		}

		void Tab::set_title(const std::string& title) TITANIUM_NOEXCEPT
		{
			Titanium::UI::Tab::set_title(title);
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
			pivotItem__->Header = TitaniumWindows::Utility::ConvertUTF8String(title);
#else

#endif
		}

		void Tab::set_window(const std::shared_ptr<Titanium::UI::Window>& window) TITANIUM_NOEXCEPT
		{
			if (window != window__) {
				const auto windows_window = dynamic_cast<TitaniumWindows::UI::Window*>(window.get());
				const auto view = windows_window->getComponent();
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
				pivotItem__->Content = view;
#else
				if (grid__->Children->Size > 0) {
					grid__->Children->RemoveAt(0);
				}
				grid__->Children->Append(view);
				grid__->SetColumn(view, 0);
				grid__->SetRow(view, 0);
#endif
			}

			Titanium::UI::Tab::set_window(window);
		}

		void Tab::blur() 
		{
			if (window__) {
				window__->blur();
			}
			Titanium::UI::Tab::blur();
		}

		void Tab::focus() 
		{
			if (window__) {
				window__->focus();
			}
			Titanium::UI::Tab::focus();
		}

		void Tab::JSExportInitialize() 
		{
			JSExport<Tab>::SetClassVersion(1);
			JSExport<Tab>::SetParent(JSExport<Titanium::UI::Tab>::Class());
		}

	}  // namespace UI
}  // namespace TitaniumWindows