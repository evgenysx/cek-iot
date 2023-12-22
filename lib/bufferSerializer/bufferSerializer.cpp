#include "bufferSerializer.h"

constexpr auto szSize_t = sizeof(size_t);
const size_t MEM_BLOCK = 512;


byte* BufferSerializer::getBytes()
{
    return data;
}

size_t BufferSerializer::getSize()
{
    return offset;
}

bool BufferSerializer::put(size_t value){
  if (!chkAddMemory(szSize_t))
    return false;
  memcpy(data + offset, &value, szSize_t);
  offset += szSize_t;
  return true;
}

bool BufferSerializer::put(String& value){
  const auto ln = value.length();
  if (!chkAddMemory(ln + szSize_t))
    return false;
  memcpy(data + offset, &ln, szSize_t);
  offset += szSize_t;
  memcpy(data + offset, value.c_str(), ln);
  offset += ln;
  return true;
}

String BufferSerializer::readString()
{
    uint len = readUint();
    String str = String(data + offset, len);
    offset += len;
    return str;
}

size_t BufferSerializer::readUint()
{
    uint len;
    memcpy(&len, data + offset, szSize_t);
    offset += szSize_t;
    return len;
}

bool BufferSerializer::chkAddMemory(size_t szData)
{
  if (szData == 0) return false;
  if (szData > 1024*1024) return false;

  if (capicity - offset < szData){
    if (szData > MEM_BLOCK)
      capicity += szData;
    else
      capicity += MEM_BLOCK;
    
    // no enouth memory
    data = (byte*)realloc(data, capicity);
  }

  return true;
}

BufferSerializer::BufferSerializer(){
  capicity = MEM_BLOCK;
  setOffset(0);
  data = (byte*)malloc(capicity);
}

BufferSerializer::BufferSerializer(size_t capicity){
  this->capicity = capicity;
  setOffset(0);
  data = (byte*)malloc(capicity);
}

BufferSerializer::~BufferSerializer()
{
    if (data)
        free(data);
}

void BufferSerializer::setOffset(size_t offset)
{
  this->offset = offset;
}
