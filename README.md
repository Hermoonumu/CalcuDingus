# CalcuDingus

"CalcuDingus: An emulated CPU of my own deranged design."
-- Me, 2026

# System specs

- Word size: 32 bits (4bytes)
- Memory 4096 words \* 4 bytes = 16384 bytes
- Word-addressable (4bytes)
- Execution: Three-stage pipeline (FETCH-DECODE-EXECUTE)

# Arch & Registers

- General Purpose: GR1, GR2 and GR3
- Indexing: IXR
- Program Execution: PC (Program Counter) and IR (Instruction Register)
- Stack: SP (Stack Pointer) and BP (Base Pointer)
- Memory Interfacing: MEMADDR (Effective mem address)
- PROGRAMEND: Segmentation of TEXT and DATA
- Misc: FLAGS (System State) and CLKCNT (Clock Counter)

# Memory topography

For the CPU an output device, a BootROM or memory at all is just a memory address. The table lower will show you how the memory is mapped:

| Name        | Start      | End        | Description                                                                 |
| :---------- | :--------- | :--------- | :-------------------------------------------------------------------------- |
| **Program** | 0          | PROGRAMEND | Write-protected section of memory containing the program                    |
| **Heap**    | PROGRAMEND | SP         | Your heap. RW protected unless you MALLOC some of it (MTE enforced)         |
| **Stack**   | SP         | 0xFFA      | Your stack. PUSH and POP your data here.                                    |
| **MMIO**    | 0xFFB      | 0xFFF      | The MMIO. So far only 0xFFF does something.                                 |
| **BootROM** | 0x1000     | 0x1FFF     | The kernel part. Always privileged. Can do RW everywhere. Contains SYSCALLS |

# OPCODES

0xIIRPPPPP
| Instruction | Register | Payload |
| :------ | :---- | :----------- |
| 31 → 24 | 23 → 20 | 19 → 0

This CPU is kiinda a RISC-like, so ISA, inherently, somewhat represents it. Below you can see OPCODES classified by their nature. In an instruction they are represented by two MSB (Most Significant Byte) nibbles nibbles. Third nibble accepts a destination register to write the result to. All the operations are usually performed using GR1 and GR2 (unless specified otherwise).

## Logic

| OPCODE  | HEX   | Description  |
| :------ | :---- | :----------- |
| **AND** | 0x07R | GR1&GR2 → R  |
| **OR**  | 0x08R | GR1\|GR2 → R |
| **XOR** | 0x09R | GR1^GR2 → R  |

## Arithmetic

| OPCODE   | HEX        | Description  |
| :------- | :--------- | :----------- |
| **ADD**  | 0x02R      | GR1+GR2 → R  |
| **SUB**  | 0x03R      | GR1-GR2 → R  |
| **MUL**  | 0x04R      | GR1\*GR2 → R |
| **DIV**  | 0x05R      | GR1/GR2 → R  |
| **MOD**  | 0x06R      | GR1%GR2 → R  |
| **LFSH** | 0x1eRPPPPP | R << Payload |
| **RGSH** | 0x1fRPPPPP | R >> Payload |

## Flow control

| OPCODE   | HEX        | Description                                                                                                   |
| :------- | :--------- | :------------------------------------------------------------------------------------------------------------ |
| **JMP**  | 0x02RPPPPP | PC ← R (or PC ← Payload, if no register)                                                                      |
| **HLT**  | 0x0a       | Halts the CPU                                                                                                 |
| **CALL** | 0x16RPPPPP | Saves PC and BP to move to a new Frame stack, jumps to the address in register or in payload (if no register) |
| **RET**  | 0x17       | Collapses callee's stack and returns control to the caller                                                    |
| **JZ**   | 0x18RPPPPP | Jumps to the address in register or payload (if no register), if IXR is 0                                     |

## Memory

| OPCODE          | HEX       | Description                                                                                                                                                                                          |
| :-------------- | :-------- | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **RAMLOAD**     | 0x0bR     | Loads the thing at RAM[MEMADDR] into register                                                                                                                                                        |
| **RAMSTORE**    | 0x0cR     | Stores the thing in register at RAM[MEMADDR]                                                                                                                                                         |
| **WRTOMEMADDR** | 0x11RPPPP | Writes to MEMADDR the thing at the register or the payload (if no register)                                                                                                                          |
| **MEMTAG**      | 0x1bR     | Performs an operation at GR1 (address) and GR2 (tag) to create a tagged pointer. Tags the memory. Future access w/o this pointer is forbidden. Writes pointer to MEMADDR and register (if specified) |
| **GETMEMST**    | 0x20R     | Puts PROGRAMEND to specified register                                                                                                                                                                |
| **BPTOREG**     | 0x21R     | Puts current stack frame base pointer into specified register                                                                                                                                        |

## Stack

| OPCODE   | HEX   | Description                                     |
| :------- | :---- | :---------------------------------------------- |
| **PUSH** | 0x12R | Pushes the thing into the register to the stack |
| **POP**  | 0x13R | Pops the thing into the register from the stack |

## Register manipulation

| OPCODE       | HEX              | Description                                                                                                                                                  |
| :----------- | :--------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **MOVTOGR2** | 0x0d             | Stores GR1 in GR2                                                                                                                                            |
| **MOVTOGR3** | 0x0e             | Stores GR1 in GR3                                                                                                                                            |
| **GR2TOGR1** | 0x0f             | Stores GR2 in GR1                                                                                                                                            |
| **GR3TOGR1** | 0x10             | Stores GR3 in GR1                                                                                                                                            |
| **GR1TOIXR** | 0x19             | Stores GR1 in IXR                                                                                                                                            |
| **IXRTOGR1** | 0x1a             | Stores IXR in GR1                                                                                                                                            |
| **LDIREG**   | 0x19R 0xPPPPPPPP | Two-cycle operation. The first operation specifies the register to immediately load, the second is a raw payload. CPU handles it via FLAGS to set its state. |

## MISC

| OPCODE       | HEX          | Description                                                                                                                                                                                                                                                          |
| :----------- | :----------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **NOP**      | 0x00(PPPPPP) | Does absolutely nothing. But it is obligatory to be the first opcode of a user program. Payload specifies the length of the program so that the CPU knows TEXT section bounds. If the first OPCODE isn't NOP, CPU will halt. Automatically appended by the assembler |
| **SETFLAGS** | 0x14R        | Alters CPU state by pushing the flags (I wanna delete this, big security concern)                                                                                                                                                                                    |
| **GETFLAGS** | 0x15R        | Puts CPU FLAGS to the register                                                                                                                                                                                                                                       |
| **RAND**     | 0x1cR        | Puts random 32bit garbage to the register                                                                                                                                                                                                                            |

# MMU and Memory Security

The CPU is kind of security and protection oriented (because I want it to be). Lower you will find some of the security features implemented (or are yet to be implemented):

- Memory Tagging Extension (MTE) &mdash; 2 of the MSB nibbles in a pointer are reserved for a tag. Upon tagging a memory, word in RAM it's no more accessible without the tagged pointer. Mitigates Use-after-free attacks (where a program leaves its memory after FREE-ing memory) (Not yet done, but I plan on restricting all RW to untagged memory, forcing a program to go through my MALLOC syscall to get some memory, strengthening the security).

- Write XOR Execute &mdash; forbids any write to TEXT section of memory (so the program). Read is allowed tho.

- Execute-only Memory (BootROM lockdown) &mdash; real hardware has its BootROM etched in a silicon. The only thing that the end customer has an ABI and specifications on how to operate the CPU. This section of memory cannot be read nor written to from userspace.

# MMIO

- RAM address 4095 accepts data, which will be displayed in the terminal on the same cycle.

# Toolchain

Of course you can write pure assembly (I did so in the early stages of my V1 CalcuDingus), but it's EXTREMELY tiring. Thorugh blood and tears I've came up with a sloppy assembler. See below:

- Two-pass assembler &mdash; passes through assembly two times. First time to resolve the jump labels, the second time to calculate offsets and correctly jump to an address (because two-cycle operations, that are a single one).
- Debugging &mdash; upon setting DEBUG flag in emulator it'll dump all the registers and the stack trace to the console for debugging.

# Program examples

I've created a few programs for you to experience (killed like 17 hours, assembly is hard send help).

- PRINT routine (CALL 0 4096) &mdash; Push your pointer to as string in memory to the stack and call print routine. It'll print all the characters from the string. (View bootloader.hcp, @PRINT)

- MALLOC routine (CALL 0 4097) &mdash; allocates and tags the specified amount of words that you push to the stack as an argument. Returns the pointer to the heap in GR1 register. (View bootloader.hcp, @MALLOC)
