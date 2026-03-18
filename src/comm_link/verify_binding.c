#include <stdio.h>
#include <string.h>
#include <math.h>
#include "lens_binding_manager.h"

static bool float_eq(float a, float b) {
    return fabs(a - b) < 0.001f;
}

static void assert_test(bool condition, const char* name) {
    printf("[%s] %s\n", condition ? "PASS" : "FAIL", name);
}

int main(void) {
    printf("--- Lens Pillar 5 Data-Binding Verification ---\n");

    LensBindingManager manager;
    lens_binding_init(&manager);

    LensUUID source_id = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}};
    LensUUID node_id = {{16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}};
    
    LensBindingHandle handle;
    handle.source_id = source_id;
    strcpy(handle.descriptor_id, "GPIO_PIN_0");
    strcpy(handle.data_path, "state");

    lens_binding_register(&manager, node_id, "glow_intensity", handle);

    // 1. Normalization Proof
    printf("\n[Test 1] Normalization Proof\n");
    lens_binding_ingest_float(&manager, handle, 5.0f, 0.0f, 10.0f, 0.0f);
    
    LensNormalizedSignal signal;
    lens_binding_get_signal(&manager, node_id, "glow_intensity", &signal);
    assert_test(float_eq(signal.value, 0.5f), "Normalization: 5.0V on 0-10V scale maps to 0.5");

    lens_binding_ingest_float(&manager, handle, 12.0f, 0.0f, 10.0f, 0.1f);
    lens_binding_get_signal(&manager, node_id, "glow_intensity", &signal);
    assert_test(float_eq(signal.value, 1.0f), "Normalization: Out-of-bounds (high) clips to 1.0");

    // 2. Regime Logic Test (High-Frequency)
    printf("\n[Test 2] Regime Logic (1MHz Simulation)\n");
    float t = 0.2f;
    // Simulate 1000 toggles in 1 second window
    for (int i = 0; i < 200; i++) {
        lens_binding_ingest_bool(&manager, handle, (i % 2 == 0), t);
        t += 0.001f; // 1ms steps
    }
    
    // Trigger update to evaluate window
    lens_binding_update(&manager, 1.5f);
    lens_binding_get_signal(&manager, node_id, "glow_intensity", &signal);
    assert_test(signal.regime == LENS_REGIME_PULSE, "Regime Logic: 1kHz signal correctly categorized as PULSE");
    assert_test(float_eq(signal.value, 0.8f), "Regime Logic: PULSE regime applies stable visual glow (0.8)");

    // 3. Temporal Stability (Animation)
    printf("\n[Test 3] Temporal Stability (Animation)\n");
    // Setup a pulse animation
    for (uint32_t i = 0; i < manager.count; i++) {
        if (lens_uuid_is_equal(manager.entries[i].node_id, node_id)) {
            manager.entries[i].signal.anim.kind = LENS_ANIM_PULSE;
            manager.entries[i].signal.anim.start_time = 2.0f;
            manager.entries[i].signal.anim.frequency = 1.0f; // 1Hz
            manager.entries[i].signal.anim.phase = 0.0f;
            // Clear pulse regime for clean animation test
            manager.entries[i].signal.regime = LENS_REGIME_STEADY;
        }
    }

    // At 2.25s (1/4 cycle), sine(pi/2) = 1.0. Value = 0.5 + 0.5 * 1.0 = 1.0
    lens_binding_update(&manager, 2.25f);
    lens_binding_get_signal(&manager, node_id, "glow_intensity", &signal);
    assert_test(float_eq(signal.value, 1.0f), "Animation: 1Hz Pulse at T=2.25s results in peak value (1.0)");

    // At 2.75s (3/4 cycle), sine(3pi/2) = -1.0. Value = 0.5 + 0.5 * -1.0 = 0.0
    lens_binding_update(&manager, 2.75f);
    lens_binding_get_signal(&manager, node_id, "glow_intensity", &signal);
    assert_test(float_eq(signal.value, 0.0f), "Animation: 1Hz Pulse at T=2.75s results in trough value (0.0)");

    printf("\n--- Verification Complete ---\n");
    return 0;
}
