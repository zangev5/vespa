// Copyright 2017 Yahoo Inc. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#include <vespa/fastos/fastos.h>
#include "lz4_input_decoder.h"
#include <lz4frame.h>

namespace vespalib {

void
Lz4InputDecoder::fail(const char *reason)
{
    _failed = true;
    _reason = reason;
    _eof = true;
}

void
Lz4InputDecoder::decode_more()
{
    assert((_pos == _used) && !_eof);
    Memory memory = _input.obtain();
    if (memory.size == 0) {
        auto res = LZ4F_freeDecompressionContext(_ctx);
        _ctx = nullptr;
        _eof = true;
        if (LZ4F_isError(res)) {
            fail(LZ4F_getErrorName(res));
        }
    } else {
        size_t input_size = memory.size;
        size_t output_size = _buffer.size();
        auto res = LZ4F_decompress(_ctx,
                                   &_buffer[0], &output_size,
                                   memory.data, &input_size,
                                   nullptr);
        if (LZ4F_isError(res)) {
            fail(LZ4F_getErrorName(res));
        } else if (input_size == 0) {
            fail("lz4 refusing to eat input");
        } else {
            assert(input_size <= memory.size);
            assert(output_size <= _buffer.size());
            _input.evict(input_size);
            _used = output_size;
            _pos = 0;
        }
    }
}

Lz4InputDecoder::Lz4InputDecoder(Input &input, size_t buffer_size)
    : _input(input),
      _buffer(buffer_size, 0),
      _used(0),
      _pos(0),
      _eof(false),
      _failed(false),
      _reason(),
      _ctx(nullptr)
{
    auto res = LZ4F_createDecompressionContext(&_ctx, LZ4F_VERSION);
    if (LZ4F_isError(res)) {
        fail(LZ4F_getErrorName(res));
    }
}

Lz4InputDecoder::~Lz4InputDecoder()
{
    LZ4F_freeDecompressionContext(_ctx);
}

Memory
Lz4InputDecoder::obtain()
{
    while ((_pos == _used) && !_eof) {
        decode_more();
    }
    return Memory(&_buffer[_pos], (_used - _pos));
}

Input &
Lz4InputDecoder::evict(size_t bytes)
{
    _pos += bytes;
    return *this;
}

} // namespace vespalib