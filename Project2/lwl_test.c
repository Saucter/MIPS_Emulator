#include <stdio.h>
#include <stdint.h>

int main()
{
    // Simulated memory starting at address 0x1000
    uint8_t memory[4] = {0xAA, 0xBB, 0xCC, 0xDD};

    // Simulate RegFile
    uint32_t rt = 0x12345678;

    // Simulate i.rs + i.immediate = 0x1002
    uint32_t addr = 0x1000;

    // Emulate lwl logic
    uint32_t byteOffset = addr % 4;
    uint32_t alignedAddr = addr - byteOffset;

    // Simulate reading from memory[0] to memory[byteOffset]
    uint32_t combined = 0;
    for (int i = 0; i <= byteOffset; i++)
    {
        combined = (combined << 8) | memory[i]; // memory[i] simulates readByte(alignedAddr + i)
    }

    // Shift combined value to align with destination
    uint32_t shift = (3 - byteOffset) * 8;
    uint32_t preserveMask = (byteOffset == 3) ? 0 : (0xFFFFFFFF >> ((byteOffset + 1) * 8));
    uint32_t result = (rt & preserveMask) | (combined << shift);

    // Print details
    printf("Original rt:      0x%08X\n", rt);
    printf("Memory bytes:     [ %02X %02X %02X %02X ]\n", memory[0], memory[1], memory[2], memory[3]);
    printf("Address:          0x%08X\n", addr);
    printf("Byte offset:      %d\n", byteOffset);
    printf("Combined:         0x%08X\n", combined);
    printf("Shifted:          0x%08X\n", combined << shift);
    printf("Result after lwl: 0x%08X\n", result);

    return 0;
}