# SCENE UI ARCHITECTURE PLAN

============================================================
PURPOSE
============================================================

This document outlines the layered architecture for the LENS scene
and UI system.

It defines how data flows from authored definitions to a rendered
picture.

============================================================
ARCHITECTURAL LAYERS
============================================================

1. SOURCE FORMAT (Declarative)
   - .lcomp, .lmat, .lscn files
   - high-level human/agent readable data

2. ASSET DEFINITION LAYER (Read-only Registry)
   - MaterialDef, ComponentDef, ShapeDef
   - loaded and validated at startup
   - immutable at runtime

3. SCENE MODEL (Mutable Runtime)
   - SceneDocument, SceneNode, TraceInstance
   - represents "what exists right now"
   - ID-stable
   - supports undo/redo, saving, loading

4. INTERACTION / EDITOR LAYER (Session State)
   - tool management, selection state, hover state
   - maps input (pointer) to scene operations

5. RENDER GRAPH (Flattened Execution List)
   - RenderNode primitives
   - resolved world transforms
   - resolved material recipes
   - highly optimized for the drawing backend

6. PLATFORM / PACKAGE SHELL (Execution Frame)
   - window management, top-level layout, toolbars
   - gates permissions (game mode vs editor mode)

============================================================
CORE RULE: SEPARATION OF MODELS
============================================================

Do not mix the Definition, Scene, and Render models into one type.

- Definition = Blueprint
- Scene = Instance
- Render = Command

Example:

- A MaterialDef (definition) is used by a ComponentNode (scene instance)
  to generate a RECT with a specific HEX and GLOW (render command).

============================================================
DATA BINDING FLOW
============================================================

1. Engine reports state (e.g., "Pin A is HIGH")
2. Adapter maps state to normalized UI signal (0, 1, X, Z)
3. SceneNode (Component) subscribes to signal
4. SceneNode updates its local visual state property
5. Render Graph is dirtied and rebuilt

============================================================
WHY THIS LAYERING?
============================================================

- Portability: The renderer core doesn't need to know about "Logic Gates."
- Stability: IDs stay stable even if visuals or transforms change.
- Performance: Drawing doesn't require walking a deep logical graph.
- Multi-Package: The same core can draw a game UI and a Pro Lab UI.

============================================================
BOTTOM LINE
============================================================

The scene system is a data transformation pipeline.
Input: Definitions + Intent.
Output: Render commands.
Keeping these stages distinct is what makes the system powerful.
