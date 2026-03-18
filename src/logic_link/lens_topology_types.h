#ifndef LENS_TOPOLOGY_TYPES_H
#define LENS_TOPOLOGY_TYPES_H

#include "../scene_core/lens_uuid.h"

// Direction of port data flow
typedef enum {
    LENS_PORT_INPUT,
    LENS_PORT_OUTPUT,
    LENS_PORT_BIDIR
} LensPortDirection;

// A definition of a connection point on a component
typedef struct {
    char name[32];
    LensPortDirection direction;
    float anchor_x; // Relative X position on the component
    float anchor_y; // Relative Y position on the component
} LensPortDef;

#endif // LENS_TOPOLOGY_TYPES_H
