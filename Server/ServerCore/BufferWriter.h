#pragma once

/*----------------
	BufferWriter
-----------------*/

class BufferWriter
{
public:
	BufferWriter();
	BufferWriter(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferWriter();

	BYTE*			Buffer() { return _buffer; }
	uint32			Size() { return _size; }
	uint32			WriteSize() { return _pos; }
	uint32			FreeSize() { return _size - _pos; }

	template<typename T>
	bool			Write(T* src) { return Write(src, sizeof(T)); }
	bool			Write(void* src, uint32 len);

	template<typename T>
	T*				Reserve(); // Write 작업 예약(헤더 Write에 사용)

	template<typename T>
	BufferWriter&	operator<<(T&& src); // 보편 참조를 이용하여 src 데이터 Write

private:
	BYTE*			_buffer = nullptr;
	uint32			_size = 0; // 버퍼 크기
	uint32			_pos = 0; // 현재 쓰고있는 위치
};

template<typename T>
T* BufferWriter::Reserve()
{
	if (FreeSize() < sizeof(T))
		return nullptr;

	T* ret = reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);
	return ret;
}

template<typename T>
BufferWriter& BufferWriter::operator<<(T&& src)
{
	using DataType = std::remove_reference_t<T>;
	*reinterpret_cast<DataType*>(&_buffer[_pos]) = std::forward<DataType>(src);
	_pos += sizeof(T);
	return *this;
}