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

# MMIO

- RAM address 4095 accepts data, which will be displayed in the terminal on the same cycle.

# Notable features

- MTE: Hardware endorsed memory integrity
- Segmented Memory Protection: PROGRAMEND is an anchor of where memory can be written and where it can't be
- Stack: Fully implemented CALL/RET with stackframes for your recursive shenanigans
- RNG: Get a random garbage tailored for you using RAND
