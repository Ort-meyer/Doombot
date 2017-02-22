/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#ifndef __FILE_H__
#define __FILE_H__

/*
==============================================================

File Streams.

==============================================================
*/
#include "HaxHeader.h"
#include "BitMsg.h"

// mode parm for Seek
typedef enum {
	FS_SEEK_CUR,
	FS_SEEK_END,
	FS_SEEK_SET
} fsOrigin_t;

class idFileSystemLocal;


class idFile {
public:
	virtual					~idFile(void) {};
	// Get the name of the file.
	virtual const char *	GetName(void);
	// Get the full file path.
	virtual const char *	GetFullPath(void);
	// Read data from the file to the buffer.
	virtual int				Read(void *buffer, int len);
	// Write data from the buffer to the file.
	virtual int				Write(const void *buffer, int len);
	// Returns the length of the file.
	virtual int				Length(void);
	// Return a time value for reload operations.
	virtual ID_TIME_T			Timestamp(void);
	// Returns offset in file.
	virtual int				Tell(void);
	// Forces flush on files being writting to.
	virtual void			ForceFlush(void);
	// Causes any buffered data to be written to the file.
	virtual void			Flush(void);
	// Seek on a file.
	virtual int				Seek(long offset, fsOrigin_t origin);
	// Go back to the beginning of the file.
	virtual void			Rewind(void);
	// Like fprintf.
	//virtual int				Printf(const char *fmt, ...) id_attribute((format(printf, 2, 3)));
	// Like fprintf but with argument pointer
	virtual int				VPrintf(const char *fmt, va_list arg);
	// Write a string with high precision floating point numbers to the file.
	//virtual int				WriteFloatString(const char *fmt, ...) id_attribute((format(printf, 2, 3)));

	// Endian portable alternatives to Read(...)
	virtual int				ReadInt(int &value);
	virtual int				ReadUnsignedInt(unsigned int &value);
	virtual int				ReadShort(short &value);
	virtual int				ReadUnsignedShort(unsigned short &value);
	virtual int				ReadChar(char &value);
	virtual int				ReadUnsignedChar(unsigned char &value);
	virtual int				ReadFloat(float &value);
	virtual int				ReadBool(bool &value);
	//virtual int				ReadString(idStr &string);
	//virtual int				ReadVec2(idVec2 &vec);
	//virtual int				ReadVec3(idVec3 &vec);
	//virtual int				ReadVec4(idVec4 &vec);
	//virtual int				ReadVec6(idVec6 &vec);
	//virtual int				ReadMat3(idMat3 &mat);

	// Endian portable alternatives to Write(...)
	virtual int				WriteInt(const int value);
	virtual int				WriteUnsignedInt(const unsigned int value);
	virtual int				WriteShort(const short value);
	virtual int				WriteUnsignedShort(unsigned short value);
	virtual int				WriteChar(const char value);
	virtual int				WriteUnsignedChar(const unsigned char value);
	virtual int				WriteFloat(const float value);
	virtual int				WriteBool(const bool value);
	virtual int				WriteString(const char *string);
	//virtual int				WriteVec2(const idVec2 &vec);
	//virtual int				WriteVec3(const idVec3 &vec);
	//virtual int				WriteVec4(const idVec4 &vec);
	//virtual int				WriteVec6(const idVec6 &vec);
	//virtual int				WriteMat3(const idMat3 &mat);
};

class idFile_BitMsg : public idFile {
	friend class			idFileSystemLocal;

public:
	idFile_BitMsg(idBitMsg &msg);
	idFile_BitMsg(const idBitMsg &msg);
	virtual					~idFile_BitMsg(void);

	virtual const char *	GetName(void) { return name.c_str(); }
	virtual const char *	GetFullPath(void) { return name.c_str(); }
	virtual int				Read(void *buffer, int len);
	virtual int				Write(const void *buffer, int len);
	virtual int				Length(void);
	virtual ID_TIME_T			Timestamp(void);
	virtual int				Tell(void);
	virtual void			ForceFlush(void);
	virtual void			Flush(void);
	virtual int				Seek(long offset, fsOrigin_t origin);

private:
	string					name;			// name of the file
	int						mode;			// open mode
	idBitMsg *				msg;
};

#endif /* !__FILE_H__ */
