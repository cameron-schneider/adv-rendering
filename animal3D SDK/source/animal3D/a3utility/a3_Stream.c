/*
	Copyright 2011-2021 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_Stream.c
	Basic file read/write implementation

	**DO NOT MODIFY THIS FILE**
*/

#include "animal3D/a3utility/a3_Stream.h"

#include <stdio.h>
#include <stdlib.h>


//-----------------------------------------------------------------------------

a3ret a3streamAllocate(a3_Stream *stream_out, const a3ui32 size)
{
	a3_Stream ret = { 0 };
	if (stream_out && size)
	{
		if (!stream_out->contents)
		{
			// alloc
			ret.contents = ret.ptr = (a3byte *)malloc(size);
			if (ret.contents)
			{
				// copy
				ret.length = size;
				*stream_out = ret;
				return size;
			}
			return 0;
		}
	}
	return -1;
}


a3ret a3streamLoadContents(a3_Stream *stream_out, const a3byte *filePath)
{
	if (stream_out && filePath && *filePath)
	{
		if (!stream_out->contents)
		{
			// open file
			a3_Stream ret = { 0 };
			FILE *fp = fopen(filePath, "rb");
			if (fp)
			{
				// get character count
				fseek(fp, 0, SEEK_END);
				ret.length = (a3ui32)ftell(fp);
				if (ret.length)
				{
					// allocate and read from beginning
					ret.contents = ret.ptr = (a3byte *)malloc(ret.length + 1);
					rewind(fp);
					fread(ret.ptr, 1, ret.length, fp);
					ret.ptr[ret.length] = 0;
					*stream_out = ret;
				}

				// done
				fclose(fp);
				return ret.length;
			}
			return 0;
		}
	}
	return -1;
}

a3ret a3streamSaveContents(const a3_Stream *stream, const a3byte *filePath)
{
	if (stream && filePath && *filePath)
	{
		if (stream->contents)
		{
			FILE *fp = fopen(filePath, "wb");
			if (fp)
			{
				fwrite(stream->contents, 1, stream->length, fp);
				fclose(fp);
				return stream->length;
			}
		}
	}
	return -1;
}


a3ret a3streamObjectRead(a3_Stream *stream, void *object, const a3_StreamReadFunc streamReadFunc)
{
	if (stream && object && streamReadFunc)
	{
		if (stream->contents)
		{
			const a3i32 ret = streamReadFunc(object, stream->ptr);
			if (ret > 0)
			{
				stream->ptr += ret;
				stream->offset += ret;
			}
			return ret;
		}
	}
	return -1;
}

a3ret a3streamObjectWrite(a3_Stream *stream, const void *object, const a3_StreamWriteFunc streamWriteFunc)
{
	if (stream && object && streamWriteFunc)
	{
		if (stream->contents)
		{
			const a3i32 ret = streamWriteFunc(object, stream->ptr);
			if (ret > 0)
			{
				stream->ptr += ret;
				stream->offset += ret;
			}
			return ret;
		}
	}
	return -1;
}


a3ret a3streamReset(a3_Stream *stream)
{
	if (stream)
	{
		const a3ui32 ret = stream->offset;
		stream->ptr = (a3byte *)stream->contents;
		stream->offset = 0;
		return ret;
	}
	return -1;
}


a3ret a3streamReleaseContents(a3_Stream *stream)
{
	if (stream && stream->contents)
	{
		const a3ui32 length = stream->length;
		free((a3byte *)stream->contents);
		stream->contents = stream->ptr = 0;
		stream->length = stream->offset = 0;
		return length;
	}
	return -1;
}


//-----------------------------------------------------------------------------

a3ret a3fileStreamOpenRead(a3_FileStream *fileStream, const a3byte *filePath)
{
	FILE *fp = 0;
	a3i32 ret = 0;
	if (fileStream && filePath)
	{
		if (fp = fileStream->stream)
		{
			fclose(fp);
			fp = 0;
		}
		if (*filePath)
		{
			fp = fopen(filePath, "rb");
			if (fp)
			{
				fileStream->bytes = 0;
				fileStream->lastStream = 0;
				ret = 1;
			}
		}
		fileStream->stream = fp;
		fileStream->mode = +ret;
		return ret;
	}
	return -1;
}

a3ret a3fileStreamOpenWrite(a3_FileStream *fileStream, const a3byte *filePath)
{
	FILE *fp = 0;
	a3i32 ret = 0;
	if (fileStream && filePath)
	{
		if (fp = fileStream->stream)
		{
			fclose(fp);
			fp = 0;
		}
		if (*filePath)
		{
			fp = fopen(filePath, "wb");
			if (fp)
			{
				fileStream->bytes = 0;
				fileStream->lastStream = 0;
				ret = 1;
			}
		}
		fileStream->stream = fp;
		fileStream->mode = -ret;
		return ret;
	}
	return -1;
}

a3ret a3fileStreamReadObject(a3_FileStream *fileStream, void *object, const a3_FileStreamReadFunc streamReadFunc)
{
	a3i32 ret = 0;
	if (fileStream && object && streamReadFunc)
	{
		if (fileStream->stream && fileStream->mode >= +1)
		{
			ret = streamReadFunc(object, fileStream);
			fileStream->lastStream = ret;
			fileStream->bytes += ret;
			return ret;
		}
	}
	return -1;
}

a3ret a3fileStreamWriteObject(a3_FileStream *fileStream, const void *object, const a3_FileStreamWriteFunc streamWriteFunc)
{
	a3i32 ret = 0;
	if (fileStream && object && streamWriteFunc)
	{
		if (fileStream->stream && fileStream->mode <= -1)
		{
			ret = streamWriteFunc(object, fileStream);
			fileStream->lastStream = ret;
			fileStream->bytes += ret;
			return ret;
		}
	}
	return -1;
}

a3ret a3fileStreamClose(a3_FileStream *fileStream)
{
	FILE *fp = 0;
	if (fileStream)
	{
		if (fp = fileStream->stream)
		{
			fclose(fp);
			fileStream->stream = 0;
			fileStream->mode = 0;
			return (fileStream->bytes);
		}
	}
	return -1;
}


#ifdef _WIN32
#include <direct.h>
#else	// !_WIN32
#include <sys/stat.h>
#endif	// _WIN32

a3ret a3fileStreamMakeDirectory(const a3byte *path)
{
	if (path && *path)
	{
		const a3i32 ret = 
#ifdef _WIN32
			_mkdir(path);
#else	// !_WIN32
			mkdir(path, 0700);
#endif	// _WIN32
		return (ret == 0);
	}
	return -1;
}


//-----------------------------------------------------------------------------
