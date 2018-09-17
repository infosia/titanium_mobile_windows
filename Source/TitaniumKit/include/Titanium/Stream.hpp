/**
 * TitaniumKit Titanium.Stream
 *
 * Copyright (c) 2015 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _TITANIUM_STREAM_HPP_
#define _TITANIUM_STREAM_HPP_

#include "Titanium/Module.hpp"
#include "Titanium/Filesystem/Constants.hpp"
#include <unordered_set>

namespace Titanium
{
	using namespace HAL;

	class IOStream;
	class Buffer;
	class Blob;

	/*!
	  @struct
	  @discussion Argument passed to Titanium.Stream.createStream.
	  This is an abstract type. Any object meeting this description can be used where this type is used.
	  See http://docs.appcelerator.com/titanium/latest/#!/api/CreateStreamArgs
	*/
	struct CreateStreamArgs
	{
		std::unordered_set<Filesystem::MODE> modes;
		// source can be Buffer or Blob
		std::shared_ptr<Buffer> buffer_source { nullptr };
		std::shared_ptr<Blob>   blob_source { nullptr };
	};
	
	TITANIUMKIT_EXPORT CreateStreamArgs js_to_CreateStreamArgs(const JSObject& object);
	TITANIUMKIT_EXPORT JSObject CreateStreamArgs_to_js(const JSContext& js_context, const CreateStreamArgs& params);

	/*!
	  @class
	  @discussion This is the Titanium Stream Module.
	  See http://docs.appcelerator.com/titanium/latest/#!/api/Titanium.Stream
	*/
	class TITANIUMKIT_EXPORT Stream : public Module, public JSExport<Stream>
	{

	public:

		/*!
		  @method
		  @abstract  createStream
		  @discussion Returns Titanium.BufferStream or Titanium.BlobStream
		  depending on whether a Buffer or Blob is provided as the source property in params. 
		*/
		virtual std::shared_ptr<IOStream> createStream(const CreateStreamArgs& params) TITANIUM_NOEXCEPT;

		/*!
		  @method
		  @abstract read
		  @discussion Asynchronously reads data from an [IOStream](Titanium.IOStream) into a buffer.
		*/
		virtual void read(const std::shared_ptr<IOStream>& sourceStream, const std::shared_ptr<Buffer>& buffer, const std::uint32_t& offset, const std::uint32_t& length, JSObject resultsCallback) TITANIUM_NOEXCEPT;

		/*!
		  @method
		  @abstract readAll
		  @discussion Reads all data from the specified [IOStream](Titanium.IOStream).
		*/
		virtual std::shared_ptr<Buffer> readAll(const std::shared_ptr<IOStream>& sourceStream, const std::shared_ptr<Buffer>& buffer, JSObject resultsCallback) TITANIUM_NOEXCEPT;

		/*!
		  @method
		  @abstract write
		  @discussion Asynchronously writes data from a buffer to an [IOStream](Titanium.IOStream).
		*/
		virtual void write(const std::shared_ptr<IOStream>& outputStream, const std::shared_ptr<Buffer>& buffer, const uint32_t& offset, const uint32_t& length, JSObject resultsCallback) TITANIUM_NOEXCEPT;

		/*!
		  @method
		  @abstract writeStream
		  @discussion Writes all data from an input stream to an output stream.
		*/
		virtual std::uint32_t writeStream(const std::shared_ptr<IOStream>& inputStream, const std::shared_ptr<IOStream>& outputStream, const uint32_t& maxChunkSize, JSObject resultsCallback) TITANIUM_NOEXCEPT;

		/*!
		  @method
		  @abstract pump
		  @discussion Reads data from input stream and passes it to a handler method.
		*/
		virtual void pump(const std::shared_ptr<IOStream>& inputStream, JSObject handler, const uint32_t& maxChunkSize, const bool& isAsync) TITANIUM_NOEXCEPT;

		Stream(const JSContext&) TITANIUM_NOEXCEPT;
		virtual ~Stream()                  = default;
		Stream(const Stream&)            = default;
		Stream& operator=(const Stream&) = default;
#ifdef TITANIUM_MOVE_CTOR_AND_ASSIGN_DEFAULT_ENABLE
		Stream(Stream&&)                 = default;
		Stream& operator=(Stream&&)      = default;
#endif

		static void JSExportInitialize();

		TITANIUM_PROPERTY_READONLY_DEF(MODE_APPEND);
		TITANIUM_PROPERTY_READONLY_DEF(MODE_READ);
		TITANIUM_PROPERTY_READONLY_DEF(MODE_WRITE);
		TITANIUM_FUNCTION_DEF(createStream);
		TITANIUM_FUNCTION_DEF(read);
		TITANIUM_FUNCTION_DEF(readAll);
		TITANIUM_FUNCTION_DEF(write);
		TITANIUM_FUNCTION_DEF(writeStream);
		TITANIUM_FUNCTION_DEF(pump);

		const static std::uint32_t DEFAULT_CHUNK_SIZE;

	protected:
#pragma warning(push)
#pragma warning(disable : 4251)
		JSValue MODE_APPEND__;
		JSValue MODE_READ__;
		JSValue MODE_WRITE__;
		// Create a function to ensure callback is always done with Promise-like async manner
		JSObject promisifyFunc__;
		JSObject createPromisifyFunction(const JSContext& js_context) TITANIUM_NOEXCEPT;
#pragma warning(pop)
	};

} // namespace Titanium
#endif // _TITANIUM_STREAM_HPP_