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

#include "File.h"

#define	MAX_PRINT_MSG		4096

static short(*_BigShort)(short l);
static short(*_LittleShort)(short l);
static int(*_BigLong)(int l);
static int(*_LittleLong)(int l);
static float(*_BigFloat)(float l);
static float(*_LittleFloat)(float l);
static void(*_BigRevBytes)(void *bp, int elsize, int elcount);
static void(*_LittleRevBytes)(void *bp, int elsize, int elcount);
static void(*_LittleBitField)(void *bp, int elsize);
static void(*_SixtetsForInt)(byte *out, int src);
static int(*_IntForSixtets)(byte *in);

short	BigShort(short l) { return _BigShort(l); }
short	LittleShort(short l) { return _LittleShort(l); }
int		BigLong(int l) { return _BigLong(l); }
int		LittleLong(int l) { return _LittleLong(l); }
float	BigFloat(float l) { return _BigFloat(l); }
float	LittleFloat(float l) { return _LittleFloat(l); }
void	BigRevBytes(void *bp, int elsize, int elcount) { _BigRevBytes(bp, elsize, elcount); }
void	LittleRevBytes(void *bp, int elsize, int elcount) { _LittleRevBytes(bp, elsize, elcount); }
void	LittleBitField(void *bp, int elsize) { _LittleBitField(bp, elsize); }

/*
=================
FS_WriteFloatString
=================
*/
//int FS_WriteFloatString(char *buf, const char *fmt, va_list argPtr) {
//	long i;
//	unsigned long u;
//	double f;
//	char *str;
//	int index;
//	string tmp, format;
//
//	index = 0;
//
//	while (*fmt) {
//		switch (*fmt) {
//		case '%':
//			format = "";
//			format += *fmt++;
//			while ((*fmt >= '0' && *fmt <= '9') ||
//				*fmt == '.' || *fmt == '-' || *fmt == '+' || *fmt == '#') {
//				format += *fmt++;
//			}
//			format += *fmt;
//			switch (*fmt) {
//			case 'f':
//			case 'e':
//			case 'E':
//			case 'g':
//			case 'G':
//				f = va_arg(argPtr, double);
//				if (format.Length() <= 2) {
//					// high precision floating point number without trailing zeros
//					sprintf(tmp, "%1.10f", f);
//					tmp.StripTrailing('0');
//					tmp.StripTrailing('.');
//					index += sprintf(buf + index, "%s", tmp.c_str());
//				}
//				else {
//					index += sprintf(buf + index, format.c_str(), f);
//				}
//				break;
//			case 'd':
//			case 'i':
//				i = va_arg(argPtr, long);
//				index += sprintf(buf + index, format.c_str(), i);
//				break;
//			case 'u':
//				u = va_arg(argPtr, unsigned long);
//				index += sprintf(buf + index, format.c_str(), u);
//				break;
//			case 'o':
//				u = va_arg(argPtr, unsigned long);
//				index += sprintf(buf + index, format.c_str(), u);
//				break;
//			case 'x':
//				u = va_arg(argPtr, unsigned long);
//				index += sprintf(buf + index, format.c_str(), u);
//				break;
//			case 'X':
//				u = va_arg(argPtr, unsigned long);
//				index += sprintf(buf + index, format.c_str(), u);
//				break;
//			case 'c':
//				i = va_arg(argPtr, long);
//				index += sprintf(buf + index, format.c_str(), (char)i);
//				break;
//			case 's':
//				str = va_arg(argPtr, char *);
//				index += sprintf(buf + index, format.c_str(), str);
//				break;
//			case '%':
//				index += sprintf(buf + index, format.c_str());
//				break;
//			default:
//				common->Error("FS_WriteFloatString: invalid format %s", format.c_str());
//				break;
//			}
//			fmt++;
//			break;
//		case '\\':
//			fmt++;
//			switch (*fmt) {
//			case 't':
//				index += sprintf(buf + index, "\t");
//				break;
//			case 'v':
//				index += sprintf(buf + index, "\v");
//				break;
//			case 'n':
//				index += sprintf(buf + index, "\n");
//				break;
//			case '\\':
//				index += sprintf(buf + index, "\\");
//				break;
//			default:
//				common->Error("FS_WriteFloatString: unknown escape character \'%c\'", *fmt);
//				break;
//			}
//			fmt++;
//			break;
//		default:
//			index += sprintf(buf + index, "%c", *fmt);
//			fmt++;
//			break;
//		}
//	}
//
//	return index;
//}

/*
=================================================================================

idFile

=================================================================================
*/

/*
=================
idFile::GetName
=================
*/
const char *idFile::GetName(void) {
	return "";
}

/*
=================
idFile::GetFullPath
=================
*/
const char *idFile::GetFullPath(void) {
	return "";
}

/*
=================
idFile::Read
=================
*/
int idFile::Read(void *buffer, int len) {
	//common->FatalError("idFile::Read: cannot read from idFile");
	return 0;
}

/*
=================
idFile::Write
=================
*/
int idFile::Write(const void *buffer, int len) {
	//common->FatalError("idFile::Write: cannot write to idFile");
	return 0;
}

/*
=================
idFile::Length
=================
*/
int idFile::Length(void) {
	return 0;
}

/*
=================
idFile::Timestamp
=================
*/
ID_TIME_T idFile::Timestamp(void) {
	return 0;
}

/*
=================
idFile::Tell
=================
*/
int idFile::Tell(void) {
	return 0;
}

/*
=================
idFile::ForceFlush
=================
*/
void idFile::ForceFlush(void) {
}

/*
=================
idFile::Flush
=================
*/
void idFile::Flush(void) {
}

/*
=================
idFile::Seek
=================
*/
int idFile::Seek(long offset, fsOrigin_t origin) {
	return -1;
}

/*
=================
idFile::Rewind
=================
*/
void idFile::Rewind(void) {
	Seek(0, FS_SEEK_SET);
}

/*
=================
idFile::Printf
=================
*/
//int idFile::Printf(const char *fmt, ...) {
//	char buf[MAX_PRINT_MSG];
//	int length;
//	va_list argptr;
//
//	va_start(argptr, fmt);
//	length = idStr::vsnPrintf(buf, MAX_PRINT_MSG - 1, fmt, argptr);
//	va_end(argptr);
//
//	// so notepad formats the lines correctly
//	idStr	work(buf);
//	work.Replace("\n", "\r\n");
//
//	return Write(work.c_str(), work.Length());
//}

/*
=================
idFile::VPrintf
=================
*/
int idFile::VPrintf(const char *fmt, va_list args) {
	//char buf[MAX_PRINT_MSG];
	//int length;
	//
	//length = idStr::vsnPrintf(buf, MAX_PRINT_MSG - 1, fmt, args);
	//return Write(buf, length);
	return 1;
}

/*
=================
idFile::WriteFloatString
=================
*/
//int idFile::WriteFloatString(const char *fmt, ...) {
//	char buf[MAX_PRINT_MSG];
//	int len;
//	va_list argPtr;
//
//	va_start(argPtr, fmt);
//	len = FS_WriteFloatString(buf, fmt, argPtr);
//	va_end(argPtr);
//
//	return Write(buf, len);
//}

/*
=================
idFile::ReadInt
=================
*/
int idFile::ReadInt(int &value) {
	int result = Read(&value, sizeof(value));
	value = LittleLong(value);
	return result;
}

/*
=================
idFile::ReadUnsignedInt
=================
*/
int idFile::ReadUnsignedInt(unsigned int &value) {
	int result = Read(&value, sizeof(value));
	value = LittleLong(value);
	return result;
}

/*
=================
idFile::ReadShort
=================
*/
int idFile::ReadShort(short &value) {
	int result = Read(&value, sizeof(value));
	value = LittleShort(value);
	return result;
}

/*
=================
idFile::ReadUnsignedShort
=================
*/
int idFile::ReadUnsignedShort(unsigned short &value) {
	int result = Read(&value, sizeof(value));
	value = LittleShort(value);
	return result;
}

/*
=================
idFile::ReadChar
=================
*/
int idFile::ReadChar(char &value) {
	return Read(&value, sizeof(value));
}

/*
=================
idFile::ReadUnsignedChar
=================
*/
int idFile::ReadUnsignedChar(unsigned char &value) {
	return Read(&value, sizeof(value));
}

/*
=================
idFile::ReadFloat
=================
*/
int idFile::ReadFloat(float &value) {
	int result = Read(&value, sizeof(value));
	value = LittleFloat(value);
	return result;
}

/*
=================
idFile::ReadBool
=================
*/
int idFile::ReadBool(bool &value) {
	unsigned char c;
	int result = ReadUnsignedChar(c);
	value = c ? true : false;
	return result;
}

/*
=================
idFile::ReadString
=================
*/
//int idFile::ReadString(idStr &string) {
//	int len;
//	int result = 0;
//
//	ReadInt(len);
//	if (len >= 0) {
//		string.Fill(' ', len);
//		result = Read(&string[0], len);
//	}
//	return result;
//}

/*
=================
idFile::ReadVec2
=================
*/
//int idFile::ReadVec2(idVec2 &vec) {
//	int result = Read(&vec, sizeof(vec));
//	LittleRevBytes(&vec, sizeof(float), sizeof(vec) / sizeof(float));
//	return result;
//}

/*
=================
idFile::ReadVec3
=================
*/
//int idFile::ReadVec3(idVec3 &vec) {
//	int result = Read(&vec, sizeof(vec));
//	LittleRevBytes(&vec, sizeof(float), sizeof(vec) / sizeof(float));
//	return result;
//}

/*
=================
idFile::ReadVec4
=================
*/
//int idFile::ReadVec4(idVec4 &vec) {
//	int result = Read(&vec, sizeof(vec));
//	LittleRevBytes(&vec, sizeof(float), sizeof(vec) / sizeof(float));
//	return result;
//}

/*
=================
idFile::ReadVec6
=================
*/
//int idFile::ReadVec6(idVec6 &vec) {
//	int result = Read(&vec, sizeof(vec));
//	LittleRevBytes(&vec, sizeof(float), sizeof(vec) / sizeof(float));
//	return result;
//}

/*
=================
idFile::ReadMat3
=================
*/
//int idFile::ReadMat3(idMat3 &mat) {
//	int result = Read(&mat, sizeof(mat));
//	LittleRevBytes(&mat, sizeof(float), sizeof(mat) / sizeof(float));
//	return result;
//}

/*
=================
idFile::WriteInt
=================
*/
int idFile::WriteInt(const int value) {
	int v = LittleLong(value);
	return Write(&v, sizeof(v));
}

/*
=================
idFile::WriteUnsignedInt
=================
*/
int idFile::WriteUnsignedInt(const unsigned int value) {
	unsigned int v = LittleLong(value);
	return Write(&v, sizeof(v));
}

/*
=================
idFile::WriteShort
=================
*/
int idFile::WriteShort(const short value) {
	short v = LittleShort(value);
	return Write(&v, sizeof(v));
}

/*
=================
idFile::WriteUnsignedShort
=================
*/
int idFile::WriteUnsignedShort(const unsigned short value) {
	unsigned short v = LittleShort(value);
	return Write(&v, sizeof(v));
}

/*
=================
idFile::WriteChar
=================
*/
int idFile::WriteChar(const char value) {
	return Write(&value, sizeof(value));
}

/*
=================
idFile::WriteUnsignedChar
=================
*/
int idFile::WriteUnsignedChar(const unsigned char value) {
	return Write(&value, sizeof(value));
}

/*
=================
idFile::WriteFloat
=================
*/
int idFile::WriteFloat(const float value) {
	float v = LittleFloat(value);
	return Write(&v, sizeof(v));
}

/*
=================
idFile::WriteBool
=================
*/
int idFile::WriteBool(const bool value) {
	unsigned char c = value;
	return WriteUnsignedChar(c);
}

/*
=================
idFile::WriteString
=================
*/
int idFile::WriteString(const char *value) {
	int len;

	len = strlen(value);
	WriteInt(len);
	return Write(value, len);
}

/*
=================
idFile::WriteVec2
=================
*/
//int idFile::WriteVec2(const idVec2 &vec) {
//	idVec2 v = vec;
//	LittleRevBytes(&v, sizeof(float), sizeof(v) / sizeof(float));
//	return Write(&v, sizeof(v));
//}
//
///*
//=================
//idFile::WriteVec3
//=================
//*/
//int idFile::WriteVec3(const idVec3 &vec) {
//	idVec3 v = vec;
//	LittleRevBytes(&v, sizeof(float), sizeof(v) / sizeof(float));
//	return Write(&v, sizeof(v));
//}
//
///*
//=================
//idFile::WriteVec4
//=================
//*/
//int idFile::WriteVec4(const idVec4 &vec) {
//	idVec4 v = vec;
//	LittleRevBytes(&v, sizeof(float), sizeof(v) / sizeof(float));
//	return Write(&v, sizeof(v));
//}
//
///*
//=================
//idFile::WriteVec6
//=================
//*/
//int idFile::WriteVec6(const idVec6 &vec) {
//	idVec6 v = vec;
//	LittleRevBytes(&v, sizeof(float), sizeof(v) / sizeof(float));
//	return Write(&v, sizeof(v));
//}
//
///*
//=================
//idFile::WriteMat3
//=================
//*/
//int idFile::WriteMat3(const idMat3 &mat) {
//	idMat3 v = mat;
//	LittleRevBytes(&v, sizeof(float), sizeof(v) / sizeof(float));
//	return Write(&v, sizeof(v));
//}

/*
=================================================================================

idFile_BitMsg

=================================================================================
*/

/*
=================
idFile_BitMsg::idFile_BitMsg
=================
*/
idFile_BitMsg::idFile_BitMsg(idBitMsg &msg) {
	name = "*unknown*";
	mode = (1 << 1);
	this->msg = &msg;
}

/*
=================
idFile_BitMsg::idFile_BitMsg
=================
*/
idFile_BitMsg::idFile_BitMsg(const idBitMsg &msg) {
	name = "*unknown*";
	mode = (1 << 0);
	this->msg = const_cast<idBitMsg *>(&msg);
}

/*
=================
idFile_BitMsg::~idFile_BitMsg
=================
*/
idFile_BitMsg::~idFile_BitMsg(void) {
}

/*
=================
idFile_BitMsg::Read
=================
*/
int idFile_BitMsg::Read(void *buffer, int len) {

	if (!(mode & (1 << 0))) {
		//common->FatalError("idFile_BitMsg::Read: %s not opened in read mode", name.c_str());
		return 0;
	}

	return msg->ReadData(buffer, len);
}

/*
=================
idFile_BitMsg::Write
=================
*/
int idFile_BitMsg::Write(const void *buffer, int len) {

	if (!(mode & (1 << 1))) {
		//common->FatalError("idFile_Memory::Write: %s not opened in write mode", name.c_str());
		return 0;
	}

	msg->WriteData(buffer, len);
	return len;
}

/*
=================
idFile_BitMsg::Length
=================
*/
int idFile_BitMsg::Length(void) {
	return msg->GetSize();
}

/*
=================
idFile_BitMsg::Timestamp
=================
*/
ID_TIME_T idFile_BitMsg::Timestamp(void) {
	return 0;
}

/*
=================
idFile_BitMsg::Tell
=================
*/
int idFile_BitMsg::Tell(void) {
	if (mode & 0) {
		return msg->GetReadCount();
	}
	else {
		return msg->GetSize();
	}
}

/*
=================
idFile_BitMsg::ForceFlush
=================
*/
void idFile_BitMsg::ForceFlush(void) {
}

/*
=================
idFile_BitMsg::Flush
=================
*/
void idFile_BitMsg::Flush(void) {
}

/*
=================
idFile_BitMsg::Seek

returns zero on success and -1 on failure
=================
*/
int idFile_BitMsg::Seek(long offset, fsOrigin_t origin) {
	return -1;
}