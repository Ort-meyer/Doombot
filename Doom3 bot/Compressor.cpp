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

#include "Compressor.h"
#pragma hdrstop


/*
=================================================================================

idCompressor_None

=================================================================================
*/

class idCompressor_None : public idCompressor {
public:
	idCompressor_None(void);

	void			Init(idFile *f, bool compress, int wordLength);
	void			FinishCompress(void);
	float			GetCompressionRatio(void) const;

	const char *	GetName(void);
	const char *	GetFullPath(void);
	int				Read(void *outData, int outLength);
	int				Write(const void *inData, int inLength);
	int				Length(void);
	ID_TIME_T			Timestamp(void);
	int				Tell(void);
	void			ForceFlush(void);
	void			Flush(void);
	int				Seek(long offset, fsOrigin_t origin);

protected:
	idFile *		file;
	bool			compress;
};

/*
================
idCompressor_None::idCompressor_None
================
*/
idCompressor_None::idCompressor_None(void) {
	file = NULL;
	compress = true;
}

/*
================
idCompressor_None::Init
================
*/
void idCompressor_None::Init(idFile *f, bool compress, int wordLength) {
	this->file = f;
	this->compress = compress;
}

/*
================
idCompressor_None::FinishCompress
================
*/
void idCompressor_None::FinishCompress(void) {
}

/*
================
idCompressor_None::GetCompressionRatio
================
*/
float idCompressor_None::GetCompressionRatio(void) const {
	return 0.0f;
}

/*
================
idCompressor_None::GetName
================
*/
const char *idCompressor_None::GetName(void) {
	if (file) {
		return file->GetName();
	}
	else {
		return "";
	}
}

/*
================
idCompressor_None::GetFullPath
================
*/
const char *idCompressor_None::GetFullPath(void) {
	if (file) {
		return file->GetFullPath();
	}
	else {
		return "";
	}
}

/*
================
idCompressor_None::Write
================
*/
int idCompressor_None::Write(const void *inData, int inLength) {
	if (compress == false || inLength <= 0) {
		return 0;
	}
	return file->Write(inData, inLength);
}

/*
================
idCompressor_None::Read
================
*/
int idCompressor_None::Read(void *outData, int outLength) {
	if (compress == true || outLength <= 0) {
		return 0;
	}
	return file->Read(outData, outLength);
}

/*
================
idCompressor_None::Length
================
*/
int idCompressor_None::Length(void) {
	if (file) {
		return file->Length();
	}
	else {
		return 0;
	}
}

/*
================
idCompressor_None::Timestamp
================
*/
ID_TIME_T idCompressor_None::Timestamp(void) {
	if (file) {
		return file->Timestamp();
	}
	else {
		return 0;
	}
}

/*
================
idCompressor_None::Tell
================
*/
int idCompressor_None::Tell(void) {
	if (file) {
		return file->Tell();
	}
	else {
		return 0;
	}
}

/*
================
idCompressor_None::ForceFlush
================
*/
void idCompressor_None::ForceFlush(void) {
	if (file) {
		file->ForceFlush();
	}
}

/*
================
idCompressor_None::Flush
================
*/
void idCompressor_None::Flush(void) {
	if (file) {
		file->ForceFlush();
	}
}

/*
================
idCompressor_None::Seek
================
*/
int idCompressor_None::Seek(long offset, fsOrigin_t origin) {

	return -1;
}


/*
=================================================================================

idCompressor_BitStream

Base class for bit stream compression.

=================================================================================
*/

class idCompressor_BitStream : public idCompressor_None {
public:
	idCompressor_BitStream(void) {}

	void			Init(idFile *f, bool compress, int wordLength);
	void			FinishCompress(void);
	float			GetCompressionRatio(void) const;

	int				Write(const void *inData, int inLength);
	int				Read(void *outData, int outLength);

protected:
	byte			buffer[65536];
	int				wordLength;

	int				readTotalBytes;
	int				readLength;
	int				readByte;
	int				readBit;
	const byte *	readData;

	int				writeTotalBytes;
	int				writeLength;
	int				writeByte;
	int				writeBit;
	byte *			writeData;

protected:
	void			InitCompress(const void *inData, const int inLength);
	void			InitDecompress(void *outData, int outLength);
	void			WriteBits(int value, int numBits);
	int				ReadBits(int numBits);
	void			UnreadBits(int numBits);
	int				Compare(const byte *src1, int bitPtr1, const byte *src2, int bitPtr2, int maxBits) const;
};

/*
================
idCompressor_BitStream::Init
================
*/
void idCompressor_BitStream::Init(idFile *f, bool compress, int wordLength) {

	assert(wordLength >= 1 && wordLength <= 32);

	this->file = f;
	this->compress = compress;
	this->wordLength = wordLength;

	readTotalBytes = 0;
	readLength = 0;
	readByte = 0;
	readBit = 0;
	readData = NULL;

	writeTotalBytes = 0;
	writeLength = 0;
	writeByte = 0;
	writeBit = 0;
	writeData = NULL;
}

/*
================
idCompressor_BitStream::InitCompress
================
*/
ID_INLINE void idCompressor_BitStream::InitCompress(const void *inData, const int inLength) {

	readLength = inLength;
	readByte = 0;
	readBit = 0;
	readData = (const byte *)inData;

	if (!writeLength) {
		writeLength = sizeof(buffer);
		writeByte = 0;
		writeBit = 0;
		writeData = buffer;
	}
}

/*
================
idCompressor_BitStream::InitDecompress
================
*/
ID_INLINE void idCompressor_BitStream::InitDecompress(void *outData, int outLength) {

	if (!readLength) {
		readLength = file->Read(buffer, sizeof(buffer));
		readByte = 0;
		readBit = 0;
		readData = buffer;
	}

	writeLength = outLength;
	writeByte = 0;
	writeBit = 0;
	writeData = (byte *)outData;
}

/*
================
idCompressor_BitStream::WriteBits
================
*/
void idCompressor_BitStream::WriteBits(int value, int numBits) {
	int put, fraction;

	// Short circuit for writing single bytes at a time
	if (writeBit == 0 && numBits == 8 && writeByte < writeLength) {
		writeByte++;
		writeTotalBytes++;
		writeData[writeByte - 1] = value;
		return;
	}


	while (numBits) {
		if (writeBit == 0) {
			if (writeByte >= writeLength) {
				if (writeData == buffer) {
					file->Write(buffer, writeByte);
					writeByte = 0;
				}
				else {
					put = numBits;
					writeBit = put & 7;
					writeByte += (put >> 3) + (writeBit != 0);
					writeTotalBytes += (put >> 3) + (writeBit != 0);
					return;
				}
			}
			writeData[writeByte] = 0;
			writeByte++;
			writeTotalBytes++;
		}
		put = 8 - writeBit;
		if (put > numBits) {
			put = numBits;
		}
		fraction = value & ((1 << put) - 1);
		writeData[writeByte - 1] |= fraction << writeBit;
		numBits -= put;
		value >>= put;
		writeBit = (writeBit + put) & 7;
	}
}

/*
================
idCompressor_BitStream::ReadBits
================
*/
int idCompressor_BitStream::ReadBits(int numBits) {
	int value, valueBits, get, fraction;

	value = 0;
	valueBits = 0;

	// Short circuit for reading single bytes at a time
	if (readBit == 0 && numBits == 8 && readByte < readLength) {
		readByte++;
		readTotalBytes++;
		return readData[readByte - 1];
	}

	while (valueBits < numBits) {
		if (readBit == 0) {
			if (readByte >= readLength) {
				if (readData == buffer) {
					readLength = file->Read(buffer, sizeof(buffer));
					readByte = 0;
				}
				else {
					get = numBits - valueBits;
					readBit = get & 7;
					readByte += (get >> 3) + (readBit != 0);
					readTotalBytes += (get >> 3) + (readBit != 0);
					return value;
				}
			}
			readByte++;
			readTotalBytes++;
		}
		get = 8 - readBit;
		if (get > (numBits - valueBits)) {
			get = (numBits - valueBits);
		}
		fraction = readData[readByte - 1];
		fraction >>= readBit;
		fraction &= (1 << get) - 1;
		value |= fraction << valueBits;
		valueBits += get;
		readBit = (readBit + get) & 7;
	}

	return value;
}

/*
================
idCompressor_BitStream::UnreadBits
================
*/
void idCompressor_BitStream::UnreadBits(int numBits) {
	readByte -= (numBits >> 3);
	readTotalBytes -= (numBits >> 3);
	if (readBit == 0) {
		readBit = 8 - (numBits & 7);
	}
	else {
		readBit -= numBits & 7;
		if (readBit <= 0) {
			readByte--;
			readTotalBytes--;
			readBit = (readBit + 8) & 7;
		}
	}
	if (readByte < 0) {
		readByte = 0;
		readBit = 0;
	}
}

/*
================
idCompressor_BitStream::Compare
================
*/
int idCompressor_BitStream::Compare(const byte *src1, int bitPtr1, const byte *src2, int bitPtr2, int maxBits) const {
	int i;

	// If the two bit pointers are aligned then we can use a faster comparison
	if ((bitPtr1 & 7) == (bitPtr2 & 7) && maxBits > 16) {
		const byte *p1 = &src1[bitPtr1 >> 3];
		const byte *p2 = &src2[bitPtr2 >> 3];

		int bits = 0;

		int bitsRemain = maxBits;

		// Compare the first couple bits (if any)
		if (bitPtr1 & 7) {
			for (i = (bitPtr1 & 7); i < 8; i++, bits++) {
				if (((*p1 >> i) ^ (*p2 >> i)) & 1) {
					return bits;
				}
				bitsRemain--;
			}
			p1++;
			p2++;
		}

		int remain = bitsRemain >> 3;

		// Compare the middle bytes as ints
		while (remain >= 4 && (*(const int *)p1 == *(const int *)p2)) {
			p1 += 4;
			p2 += 4;
			remain -= 4;
			bits += 32;
		}

		// Compare the remaining bytes
		while (remain > 0 && (*p1 == *p2)) {
			p1++;
			p2++;
			remain--;
			bits += 8;
		}

		// Compare the last couple of bits (if any)
		int finalBits = 8;
		if (remain == 0) {
			finalBits = (bitsRemain & 7);
		}
		for (i = 0; i < finalBits; i++, bits++) {
			if (((*p1 >> i) ^ (*p2 >> i)) & 1) {
				return bits;
			}
		}

		assert(bits == maxBits);
		return bits;
	}
	else {
		for (i = 0; i < maxBits; i++) {
			if (((src1[bitPtr1 >> 3] >> (bitPtr1 & 7)) ^ (src2[bitPtr2 >> 3] >> (bitPtr2 & 7))) & 1) {
				break;
			}
			bitPtr1++;
			bitPtr2++;
		}
		return i;
	}
}

/*
================
idCompressor_BitStream::Write
================
*/
int idCompressor_BitStream::Write(const void *inData, int inLength) {
	int i;

	if (compress == false || inLength <= 0) {
		return 0;
	}

	InitCompress(inData, inLength);

	for (i = 0; i < inLength; i++) {
		WriteBits(ReadBits(8), 8);
	}
	return i;
}

/*
================
idCompressor_BitStream::FinishCompress
================
*/
void idCompressor_BitStream::FinishCompress(void) {
	if (compress == false) {
		return;
	}

	if (writeByte) {
		file->Write(buffer, writeByte);
	}
	writeLength = 0;
	writeByte = 0;
	writeBit = 0;
}

/*
================
idCompressor_BitStream::Read
================
*/
int idCompressor_BitStream::Read(void *outData, int outLength) {
	int i;

	if (compress == true || outLength <= 0) {
		return 0;
	}

	InitDecompress(outData, outLength);

	for (i = 0; i < outLength && readLength >= 0; i++) {
		WriteBits(ReadBits(8), 8);
	}
	return i;
}

/*
================
idCompressor_BitStream::GetCompressionRatio
================
*/
float idCompressor_BitStream::GetCompressionRatio(void) const {
	if (compress) {
		return (readTotalBytes - writeTotalBytes) * 100.0f / readTotalBytes;
	}
	else {
		return (writeTotalBytes - readTotalBytes) * 100.0f / writeTotalBytes;
	}
}


/*
=================================================================================

idCompressor_RunLength

The following algorithm implements run length compression with an arbitrary
word size.

=================================================================================
*/

class idCompressor_RunLength : public idCompressor_BitStream {
public:
	idCompressor_RunLength(void) {}

	void			Init(idFile *f, bool compress, int wordLength);

	int				Write(const void *inData, int inLength);
	int				Read(void *outData, int outLength);

private:
	int				runLengthCode;
};

/*
================
idCompressor_RunLength::Init
================
*/
void idCompressor_RunLength::Init(idFile *f, bool compress, int wordLength) {
	idCompressor_BitStream::Init(f, compress, wordLength);
	runLengthCode = (1 << wordLength) - 1;
}

/*
================
idCompressor_RunLength::Write
================
*/
int idCompressor_RunLength::Write(const void *inData, int inLength) {
	int bits, nextBits, count;

	if (compress == false || inLength <= 0) {
		return 0;
	}

	InitCompress(inData, inLength);

	while (readByte <= readLength) {
		count = 1;
		bits = ReadBits(wordLength);
		for (nextBits = ReadBits(wordLength); nextBits == bits; nextBits = ReadBits(wordLength)) {
			count++;
			if (count >= (1 << wordLength)) {
				if (count >= (1 << wordLength) + 3 || bits == runLengthCode) {
					break;
				}
			}
		}
		if (nextBits != bits) {
			UnreadBits(wordLength);
		}
		if (count > 3 || bits == runLengthCode) {
			WriteBits(runLengthCode, wordLength);
			WriteBits(bits, wordLength);
			if (bits != runLengthCode) {
				count -= 3;
			}
			WriteBits(count - 1, wordLength);
		}
		else {
			while (count--) {
				WriteBits(bits, wordLength);
			}
		}
	}

	return inLength;
}

/*
================
idCompressor_RunLength::Read
================
*/
int idCompressor_RunLength::Read(void *outData, int outLength) {
	int bits, count;

	if (compress == true || outLength <= 0) {
		return 0;
	}

	InitDecompress(outData, outLength);

	while (writeByte <= writeLength && readLength >= 0) {
		bits = ReadBits(wordLength);
		if (bits == runLengthCode) {
			bits = ReadBits(wordLength);
			count = ReadBits(wordLength) + 1;
			if (bits != runLengthCode) {
				count += 3;
			}
			while (count--) {
				WriteBits(bits, wordLength);
			}
		}
		else {
			WriteBits(bits, wordLength);
		}
	}

	return writeByte;
}


/*
=================================================================================

idCompressor_RunLength_ZeroBased

The following algorithm implements run length compression with an arbitrary
word size for data with a lot of zero bits.

=================================================================================
*/

class idCompressor_RunLength_ZeroBased : public idCompressor_BitStream {
public:
	idCompressor_RunLength_ZeroBased(void) {}

	int				Write(const void *inData, int inLength);
	int				Read(void *outData, int outLength);

private:
};

/*
================
idCompressor_RunLength_ZeroBased::Write
================
*/
int idCompressor_RunLength_ZeroBased::Write(const void *inData, int inLength) {
	int bits, count;

	if (compress == false || inLength <= 0) {
		return 0;
	}

	InitCompress(inData, inLength);

	while (readByte <= readLength) {
		count = 0;
		for (bits = ReadBits(wordLength); bits == 0 && count < (1 << wordLength); bits = ReadBits(wordLength)) {
			count++;
		}
		if (count) {
			WriteBits(0, wordLength);
			WriteBits(count - 1, wordLength);
			UnreadBits(wordLength);
		}
		else {
			WriteBits(bits, wordLength);
		}
	}

	return inLength;
}

/*
================
idCompressor_RunLength_ZeroBased::Read
================
*/
int idCompressor_RunLength_ZeroBased::Read(void *outData, int outLength) {
	int bits, count;

	if (compress == true || outLength <= 0) {
		return 0;
	}

	InitDecompress(outData, outLength);

	while (writeByte <= writeLength && readLength >= 0) {
		bits = ReadBits(wordLength);
		if (bits == 0) {
			count = ReadBits(wordLength) + 1;
			while (count-- > 0) {
				WriteBits(0, wordLength);
			}
		}
		else {
			WriteBits(bits, wordLength);
		}
	}

	return writeByte;
}


/*
================
idCompressor::AllocRunLength_ZeroBased
================
*/
idCompressor * idCompressor::AllocRunLength_ZeroBased(void) {
	return new idCompressor_RunLength_ZeroBased();
}
