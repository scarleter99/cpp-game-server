#pragma once

/*----------------
	BufferReader
-----------------*/

class BufferReader
{
public:
	BufferReader();
	BufferReader(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferReader();

	BYTE* Buffer() { return _buffer; }
	uint32			Size() { return _size; }
	uint32			ReadSize() { return _pos; }
	uint32			FreeSize() { return _size - _pos; }

	template<typename T>
	bool			Peek(T* dest) { return Peek(dest, sizeof(T)); }
	bool			Peek(void* dest, uint32 len); // _pos를 조작하지 않고 Peek

	template<typename T>
	bool			Read(T* dest) { return Read(dest, sizeof(T)); }
	bool			Read(void* dest, uint32 len); // _pos를 조작하여 Read

	template<typename T>
	BufferReader& operator>>(OUT T& dest); // dest에 Read한 데이터 복사

private:
	BYTE* _buffer = nullptr;
	uint32			_size = 0; // 버퍼 크기
	uint32			_pos = 0; // 현재 읽고있는 위치
};

template<typename T>
inline BufferReader& BufferReader::operator>>(OUT T& dest)
{
	dest = *reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);
	return *this;
}
