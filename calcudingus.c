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
//  15: GETFLAGS (FLAGS -> 0x(op)Fxxxxx)
#define GETFLAGS 21
//  16: CALL (calls a method at an address)
#define CALL 22
//  17: RET (returns from a method)
#define RET 23
//  18: JZ (jump to addr in payload or to 0x(op)Fxxxxx if IXR is 0)
#define JZ 24
//  19: GR1TOIXR (GR1 -> IXR)
#define GR1TOIXR 25
//  1a: IXRTOGR1 (IXR -> GR1)
#define IXRTOGR1 26
//  1b: MEMTAG (tag (gr2)+ptr(gr1)=tagged pointer, writes it to MEMADDR as well as to 0x(op)Fxxxxx)
#define MEMTAG 27
//  1c: RAND (writes random 32bit garbage to 0x(op)Fxxxxx)
#define RAND 28
//  1d: LDIREG (loads into a register specified in 0x(op)Fxxxxx immediately) [two-cycle operation]
#define LDIREG 29
//  1e: LFSH (0x(op)Fxxxxx<<n)
#define LFSH 30
//  1f: RGSH (0x(op)Fxxxxx>>n)
#define RGSH 31
//  20: GETMEMST (PROGRAMEND -> 0x(op)Fxxxxx)
#define GETMEMST 32
//  21: BPTOREG (BP -> 0x(op)Fxxxxx)
#define BPTOREG 33

// FLAGS
//  00000000 -- nothing extraordinary
//  00000001 -- Pending console output
//  00000002 -- Popping attempt failed (stack empty)
//  00000004 -- Tried overwriting the program
//  00000008 -- MTE violation
//  00000010 -- Performing a register immediate load
//  00000020 -- Division by zero

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
    unsigned int RAM[4096];
    unsigned int TAG_MEM[4096];
    unsigned int PC;         // Program Counter
    unsigned int GR1;        // General Register
    unsigned int GR2;        // General Register
    unsigned int GR3;        // General Register
    unsigned int MEMADDR;    // Memory Address
    unsigned int IR;         // Instruction Register
    unsigned int SP;         // Stack Pointer
    unsigned int BP;         // Base Pointer
    unsigned int IXR;        // Index Register
    unsigned int PROGRAMEND; // Program End (also anchor for the heap)
    unsigned int FLAGS;      // Flags
    unsigned int CLKCNT;     // Clock Counter
    enum State s;            // Machine state

    // Now (usually) private CPU stuff
    unsigned int Operand1;
    unsigned int Operand2;
    unsigned int Payload;
    unsigned int DestReg;
};

void FetchInstruction(struct CPU *core)
{
    core->IR = core->RAM[core->PC];
    core->PC++;
}

void DecodeInstruction(struct CPU *core)
{
    if ((core->FLAGS & 0x10) == 0x0)
    {
        core->Operand1 = (core->GR1);
        core->Operand2 = (core->GR2);
        core->DestReg = ((core->IR) & 0x00F00000) >> 20;
        core->Payload = (core->IR) & 0x000FFFFF;
        core->IR = ((core->IR) & 0xFF000000) >> 24;
    }
    else
    {
        core->Payload = core->IR;
        core->IR = LDIREG;
    }
}

unsigned int *RegisterSelector(unsigned int reg, struct CPU *core)
{
    switch (reg)
    {
    case 1:
        return &core->GR1;
    case 2:
        return &core->GR2;
    case 3:
        return &core->GR3;
    case 4:
        return &core->MEMADDR;
    case 5:
        return &core->IXR;
    case 6:
        return &core->BP;
    case 7:
        return &core->PROGRAMEND;
    case 8:
        return &core->FLAGS;
    case 9:
        return &core->CLKCNT;
    default:
        return 0;
    }
}

void ExecuteInstruction(struct CPU *core)
{
    switch (core->IR)
    {
    case NOP:
        break;
    case JMP:
    {
        unsigned int *JmpTo = RegisterSelector(core->DestReg, core);
        if (JmpTo == 0)
        {
            core->PC = core->Payload;
            break;
        }
        core->PC = *JmpTo;
        break;
    }
    case ADD:
    {
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (Dest == 0)
        {
            core->GR1 = core->Operand1 + core->Operand2;
            break;
        }
        *Dest = core->Operand1 + core->Operand2;
        break;
    }
    case SUB:
    {
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (Dest == 0)
        {
            core->GR1 = core->Operand1 - core->Operand2;
            break;
        }
        *Dest = core->Operand1 - core->Operand2;
        break;
    }
    case MUL:
    {
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (Dest == 0)
        {
            core->GR1 = core->Operand1 * core->Operand2;
            break;
        }
        *Dest = core->Operand1 * core->Operand2;
        break;
    }
    case DIV:
    {
        if (core->Operand2 == 0)
        {
            core->FLAGS = core->FLAGS | 0x00000020;
            break;
        }
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (Dest == 0)
        {
            core->GR1 = core->Operand1 / core->Operand2;
            break;
        }
        *Dest = core->Operand1 / core->Operand2;
        core->FLAGS = core->FLAGS & 0xFFFFFFDF;
        break;
    }
    case MOD:
    {
        if (core->Operand2 == 0)
        {
            core->FLAGS = core->FLAGS | 0x00000020;
            break;
        }
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (Dest == 0)
        {
            core->GR1 = core->Operand1 % core->Operand2;
            break;
        }
        *Dest = core->Operand1 % core->Operand2;
        core->FLAGS = core->FLAGS & 0xFFFFFFDF;
        break;
    }
    case AND:
    {
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (Dest == 0)
        {
            core->GR1 = core->Operand1 & core->Operand2;
            break;
        }
        *Dest = core->Operand1 & core->Operand2;
        break;
    }
    case OR:
    {
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (Dest == 0)
        {
            core->GR1 = core->Operand1 | core->Operand2;
            break;
        }
        *Dest = core->Operand1 | core->Operand2;
        break;
    }
    case XOR:
    {
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (Dest == 0)
        {
            core->GR1 = core->Operand1 ^ core->Operand2;
            break;
        }
        *Dest = core->Operand1 ^ core->Operand2;
        break;
    }
    case HLT:
        core->s = HALT;
        break;
    case RAMLOAD:
    {
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (core->TAG_MEM[core->MEMADDR & 0x00000FFF] != (core->MEMADDR & 0xFF000000) >> 24 &&
            core->TAG_MEM[core->MEMADDR & 0x00000FFF] != 0)
        {
            core->FLAGS = core->FLAGS | 0x00000008;
            break;
        }
        if (Dest == 0)
        {
            core->GR1 = core->RAM[core->MEMADDR & 0x00000FFF];
            break;
        }
        *Dest = core->RAM[core->MEMADDR & 0x00000FFF];
        core->FLAGS = core->FLAGS & 0xFFFFFFF7;
        break;
    }
    case RAMSTORE:
    {
        unsigned int *Src = RegisterSelector(core->DestReg, core);
        if (core->TAG_MEM[core->MEMADDR & 0x00000FFF] != (core->MEMADDR & 0xFF000000) >> 24 &&
            core->TAG_MEM[core->MEMADDR & 0x00000FFF] != 0)
        {
            core->FLAGS = core->FLAGS | 0x00000008;
            break;
        }
        if (core->MEMADDR & 0x00000FFF <= core->PROGRAMEND)
        {
            core->FLAGS = core->FLAGS | 0x00000004;
            break;
        }
        if (Src == 0)
        {
            core->RAM[core->MEMADDR & 0x00000FFF] = core->GR1;
            break;
        }
        core->RAM[core->MEMADDR & 0x00000FFF] = *Src;
        core->FLAGS = core->FLAGS & 0xFFFFFFF7;
        core->FLAGS = core->FLAGS & 0xFFFFFFFB;
        break;
    }
    case MOVTOGR2:
    {
        core->GR2 = core->GR1;
        break;
    }
    case MOVTOGR3:
    {
        core->GR3 = core->GR1;
        break;
    }
    case GR2TOGR1:
    {
        core->GR1 = core->GR2;
        break;
    }
    case GR3TOGR1:
    {
        core->GR1 = core->GR3;
        break;
    }
    case WRTOMEMADDR:
    {
        unsigned int *Src = RegisterSelector(core->DestReg, core);
        if (Src == 0)
        {
            core->MEMADDR = core->Payload;
            break;
        }
        core->MEMADDR = *Src;
        break;
    }
    case PUSH:
    {
        core->FLAGS = core->FLAGS & 0xFFFFFFFD;
        unsigned int *Src = RegisterSelector(core->DestReg, core);
        if (Src == 0)
        {
            core->RAM[core->SP] = core->GR1;
            core->SP--;
            break;
        }
        core->RAM[core->SP] = *Src;
        core->SP--;
        break;
    }
    case POP:
    {
        core->SP++;
        if (core->SP == core->BP + 1)
        {
            core->FLAGS = core->FLAGS | 0x00000002;
            core->SP--;
            break;
        }
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (Dest == 0)
        {
            core->GR1 = core->RAM[core->SP];
            break;
        }
        *Dest = core->RAM[core->SP];
        break;
    }
    case SETFLAGS:
    {
        unsigned int *Src = RegisterSelector(core->DestReg, core);
        if (Src == 0)
        {
            core->FLAGS = core->GR1;
            break;
        }
        core->FLAGS = *Src;
        break;
    }
    case LDIREG:
    {
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (!Dest)
        {
            Dest = &core->GR1;
        }
        if (core->FLAGS & 0x10)
        {
            *Dest = core->Payload;
            core->FLAGS = core->FLAGS & 0xFFFFFFEF;
        }
        else
        {
            core->FLAGS = core->FLAGS | 0x10;
        }
        break;
    }
    case CALL:
    {
        core->RAM[core->SP] = core->PC;
        core->SP--;
        core->RAM[core->SP] = core->BP;
        core->SP--;
        core->BP = core->SP;
        core->PC = core->Payload;
        break;
    }
    case RET:
    {
        core->SP = core->BP;
        core->BP = core->RAM[core->SP + 1];
        core->PC = core->RAM[core->SP + 2];
        core->SP += 2;
        break;
    }
    case JZ:
    {
        if (core->IXR == 0)
        {
            unsigned int *Dest = RegisterSelector(core->DestReg, core);
            if (Dest == 0)
            {
                core->PC = core->Payload;
                break;
            }
            core->PC = *Dest;
            break;
        }
        break;
    }
    case GR1TOIXR:
    {
        core->IXR = core->GR1;
        break;
    }
    case IXRTOGR1:
    {
        core->GR1 = core->IXR;
        break;
    }
    case MEMTAG:
    {
        unsigned int TaggedPointer = (core->Operand1 & 0x00000FFF) | (core->Operand2 & 0x000000FF) << 24;
        core->MEMADDR = TaggedPointer;
        unsigned int *Src = RegisterSelector(core->DestReg, core);
        if (Src)
        {
            *Src = TaggedPointer;
        }
        core->TAG_MEM[TaggedPointer & 0x00000FFF] = TaggedPointer >> 24;
        break;
    }
    case RAND:
    {
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (Dest == 0)
        {
            core->GR1 = rand();
            break;
        }
        *Dest = rand();
        break;
    }
    case GETFLAGS:
    {
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (Dest == 0)
        {
            core->GR1 = core->FLAGS;
            break;
        }
        *Dest = core->FLAGS;
        break;
    }
    case LFSH:
    {
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (Dest == 0)
        {
            core->GR1 = core->Operand1 << core->Payload;
            break;
        }
        *Dest = *Dest << core->Payload;
        break;
    }
    case RGSH:
    {
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (Dest == 0)
        {
            core->GR1 = core->Operand1 >> core->Payload;
            break;
        }
        *Dest = *Dest >> core->Payload;
        break;
    }
    case GETMEMST:
    {
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (Dest == 0)
        {
            core->GR1 = core->PROGRAMEND;
            break;
        }
        *Dest = core->PROGRAMEND;
        break;
    }
    case BPTOREG:
    {
        unsigned int *Dest = RegisterSelector(core->DestReg, core);
        if (Dest == 0)
        {
            core->GR1 = core->BP;
            break;
        }
        *Dest = core->BP;
        break;
    }
    default:
        break;
    }
}

unsigned int main()
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
            if (core.s != HALT)
            {
                core.s = FETCH;
            }
            break;
        }
        core.CLKCNT++;
    }

    return 0;
}