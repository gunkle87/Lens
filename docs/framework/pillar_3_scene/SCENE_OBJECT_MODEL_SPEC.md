# SCENE OBJECT MODEL SPECIFICATION

============================================================
PURPOSE
============================================================

This document defines the runtime scene object model for the LENS
renderer/editor.

It represents the hierarchical structure of a working scene.

============================================================
CORE HIERARCHY
============================================================

1. SCENE DOCUMENT (Root)
   - contains all nodes, layers, and configuration

2. SCENE NODE (Base Type)
   - the common interface for everything in the tree

3. GROUP NODE
   - a logical container for other nodes
   - provides a parent transform

4. COMPONENT NODE (Instance)
   - a visual/logic object placed from an asset definition
   - can have state, parameters, and ports

5. TRACE NODE
   - a semantic connection between two endpoints
   - has vertices, style, and signal state

6. TEXT NODE
   - for labels, values, and documentation strings

7. PRIMITIVE NODE (Optional/Internal)
   - for manual low-level geometry used outside components

============================================================
COMMON NODE FIELDS
============================================================

Every SceneNode must have:

- ID: UUID (Stable across sessions)
- Name: string (Optional, for developer/user reference)
- Kind: Enum (GROUP, COMPONENT, TRACE, TEXT)
- Visibility: bool
- LayerClass: string (e.g., BACKGROUND, TRACE, UI_CHROME)
- LocalTransform: 2D Matrix (Position, Rotation, Scale)
- CustomMetadata: Map<string, any>

============================================================
TRANSFORM MODEL
============================================================

Transforms are hierarchical.
`WorldTransform = ParentWorldTransform * LocalTransform`

The renderer should resolve these into the flattened Render Graph.

============================================================
DEFINITION vs INSTANCE
============================================================

ComponentNodes must NOT store their entire geometry.
They store:

- DefinitionID (Reference to ComponentDef)
- ParameterOverrides (Instance-specific settings)
- State (Dynamic runtime signal state)

The renderer looks up the geometry in the Asset Registry at draw-time.

============================================================
STATE MANAGEMENT
============================================================

Nodes should be reactive.
When a bound property or signal changes:

1. The node marks itself as DIRTY.
2. The dirty flag propagates to the document.
3. The Render Graph for that branch is rebuilt in the next frame.

============================================================
BOTTOM LINE
============================================================

The Scene Object Model is the source of truth for the workspace.
Keep it separate from the low-level render list.
Stable IDs and hierarchical transforms are mandatory.
