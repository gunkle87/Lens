# CUSTOMIZATION TIER POLICY

============================================================
PURPOSE
============================================================

This document defines the three customization tiers for the LENS
renderer, scene system, and UI/tool environment.

This policy exists to prevent two failures:

1. giving end users too much low-level control
2. making package authors too constrained to build distinct products

============================================================
PRIMARY RULE
============================================================

Not all editable things are editable by the same audience.

Use three explicit tiers:

1. engine-authoring only
2. package-authoring only
3. runtime-user-editable

Every exposed property, asset family, and script operation should be
assigned to one of these tiers.

============================================================
TIER 1: ENGINE-AUTHORING ONLY
============================================================

This tier is for renderer/core-system authors.

These things are locked before ship:

- primitive shape vocabulary
- path grammar
- internal renderer behavior
- bloom model
- highlight/shadow model
- chamfer/rounding generation rules
- 2.5D projection rules
- render pass structure
- low-level material grammar
- scripting VM/runtime internals

End users must never edit these directly.

Package authors should also avoid editing these unless they are acting as
core engine maintainers.

============================================================
TIER 2: PACKAGE-AUTHORING ONLY
============================================================

This tier is for building products/packages from the shared engine.

Package authors may define:

- component definitions
- material definitions
- style families
- trace style families
- theme packs
- view variants
- toolbar layouts
- scene templates
- allowed user-editable property sets
- package-specific scripts and tools

This tier allows:

- a playful game-like shell
- a professional enterprise shell
- a guided educational shell

without changing the renderer core.

============================================================
TIER 3: RUNTIME-USER-EDITABLE
============================================================

This tier is for safe shipped-tool customization.

Users may edit only whitelisted instance-level or scene-level properties.

Examples:

- trace color
- trace bend radius
- trace thickness within allowed range
- component label text
- allowed visual preset selection
- placement and movement
- grouping and layout

Users must not be able to redefine:

- primitive geometry grammar
- bloom implementation
- chamfer generation rules
- low-level material engine
- render backend behavior

============================================================
PROPERTY CLASSIFICATION RULE
============================================================

Every editable property should declare:

- property name
- value type
- allowed range or enum set
- default value
- owning tier
- optional UI hint

Example:

- `trace.color`
  - tier: runtime-user-editable

- `trace.bend_radius`
  - tier: runtime-user-editable
  - range-limited

- `trace.bloom_width`
  - tier: package-authoring only

- `component.corner_chamfer`
  - tier: package-authoring only

- `renderer.bloom_kernel`
  - tier: engine-authoring only

============================================================
SCRIPTING POLICY
============================================================

Scripts must respect customization tiers.

That means:

- engine-authoring scripts may define low-level structures
- package-authoring scripts may build components/themes/tools
- runtime-user scripts, if allowed at all, may only edit whitelisted
  runtime-safe properties and scene actions

No script tier should bypass the property gate.

============================================================
PACKAGE DIFFERENTIATION
============================================================

This policy is what allows one core to produce multiple products:

- game package
- guided lab package
- enterprise tool package
- future advanced editor package

The difference comes from:

- tier-2 assets and tools
- tier-3 permissions

not from renderer forks.

============================================================
VALIDATION RULE
============================================================

The system should reject edits that cross tiers.

Examples:

- runtime user tries to edit bloom kernel -> reject
- runtime user tries to redefine gate silhouette grammar -> reject
- runtime user changes trace color -> allow if package permits

============================================================
BOTTOM LINE
============================================================

Tiering is the control system that lets the engine stay powerful while
the shipped product stays disciplined.
