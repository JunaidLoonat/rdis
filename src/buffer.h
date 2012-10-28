#ifndef buffer_HEADER
#define buffer_HEADER

#include <inttypes.h>
#include <stdlib.h>

#include "object.h"
#include "serialize.h"

struct _buffer {
    const struct _object * object;
    uint8_t * bytes;
    size_t    size;
};

struct _buffer * buffer_create      (uint8_t * bytes, size_t size);
void             buffer_delete      (struct _buffer * buffer);
struct _buffer * buffer_copy        (struct _buffer * buffer);
json_t *         buffer_serialize   (struct _buffer * buffer);
struct _buffer * buffer_deserialize (json_t * json);

#endif