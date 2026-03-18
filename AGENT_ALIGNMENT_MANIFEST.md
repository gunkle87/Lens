# Lens Agent Alignment Manifest

This document is the canonical alignment file for planning work in
`C:\DEV\Lens`.

Lens is a layered UI framework substrate for engine-facing applications.
Its purpose is to make multiple authored interfaces possible from one bounded
core rather than to define one fixed product.

## Core Identity

Lens is:
- a native rendering substrate
- a declarative visual description system
- a scene and workspace composition framework
- a layout and shell integration framework
- a generic interaction framework
- a data-binding and engine-communication layer

Lens is not:
- a one-off renderer
- a finished editor
- a bundle of fixed assets
- a hardcoded application
- a general-purpose desktop shell

## Mission

Lens must provide the structure, language, and runtime model needed to define
and execute interactive UI through declarative data and bounded runtime logic.

The system must stay reusable across multiple packages without forking the
core architecture.

## Authoring And Runtime Execution Model

- Lens authoring is pre-compile and non-consumer-facing.
- Packages author UI definitions, assets, layout, and bindings before ship.
- Shipped applications execute authored UI through the Lens runtime layer.
- End users interact with product UI, not Lens authoring systems.
- Runtime edits remain limited to explicitly whitelisted capabilities.

## Hard Constraints

1. Lens must remain layered.
   - Rendering, visual description, asset definition, scene construction,
     layout, interaction, engine communication, and shell integration must
     remain distinct concerns.

2. Lens must be declarative-first.
   - UI structure, visuals, layout, bindings, and interaction configuration
     should be authored primarily as structured definitions rather than as
     hardcoded product logic.

3. Lens must not hardcode one product workflow.
   - Product behavior and package behavior must live above the shared
     framework.

4. Lens must not depend on engine internals.
   - All engine interaction must pass through a defined communication
     boundary.

5. Lens must preserve explicit customization tiers.
   - Some capabilities are engine-authoring only.
   - Some are package-authoring only.
   - Some are runtime-user-editable.

6. Lens must not give end users renderer-authoring power.
   - Runtime editing is limited to whitelisted instance properties and
     approved behaviors.

7. Lens must support both workspace composition and UI composition.
   - A separate ad hoc framework for panels, overlays, inspectors, and
     authored workspace objects should not be required.

8. Lens must be validation-driven.
   - Plans and implementations must define how correctness is checked, not
     only what modules exist.

9. Lens must not be mistaken for the final application.
   - It is substrate.
   - Concrete products sit on top of it.

10. Lens must preserve the authoring/runtime split.
   - Authoring and content preparation are pre-compile concerns.
   - Runtime execution remains in-app through Lens core layers.
   - Compile-time artifact generation does not replace runtime execution.

## Architectural Layers

Every serious plan for Lens must account for these layers.

### 1. Native Render Layer
- Draw resolved primitives.
- Handle transforms, clipping, ordering, compositing, and related low-level
  presentation behavior.
- Must not contain engine logic or product workflows.

### 2. Visual Description Layer
- Define the declarative grammar for visuals.
- Express shapes, materials, effects, text placement, variants, and 2.5D
  presentation.
- Must not absorb renderer behavior into asset semantics.

### 3. Asset Definition Layer
- Define reusable authored assets and component visuals.
- Support IDs, parameters, variants, materials, primitive relationships, and
  author-level overrides.
- Must distinguish framework grammar from package assets and runtime instance
  overrides.

### 4. Scene And Workspace Construction Layer
- Assemble authored objects into a working scene.
- Manage transforms, grouping, hierarchy, layering, visibility, stable IDs,
  and attachment/reference integrity.

### 5. Layout Layer
- Define structured UI composition for panels, toolbars, overlays, dialogs,
  menus, and related container structures.
- Must remain declarative and reusable.

### 6. Interaction And Editing Layer
- Define selection, dragging, snapping, hit testing, viewport control,
  parameter editing, and related generic behavior.
- Must not be hardcoded per product family.

### 7. Engine Communication Layer
- Bridge Lens and the engine API.
- Normalize engine state into UI-facing data objects.
- Propagate user commands back through the defined boundary.

### 8. Application Shell Integration Layer
- Provide integration points for windows, workspaces, project lifecycle,
  module loading, and application commands.
- Must not become a general-purpose desktop shell project.

## Customization Tiers

Lens planning must preserve three explicit tiers.

### Engine-Authoring Only
- primitive grammar
- renderer internals
- effect semantics
- low-level material grammar
- 2.5D construction rules

### Package-Authoring Only
- component definitions
- materials
- themes
- toolbar layouts
- allowed runtime-editable property sets
- mappings from engine state to appearance

### Runtime-User-Editable
- safe whitelisted instance properties
- placement and movement
- labels
- approved variant selection
- other explicitly allowed instance overrides

Users may edit instances.
Users may not author the renderer.

## Required Planning Output

A useful Lens planning output must include:
- module list
- layer boundaries
- allowed dependencies between layers
- asset model
- scene model
- layout model
- interaction model
- engine communication model
- customization tier model
- validation approach

It must not stop at vague architecture language.
It must show how the system is divided and how data moves through it.

## Read First

Read at minimum:
- [RULES.md](RULES.md)
- [PROGRAMMING_MODEL.md](PROGRAMMING_MODEL.md)
- [ARCHITECTURE_OVERVIEW.md](ARCHITECTURE_OVERVIEW.md)
- [VALIDATION_APPROACH.md](VALIDATION_APPROACH.md)
- [CODEX_SUGGESTIONS.md](CODEX_SUGGESTIONS.md)

## Process Expectations

Use the same discipline that made Strata effective:
- define policy before implementation
- keep boundaries explicit
- prefer one source of truth for contracts
- keep validation expectations explicit
- separate framework responsibilities from package responsibilities
- avoid hidden parallel truths

## Final Reminder

Lens is the framework that makes authored UI possible.
It is not the renderer alone, the editor alone, or the shipped application.
