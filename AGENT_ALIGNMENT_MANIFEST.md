# Lens Agent Alignment Manifest

This document is the canonical alignment file for planning work in `C:\DEV\Lens`.

It exists to prevent drift.
The project is easy to misread as:
- a renderer project
- a scene editor project
- a desktop shell project
- a graphics asset pack

It is none of those by itself.

Lens is a **layered UI framework substrate** for engine-facing applications.
Its job is to make many UIs possible, not to be one UI.

## Mission

Lens must provide the structure, language, and runtime model needed to define and execute interactive UI through declarative data and bounded runtime logic.

It must support:
- a game-like logic lab interface
- a professional enterprise tool interface
- future engine-facing applications built on the same UI core

without forking the framework.

## What Lens Is

Lens is:
- a native rendering substrate
- a declarative visual description system
- a scene and workspace composition framework
- a UI layout framework
- a generic interaction framework
- a data-binding and engine-communication layer
- an application-shell integration substrate

Lens is not:
- a one-off renderer
- a finished scene editor
- a bundle of concrete visual assets
- a game UI
- an enterprise UI
- a general-purpose desktop shell

## Hard Constraints

These are mandatory.

1. Lens must be layered.
- Rendering, visual description, scene composition, layout, interaction, engine communication, and shell integration must remain distinct layers.

2. Lens must be declarative-first.
- UI structure, visuals, layout, bindings, and interaction configuration must be driven primarily by structured definitions rather than hardcoded product logic.

3. Lens must not hardcode a single product UI.
- The framework cannot assume one final application.
- It must support multiple packages on top of one core.

4. Lens must not depend on engine internals.
- All engine interaction must pass through a defined communication boundary.
- The UI may consume stable reported state and descriptor identities.
- The UI may not reinterpret engine internals directly.

5. Lens must preserve tiered customization.
- Some capabilities are engine-authoring only.
- Some are package-authoring only.
- Some are runtime-user-editable.
- These tiers must be explicit.

6. Lens must not give end users renderer-authoring power.
- Users may edit whitelisted instance properties.
- Users may not redefine primitive grammar, effect semantics, material grammar, or low-level shape rules.

7. Lens must support both workspace scene composition and UI composition.
- Logic board objects and UI objects must be supported by the same structural system where appropriate.
- Toolbars, panels, overlays, and inspectors must not require a separate ad hoc framework.

8. Lens must remain package-flexible.
- The same framework must be able to support a playful game package and a more professional package without re-architecting the core.

9. Lens must be validation-driven.
- Planning output must include a validation protocol, not just module descriptions.
- Validation must cover parser/format, assets, scene integrity, rendering, interaction, and policy enforcement.

10. Lens must not be mistaken for the final application.
- It is a substrate.
- Product workflows, concrete visuals, and shipped experience live on top of it.

## Required Architectural Layers

Every serious plan for Lens must account for these layers.

### 1. Native Render Layer
Responsibility:
- draw resolved primitives
- handle transforms, clipping, ordering, compositing, and text/effects execution

Must not:
- contain engine logic
- define application workflows
- hardcode component semantics

### 2. Visual Description Layer
Responsibility:
- define the declarative grammar for visuals
- express shapes, materials, effects, text placement, variants, and 2.5D presentation

Must not:
- act as the final asset pack
- absorb renderer behavior into asset semantics

### 3. Asset Definition Layer
Responsibility:
- define reusable authored assets and component visuals
- support IDs, parameters, variants, materials, relationships to primitives, and author-level overrides

Must distinguish:
- framework-defined grammar
- package-authored assets
- runtime instance overrides

### 4. Scene And Workspace Construction Layer
Responsibility:
- assemble authored objects into a working scene
- manage transforms, grouping, hierarchy, layering, visibility, stable IDs, and trace attachment

Must support:
- logic workspace scenes
- UI composition scenes

### 5. Layout Layer
Responsibility:
- define structured UI composition for toolbars, menus, panels, overlays, dialogs, HUD-like surfaces, and inspectors

Must be:
- declarative
- reusable
- separate from application-specific workflows

### 6. Interaction And Editing Layer
Responsibility:
- define generic behaviors such as selection, dragging, snapping, hit testing, viewport control, parameter editing, and trace editing

Must not:
- be hardcoded per component family
- assume only one product workflow

### 7. Engine Communication Layer
Responsibility:
- bridge Lens and the engine API
- consume reported runtime state
- normalize that state into UI-facing data objects
- propagate user commands back through the API

Must use:
- stable runtime-facing IDs
- normalized UI-facing state models
- data-binding objects for probes, outputs, indicators, analyzers, and overlays

The UI should be semantics-light, not semantics-blind.

### 8. Application Shell Integration Layer
Responsibility:
- provide integration points for windows, workspaces, project lifecycle, module loading, and application commands

Must not:
- become a full general-purpose desktop shell project

## Required Modules List

The planning output must identify concrete modules or equivalent subsystems covering at least the following responsibilities.

### Render Core
- primitive draw execution
- compositing/order
- clipping
- transform application
- text draw support
- effect execution

### Visual Grammar
- primitive vocabulary
- path/shape composition model
- material references
- variant rules
- effect declaration model
- 2.5D/isometric rules

### Asset Registry
- asset IDs
- asset loading
- asset lookup
- asset parameter exposure
- variant resolution
- theme/style resolution

### Scene Core
- scene object model
- hierarchy/grouping
- transform graph
- layer membership
- object visibility
- object identity/reference integrity

### Trace And Port System
- trace objects
- endpoint binding
- port identity
- route metadata
- editable trace properties

### Layout System
- panels
- toolbars
- overlays
- menus
- dialogs
- docking/floating rules if supported

### Interaction System
- hit testing
- selection
- hover/focus
- drag/move
- snapping
- edit operations
- context actions
- viewport pan/zoom

### Property And Parameter System
- typed properties
- edit-tier classification
- runtime overrides
- package-level overrides
- validation and range limits

### Binding And Data Model
- runtime source binding
- normalized UI-facing state objects
- subscription/update flow
- probe/analyzer/widget data consumption

### Engine Adapter
- strict boundary to engine API
- request/response/update flow
- mapping from engine-facing state to UI-facing normalized state

### Shell Integration
- workspace lifecycle hooks
- project load/save hooks
- module insertion/removal hooks
- window/workspace integration points

### Validation Model
- format validation
- asset validation
- scene validation
- render validation
- interaction validation
- policy/tier enforcement validation

## Customization Tier Policy

Lens planning must preserve three explicit tiers.

### Engine-Authoring Only
Examples:
- primitive grammar
- renderer internals
- effect semantics
- low-level material grammar
- 2.5D construction rules

### Package-Authoring Only
Examples:
- component visuals
- materials
- themes
- UI skinning
- toolbar layouts
- package-level visual mappings from engine state to appearance

### Runtime-User-Editable
Examples:
- placement
- labels
- allowed style selections
- trace color
- trace bend radius
- other explicitly whitelisted instance properties

Users may edit instance properties.
Users may not author the rendering system.

## Planning Output Requirements

A useful Lens planning output must include:
- module list
- layer boundaries
- allowed dependencies between layers
- identity/reference model
- asset model
- scene model
- layout model
- interaction model
- engine communication model
- customization tier model
- validation protocol

It must not stop at vague architecture language.
It must show how the system is divided and how data moves through it.

## Existing Lens References

The agent must use the existing Lens docs as reference material, not ignore them.

Read at minimum:
- `C:\DEV\Lens\docs\RULES.md`
- [UI Framework Spec](docs/framework/pillar_5_binding/GENERIC_UI_FRAMEWORK_SPEC.md)
- [Render Format Spec](docs/framework/pillar_1_rendering/RENDER_FORMAT_SPEC.md)
- [Architecture Plan](docs/framework/pillar_1_rendering/SCENE_UI_ARCHITECTURE_PLAN.md)
- [Object Model Spec](docs/framework/pillar_3_scene/SCENE_OBJECT_MODEL_SPEC.md)
- [ID & Reference Policy](docs/framework/pillar_3_scene/SCENE_ID_AND_REFERENCE_POLICY.md)
- [Trace & Port Model](docs/framework/pillar_4_topology/TRACE_AND_PORT_MODEL.md)
- [Customization Tier Policy](docs/framework/cross_cutting/CUSTOM_TIER_POLICY.md)
- [Scene Scripting Boundary](docs/framework/pillar_3_scene/SCENE_SCRIPTING_BOUNDARY.md)
- [View Variant & 2.5D Policy](docs/framework/pillar_2_grammar/VIEW_VARIANT_AND_2P5D_POLICY.md)
- [Validation Protocol](docs/framework/cross_cutting/VALIDATION_PROTOCOL.md)

## Process Expectations

Use the same discipline that made Strata effective:
- define policy before implementation
- keep boundaries explicit
- prefer one source of truth for contracts
- keep validation expectations explicit
- separate framework responsibilities from package responsibilities
- avoid collapsing everything into one giant runtime blob

## Final Reminder

Lens is the framework that makes authored UI possible.

It is not:
- the renderer alone
- the editor alone
- the application alone

It is the layered substrate that allows all of those to be built coherently.
