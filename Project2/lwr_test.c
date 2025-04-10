#include <stdio.h>
#include <stdint.h>
#include <math.h>

int main()
{
    // Simulated memory starting at address 0x1000
    uint8_t memory[7] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00};

    // Simulate RegFile
    uint32_t rt = 0x12345678;

    // Simulate i.rs + i.immediate = 0x1002
    uint32_t offset = 2;
    uint32_t addr = 0x00 + offset;
    uint32_t byteOffset = offset % 4;

    // Simulate reading from memory[0] to memory[byteOffset]
    uint32_t combined = 0;
    for (int i = addr - byteOffset; i <= addr; i++)
    {
        combined = (combined << 8) | memory[i];
    }

    // Shift combined value to align with destination
    uint32_t preserveMask = (pow(2, 33) - 1) - (pow(2, (byteOffset + 1) * 8) - 1);
    uint32_t result = (rt & preserveMask) | combined;

    // Print details
    printf("Original rt:      0x%08X\n", rt);
    printf("Memory bytes:     [ %02X %02X %02X %02X ]\n", memory[0], memory[1], memory[2], memory[3]);
    printf("Address:          0x%08X\n", addr);
    printf("Byte offset:      %d\n", byteOffset);
    printf("Combined:         0x%08X\n", combined);
    printf("Result after lwl: 0x%08X\n", result);

    return 0;
}