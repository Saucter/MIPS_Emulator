#include <stdint.h> /* uint32_t */
#include <stdio.h>	/* fprintf(), printf() */
#include <stdlib.h> /* atoi() */

#include "RegFile.h"
#include "Syscall.h"
#include "elf_reader/elf_reader.h"
#include "utils/heap.h"

uint32_t ProgramCounter;

typedef struct RType
{
	uint32_t opcode;
	uint32_t rs;
	uint32_t rt;
	uint32_t rd;
	uint32_t shamt;
	uint32_t funct;
} RType;

typedef struct IType
{
	uint32_t opcode;
	uint32_t rs;
	uint32_t rt;
	uint32_t immediate;
} IType;

typedef struct JType
{
	uint32_t opcode;
	uint32_t address;
} JType;

int main(int argc, char *argv[])
{

	/*
	 * This variable will store the maximum
	 * number of instructions to run before
	 * forcibly terminating the program. It
	 * is set via a command line argument.
	 */
	uint32_t MaxInstructions;

	/*
	 * This variable will store the address
	 * of the next instruction to be fetched
	 * from the instruction memory.
	 */
	ProgramCounter = exec.GPC_START;

	/*
	 * This variable will store the instruction
	 * once it is fetched from instruction memory.
	 */
	uint32_t CurrentInstruction;

	// IF THE USER HAS NOT SPECIFIED ENOUGH COMMAND LINE ARUGMENTS
	if (argc < 3)
	{

		// PRINT ERROR AND TERMINATE
		fprintf(stderr, "ERROR: Input argument missing!\n");
		fprintf(stderr, "Expected: file-name, max-instructions\n");
		return -1;
	}

	// CONVERT MAX INSTRUCTIONS FROM STRING TO INTEGER
	MaxInstructions = atoi(argv[2]);

	// Open file pointers & initialize Heap & Regsiters
	initHeap();
	initFDT();
	initRegFile(0);

	// LOAD ELF FILE INTO MEMORY AND STORE EXIT STATUS
	int status = LoadOSMemory(argv[1]);

	// IF LOADING FILE RETURNED NEGATIVE EXIT STATUS
	if (status < 0)
	{
		// PRINT ERROR AND TERMINATE
		fprintf(stderr, "ERROR: Unable to open file at %s!\n", argv[1]);
		return status;
	}

	printf("\n ----- BOOT Sequence ----- \n");
	printf("Initializing sp=0x%08x; gp=0x%08x; start=0x%08x\n", exec.GSP, exec.GP,
		   exec.GPC_START);

	RegFile[28] = exec.GP;
	RegFile[29] = exec.GSP;
	RegFile[31] = exec.GPC_START;

	printRegFile();

	printf("\n ----- Execute Program ----- \n");
	printf("Max Instruction to run = %d \n", MaxInstructions);
	fflush(stdout);

	int i;
	for (i = 0; i < MaxInstructions; i++)
	{
		CurrentInstruction = readWord(
			ProgramCounter, false); // Fetch instruction at 'ProgramCounter'

		printRegFile();

		uint32_t initOpcode = (CurrentInstruction >> 26) & 0x3F;

		if (initOpcode == 0x00)
		{
			executeR(decodeR(CurrentInstruction));
		}
		else if (initOpcode == 0x02 || initOpcode == 0x03)
			decodeJ(CurrentInstruction);
		else
			decodeI(CurrentInstruction);
	}

	printRegFile(); // Print the final contents of the register file
	closeFDT();		// Close file pointers & free allocated Memory
	CleanUp();

	return 0;
}

RType decodeR(uint32_t inst)
{
	RType rInstruction = {
		(inst >> 26) & 0x3F, // opcode
		(inst >> 21) & 0x1F, // rs
		(inst >> 16) & 0x1F, // rt
		(inst >> 11) & 0x1F, // rd
		(inst >> 6) & 0x1F,	 // shamt
		(inst & 0x3F)		 // funct
	};

	return rInstruction;
}

IType decodeI(uint32_t inst)
{
	IType iInstruction = {
		(inst >> 26) & 0x3F, // opcode
		(inst >> 21) & 0x1F, // rs
		(inst >> 16) & 0x1F, // rt
		inst & 0xFFFF		 // immediate
	};

	return iInstruction;
}

uint32_t decodeJ(uint32_t inst) {}

uint32_t decodeSpecial(uint32_t inst) {}

void executeR(RType r)
{
	uint32_t res = 0;
	bool writeRegister = true;
	int64_t product = 0;
	uint64_t uProduct = 0;

	switch (r.funct)
	{
	case 0x20: // add
		res = RegFile[r.rs] + RegFile[r.rt];
		break;

	case 0x21: // addu
		res = (uint32_t)RegFile[r.rs] + (uint32_t)RegFile[r.rt];
		break;

	case 0x22: // sub
		res = RegFile[r.rs] - RegFile[r.rt];
		break;

	case 0x23: // subu
		res = (uint32_t)RegFile[r.rs] - (uint32_t)RegFile[r.rt];
		break;

	case 0x24: // and
		res = RegFile[r.rs] & RegFile[r.rt];
		break;

	case 0x25: // or
		res = RegFile[r.rs] | RegFile[r.rt];
		break;

	case 0x26: // xor
		res = RegFile[r.rs] ^ RegFile[r.rt];
		break;

	case 0x27: // nor
		res = ~(RegFile[r.rs] | RegFile[r.rt]);
		break;

	case 0x2A: // slt
		res = (RegFile[r.rs] < RegFile[r.rt]) ? 1 : 0;
		break;

	case 0x2B: // sltu
		res = ((uint32_t)RegFile[r.rs] < (uint32_t)RegFile[r.rt]) ? 1 : 0;
		break;

	case 0x00: // sll
		res = (uint32_t)RegFile[r.rt] << r.shamt;
		break;

	case 0x02: // srl
		res = (uint32_t)RegFile[r.rt] >> r.shamt;
		break;

	case 0x03: // sra
		res = RegFile[r.rt] >> r.shamt;
		break;

	case 0x04: // sllv
		res = (uint32_t)RegFile[r.rt] << ((uint32_t)RegFile[r.rs] & 0x1F);
		break;

	case 0x06: // srlv
		res = (uint32_t)RegFile[r.rt] >> ((uint32_t)RegFile[r.rs] & 0x1F);
		break;

	case 0x07: // srav
		res = RegFile[r.rt] >> (RegFile[r.rs] & 0x1F);
		break;

	case 0x10: // mfhi
		res = RegFile[32];
		break;

	case 0x11: // mthi
		RegFile[32] = RegFile[r.rs];
		break;

	case 0x12: // mflo
		res = RegFile[33];
		break;

	case 0x13: // mtlo
		RegFile[33] = RegFile[r.rs];
		break;

	case 0x18: // mult
		product = (int64_t)RegFile[r.rs] * (int64_t)RegFile[r.rt];
		RegFile[33] = (int32_t)(product & 0xFFFFFFFF);		   // LO
		RegFile[32] = (int32_t)((product >> 32) & 0xFFFFFFFF); // HI
		break;

	case 0x19: // multu
		uProduct = (uint64_t)RegFile[r.rs] * (uint64_t)RegFile[r.rt];
		RegFile[33] = (uint32_t)(uProduct & 0xFFFFFFFF);		 // LO
		RegFile[32] = (uint32_t)((uProduct >> 32) & 0xFFFFFFFF); // HI
		break;

	case 0x1A: // div
		product = (int64_t)RegFile[r.rs] / (int64_t)RegFile[r.rt];
		RegFile[33] = (int32_t)(product & 0xFFFFFFFF);		   // LO
		RegFile[32] = (int32_t)((product >> 32) & 0xFFFFFFFF); // HI
		break;

	case 0x1B: // divu
		uProduct = (uint64_t)RegFile[r.rs] * (uint64_t)RegFile[r.rt];
		RegFile[33] = (uint32_t)(uProduct & 0xFFFFFFFF);		 // LO
		RegFile[32] = (uint32_t)((uProduct >> 32) & 0xFFFFFFFF); // HI
		break;

	case 0x08: // jr
		ProgramCounter = RegFile[r.rs];
		break;

	case 0x09: // jalr
		res = ProgramCounter + 4;
		ProgramCounter = RegFile[r.rs];
		break;

	default:
		writeRegister = false;
		break;
	}

	if (writeRegister && r.rd != 0)
	{
		RegFile[r.rd] = res;
		product = 0;
		uProduct = 0;
	}
}

void executeI(IType i)
{
	switch (i.opcode)
	{
	case 0x08: // addi
		break;

	case 0x09: // addiu
		break;

	case 0x0C: // andi
		break;

	case 0x0D: // ori
		break;

	case 0x0E: // xori
		break;

	case 0x0A: // slti
		break;

	case 0x0B: // sltiu
		break;

	case 0x04: // beq
		break;

	case 0x05: // bne
		break;

	case 0x20: // lb
		break;

	case 0x24: // lbu
		break;

	case 0x21: // lh
		break;

	case 0x25: // lhu
		break;

	case 0x0F: // lui
		break;

	case 0x23: // lw
		break;

	case 0x28: // sb
		break;

	case 0x29: // sh
		break;

	case 0x2B: // sw
		break;

	default:
		break;
	}
}