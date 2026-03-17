# SCENE ID AND REFERENCE POLICY

============================================================
PURPOSE
============================================================

This document defines the rules for identifying and referencing objects
within the LENS scene system.

Stable identities are required for:

- undo/redo
- save/load
- data binding
- scripting
- cross-object relationships (traces)

============================================================
PRIMARY ID RULES
============================================================

1. IDs must be UNIQUE within a SceneDocument.
2. IDs must be PERSISTENT (stable across save/load).
3. Use UUIDs for instance-based IDs (SceneNodes).
4. Use human-readable strings for definition-based IDs (Assets).

============================================================
REFERENCE TYPES
============================================================

There are four primary ways to refer to data:

1. ObjectRef (Instance to Instance)
   - e.g., a Trace pointing to two Components
   - used by SCENE MODEL

2. DefinitionRef (Instance to Asset)
   - e.g., a ComponentNode referencing an XOR gate definition
   - used by ASSET REGISTRY

3. EndpointRef (Trace to Port)
   - e.g., a Trace pointing to "ComponentA.Input1"
   - combines an ObjectRef and a PortID

4. LayerRef (Object to Semantic Group)
   - e.g., an LED assigned to the "UI_OVERLAY" bucket

============================================================
REFERENCE INTEGRITY
============================================================

The system must ensure references remain valid.

On Deletion:
- If a Node is deleted, all Traces connected to it must be notified.
- Traces with invalid endpoints should either be deleted or marked as
  BROKEN.

On Copy/Paste:
- New UUIDs must be generated for the copies.
- Internal references between copied nodes (e.g., a trace between two
  copied components) must be remapped to the new UUIDs.

============================================================
STATE BINDING HANDLE
============================================================

For data-driven UIs, use a BINDING HANDLE.
It is an external reference to an engine source.

Binding Handle includes:
- SourceID: UUID (The authoritative source)
- DescriptorID: String (Static identifier)
- DataPath: String (Semantic endpoint)
- UpdatePolicy: Enum (Push/Poll rules)

============================================================
BOTTOM LINE
============================================================

Never rely on array indices or memory pointers for object identity.
Identity must be explicit and stored in the data model.
If an ID changes, the link is broken.
