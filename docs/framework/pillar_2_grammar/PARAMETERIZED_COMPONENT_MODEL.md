# PARAMETERIZED COMPONENT MODEL

============================================================
PURPOSE
============================================================

This document defines how component definitions expose controlled
parameters for package authors and runtime users.

The goal is flexibility without turning components into arbitrary scripts.

============================================================
PRIMARY RULE
============================================================

A component definition may expose parameters.

But every parameter must be:

- named
- typed
- range-checked
- tier-classified
- mapped to explicit component behavior

============================================================
PARAMETER CATEGORIES
============================================================

Recommended categories:

1. geometry parameters
2. material/style parameters
3. variant parameters
4. label/content parameters
5. state-linked parameters

============================================================
GEOMETRY PARAMETERS
============================================================

Examples:

- width
- height
- body radius
- chamfer amount
- extrusion depth hint for 2.5D
- trace depth
- stroke width

Policy:

- low-level geometry generation model remains engine-owned
- package authors may expose controlled geometry knobs
- runtime users should only get narrow, safe geometry edits

Example:

- user may change trace bend radius
- user may not redefine the gate path grammar

============================================================
MATERIAL / STYLE PARAMETERS
============================================================

Examples:

- material preset
- primary color
- accent color
- glow level
- stroke intensity
- surface finish

Policy:

- custom material engine remains engine/package authored
- runtime user may choose allowed material presets or safe color channels

============================================================
VARIANT PARAMETERS
============================================================

Variants switch between pre-authored structures.

Examples:

- flat vs isometric
- compact vs large component body
- beginner vs advanced skin
- enterprise vs playful style pack

Variants should be preferred over freeform user access when visual
consistency matters.

============================================================
LABEL / CONTENT PARAMETERS
============================================================

Examples:

- label text
- caption visibility
- icon selection from approved set
- value display mode

These are often safe runtime-user-editable properties.

============================================================
STATE-LINKED PARAMETERS
============================================================

Some parameters may react to state without being directly edited by users.

Examples:

- ON state glow
- ERROR state border color
- HOVER elevation
- CONFLICT state highlight

These should be authored declaratively, not improvised at runtime.

============================================================
PARAMETER DECLARATION MODEL
============================================================

Each component parameter should declare:

- name
- type
- default
- allowed range or enum values
- customization tier
- whether it is instance-overridable
- whether it is variant-controlled

============================================================
INSTANCE OVERRIDES
============================================================

Component instances may store parameter overrides.

Examples:

- one trace uses a different color
- one label uses a custom caption
- one gate uses a larger size within allowed range

Overrides must not bypass tier policy.

============================================================
RECOMMENDED V1 PARAMETER SET
============================================================

Support early:

- width
- height
- color/material preset
- label text
- trace thickness
- trace depth (simulated)
- view variant

Defer:

- arbitrary expressions
- procedural geometry scripts
- unrestricted user-authored path data

============================================================
BOTTOM LINE
============================================================

Parameters are the safe control surface for components.

Use them to expose flexibility without giving away renderer internals.
