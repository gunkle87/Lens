```text
============================================================
HIGHZ KIT CPU — BLOCK SPEC (HIERARCHICAL STRUCTURE)
============================================================

CPU (4-bit data / 8-bit instruction)
|
+-- CORE REGISTERS
|   |
|   +-- Program Counter (PC)
|   |   - Width: 4-bit
|   |   - Functions:
|   |       - increment
|   |       - load (jump)
|   |
|   +-- Instruction Register (IR)
|   |   - Width: 8-bit
|   |   - Fields:
|   |       - opcode [7:4]
|   |       - operand [3:0]
|   |
|   +-- Accumulator (A)
|   |   - Width: 4-bit
|   |   - Primary ALU operand
|   |
|   +-- General Register (B)
|   |   - Width: 4-bit
|   |   - Secondary ALU operand / temp storage
|   |
|   +-- Flags
|       |
|       +-- Zero Flag (Z)
|           - Set when ALU result == 0
|
+-- DATA PATH
|   |
|   +-- Data Bus
|   |   - Width: 4-bit
|   |   - Shared between:
|   |       - registers
|   |       - ALU
|   |       - RAM
|   |       - I/O
|   |
|   +-- Bus Control
|       - single active driver at a time
|       - controlled via enable lines
|
+-- ARITHMETIC LOGIC UNIT (ALU)
|   |
|   +-- Inputs:
|   |   - A register
|   |   - B register or bus
|   |
|   +-- Operations:
|   |   - ADD
|   |   - SUB or INC
|   |   - PASS (transfer)
|   |
|   +-- Outputs:
|       - result to data bus
|       - updates Zero Flag
|
+-- MEMORY
|   |
|   +-- Program ROM
|   |   - Addressed by PC
|   |   - Width: 8-bit words
|   |   - Size: 16–32 instructions
|   |
|   +-- Data RAM
|       - Addressed by operand field
|       - Width: 4-bit
|       - Size: 16 locations
|       - Supports read/write
|
+-- CONTROL UNIT
|   |
|   +-- Decoder
|   |   - Input: opcode (IR[7:4])
|   |   - Output: control signals
|   |
|   +-- Sequencer
|   |   - Phases:
|   |       - FETCH
|   |       - PC_INC
|   |       - DECODE
|   |       - EXECUTE
|   |
|   +-- Control Signals
|       - register load enables
|       - bus enable signals
|       - ALU operation select
|       - memory read/write
|       - PC control (inc/load)
|
+-- INPUT SYSTEM
|   |
|   +-- Keypad Interface (4x4)
|   |   - row/column scan logic
|   |
|   +-- Input Latch
|       - captures keypad value
|       - places value onto data bus
|
+-- OUTPUT SYSTEM
|   |
|   +-- Output Latch
|   |   - stores output value from bus
|   |
|   +-- Display Driver
|       - LED or 7-segment
|       - driven by output latch
|
+-- CLOCK / STEP CONTROL
    |
    +-- Manual Step
    |   - single-cycle advancement
    |
    +-- Auto Clock
    |   - continuous execution
    |
    +-- Reset
        - clears PC, registers, flags

============================================================
INSTRUCTION FORMAT
============================================================

[7:4] opcode
[3:0] operand (immediate or address)

============================================================
MINIMAL INSTRUCTION SET
============================================================

NOP   - no operation
LDI   - load immediate into A
LDA   - load A from RAM
STA   - store A into RAM
ADD   - A = A + value
SUB/INC - A = A - value or increment
JMP   - jump to address
JZ    - jump if zero flag set
IN    - read from input
OUT   - write to output

============================================================
END OF BLOCK SPEC
============================================================


============================================================
HIGHZ KIT CPU — CONTROL SIGNALS + CYCLE TABLE
============================================================

DESIGN GOAL
- Keep control simple, visible, and teachable
- One shared 4-bit data bus
- One active bus driver at a time
- Fixed phase flow:
  T0 = FETCH
  T1 = PC_INC
  T2 = DECODE
  T3 = EXECUTE

============================================================
CONTROL SIGNAL GROUPS
============================================================

1. BUS SOURCE ENABLES
- ROM_LO_TO_BUS     : drive IR low nibble / immediate field onto bus
- RAM_TO_BUS        : drive RAM[address] onto bus
- ALU_TO_BUS        : drive ALU result onto bus
- IN_TO_BUS         : drive input latch onto bus
- A_TO_BUS          : drive A register onto bus
- B_TO_BUS          : drive B register onto bus

2. REGISTER LOAD ENABLES
- IR_LOAD           : load 8-bit instruction from ROM into IR
- A_LOAD            : load A from bus
- B_LOAD            : load B from bus
- OUT_LOAD          : load output latch from bus
- IN_LATCH_LOAD     : capture keypad/input value into input latch
- Z_LOAD            : update zero flag from ALU result
- PC_LOAD           : load PC from bus
- PC_INC            : increment PC by 1

3. MEMORY CONTROL
- ROM_READ          : read ROM at PC
- RAM_READ          : read RAM at operand address
- RAM_WRITE         : write bus value into RAM at operand address

4. ALU CONTROL
- ALU_OP_PASS       : pass-through source value
- ALU_OP_ADD        : A + B_or_bus
- ALU_OP_SUB        : A - B_or_bus
- ALU_OP_INC        : A + 1
- ALU_OP_DEC        : A - 1

5. DECODE / FLOW CONTROL
- DECODE_ENABLE     : decode IR[7:4]
- EXEC_ENABLE       : enable instruction-specific execute path
- JUMP_IF_ZERO      : load PC only if Z == 1

6. SYSTEM CONTROL
- RESET             : clear PC, A, B, IR, OUT, flags
- STEP_CLOCK        : advance one phase/cycle
- RUN_CLOCK         : continuous clocking

============================================================
ASSUMED INTERNAL PATHS
============================================================

- IR high nibble feeds decoder directly
- IR low nibble is available as operand/immediate source
- RAM address comes from IR low nibble
- ROM address comes from PC
- ALU primary operand is A
- ALU secondary operand is either:
  - B register
  - bus value
  depending on implementation choice
- OUT latch drives LED / 7-seg display
- Input latch captures keypad scan result

============================================================
GLOBAL CYCLE SHAPE
============================================================

T0 FETCH
- ROM_READ
- IR_LOAD

Effect:
- IR <- ROM[PC]

T1 PC_INC
- PC_INC

Effect:
- PC <- PC + 1

T2 DECODE
- DECODE_ENABLE

Effect:
- decoder interprets IR[7:4]
- operand = IR[3:0] becomes available to control path

T3 EXECUTE
- instruction-specific control signals asserted

Effect:
- actual transfer / ALU / memory / I/O action occurs

============================================================
INSTRUCTION SET
============================================================

0x0 NOP
0x1 LDI imm
0x2 LDA addr
0x3 STA addr
0x4 ADD addr
0x5 SUB addr
0x6 JMP addr
0x7 JZ addr
0x8 IN
0x9 OUT
0xA LDB addr
0xB MOV A,B
0xC MOV B,A
0xD INC
0xE DEC
0xF reserved / future

============================================================
PER-INSTRUCTION EXECUTE TABLE
============================================================

NOP
T3:
- no control signals asserted

Effect:
- no state change

------------------------------------------------------------

LDI imm
Meaning:
- A <- immediate nibble

T3:
- ROM_LO_TO_BUS
- A_LOAD

Effect:
- A <- IR[3:0]

------------------------------------------------------------

LDA addr
Meaning:
- A <- RAM[addr]

T3:
- RAM_READ
- RAM_TO_BUS
- A_LOAD

Effect:
- A <- RAM[IR[3:0]]

------------------------------------------------------------

STA addr
Meaning:
- RAM[addr] <- A

T3:
- A_TO_BUS
- RAM_WRITE

Effect:
- RAM[IR[3:0]] <- A

------------------------------------------------------------

ADD addr
Meaning:
- A <- A + RAM[addr]
- Z <- result == 0

T3:
- RAM_READ
- RAM_TO_BUS
- ALU_OP_ADD
- ALU_TO_BUS
- A_LOAD
- Z_LOAD

Effect:
- A <- A + RAM[IR[3:0]]

------------------------------------------------------------

SUB addr
Meaning:
- A <- A - RAM[addr]
- Z <- result == 0

T3:
- RAM_READ
- RAM_TO_BUS
- ALU_OP_SUB
- ALU_TO_BUS
- A_LOAD
- Z_LOAD

Effect:
- A <- A - RAM[IR[3:0]]

------------------------------------------------------------

JMP addr
Meaning:
- PC <- addr

T3:
- ROM_LO_TO_BUS
- PC_LOAD

Effect:
- PC <- IR[3:0]

------------------------------------------------------------

JZ addr
Meaning:
- if Z == 1 then PC <- addr

T3:
- ROM_LO_TO_BUS
- JUMP_IF_ZERO
- PC_LOAD

Effect:
- if Z == 1, PC <- IR[3:0]
- else no change

------------------------------------------------------------

IN
Meaning:
- A <- input latch

T3:
- IN_TO_BUS
- A_LOAD

Effect:
- A <- input latch

------------------------------------------------------------

OUT
Meaning:
- output latch <- A

T3:
- A_TO_BUS
- OUT_LOAD

Effect:
- OUT <- A

------------------------------------------------------------

LDB addr
Meaning:
- B <- RAM[addr]

T3:
- RAM_READ
- RAM_TO_BUS
- B_LOAD

Effect:
- B <- RAM[IR[3:0]]

------------------------------------------------------------

MOV A,B
Meaning:
- A <- B

T3:
- B_TO_BUS
- A_LOAD

Effect:
- A <- B

------------------------------------------------------------

MOV B,A
Meaning:
- B <- A

T3:
- A_TO_BUS
- B_LOAD

Effect:
- B <- A

------------------------------------------------------------

INC
Meaning:
- A <- A + 1
- Z <- result == 0

T3:
- ALU_OP_INC
- ALU_TO_BUS
- A_LOAD
- Z_LOAD

Effect:
- A <- A + 1

------------------------------------------------------------

DEC
Meaning:
- A <- A - 1
- Z <- result == 0

T3:
- ALU_OP_DEC
- ALU_TO_BUS
- A_LOAD
- Z_LOAD

Effect:
- A <- A - 1

============================================================
OPTIONAL TWO-STEP MEMORY VARIANT
============================================================

If RAM timing or teaching clarity needs more visibility, memory ops can be split:

T3:
- RAM_READ

T4:
- RAM_TO_BUS
- A_LOAD / B_LOAD / ALU path / RAM_WRITE

This is optional.
The simpler first cut keeps all work in T3.

============================================================
PROJECT MAPPING VALUE
============================================================

These control signals naturally break into project groups:
- bus enables
- register loads
- ALU ops
- memory controls
- branch controls
- fetch/decode/execute sequencing

That means the CPU can be built as a sequence of visible subassemblies
rather than as one opaque final machine.

============================================================
END OF CONTROL SIGNALS + CYCLE TABLE
============================================================
```
