/*
 * elf_reader.h
 *
 *  Created on: Jul 28, 2014
 *      Author: irichter
 */

 #ifndef ELF_READER_H_
 #define ELF_READER_H_
 
 #include <stdbool.h>
 #include "../utils/uthash.h"
 
 typedef struct Exe_Segment {
         uint32_t offsetInFile;  /* Offset of segment in executable file */
         uint32_t lengthInFile;  /* Length of segment data in executable file */
         uint32_t startAddress;  /* Start address of segment in user memory */
         uint32_t sizeInMemory;  /* Size of segment in memory */
         int protFlags;          /* VM protection flags; combination of VM_READ,VM_WRITE,VM_EXEC */
         uint32_t type;
 } Exe_Segment;
 
 typedef struct fpointer {
         const char  *fname;
         uint32_t    *faddr;
         UT_hash_handle hh;
 } fpointer;
 
 struct memElement {
         uint32_t addr;
         uint8_t  data;
         UT_hash_handle hh;
 };
 
 typedef struct Exe_Format {
         int      numSegments;
         uint32_t entryAddr;
         uint32_t globalPointer;
         uint32_t maxUsedAddr;
         struct Exe_Segment segmentList[12];
         struct fpointer *function_pointers;
 } Exe_Format;
 
 struct execinfo {
         int GSP;
         int GRA;
         int GPC_START;
         int HEAPSTART;
         int BREAKSTART;
         int GP;
 };
 
 struct syscall_addresses {
         uint32_t LIBC_OPEN_ADDRESS;
         uint32_t LIBC_READ_ADDRESS;
         uint32_t EXIT_ADDRESS;
         uint32_t MUNMAP_ADDRESS;
         uint32_t GETEUID_ADDRESS;
         uint32_t GETUID_ADDRESS;
         uint32_t UNAME_ADDRESS;
         uint32_t GETPID_ADDRESS;
         uint32_t GETGID_ADDRESS;
         uint32_t GETEGID_ADDRESS;
         uint32_t LIBC_MALLOC_ADDRESS;
         uint32_t CFREE_ADDRESS;
         uint32_t FXSTAT64_ADDRESS;
         uint32_t MMAP_ADDRESS;
         uint32_t BRK_ADDRESS;
         uint32_t LIBC_WRITE_ADDRESS;
         uint32_t CXX_EX_AND_ADD_ADDRESS;
         uint32_t CXX_ATOMIC_ADD_ADDRESS;
 };
 
 /* Global Variables (extern) */
 extern struct syscall_addresses syscalls;
 extern struct memElement memElement;
 extern struct memElement *MAIN_MEMORY;
 extern struct execinfo exec;
 
 /* Function Declarations */
 extern void writefPointer(const char *fName, uint32_t *fAddr, struct Exe_Format *exFormat, bool DEBUG);
 extern uint32_t *readfPointer(const char *fName, struct Exe_Format *exFormat, bool DEBUG);
 extern struct fpointer *findfPointer(const char *fName, struct Exe_Format *exFormat, bool DEBUG);
 
 extern void writeByte(uint32_t ADDR, uint8_t DATA, bool DEBUG);
 extern void writeWord(uint32_t ADDR, uint32_t DATA, bool DEBUG1);
 extern uint8_t readByte(uint32_t ADDR, bool DEBUG);
 extern uint32_t readWord(uint32_t ADDR, bool DEBUG);
 
 extern void init_syscalls(); 
 extern void fill_syscall(uint32_t address, uint16_t call);
 extern void fill_ex_and_add(uint32_t address);
 extern void fill_atomic_add(uint32_t address);
 extern void fill_syscall_redirects();
  
 extern int parse_elf(const char *elf_data, size_t elf_length, struct Exe_Format *exeFormat);
 extern int LoadOSMemory(const char *file_name);
 extern void CleanUp();
 
 #endif /* ELF_READER_H_ */
 