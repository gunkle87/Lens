#ifndef LENS_UUID_H
#define LENS_UUID_H

#include <stdint.h>
#include <stdbool.h>

// 16-byte UUID representation allowing stable object identities
typedef struct {
    uint8_t bytes[16];
} LensUUID;

static inline bool lens_uuid_is_equal(LensUUID a, LensUUID b) {
    for (int i = 0; i < 16; i++) {
        if (a.bytes[i] != b.bytes[i]) return false;
    }
    return true;
}

static inline bool lens_uuid_is_null(LensUUID a) {
    for (int i = 0; i < 16; i++) {
        if (a.bytes[i] != 0) return false;
    }
    return true;
}

static inline LensUUID lens_uuid_null(void) {
    LensUUID id = {0};
    return id;
}

#endif // LENS_UUID_H
