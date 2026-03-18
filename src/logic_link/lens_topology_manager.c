#include "lens_topology_manager.h"
#include "../scene_core/lens_scene_manager.h"
#include <string.h>

LensSceneNode* lens_topology_route_trace(
    LensSceneDocument* doc, 
    LensUUID source_node, const char* source_port,
    LensUUID dest_node, const char* dest_port,
    const char* style_id
) {
    if (!doc || !source_port || !dest_port || lens_uuid_is_null(source_node) || lens_uuid_is_null(dest_node)) {
        return NULL;
    }
    
    LensSceneNode* trace = lens_scene_create_node(doc, LENS_NODE_TRACE, lens_uuid_null());
    if (!trace) return NULL;
    
    LensTraceData* tdata = &trace->data.trace;
    tdata->source_node_id = source_node;
    strncpy(tdata->source_port, source_port, 31);
    tdata->source_port[31] = '\0';
    
    tdata->dest_node_id = dest_node;
    strncpy(tdata->dest_port, dest_port, 31);
    tdata->dest_port[31] = '\0';
    
    if (style_id) {
        strncpy(tdata->style_id, style_id, 63);
        tdata->style_id[63] = '\0';
    } else {
        tdata->style_id[0] = '\0';
    }
    
    tdata->vertex_count = 0;
    
    return trace;
}

bool lens_topology_add_vertex(LensSceneNode* trace_node, float x, float y) {
    if (!trace_node || trace_node->kind != LENS_NODE_TRACE) return false;
    LensTraceData* tdata = &trace_node->data.trace;
    if (tdata->vertex_count >= LENS_MAX_TRACE_VERTICES) return false;
    
    tdata->vertices[tdata->vertex_count].x = x;
    tdata->vertices[tdata->vertex_count].y = y;
    tdata->vertex_count++;
    return true;
}

uint32_t lens_topology_find_dangling_traces(LensSceneDocument* doc, LensUUID* out_dangling_ids, uint32_t max_out) {
    if (!doc || !out_dangling_ids) return 0;
    
    uint32_t found = 0;
    for (uint32_t i = 0; i < doc->node_count; i++) {
        LensSceneNode* node = &doc->nodes[i];
        if (node->kind == LENS_NODE_TRACE) {
            LensTraceData* tdata = &node->data.trace;
            
            bool source_missing = (lens_scene_get_node(doc, tdata->source_node_id) == NULL);
            bool dest_missing = (lens_scene_get_node(doc, tdata->dest_node_id) == NULL);
            
            if (source_missing || dest_missing) {
                if (found < max_out) {
                    out_dangling_ids[found] = node->id;
                    found++;
                }
            }
        }
    }
    return found;
}

uint32_t lens_topology_get_connected_traces(LensSceneDocument* doc, LensUUID target_node, const char* port_name, LensSceneNode** out_traces, uint32_t max_out) {
    if (!doc || !out_traces || lens_uuid_is_null(target_node)) return 0;
    
    uint32_t found = 0;
    for (uint32_t i = 0; i < doc->node_count; i++) {
        LensSceneNode* node = &doc->nodes[i];
        if (node->kind == LENS_NODE_TRACE) {
            LensTraceData* tdata = &node->data.trace;
            
            bool matches_source = lens_uuid_is_equal(tdata->source_node_id, target_node) && 
                                  (!port_name || strcmp(tdata->source_port, port_name) == 0);
                                  
            bool matches_dest = lens_uuid_is_equal(tdata->dest_node_id, target_node) && 
                                  (!port_name || strcmp(tdata->dest_port, port_name) == 0);
                                  
            if (matches_source || matches_dest) {
                if (found < max_out) {
                    out_traces[found] = node;
                    found++;
                }
            }
        }
    }
    return found;
}
