#ifndef LENS_RENDER_GRAPH_H
#define LENS_RENDER_GRAPH_H

#include "lens_render_types.h"

// Lifecycle
void lens_command_buffer_init(LensCommandBuffer* buffer, uint32_t capacity);
void lens_command_buffer_destroy(LensCommandBuffer* buffer);

// Flattener pass: traverses the hierarchy starting at root, 
// composes transformations and emits flat sequence to out_buffer.
// Returns true on success, false if capacity is breached.
bool lens_flatten_scene(LensSceneNode* root, LensCommandBuffer* out_buffer);

#endif // LENS_RENDER_GRAPH_H
