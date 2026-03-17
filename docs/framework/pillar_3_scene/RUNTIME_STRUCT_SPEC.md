# RUNTIME STRUCT SPECIFICATION

This document defines the core data structures for the Lens runtime. These structs are designed for deterministic evaluation, efficient flattening, and stable data binding.

## 1. COMPOSITE IDENTITY & BINDING

All runtime objects and data bindings must use these identity structures.

### BindingHandle
Used by UI components to subscribe to simulation data.
- SourceID: UUID (Authoritative session/sim instance)
- DescriptorID: String (Static ID of the pin, bus, or analyzer)
- DataPath: String (Semantic endpoint, e.g., "state", "freq", "value")
- UpdatePolicy: Enum (Push_On_Change, Poll_Interval)

### SceneIdentity
Used for stable references across scenes.
- SceneID: UUID (World, UI, or Overlay document)
- NodeID: UUID (Stable node identifier)

---

## 2. SCENE MODEL (EDITABLE)

### SceneDocument
The top-level container for a specific visual domain.
- DocumentID: UUID
- Kind: Enum (WORLD, UI, OVERLAY)
- Nodes: Map<NodeID, SceneNode>
- RootNodes: List<NodeID> (Ordered draw/iteration root)
- Layers: List<LayerDef> (Z-order and visibility grouping)

### SceneNode (Base)
- ID: NodeID
- Kind: Enum (GROUP, COMPONENT, TRACE, TEXT)
- ParentID: NodeID (Optional)
- Transform: Transform2D (Position, Rotation, Scale)
- Visibility: Boolean
- LayerClass: String (Used for semantic Z-ordering)
- Metadata: Map<String, Any>

### ComponentNode (Instance)
- DefinitionID: String (Reference to ComponentDef)
- ActiveVariantID: String (The authored variant currently in use)
- Binding: BindingHandle (Optional, for logic-linked components)
- PropOverrides: Map<PropName, Any> (Instance-specific visual properties)
- CurrentRegime: Enum (PULSE, FLICKER, STEADY - Runtime calculated)

### TraceNode
- Source: SceneIdentity (Port or Node reference)
- Destination: SceneIdentity
- Vertices: List<Vector2> (Route points)
- StyleID: String (Reference to TraceStyleDef)
- State: UiSignalState (Normalized state from Adapter)

---

## 3. DEFINITION MODEL (READ-ONLY)

### ComponentDef
The blueprint for a reusable UI/Logic object.
- DefID: String
- Symbol: ShapeDef (Geometry/Vector data)
- Variants: Map<VariantID, VariantDef> (Pre-authored structure/style sets)
- Ports: List<PortDef> (Anchor points for traces)
- DefaultProps: Map<PropName, Any>
- TierPolicy: Map<PropName, CustomizationTier> (Enforcement gate for overrides)

### MaterialDef
- MaterialID: String
- BaseColor: Color (HEX/RGBA)
- BloomRadius: Float
- Opacity: Float
- Techniques: Map<Regime, VisualRecipe> (Regime-based mapping: PULSE -> Animated Pulse, STEADY -> Intensity Glow)

---

## 4. RENDER MODEL (FLATTENED)

The Flattening Engine transforms the hierarchical SceneNodes into a linear list of RenderNodes for the low-level graphics API.

### RenderNode
- Primitive: Enum (RECT, CIRCLE, PATH, TEXT)
- WorldTransform: Matrix3x3
- Material: ResolvedMaterial (Colors/Glows applied)
- Bounds: Rect (Culling/Hit-test optimization)
- ZIndex: Int (Resolved from LayerClass and hierarchy)

---

## 5. REPLAY & TIMING CONTRACT

### TemporalState
- SimulationTime: Authoritative Monotonic Timestamp (provided by Engine)
- RenderTime: Wall-clock assisted interpolation timestamp
- TimeScale: Float (Simulation speed multiplier)

All animations and regime transitions must be calculated as: 
`VisualProperty = f(SimulationTime, LastStateChangeTime)`
