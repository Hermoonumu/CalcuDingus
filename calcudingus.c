// Structure:
// A core featuring has three general registers: GR1, GR2 and GR3

// 4096*4=16384 bytes of memory
// A word, length of which is 32bits.
// Word-addressable.

// Index register (IXR) for loops and whatev.
// Program counter (PC) for executing programs.
// Memory address register (MEMADDR) for storing effective mem address
// Instruction register (IR) for holding current instruction
// Stack pointer and Base pointer (SP/BP) for managing stack
// Program end (PROGRAMEND) to separate the memory into data and text sections
// FLAGS for.. flags
// CLKCNT for clock counting
// State variable that will help the code track the state of CPU.

// Notable features:
// MTE implementation
// Stackframe (CALL/RET)
// Branching
// Maybe something else

// We'll define some opcodes I had from my previous failed attempts
//  OPCODES
//  0: NOP (does nothing)
#define NOP 0
//  1: JMP (sets PC to the thing in GR1)
#define JMP 1
//  2: ADD (sums GR1 with GR2 and stores in GR1)
#define ADD 2
//  3: SUB (subtracts GR2 from GR1 and stores in GR1)
#define SUB 3
//  4: MUL (stores GR1*GR2 in GR1)
#define MUL 4
//  5: DIV (stores GR1/GR2 in GR1)
#define DIV 5
//  6: MOD (Stores GR1%GR2 in GR1)
#define MOD 6
//  7: AND (stores GR1&GR2 in GR1)
#define AND 7
//  8: OR (stores GR1|GR2 in GR1)
#define OR 8
//  9: XOR (stores GR1^GR2 in GR1)
#define XOR 9
//  a: HLT (halts the thing)
#define HLT 10
//  b: RAMLOAD (loads thing from RAM at MEMADDR into GR1)
#define RAMLOAD 11
//  c: RAMSTORE (stores thing into RAM at MEMADDR that's in GR1)
#define RAMSTORE 12
//  d: MOVTOGR2 (GR1 -> GR2)
#define MOVTOGR2 13
//  e: MOVTOGR3 (GR1 -> GR3)
#define MOVTOGR3 14
//  f: GR2TOGR1 (GR2 -> GR1)
#define GR2TOGR1 15
//  10: GR3TOGR1 (GR3 -> GR1)
#define GR3TOGR1 16
//  11: WRTOMEMADDR (GR1 -> MEMADDR)
#define WRTOMEMADDR 17
//  12: PUSH (pushes to stack from the register specified in 0x(op)Fxxxxx)
#define PUSH 18
//  13: POP (pops off the stack to the register specified in 0x(op)Fxxxxx)
#define POP 19
//  14: SETFLAGS (sets flags from GR1)
#define SETFLAGS 20
//  15: LDIREG (loads into a register specified in 0x(op)Fxxxxx immediately)
#define LDIREG 21
//  16: CALL (calls a method at an address)
#define CALL 22
//  17: RET (returns from a method)
#define RET 23
//  18: JZ (jump to addr if IXR is 0)
#define JZ 24
//  19: GR1TOIXR (GR1 -> IXR)
#define GR1TOIXR 25
//  1a: IXRTOGR1 (IXR -> GR1)
#define IXRTOGR1 26
//  1b: MEMTAG (tag (gr2)+ptr(gr1)=tagged pointer)
#define MEMTAG 27
//  1c: RAND (writes random 32bit garbage to GR1)
#define RAND 28
//  1d: GETFLAGS (FLAGS -> GR1)
#define GETFLAGS 29
//  1e: LFSH (GR1<<n)
#define LFSH 30
//  1f: RGSH (GR1>>n)
#define RGSH 31
//  20: GETMEMST (PROGRAMEND -> GR1)
#define GETMEMST 32
//  21: BPTOGR1 (BP -> GR1)
#define BPTOGR1 33

enum State
{
    FETCH,
    DECODE,
    EXECUTE,
    REGLOAD,
    HALT
};

struct CPU
{
    int RAM[4096];
    int TAG_MEM[4096];
    int PC;         // Program Counter
    int GR1;        // General Register
    int GR2;        // General Register
    int GR3;        // General Register
    int MEMADDR;    // Memory Address
    int IR;         // Instruction Register
    int SP;         // Stack Pointer
    int BP;         // Base Pointer
    int IXR;        // Index Register
    int PROGRAMEND; // Program End (also anchor for the heap)
    int FLAGS;      // Flags
    int CLKCNT;     // Clock Counter
    enum State s;   // Machine state
};
