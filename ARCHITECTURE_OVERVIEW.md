# Lens Architecture Overview

This document summarizes the current architectural shape of Lens.

Lens is a shared framework substrate, not a finished application.
Its job is to provide reusable structure for authored interfaces while keeping
runtime boundaries explicit.

Authoring is pre-compile and non-consumer-facing.
Shipped applications execute authored UI through Lens runtime layers.

## Layer Stack

### 1. Native Render Layer
- Executes resolved drawing primitives.
- Owns transforms, clipping, ordering, compositing, and related low-level
  presentation behavior.

### 2. Visual Description Layer
- Defines the grammar for visuals.
- Owns shapes, materials, effects, variants, and authored presentation rules.

### 3. Asset Definition Layer
- Defines reusable authored assets and component visuals.
- Owns IDs, parameters, variants, and author-level overrides.

### 4. Scene And Workspace Construction Layer
- Assembles authored objects into working scenes.
- Owns hierarchy, grouping, layering, visibility, transforms, and stable
  reference relationships.

### 5. Layout Layer
- Defines structured UI composition for panels, menus, dialogs, overlays, and
  container organization.

### 6. Interaction Layer
- Defines generic behaviors such as selection, dragging, snapping, hit
  testing, viewport control, and approved editing flows.

### 7. Engine Communication Layer
- Bridges Lens and the engine API.
- Normalizes engine truth into UI-facing data and sends approved commands back
  through the boundary.

### 8. Application Shell Integration Layer
- Provides integration points for workspaces, lifecycle, module loading, and
  application commands.

## Structural Rules

- Definitions must remain separate from instances.
- Scene graph and derived render graph must remain separate.
- The framework must support both workspace composition and UI composition.
- Runtime editing must remain bounded by explicit customization policy.
- Product workflows must live above the shared core.
- Compile-time artifact generation must not replace runtime execution.

## Customization Tiers

Lens preserves three explicit tiers:
- engine-authoring only
- package-authoring only
- runtime-user-editable

The shared core must make these boundaries enforceable rather than leaving
them implicit.

## Implementation Posture

- Policy before implementation.
- Boundaries before coupling.
- Validation before trust.
- Reuse before package-specific hardcoding.
