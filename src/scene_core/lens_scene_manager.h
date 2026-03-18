#ifndef LENS_SCENE_MANAGER_H
#define LENS_SCENE_MANAGER_H

#include "lens_scene_types.h"

// Lifecycle
void lens_scene_document_init(LensSceneDocument* doc);

// Abstract UUID Generator
LensUUID lens_generate_uuid(void);

// Table manipulations
LensSceneNode* lens_scene_create_node(LensSceneDocument* doc, LensNodeKind kind, LensUUID parent);
LensSceneNode* lens_scene_get_node(LensSceneDocument* doc, LensUUID id);

// Transform propagation
LensMatrix3x3 lens_scene_get_world_transform(LensSceneDocument* doc, LensUUID id);

// Property Overrides (Instance vs Definition pattern)
void lens_scene_set_override_float(LensSceneNode* node, const char* param_name, float value);
void lens_scene_set_override_string(LensSceneNode* node, const char* param_name, const char* value);

bool lens_scene_get_override_float(LensSceneNode* node, const char* param_name, float* out_value);
bool lens_scene_get_override_string(LensSceneNode* node, const char* param_name, char* out_buf, int max_len);

#endif // LENS_SCENE_MANAGER_H
