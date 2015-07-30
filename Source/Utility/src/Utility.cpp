/**
 * Titanium.Utility for Windows
 * Author: Gary Mathews
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "TitaniumWindows/Utility.hpp"
#include "HAL/HAL.hpp"
#include <boost/algorithm/string/predicate.hpp>

namespace TitaniumWindows
{
	namespace Utility
	{

		//
		// Convert std::string into Platform::String^
		//
		::Platform::String^ ConvertString(const std::string& str) 
		{
			return ref new ::Platform::String(std::wstring(str.begin(), str.end()).c_str());
		}

		//
		// Convert Platform::String^ into std::string
		//
		std::string ConvertString(::Platform::String^ str) 
		{
			return std::string(str->Begin(), str->End());
		}

		//
		// Convert UTF-8 std::string into Platform::String^
		//
		::Platform::String^ ConvertUTF8String(const std::string& str) 
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			return ref new ::Platform::String(converter.from_bytes(str).data());
		}

		//
		// Convert Platform::String^ into UTF-8 std::string
		//
		std::string ConvertUTF8String(::Platform::String^ str) 
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			return std::string(converter.to_bytes(str->Data()));
		}

		//
		// Convert unsigned char array into plain-text hex std::string
		//
		std::string HexString(unsigned char* data, size_t length) 
		{
			std::stringstream ss;
			for (size_t i = 0; i<length; ++i) {
				ss << std::hex
					<< std::noshowbase
					<< std::setw(2)
					<< std::setfill('0')
					<< (uint32_t)data[i];
			}
			return ss.str();
		}

		std::vector<std::uint8_t> GetContentFromBuffer(Windows::Storage::Streams::IBuffer^ buffer) 
		{
			const auto reader = Windows::Storage::Streams::DataReader::FromBuffer(buffer);
			std::vector<std::uint8_t> data(reader->UnconsumedBufferLength);
			if (!data.empty()) {
				reader->ReadBytes(::Platform::ArrayReference<std::uint8_t>(&data[0], data.size()));
			}
			return data;
		}

		std::vector<std::uint8_t> GetContentFromFile(Windows::Storage::StorageFile^ file) 
		{
			std::vector<std::uint8_t> content;
			concurrency::event event;
			concurrency::task<Windows::Storage::Streams::IBuffer^>(Windows::Storage::FileIO::ReadBufferAsync(file)).then([&content, &event](concurrency::task<Windows::Storage::Streams::IBuffer ^ > task) {
				try {
					content = std::move(GetContentFromBuffer(task.get()));
				}
				catch (::Platform::COMException^ ex) {
					// TODO throw JavaScript Exception
				}
				event.set();
			},
				concurrency::task_continuation_context::use_arbitrary());
			event.wait();

			return content;
		}

		const std::string MimeTypeForExtension(const std::string& path) 
		{
			const static std::unordered_map<std::string, const std::string> mimeTypeFromExtensionDict = {
				{ "css", "text/css" },
				{ "m4v", "video/x-m4v" } };

			const auto pos = path.find_last_of(".");
			if (pos != std::string::npos && path.size() > pos) {
				const auto ext = path.substr(pos + 1);
				const auto found = mimeTypeFromExtensionDict.find(ext);
				if (found != mimeTypeFromExtensionDict.end()) {
					return found->second;
				}
			}
			return "application/octet-stream";
		}

		std::chrono::milliseconds GetMSecSinceEpoch(const Windows::Foundation::DateTime& d) 
		{
			long long intervals = d.UniversalTime - EPOCH_BIAS; // this gives us number of 100 nanosecond intervals since unix epoch
			long long milliseconds = intervals / 10000; // convert 100 nanosecond intervals to milliseconds
			return std::chrono::milliseconds(milliseconds); // wrap in data type
		}

		std::chrono::milliseconds GetMSec(const Windows::Foundation::TimeSpan& t) 
		{
			return std::chrono::milliseconds(static_cast<std::chrono::milliseconds::rep>(t.Duration / 10000));
		}

		Windows::Foundation::Uri^ GetUriFromPath(const std::string& path) 
		{
			std::string modified = path;
			// if the path isn't an http/s URI already, fix URI to point to local files in app
			if (!boost::starts_with(modified, "http://") && !boost::starts_with(modified, "https://")) {
				// URIs must be absolute
				if (!boost::starts_with(modified, "/")) {
					modified = "/" + modified;
				}
				// use MS's in-app URL scheme
				modified = "ms-appx://" + modified;
			}
			return ref new Windows::Foundation::Uri(TitaniumWindows::Utility::ConvertUTF8String(modified));
		}

		std::uint32_t GetHResultErrorCode(::Platform::String^ errorMessage, const std::uint32_t& defaultCode)
		{
			// Extract HRESULT error code from error message
			const auto message = TitaniumWindows::Utility::ConvertString(errorMessage);
			std::uint32_t codeAsInt = defaultCode;
			const std::string needle = "HRESULT - ";
			const auto codeStart = message.find(needle);
			if (codeStart != std::string::npos) {
				const auto code = message.substr(message.find(needle) + needle.size());
				codeAsInt = static_cast<std::uint32_t>(std::stoul(code, nullptr, 16));
			}
			return codeAsInt;
		}

		Windows::Foundation::DateTime GetDateTime(const HAL::JSValue& dateObject) 
		{
			Windows::Foundation::DateTime date;
			const auto intervals = static_cast<std::uint64_t>(static_cast<double>(dateObject)) * 10000;
			date.UniversalTime = intervals + EPOCH_BIAS;
			return date;
		}

		// Add hidden UI onto current Window
		void SetHiddenViewForCurrentWindow(Windows::UI::Xaml::UIElement^ view, Windows::Foundation::EventRegistrationToken& token)
		{
			view->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			const auto rootFrame = dynamic_cast<Windows::UI::Xaml::Controls::Frame^>(Windows::UI::Xaml::Window::Current->Content);
			const auto registerUI = ref new Windows::UI::Core::DispatchedHandler([view, rootFrame]() {
				if (rootFrame->Content) {
					const auto page = dynamic_cast<Windows::UI::Xaml::Controls::Page^>(rootFrame->Content);
					if (page && page->Content) {
						const auto content = static_cast<Windows::UI::Xaml::Controls::Panel^>(page->Content);
						if (content) {
							content->Children->Append(view);
						}
					}
				}
			});
			if (rootFrame->Content == nullptr) {
				// If there's no Window (Window.open() is not called) yet, wait for the first Window opened
				token = rootFrame->Navigated += ref new Windows::UI::Xaml::Navigation::NavigatedEventHandler([rootFrame, registerUI, &token](Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) {
					rootFrame->Navigated -= token; // unregister current event, you just need to do this only once
					Windows::UI::Xaml::Window::Current->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, registerUI);
				});
			} else {
				// If there's a Window already, just attach to it
				Windows::UI::Xaml::Window::Current->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, registerUI);
			}
		}
	}  // namespace Utility
}  // namespace TitaniumWindows
