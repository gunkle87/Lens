# LENS AGENT STARTUP MANIFEST

============================================================
PURPOSE
============================================================

This document is the startup manifest for any agent working on the
C:\DEV\Lens project.

> [!IMPORTANT]
> READ FIRST: [docs/RULES.md](docs/RULES.md) contains the strict behavioral and code provenance rules that MUST be followed when working in this repository.

It is intended to align planning, structure, and documentation work with
the discipline that has worked well in the `Strata` project.

This is not a casual note.
Treat it as an operating directive.

============================================================
MISSION CONTEXT
============================================================

The Lens project is building a native-rendered scene/UI system for:

- a game-like electronics/logic lab experience
- a structured path toward a 4-bit computer
- future expansions into micro-instruction sets, custom ISAs, CPU control,
  and memory structures

The renderer is NOT meant to be:

- SVG-based
- browser-driven
- a full freeform graphics editor for end users

The renderer IS meant to be:

- native
- data-driven
- layered correctly
- reusable across multiple application packages
- visually flexible for package authors
- safe and bounded for end users

============================================================
WORKING MODE FOR THIS PROJECT
============================================================

Current scope is:

- architecture
- document structure
- policy definition
- system boundary clarification

Current scope is NOT:

- implementation
- renderer coding
- parser coding
- asset compiler coding
- runtime/editor coding

Unless the user explicitly changes scope, stay in planning/spec mode.

============================================================
REFERENCE DISCIPLINE FROM STRATA
============================================================

Use `Strata` as a reference model for process quality and boundary hygiene.

The important habits to copy from `Strata` are:

1. Separate policy from implementation.
2. Separate layers aggressively before code grows.
3. Keep one source of truth for contracts.
4. Document boundaries before building across them.
5. Fix known issues immediately when safely in scope.
6. Keep audits report-only.
7. Prefer small, coherent slices over sprawling mixed work.
8. Keep packaging, discovery, and capability boundaries explicit.

Do NOT import `Strata` concepts blindly.
Import the discipline, not the domain model.

============================================================
PROCESS POLICIES TO COPY FROM STRATA
============================================================

These process policies should be followed here as well:

1. Audit agents are report-only.
   - no code edits
   - no doc edits
   - no staging
   - no cleanup beyond temporary test outputs required for verification

2. If an issue is known and safely fixable within scope, fix it now.
   - do not leave cleanup debt intentionally

3. Keep boundaries explicit.
   - renderer vs interaction
   - definitions vs instances
   - scene graph vs render graph
   - package authoring vs runtime user editing

4. Avoid hidden parallel truths.
   - one contract
   - one definition source
   - no duplicated semantics in multiple files without reason

5. Keep documents implementation-facing.
   - each doc should reduce ambiguity for later code
   - avoid vague inspirational text unless it clarifies a real decision

============================================================
PRIMARY ARCHITECTURAL RULES
============================================================

The following rules are already established and must be preserved:

1. One shared native scene/render core.
2. Separate reusable definitions from placed instances.
3. Separate scene graph from derived render graph.
4. Separate world scene, UI scene, and overlay scene.
5. Treat ports as semantic endpoints.
6. Treat traces as semantic scene objects, not decoration only.
7. Keep editor interaction outside the renderer core.
8. Support multiple app packages from one core instead of forking.

============================================================
CURRENT DOCUMENT SET IN LENS
============================================================

These are the current local source-of-truth docs and must be read first:

Core framework / structure:
- C:\DEV\Lens\docs\framework\pillar_1_rendering\RENDER_FORMAT_SPEC.md
- C:\DEV\Lens\docs\framework\pillar_1_rendering\SCENE_UI_ARCHITECTURE_PLAN.md
- C:\DEV\Lens\docs\framework\pillar_3_scene\SCENE_OBJECT_MODEL_SPEC.md
- C:\DEV\Lens\docs\framework\pillar_3_scene\SCENE_ID_AND_REFERENCE_POLICY.md
- C:\DEV\Lens\docs\framework\pillar_4_topology\TRACE_AND_PORT_MODEL.md
- C:\DEV\Lens\docs\framework\pillar_3_scene\LAYER_CLASS_POLICY.md
- C:\DEV\Lens\docs\framework\pillar_6_interaction\EDITOR_INTERACTION_BOUNDARY.md

Customization / policy:
- C:\DEV\Lens\docs\framework\cross_cutting\CUSTOM_TIER_POLICY.md
- C:\DEV\Lens\docs\framework\pillar_2_grammar\PARAMETERIZED_COMPONENT_MODEL.md
- C:\DEV\Lens\docs\framework\pillar_2_grammar\CUSTOM_MATERIAL_MODEL.md
- C:\DEV\Lens\docs\framework\pillar_3_scene\SCENE_SCRIPTING_BOUNDARY.md
- C:\DEV\Lens\docs\framework\pillar_2_grammar\VIEW_VARIANT_AND_2P5D_POLICY.md

Project/product context:
- C:\DEV\Lens\docs\cpu\CPU_BLOCK_SPEC.md
- C:\DEV\Lens\docs\planning\PROJECT_PHASES.md
- C:\DEV\Lens\docs\planning\180_PROJECTS.md

============================================================
STRATA REFERENCE DOCS
============================================================

These `Strata` docs are useful as process and architecture references:

- `C:\DEV\Strata\RULES.md`
- `C:\DEV\Strata\docs\STRATA_MULTI_AGENT_TRIAL_PROTOCOL.md`

Profile / policy discipline references:
- `C:\DEV\Strata\docs\profile_gating\README.md`
- `C:\DEV\Strata\docs\profile_gating\STRATA_PRODUCT_PROFILE_MODEL.md`
- `C:\DEV\Strata\docs\profile_gating\STRATA_FEATURE_GATING_POLICY.md`
- `C:\DEV\Strata\docs\profile_gating\STRATA_FORGE_PROFILE_ENFORCEMENT_PLAN.md`
- `C:\DEV\Strata\docs\profile_gating\STRATA_PROFILE_RESULT_CODE_POLICY.md`
- `C:\DEV\Strata\docs\profile_gating\STRATA_FORGE_EFFECTIVE_PROFILE_MODEL.md`
- `C:\DEV\Strata\docs\profile_gating\STRATA_FORGE_PROBE_VISIBILITY_MODEL.md`

These are NOT domain templates for `180in1`.
They are examples of:

- policy layering
- enforcement clarity
- capability gating discipline
- keeping documentation ahead of implementation

============================================================
CUSTOMIZATION BOUNDARY
============================================================

The renderer must support three explicit customization tiers:

1. engine-authoring only
2. package-authoring only
3. runtime-user-editable

This is non-negotiable.

Interpret it as follows:

Engine-authoring only:
- primitive shape vocabulary
- renderer internals
- bloom model
- highlight/shadow engine
- chamfer/rounding generation rules
- low-level material grammar
- 2.5D projection rules

Package-authoring only:
- component definitions
- material definitions
- themes
- trace style families
- toolbar skins/layouts
- view variants
- allowed user-editable property sets

Runtime-user-editable:
- safe whitelisted instance properties only
- examples:
  - trace color
  - trace bend radius
  - trace thickness within allowed range
  - placement/movement
  - label text
  - approved variant selection

End users must NOT get low-level renderer control.

============================================================
SCRIPTING POLICY
============================================================

Scripting is allowed only inside the declared boundary.

Scripts may:

- create and edit scene instances
- manipulate whitelisted properties
- drive tool workflows
- support package-authored behavior

Scripts may NOT:

- bypass renderer internals
- define arbitrary draw opcodes
- redefine primitive grammar at runtime
- bypass customization tiers

The renderer remains internally authoritative.

============================================================
2.5D / VIEW POLICY
============================================================

The system may support isometric/2.5D appearance.

It must NOT become a real 3D engine.

Policy:

- fixed-view authored variants only
- no free camera orbit requirement
- no dynamic lighting system requirement
- depth cues authored into components/materials
- 2D renderer remains the core

============================================================
DOCUMENT STRUCTURE GOAL
============================================================

The `180in1` documentation is large enough now that it should be organized.

Recommended target structure:

- `docs/`
- `docs/framework/`
- `docs/framework/pillar_1_rendering/`
- `docs/framework/pillar_2_grammar/`
- `docs/framework/pillar_3_scene/`
- `docs/framework/pillar_4_topology/`
- `docs/framework/pillar_5_binding/`
- `docs/framework/pillar_6_interaction/`
- `docs/framework/pillar_7_shell/`
- `docs/framework/cross_cutting/`
- `docs/cpu/`
- `docs/planning/`

Suggested placement:

- `docs/framework/pillar_1_rendering/RENDER_FORMAT_SPEC.md`
- `docs/framework/pillar_1_rendering/SCENE_UI_ARCHITECTURE_PLAN.md`
- `docs/framework/pillar_3_scene/SCENE_OBJECT_MODEL_SPEC.md`
- `docs/framework/pillar_3_scene/SCENE_ID_AND_REFERENCE_POLICY.md`
- `docs/framework/pillar_4_topology/TRACE_AND_PORT_MODEL.md`
- `docs/framework/pillar_2_grammar/CAPABILITY_SET.md`
- `docs/framework/pillar_2_grammar/CUSTOM_MATERIAL_MODEL.md`
- `docs/framework/pillar_2_grammar/PARAMETERIZED_COMPONENT_MODEL.md`
- `docs/framework/pillar_2_grammar/VIEW_VARIANT_AND_2P5D_POLICY.md`
- `docs/framework/pillar_3_scene/LAYER_CLASS_POLICY.md`
- `docs/framework/pillar_3_scene/RUNTIME_STRUCT_SPEC.md`
- `docs/framework/pillar_3_scene/SCENE_SCRIPTING_BOUNDARY.md`
- `docs/framework/pillar_5_binding/GENERIC_UI_FRAMEWORK_SPEC.md`
- `docs/framework/pillar_6_interaction/EDITOR_INTERACTION_BOUNDARY.md`
- `docs/framework/pillar_7_shell/PACKAGE_SHELL_MODEL.md`
- `docs/framework/cross_cutting/VALIDATION_PROTOCOL.md`
- `docs/framework/cross_cutting/CUSTOM_TIER_POLICY.md`
- `docs/cpu/CPU_BLOCK_SPEC.md`
- `docs/planning/PROJECT_PHASES.md`
- `docs/planning/180_PROJECTS.md`

The reorganization has been performed. This structure is now the source of truth for documentation.

============================================================
WHAT THE NEXT AGENT SHOULD DO
============================================================

If the next agent is planning/documenting this project, they should:

1. Read the current Lens docs listed above.
2. Read the `Strata` process/policy references listed above.
3. Preserve the current architectural boundaries.
4. Avoid implementation unless explicitly requested.
5. Prefer clarifying the document tree and policy gaps over adding endless
   speculative detail.
6. Identify contradictions or overlap between docs and reconcile them.
7. Propose next docs only if they reduce ambiguity for later implementation.

============================================================
WHAT THE NEXT AGENT SHOULD NOT DO
============================================================

Do NOT:

- start coding the renderer
- start coding the parser
- turn this into a browser/SVG pipeline
- collapse scene graph and render graph together
- give end users unrestricted rendering/material control
- treat traces as decorative-only paths
- treat UI scene and world scene as the same document by default
- add implementation detail beyond what the user is asking for

============================================================
RECOMMENDED NEXT DOCUMENT TASKS
============================================================

Only if the user asks to continue documentation, the best next steps are:

1. reorganize the document tree into `docs/...`
2. add `docs/INDEX.md`
3. add `docs/framework/README.md`
4. define a runtime struct model
5. define a package-shell model for game vs enterprise variants

Do not do all of these automatically.
Follow user scope.

============================================================
QUALITY BAR
============================================================

All planning output for this project should be:

- explicit
- layered
- non-redundant
- implementation-useful
- bounded to scope

The agent should aim to leave behind:

- less ambiguity
- better structure
- no accidental scope expansion

============================================================
VALIDATION EXPECTATION FOR 180IN1
============================================================

The next agent must include a validation/test protocol in their planning
output.

Do not assume `180in1` validation should look exactly like `Strata`.
The domains are different.

Use `Strata` as a reference for discipline and explicitness, not as a
direct test-template copy.

The expected validation model for `180in1` should include at least:

1. format validation
- parse success/failure
- schema enforcement
- unknown-key rejection policy

2. asset validation
- missing references
- duplicate IDs
- invalid component/material/shape references
- invalid port and anchor definitions

3. scene validation
- invalid object references
- invalid trace endpoints
- transform propagation correctness
- state/override resolution correctness
- layer ordering correctness

4. render validation
- deterministic render graph generation
- stable ordering
- expected primitive expansion
- bounds/hit-region sanity

5. interaction validation
- selection behavior expectations
- move/edit/update correctness
- trace attach/detach behavior
- overlay and preview separation from authored scene content

6. policy validation
- customization tier enforcement
- package-author-only edits vs runtime-user edits
- scripting boundary enforcement
- forbidden low-level edits rejected explicitly

The planning output should identify:

- which validations are parser-level
- which are scene-model-level
- which are render-level
- which are interaction-level
- which are policy-level

It should also identify:

- what can be validated structurally without rendering
- what requires render-graph comparison
- what requires interactive/tool behavior checks

The quality bar is:

- explicit validation categories
- clear proof strategy
- no hand-waving around correctness

============================================================
FINAL DIRECTIVE
============================================================

Use `Strata` as a reference for discipline, policy structure, and boundary
clarity.

Do NOT turn `180in1` into `Strata`.

Respect the domain differences:

- `Strata` is runtime/compiler/backend infrastructure
- `180in1` is scene/render/UI/package architecture

The transfer target is:

- process rigor
- policy layering
- clear boundaries
- controlled capability exposure

That is the model to follow.
