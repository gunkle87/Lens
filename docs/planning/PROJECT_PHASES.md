============================================================
PHASE 1–3 — FOUNDATION MAP (ALIGNED TO CPU ARCHITECTURE)
============================================================

DESIGN GOAL:
Everything in Phases 1–3 must directly prepare for:
- shared data bus
- register load/hold behavior
- ALU operations
- control signal discipline
- clocked state transitions

NO GENERIC LESSONS.
EVERY PROJECT FEEDS A FUTURE CPU COMPONENT.

============================================================
PHASE 1 — SIGNALS & GATES (FOUNDATION FOR BUS + CONTROL)
============================================================

GOAL:
Understand signals, drive strength, conflicts, and basic logic
→ prepares for BUS CONTROL + SIGNAL DISCIPLINE

------------------------------------------------------------
PROJECT GROUP A — SIGNAL BASICS
------------------------------------------------------------

1  Switch -> LED
2  Constant HIGH
3  Constant LOW
4  Inverter (NOT)
5  Signal propagation chain

------------------------------------------------------------
PROJECT GROUP B — BASIC LOGIC
------------------------------------------------------------

6  AND gate
7  OR gate
8  XOR gate
9  NAND (derived)
10 NOR (derived)

------------------------------------------------------------
PROJECT GROUP C — SIGNAL BEHAVIOR (CRITICAL FOR BUS)
------------------------------------------------------------

11 Signal fan-out
12 Signal conflict (introduce X)
13 Floating (Z behavior)
14 Strong vs weak drive
15 Multi-driver conflict

------------------------------------------------------------
PROJECT GROUP D — LOGIC CONSTRUCTION
------------------------------------------------------------

16 AND from NAND
17 OR from NOR
18 NOT from NAND
19 Multi-input AND
20 Multi-input OR

------------------------------------------------------------
PROJECT GROUP E — SIGNAL STABILITY & DEBUGGING
------------------------------------------------------------

21 Gate delay (step observation)
22 Stable vs unstable signal
23 Signal stabilization
24 Probe basics
25 Probe deep dive

------------------------------------------------------------
PROJECT GROUP F — PRE-BUS DISCIPLINE
------------------------------------------------------------

26 Single driver rule (one output active)
27 Conflict debugging
28 Clean signal handoff
29 Controlled signal path
30 Phase consolidation

RESULT:
User understands:
- signals can conflict (X)
- signals can float (Z)
- only one driver per line
→ REQUIRED for shared data bus later

============================================================
PHASE 2 — COMPOSITION & ROUTING (FOUNDATION FOR DATAPATH)
============================================================

GOAL:
Build combinational systems and controlled routing
→ prepares for ALU + BUS + DATAPATH

------------------------------------------------------------
PROJECT GROUP A — ARITHMETIC (ALU FOUNDATION)
------------------------------------------------------------

31 Half Adder
32 Full Adder (stage 1)
33 Full Adder (complete)
34 2-bit adder
35 4-bit ripple adder

------------------------------------------------------------
PROJECT GROUP B — DATA SELECTION (MUX = BUS CONTROL)
------------------------------------------------------------

36 Basic MUX (2:1)
37 MUX control behavior
38 4:1 MUX
39 MUX built from gates

------------------------------------------------------------
PROJECT GROUP C — ROUTING & BUS CONCEPT
------------------------------------------------------------

40 Shared line (intro bus)
41 Bus conflict (X)
42 Bus control (enable lines)
43 Bus arbitration example

------------------------------------------------------------
PROJECT GROUP D — LOGIC SYSTEMS
------------------------------------------------------------

44 Comparator (equal)
45 Comparator (greater/less)
46 Parity checker
47 Bit masking
48 Conditional logic block

------------------------------------------------------------
PROJECT GROUP E — DATAPATH THINKING
------------------------------------------------------------

49 Adder + MUX integration
50 Basic datapath concept
51 Multi-stage logic chain
52 Routing network
53 Signal selection challenge

------------------------------------------------------------
PROJECT GROUP F — ROBUSTNESS & DEBUG
------------------------------------------------------------

54 Fault injection
55 Debug incorrect logic
56 Logic simplification
57 Optimization challenge

------------------------------------------------------------
PROJECT GROUP G — SYSTEM COMPOSITION
------------------------------------------------------------

58 Mini system build
59 Composition challenge
60 Phase consolidation

------------------------------------------------------------
PROJECT GROUP H — ADVANCED ROUTING PREP
------------------------------------------------------------

61 Enable/disable outputs
62 Controlled data flow
63 Priority selection
64 Structured composition
65 Advanced routing scenario

------------------------------------------------------------
PROJECT GROUP I — MASTERY
------------------------------------------------------------

66 Validation project
67 Composition mastery challenge
68 Bus discipline reinforcement
69 Datapath integrity test
70 Phase consolidation

RESULT:
User understands:
- how to route data
- how to select sources
- how to avoid bus conflicts
→ REQUIRED for CPU datapath + ALU routing

============================================================
PHASE 3 — STATE & CLOCK (FOUNDATION FOR REGISTERS + CPU)
============================================================

GOAL:
Introduce memory, timing, and sequential behavior
→ prepares for REGISTERS + CONTROL + EXECUTION

------------------------------------------------------------
PROJECT GROUP A — FEEDBACK & MEMORY
------------------------------------------------------------

71 Feedback loop intro
72 SR latch (unstable)
73 SR latch (stable)
74 D latch
75 Latch enable behavior

------------------------------------------------------------
PROJECT GROUP B — CLOCKING
------------------------------------------------------------

76 Edge concept intro
77 Manual clock
78 Automatic clock

------------------------------------------------------------
PROJECT GROUP C — FLIP-FLOPS
------------------------------------------------------------

79 D flip-flop (concept)
80 D flip-flop (build)
81 T flip-flop

------------------------------------------------------------
PROJECT GROUP D — REGISTERS
------------------------------------------------------------

82 Register (1-bit)
83 Register (4-bit)
84 Register load/hold
85 Register reset

------------------------------------------------------------
PROJECT GROUP E — DATA MOVEMENT IN TIME
------------------------------------------------------------

86 Shift register (basic)
87 Shift register (multi-bit)

------------------------------------------------------------
PROJECT GROUP F — COUNTERS (PC FOUNDATION)
------------------------------------------------------------

88 Counter (1-bit)
89 Counter (2-bit)
90 Counter (4-bit)
91 Counter reset
92 Counter enable

------------------------------------------------------------
PROJECT GROUP G — TIMING DISCIPLINE
------------------------------------------------------------

93 Clock gating concept
94 Timing vs logic
95 Stable clock design

------------------------------------------------------------
PROJECT GROUP H — STATE VISIBILITY
------------------------------------------------------------

96 State visualization (step mode)
97 Memory vs combinational
98 Data storage validation

------------------------------------------------------------
PROJECT GROUP I — INTEGRATION
------------------------------------------------------------

99 Register + counter integration
100 Sequential debugging

------------------------------------------------------------
PROJECT GROUP J — FAILURE MODES (CRITICAL UNDERSTANDING)
------------------------------------------------------------

101 Glitch introduction
102 Glitch mitigation
103 Latch instability deep dive

------------------------------------------------------------
PROJECT GROUP K — CONTROL PREP
------------------------------------------------------------

104 State machine intro
105 Controlled sequence
106 Multi-step sequence

------------------------------------------------------------
PROJECT GROUP L — SYSTEM PREPARATION
------------------------------------------------------------

107 Sequential challenge
108 Mini system with state
109 Validation project
110 Phase consolidation

RESULT:
User understands:
- how state is stored
- how time affects logic
- how registers behave
- how sequences are controlled
→ REQUIRED for CPU registers + control unit

============================================================
FINAL FOUNDATION RESULT (PHASES 1–3)
============================================================

By project 110, the user has built:

- signal discipline (Phase 1)
- datapath and routing (Phase 2)
- registers and timing (Phase 3)

Which directly enables:

- control signals
- instruction execution
- CPU assembly

NO CONCEPT IS UNUSED.
EVERYTHING FLOWS INTO PHASE 4–5.

============================================================
END OF PHASE 1–3 MAP
============================================================

============================================================
PHASE 4 — SYSTEMS & CONTROL (MAPPED TO CPU)
============================================================

GOAL:
Introduce and assemble the CONTROL UNIT and EXECUTION FLOW

CPU TARGETS IN THIS PHASE:
- Instruction Register (IR)
- Decoder
- Control signals
- Program Counter (PC)
- Fetch → Decode → Execute cycle
- Basic datapath coordination

------------------------------------------------------------
PROJECT GROUP A — INSTRUCTION REPRESENTATION
------------------------------------------------------------

111 Instruction concept
    - What is an instruction (opcode + operand)

112 Instruction format (8-bit)
    - Split into opcode [7:4] and operand [3:0]

113 Instruction Register (IR) build
    - Store and expose instruction

114 IR field extraction
    - Separate opcode vs operand lines

------------------------------------------------------------
PROJECT GROUP B — DECODER
------------------------------------------------------------

115 2-to-4 decoder
116 4-to-16 decoder

117 Opcode decoding
    - Map opcode → one-hot control line

118 Invalid opcode behavior
    - What happens if decode fails

------------------------------------------------------------
PROJECT GROUP C — CONTROL SIGNALS
------------------------------------------------------------

119 Control signal concept
    - Signals drive hardware behavior

120 Register load signals
    - A_LOAD, B_LOAD, etc.

121 Bus enable signals
    - One driver at a time

122 Memory control signals
    - RAM_READ, RAM_WRITE

123 ALU control signals
    - ADD, SUB, PASS

124 PC control signals
    - PC_INC, PC_LOAD

125 Control signal conflict (debug)
    - What happens if two drivers enable

------------------------------------------------------------
PROJECT GROUP D — PROGRAM COUNTER (PC)
------------------------------------------------------------

126 PC concept
127 PC build (4-bit register)
128 PC increment
129 PC load (jump)
130 PC reset

------------------------------------------------------------
PROJECT GROUP E — EXECUTION CYCLE
------------------------------------------------------------

131 FETCH phase
    - ROM → IR

132 PC increment phase

133 DECODE phase
    - opcode activates control path

134 EXECUTE phase
    - perform operation

135 Step-by-step cycle visualization

136 Multi-cycle observation
    - run vs step

------------------------------------------------------------
PROJECT GROUP F — SIMPLE CONTROL UNIT
------------------------------------------------------------

137 Build simple control sequencer
    - T0, T1, T2, T3

138 Tie decoder to sequencer

139 Execute simple instruction (LDI)

140 Phase 4 consolidation
    - full fetch/decode/execute loop

============================================================
PHASE 5 — COMPUTER SYSTEM (MAPPED TO CPU)
============================================================

GOAL:
Assemble full CPU + MEMORY + I/O and run programs

CPU TARGETS IN THIS PHASE:
- ALU integration
- RAM + ROM integration
- Input + Output
- Instruction execution
- Complete system behavior

------------------------------------------------------------
PROJECT GROUP A — MEMORY
------------------------------------------------------------

141 ROM concept
142 ROM build (addressed by PC)
143 RAM concept
144 RAM read/write
145 RAM + datapath integration

------------------------------------------------------------
PROJECT GROUP B — ALU + REGISTERS
------------------------------------------------------------

146 Accumulator (A) register
147 B register
148 ALU (add/sub/pass)
149 ALU → bus → register flow
150 Zero flag (Z)

------------------------------------------------------------
PROJECT GROUP C — INSTRUCTION EXECUTION
------------------------------------------------------------

151 LDI execution
152 LDA execution
153 STA execution
154 ADD execution
155 SUB execution

156 JMP execution
157 JZ execution

------------------------------------------------------------
PROJECT GROUP D — I/O SYSTEM
------------------------------------------------------------

158 Output latch
159 LED / display driver

160 Input latch
161 Keypad scan (rows/columns)
162 Keypad integration

------------------------------------------------------------
PROJECT GROUP E — FULL CPU ASSEMBLY
------------------------------------------------------------

163 CPU skeleton (connect all blocks)

164 Step execution (manual clock)
165 Run mode (auto clock)

166 Debugging execution
    - probe signals, observe flow

------------------------------------------------------------
PROJECT GROUP F — PROGRAMS (OBSERVATION, NOT CODING)
------------------------------------------------------------

167 Program: counter
168 Program: input echo
169 Program: addition
170 Program: loop
171 Program: condition

172 Program: keypad display
173 Program: memory test
174 Program: branching demo
175 Program: reaction timer

------------------------------------------------------------
PROJECT GROUP G — FINALIZATION
------------------------------------------------------------

176 System integration polish
177 Debugging walkthrough
178 Pre-capstone validation

179 Free build introduction

180 BUILD A COMPUTER (CAPSTONE)

============================================================
KEY RESULT
============================================================

Every Phase 4–5 project now maps directly to:

- a CPU block
- a control signal
- or a cycle phase

Nothing is abstract.
Everything builds toward the final machine.

============================================================