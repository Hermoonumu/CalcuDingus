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
    "BPTOREG": 33
}

output_file = open("machine.ohcp", "w")
print("What are we translating to assembler: ")
name = input()
with open(name) as f:
    for line in f:
        line = line.split('//')[0].strip()
        if not line: continue 
        
        parts = line.split(' ')
        mnemonic = parts[0]
        
        if mnemonic in OPCODES:
            instruction = OPCODES[mnemonic] << 24
            
            
            
            if len(parts) == 3:
                temp1 = int(parts[1], 0) << 12
                temp2 = int(parts[2], 0)
                instruction |= (temp1 | temp2)

            # Write as a decimal integer followed by a newline
            output_file.write(f"{instruction}\n")
        if (mnemonic.isdigit()):
            output_file.write(f"{int(mnemonic)}\n")


output_file.close()