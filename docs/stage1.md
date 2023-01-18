# [stage1](/docs/stage1.md)

# description

this is, once again, just a dropper for the next stage, however this time, the next stage, besides being encrypted, is also compressed by an algorithm that looks like a variation of LZ77. this stage uses a "pointer table" that holds a pointer to encrypted and compressed data, and a [decheader_t](/src/stage2.h#L11) structure, which contains information about data that should be decrypted and decompressed, as well as an initializer key for decryption.

# anti-analysis

the pointer table will also contain some PFNs to win32 routines, use of which don't actually make any sense and seemingly are there just to confuse anyone who's trying to analyze this. the only useful PFN is `VirtualAlloc`, which is used to create a buffer that will hold the decompressed data.

# behaviour

1. fill the pointer table.
  - set 1st dword to be 0.
  - set 2nd dword to be a pointer to a `decheader_t` structure. this structure aids in decryption and decompression of required data.
  - set 3rd dword to be a pointer to encrypted and compressed data that contains the next stage of execution.
  - 4th dword is reserved. it is used in the decryption routine to store the current [key_recipe](/src/stage2.h#L22).
  - set 5th dword to be a pointer to `LoadLibraryA`.
  - set 6th dword to be a pointer to `GetProcAddress`.
  - load `kernel32.dll` module using the fifth dword from the pointer table.
  - set 7th dword to be a pointer to `GlobalAlloc`.
  - set 8th dword to be a pointer to `GetLastError`.
  - set 9th dword to be a pointer to `Sleep`.
  - set 10th dword to be a pointer to `VirtualAlloc` .
  - set 11th dword to be a pointer to `CreateToolhelp32Snapshot`.
  - set 12th dword to be a pointer to `Module32First`.
  - set 13th dword to be a pointer to `CloseHandle`.
2. try to create a toolhelp32 snapshot with `TH32CS_SNAPMODULE` 100 times, sleeping for 100ms each iteration. if the creation succeeds, the loop is broken.
3. call `Module32First` with the created snapshot. if the function succeeds, the execution flow continues to the next stage. if not, the snapshot is closed by `CloseHandle` and the function returns. the product of these operations with th32 snapshots is not being used at all later on.
4. set the jump pointer to encrypted data.
5. begin decrypting encrypted data. the encryption/decryption method is XOR. [watch this](/src/stage2.h#L20)
6. check the `is_compressed` flag in the `decheader_t` structure. if it is `true`, do the following:
  - create a new memory buffer of size `decheader_t::alloc_data_size` using `VirtualAlloc`, with `MEM_COMMIT` and `PAGE_EXECUTE_READWRITE`.
  - call the [decompression routine](/src/stage2.h#L33). it will decompress the source data and put all results in the newly allocated buffer. (dont mind the chaotic implementation, this code was ripped off from HexRays decompiler, as i have no will to rewrite this monstrosity entirely by hand)
  - set `decheader_t::data_size` to the decompressed data size, as returned by the decompression routine.
  - set the jump pointer to decompressed data.
7. jump to the next stage.

# next: [stage2](/docs/stage2.md)
