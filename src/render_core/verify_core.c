#include <stdio.h>
#include <string.h>
#include <math.h>
#include "lens_math.h"
#include "lens_render_types.h"
#include "lens_render_graph.h"

static bool float_eq(float a, float b) {
    return fabs(a - b) < 0.0001f;
}

static void assert_eq(bool condition, const char* msg) {
    if (!condition) {
        printf("FAIL: %s\n", msg);
    } else {
        printf("PASS: %s\n", msg);
    }
}

int main(void) {
    printf("--- Lens Core Verification ---\n");

    // 1. Math Verification
    LensMatrix3x3 id = lens_matrix_identity();
    LensMatrix3x3 t1 = lens_matrix_translation(10.0f, 20.0f);
    LensMatrix3x3 t2 = lens_matrix_translation(5.0f, 5.0f);
    LensMatrix3x3 tc = lens_matrix_multiply(t1, t2); 

    assert_eq(float_eq(tc.m[2], 15.0f) && float_eq(tc.m[5], 25.0f), "Nested transform composition (Translation)");

    // 2. Flattener Data Flow Verification
    LensSceneNode root;
    memset(&root, 0, sizeof(root));
    root.id = 1;
    root.local_transform = lens_matrix_translation(100.0f, 100.0f);
    root.local_opacity = 0.5f;
    root.primitive.type = LENS_PRIMITIVE_NONE;

    LensSceneNode child;
    memset(&child, 0, sizeof(child));
    child.id = 2;
    child.local_transform = lens_matrix_translation(50.0f, 50.0f);
    child.local_opacity = 0.5f;
    
    LensMaterial mat = {
        .base_color = 0xFF00FF,
        .opacity = 1.0f
    };
    child.material = mat;
    child.primitive.type = LENS_PRIMITIVE_RECT;
    child.primitive.data.rect = (LensRectPrimitive){0.0f, 0.0f, 10.0f, 10.0f, 0.0f};

    root.child_count = 1;
    root.children[0] = &child;

    // Buffer preparation (Pure SOA)
    LensCommandBuffer buffer;
    lens_command_buffer_init(&buffer, 10);

    bool success = lens_flatten_scene(&root, &buffer);
    assert_eq(success, "Scene flattening pass succeeded");

    // 3. Command Buffer Correctness
    assert_eq(buffer.count == 1, "Flattener produces dense correct command stream item count");
    
    if (buffer.count > 0) {
        assert_eq(buffer.ids[0] == 2, "Flattened ID matches correct primitive inner node");
        assert_eq(buffer.types[0] == LENS_PRIMITIVE_RECT, "Flattened type preserved across pass");
        
        // Ensure cascading global opacity incorporates root factor (0.5 * 0.5)
        assert_eq(float_eq(buffer.global_opacities[0], 0.25f), "Global opacity propagates down tree");
        assert_eq(float_eq(buffer.resolved_materials[0].opacity, 0.25f), "Propagated Material Opacity matches global accumulation");

        // Assure position properly applies transformations from root down
        assert_eq(float_eq(buffer.global_transforms[0].m[2], 150.0f) && float_eq(buffer.global_transforms[0].m[5], 150.0f), "Transform accumulates nested hierarchy translation properly");
    }

    lens_command_buffer_destroy(&buffer);
    
    printf("--- Verification Complete ---\n");
    return 0;
}
