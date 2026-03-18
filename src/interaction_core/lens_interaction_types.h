#ifndef LENS_INTERACTION_TYPES_H
#define LENS_INTERACTION_TYPES_H

#include "../scene_core/lens_scene_types.h"
#include <stdint.h>
#include <stdbool.h>

// Event types normalized for the interaction brain
typedef enum {
    LENS_EVENT_POINTER_DOWN,
    LENS_EVENT_POINTER_UP,
    LENS_EVENT_POINTER_MOVE
} LensInteractionEventType;

typedef struct {
    LensInteractionEventType type;
    float screen_x;
    float screen_y;
} LensInteractionEvent;

typedef struct {
    bool hit;
    LensUUID node_id;
    float world_x;
    float world_y;
} LensHitResult;

#define LENS_MAX_SELECTION 256

// DOD: Dense array for selection state mapping (Identity table)
typedef struct {
    LensUUID selected_nodes[LENS_MAX_SELECTION];
    uint32_t count;
} LensSelectionState;

// Screen space mapping context
typedef struct {
    float x;
    float y;
    float width;
    float height;
    float pan_x;  // world offset
    float pan_y;
    float zoom;
} LensViewport;

#endif // LENS_INTERACTION_TYPES_H
