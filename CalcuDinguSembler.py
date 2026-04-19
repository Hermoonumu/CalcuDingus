from asyncio import sleep


OPCODES = {
    "NOP": 0,
    "JMP": 1,
    "ADD": 2,
    "SUB": 3,
    "MUL": 4,
    "DIV": 5,
    "MOD": 6,
    "AND": 7,
    "OR": 8,
    "XOR": 9,
    "HLT": 10,
    "RAMLOAD": 11,
    "RAMSTORE": 12,
    "MOVTOGR2": 13,
    "MOVTOGR3": 14,
    "GR2TOGR1": 15,
    "GR3TOGR1": 16,
    "WRTOMEMADDR": 17,
    "PUSH": 18,
    "POP": 19,
    "SETFLAGS": 20,
    "GETFLAGS": 21,
    "CALL": 22,
    "RET": 23,
    "JZ": 24,
    "GR1TOIXR": 25,
    "IXRTOGR1": 26,
    "MEMTAG": 27,
    "RAND": 28,
    "LDIREG": 29,
    "LFSH": 30,
    "RGSH": 31,
    "GETMEMST": 32,
    "BPTOREG": 33,
    "DLFSH": 30,
    "DRGSH": 31
}

BARE_OPCODES = {
    "NOP",
    "HLT",
    "MOVTOGR2",
    "MOVTOGR3",
    "GR2TOGR1",
    "GR3TOGR1",
    "RET",
    "GR1TOIXR",
    "IXRTOGR1"
}

REG_OPCODES={
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "MOD",
    "AND",
    "OR",
    "XOR",
    "RAMLOAD",
    "RAMSTORE",
    "PUSH",
    "POP",
    "GETFLAGS",
    "SETFLAGS",
    "MEMTAG",
    "RAND",
    "LDIREG",
    "GETMEMST",
    "BPTOREG"
}

STACKED_OPCODES={
    "JMP",
    "CALL",
    "JZ",
    "WRTOMEMADDR",
    "LFSH",
    "RGSH"
}

DOUBLE_REG_ARG_OPCODES={
    "DLFSH",
    "DRGSH"
}

TWO_CYCLE_INSTRUCTIONS = {
    "LDIREG"
}

# Opcodes that do not accept payload nor do they accept register:
#NOP, HLT, MOVTOGR2, MOVTOGR3, GR2TOGR1, GR3TOGR1, RET, GR1TOIXR, IXRTOGR1


#Opcodes that take only register
#ADD, SUB, MUL, DIV, MOD, AND, OR, XOR, RAMLOAD, RAMSTORE,
#PUSH, POP, GETFLAGS, SETFLAGS, MEMTAG, RAND,
#LDIREG, GETMEMST, BPTOREG

#Opcodes that take all three
#JMP, CALL, JZ, WRTOMEMADDR, LFSH, RGSH 0xOPR00000

#We should implement some operation to jump to some parts of code, something like @HALT

print("Machine code file name: ", end='')
name = input()
output_file = open(name, "w")
print("What are we translating to assembler: ", end='')
name = input()
two_cycle_instr_prog_offset_ctr=0
prog_len=0
two_cycle_instr_prog_offset={}
JUMP_LOOKUP_TABL = dict()
boot = False
with open(name) as f:
    for line in f:
        line = line.split('//')[0].strip()
        if not line: continue

        instr = line.split(' ')
        two_cycle_instr_prog_offset[prog_len]=two_cycle_instr_prog_offset_ctr
        if instr[0]=="BOOT":
            boot = True
            continue
        if '@' in instr[0]:
            JUMP_LOOKUP_TABL[instr[0][1:]] = prog_len+two_cycle_instr_prog_offset_ctr-1
        if instr[0] in TWO_CYCLE_INSTRUCTIONS:
            two_cycle_instr_prog_offset_ctr+=1
        if instr[0] in OPCODES:
            prog_len+=1

print(two_cycle_instr_prog_offset)
print(JUMP_LOOKUP_TABL)

BL_offset=0
with open(name) as f:
    instr_ctr=-1
    for line in f:
        line = line.split('//')[0].strip()
        if not line: continue 
        parts = line.split(' ')
        mnemonic = parts[0]
        if mnemonic == "BOOT":
            BL_offset=4096
            instr_ctr+=1
            continue
        elif instr_ctr == -1 and mnemonic!="BOOT":
            output_file.write(f"0x{(OPCODES['NOP']<<24|prog_len+two_cycle_instr_prog_offset_ctr+1):08X}\n")
            instr_ctr+=1
        if mnemonic in OPCODES:
            instruction = OPCODES[mnemonic] << 24
            if mnemonic in BARE_OPCODES:
                print(mnemonic)
                pass
            
            if mnemonic in REG_OPCODES:
                instruction |= int(parts[1], 0)<<20
                print(mnemonic)
                if mnemonic in TWO_CYCLE_INSTRUCTIONS:
                    output_file.write(f"0x{instruction:08X}\n")
                    output_file.write(f"{int(parts[2])}\n")
                    continue
            if mnemonic in DOUBLE_REG_ARG_OPCODES:
                instruction |= int(parts[1], 0)<<20
                instruction |= int(parts[2], 0)<<16
                print(mnemonic)
                output_file.write(f"0x{instruction:08X}\n")
                continue
            if mnemonic in STACKED_OPCODES:
                print(mnemonic)
                instruction |= int(parts[1], 0)<<20
                if mnemonic in {"CALL", "JZ", "JMP"}:
                    if '@' in parts[2]:
                        instruction |= JUMP_LOOKUP_TABL[parts[2][1:]]+BL_offset+1
                    elif int(parts[2], 0) >= 4096 and boot:
                        instruction |= int(parts[2], 0)+two_cycle_instr_prog_offset[int(parts[2], 0)-4096]+BL_offset
                    elif int(parts[2], 0) >= 4096:
                        instruction |= int(parts[2], 0)+BL_offset
                    else: 
                        instruction |= int(parts[2], 0)+two_cycle_instr_prog_offset[int(parts[2], 0)]+BL_offset
                else:
                    instruction |= int(parts[2], 0)
                
            # Write as a decimal integer followed by a newline
            output_file.write(f"0x{instruction:08X}\n")
        instr_ctr+=1


output_file.close()