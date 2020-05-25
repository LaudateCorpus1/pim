// Copyright 2009-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "exception.h"
#include "tza.h"

namespace oidn {

  // Checks for buffer overrun
  __forceinline void checkBounds(char* ptr, char* end, size_t size)
  {
    if (end - ptr < (ptrdiff_t)size)
      throw Exception(Error::InvalidOperation, "corrupted tensor format");
  }

  // Reads a value from a buffer (with bounds checking) and advances the pointer
  template<typename T>
  __forceinline T read(char*& ptr, char* end)
  {
    checkBounds(ptr, end, sizeof(T));
    T value;
    memcpy(&value, ptr, sizeof(T));
    ptr += sizeof(T);
    return value;
  }

  std::map<std::string, Tensor> parseTZA(void* buffer, size_t size)
  {
    char* input = (char*)buffer;
    char* const bufferEnd = input + size;

    // Parse the magic value
    const int magic = read<uint16_t>(input, bufferEnd);
    if (magic != 0x41D7)
      throw Exception(Error::InvalidOperation, "invalid tensor format");

    // Parse the version
    const int majorVersion = read<uint8_t>(input, bufferEnd);
    const int minorVersion = read<uint8_t>(input, bufferEnd);
    UNUSED(minorVersion);
    if (majorVersion != 2)
      throw Exception(Error::InvalidOperation, "unsupported tensor format version");

    // Parse the table offset and jump to the table
    const uint64_t tableOffset = read<uint64_t>(input, bufferEnd);
    input = (char*)buffer + tableOffset;

    // Parse the number of tensors
    const size_t numTensors = read<uint32_t>(input, bufferEnd);

    // Parse the tensors
    std::map<std::string, Tensor> tensorMap;
    for (size_t i = 0; i < numTensors; ++i)
    {
      Tensor tensor;

      // Parse the name
      const size_t nameLen = read<uint16_t>(input, bufferEnd);
      checkBounds(input, bufferEnd, nameLen);
      std::string name(input, nameLen);
      input += nameLen;

      // Parse the number of dimensions
      const int ndims = read<uint8_t>(input, bufferEnd);

      // Parse the shape of the tensor
      tensor.dims.resize(ndims);
      for (int j = 0; j < ndims; ++j)
        tensor.dims[j] = read<uint32_t>(input, bufferEnd);

      // Parse the layout of the tensor
      checkBounds(input, bufferEnd, ndims);
      tensor.layout = std::string(input, input + ndims);
      input += ndims;

      // Parse the data type of the tensor
      const char type = read<char>(input, bufferEnd);
      if (type != 'f') // only float32 is supported
        throw Exception(Error::InvalidOperation, "unsupported tensor data type");

      // Parse the offset to the tensor data
      const uint64_t tensorOffset = read<uint64_t>(input, bufferEnd);
      char* tensorData = (char*)buffer + tensorOffset;
      checkBounds(tensorData, bufferEnd, tensor.byteSize());
      tensor.data = (float*)tensorData;

      // Add the tensor to the map
      tensorMap.emplace(name, std::move(tensor));
    }

    return tensorMap;
  }

} // namespace oidn
