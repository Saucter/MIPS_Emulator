#include <stdio.h>
#include <stdint.h>
#include <math.h>

uint32_t lwl(uint8_t *memory, uint32_t addr, uint32_t rt, uint32_t offset);
uint32_t lwr(uint8_t *memory, uint32_t addr, uint32_t rt, uint32_t offset);

int main()
{
    // Simulated memory starting at address 0x1000
    uint8_t memory[7] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00};

    // Simulate RegFile
    uint32_t rt = 0x12345678;

    // Simulate i.rs + i.immediate = 0x1002
    uint32_t offset = 0;
    uint32_t addr = 0x00 + offset;
    uint32_t byteOffset = 4 - offset % 4;

    // Print details
    printf("Original rt:      0x%08X\n", rt);
    printf("Memory bytes:     [ %02X %02X %02X %02X ]\n", memory[0], memory[1], memory[2], memory[3]);
    printf("Address:          0x%08X\n", addr);
    printf("Byte offset:      %d\n", byteOffset);

    rt = lwl(memory, addr, rt, 3);
    printf("Result after lwl: 0x%08X\n", rt);

    printf("\n\n\nOriginal rt:      0x%08X\n", rt);
    printf("Memory bytes:     [ %02X %02X %02X %02X ]\n", memory[0], memory[1], memory[2], memory[3]);
    printf("Address:          0x%08X\n", addr);
    printf("Byte offset:      %d\n", byteOffset);

    rt = lwr(memory, addr, rt, 4);
    printf("Result after lwl: 0x%08X\n", rt);

    return 0;
}

uint32_t lwl(uint8_t *memory, uint32_t addr, uint32_t rt, uint32_t offset)
{
    // Emulate lwl logic
    uint32_t byteOffset = 4 - offset % 4;
    addr = addr + offset;
    // Simulate reading from memory[0] to memory[byteOffset]
    uint32_t combined = 0;
    for (int i = addr; i < addr + byteOffset; i++)
    {
        combined = (combined << 8) | memory[i]; // memory[i] simulates readByte(alignedAddr + i)
        printf("Combined:         0x%08X\n", combined);
    }

    // Shift combined value to align with destination
    uint32_t shift = (offset % 4) * 8;
    uint32_t preserveMask = (byteOffset == 4) ? 0 : (0xFFFFFFFF >> (byteOffset * 8));
    uint32_t result = (rt & preserveMask) | (combined << shift);

    printf("Combined:         0x%08X\n", combined);
    printf("Shifted:          0x%08X\n", combined << shift);

    return result;
}

uint32_t lwr(uint8_t *memory, uint32_t addr, uint32_t rt, uint32_t offset)
{
    // Emulate lwr logic
    addr = addr + offset;
    uint32_t byteOffset = offset % 4;

    // Simulate reading from memory[0] to memory[byteOffset]
    uint32_t combined = 0;
    for (int i = addr - byteOffset; i <= addr; i++)
    {
        combined = (combined << 8) | memory[i];
        printf("Combined:         0x%08X\n", combined);
    }

    // Shift combined value to align with destination
    uint32_t preserveMask = (pow(2, 33) - 1) - (pow(2, (byteOffset + 1) * 8) - 1);
    uint32_t result = (rt & preserveMask) | combined;

    printf("Combined:         0x%08X\n", combined);
    printf("Result after lwl: 0x%08X\n", rt & preserveMask);

    return result;
}
