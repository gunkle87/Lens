# Lens Framework Documentation

This directory contains the specifications for the Lens framework, organized by architectural pillar.

## Pillar 1: Rendering Engine (Foundation)
- [Render Format Spec](pillar_1_rendering/RENDER_FORMAT_SPEC.md): The text-based visual format for materials, shapes, and components.
- [Architecture Plan](pillar_1_rendering/SCENE_UI_ARCHITECTURE_PLAN.md): The high-level layered architecture (Asset -> Scene -> Render).

## Pillar 2: Visual Grammar (Language)
- [Capability Set](pillar_2_grammar/CAPABILITY_SET.md): Initial vocabulary of shapes, components, and tools.
- [Materials Model](pillar_2_grammar/CUSTOM_MATERIAL_MODEL.md): Material definitions and visual recipes.
- [Parameterized Components](pillar_2_grammar/PARAMETERIZED_COMPONENT_MODEL.md): Component parameters and tier-based customization.
- [2.5D & View Policy](pillar_2_grammar/VIEW_VARIANT_AND_2P5D_POLICY.md): Isometric view variants and projection rules.

## Pillar 3: Scene & Identity Core (Workspace)
- [Object Model](pillar_3_scene/SCENE_OBJECT_MODEL_SPEC.md): The runtime scene node hierarchy and types.
- [Runtime Struct Spec](pillar_3_scene/RUNTIME_STRUCT_SPEC.md): Core data structures for SceneNodes, ComponentDefs, and Render output.
- [ID & Reference Policy](pillar_3_scene/SCENE_ID_AND_REFERENCE_POLICY.md): Rules for stable IDs and cross-object references.
- [Layer Class Policy](pillar_3_scene/LAYER_CLASS_POLICY.md): Semantic grouping and Z-ordering rules.
- [Scripting Boundary](pillar_3_scene/SCENE_SCRIPTING_BOUNDARY.md): Allowed vs. forbidden scripting operations.

## Pillar 4: Semantic Connectivity (Topology)
- [Trace & Port Model](pillar_4_topology/TRACE_AND_PORT_MODEL.md): Semantic connections and trace routing.

## Pillar 5: Data-Binding & Adaptation (The Bridge)
- [Generic UI Framework Spec](pillar_5_binding/GENERIC_UI_FRAMEWORK_SPEC.md): The universal toolkit for logic-interfacing (Regimes, Animations, Data-Binding).

## Pillar 6: Generic Interaction (Toolset)
- [Interaction Boundary](pillar_6_interaction/EDITOR_INTERACTION_BOUNDARY.md): The split between the renderer and the editor/shell.

## Pillar 7: Shell & Layout Substrate (Execution Frame)
- [Package-Shell Model](pillar_7_shell/PACKAGE_SHELL_MODEL.md): Gating tools, layouts, and interaction permissions per package.

## Cross-Cutting & Validation
- [Validation Protocol](cross_cutting/VALIDATION_PROTOCOL.md): Multi-gate verification for schema, assets, scene, and policy.
- [Customization Tiers](cross_cutting/CUSTOM_TIER_POLICY.md): Engine vs. Package vs. User permissions.
