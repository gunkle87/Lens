#ifndef LENS_TOPOLOGY_MANAGER_H
#define LENS_TOPOLOGY_MANAGER_H

#include "lens_topology_types.h"
#include "../scene_core/lens_scene_types.h"

// Establish a connection between two semantic endpoints. 
// Uses SceneDocument to find or append a LENS_NODE_TRACE.
LensSceneNode* lens_topology_route_trace(
    LensSceneDocument* doc, 
    LensUUID source_node, const char* source_port,
    LensUUID dest_node, const char* dest_port,
    const char* style_id
);

// Append a vertex to an existing trace
bool lens_topology_add_vertex(LensSceneNode* trace_node, float x, float y);

// Pass: Detect "Dangling Wires" (traces where one or both endpoints no longer exist)
// Returns the number of dangling traces found
uint32_t lens_topology_find_dangling_traces(LensSceneDocument* doc, LensUUID* out_dangling_ids, uint32_t max_out);

// Query: Find all traces connected to a specific node (and optionally a specific port if port_name != NULL)
uint32_t lens_topology_get_connected_traces(LensSceneDocument* doc, LensUUID target_node, const char* port_name, LensSceneNode** out_traces, uint32_t max_out);

#endif // LENS_TOPOLOGY_MANAGER_H
