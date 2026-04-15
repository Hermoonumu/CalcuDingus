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
//  1: JMP (sets PC to the thing in 0x(op)Fxxxxx)
#define JMP 1
//  2: ADD (sums GR1 with GR2 and stores in 0x(op)Fxxxxx)
#define ADD 2
//  3: SUB (subtracts GR2 from GR1 and stores in 0x(op)Fxxxxx)
#define SUB 3
//  4: MUL (stores GR1*GR2 in 0x(op)Fxxxxx)
#define MUL 4
//  5: DIV (stores GR1/GR2 in 0x(op)Fxxxxx)
#define DIV 5
//  6: MOD (Stores GR1%GR2 in 0x(op)Fxxxxx)
#define MOD 6
//  7: AND (stores GR1&GR2 in 0x(op)Fxxxxx)
#define AND 7
//  8: OR (stores GR1|GR2 in 0x(op)Fxxxxx)
#define OR 8
//  9: XOR (stores GR1^GR2 in 0x(op)Fxxxxx)
#define XOR 9
//  a: HLT (halts the thing)
#define HLT 10
//  b: RAMLOAD (loads thing from RAM at MEMADDR into the register specified in 0x(op)Fxxxxx)
#define RAMLOAD 11
//  c: RAMSTORE (stores thing into RAM at MEMADDR that's in the register specified in 0x(op)Fxxxxx)
#define RAMSTORE 12
//  d: MOVTOGR2 (GR1 -> GR2)
#define MOVTOGR2 13
//  e: MOVTOGR3 (GR1 -> GR3)
#define MOVTOGR3 14
//  f: GR2TOGR1 (GR2 -> GR1)
#define GR2TOGR1 15
//  10: GR3TOGR1 (GR3 -> GR1)
#define GR3TOGR1 16
//  11: WRTOMEMADDR (0x(op)Fxxxxx -> MEMADDR)
#define WRTOMEMADDR 17
//  12: PUSH (pushes to stack from the register specified in 0x(op)Fxxxxx)
#define PUSH 18
//  13: POP (pops off the stack to the register specified in 0x(op)Fxxxxx)
#define POP 19
//  14: SETFLAGS (sets flags from 0x(op)Fxxxxx)
#define SETFLAGS 20
//  15: LDIREG (loads into a register specified in 0x(op)Fxxxxx immediately) [two-cycle operation]
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
//  1c: RAND (writes random 32bit garbage to 0x(op)Fxxxxx)
#define RAND 28
//  1d: GETFLAGS (FLAGS -> 0x(op)Fxxxxx)
#define GETFLAGS 29
//  1e: LFSH (0x(op)Fxxxxx<<n)
#define LFSH 30
//  1f: RGSH (0x(op)Fxxxxx>>n)
#define RGSH 31
//  20: GETMEMST (PROGRAMEND -> 0x(op)Fxxxxx)
#define GETMEMST 32
//  21: BPTOREG (BP -> 0x(op)Fxxxxx)
#define BPTOREG 33

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

    // Now (usually) private CPU stuff
    int Operand1;
    int Operand2;
    int DestReg;
};

void FetchInstruction(struct CPU *core)
{
}

void DecodeInstruction(struct CPU *core)
{
}

void ExecuteInstruction(struct CPU *core)
{
}

void LoadRegister(struct CPU *core)
{
}

int main()
{
    struct CPU core = {0}; // Our core
    core.s = FETCH;        // We manually set the state

    // Stack init
    core.BP = 4090;
    core.SP = 4090;

    while (core.s != HALT)
    {
        switch (core.s)
        {
        case FETCH:
            FetchInstruction(&core);
            core.s = DECODE;
            break;
        case DECODE:
            DecodeInstruction(&core);
            core.s = EXECUTE;
            break;
        case EXECUTE:
            ExecuteInstruction(&core);
            core.s = FETCH;
            break;
        case REGLOAD:
            LoadRegister(&core);
            core.s = FETCH;
            break;
        }
        core.CLKCNT++;
    }

    return 0;
}