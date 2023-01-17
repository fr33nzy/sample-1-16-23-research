# [stage0](/docs/stage0.md)

`stage0.bin` is the entry point for this sample. it's a PE file which can be executed as a regular program.

# description

this is solely a dropper for stage1. amongst all the useless API calls, variables and loop wrappers, there are routines for stage1 decryption and execution.

# anti-analysis

the binary is highly obfuscated and API hammered, trying to confuse anyone who's performing static and dynamic analysis. however you can simply ignore the bogus API calls which don't make any sense (like `LoadLibraryA("Bavobabim tapic zehadosidoviwe");`).

there are also a lot of useless variables that get written and read from frequently. you can also ignore them, because they don't make any sense in terms of this sample's behaviour.

some routines are also wrapped in an incremental loop which checks the current iteration number for a specific value, and if the check is passed, the actual code gets executed. this happens only once per loop.
example is provided below.

```c
i = 0;
do {
    if (i == 5456) {
        stage1_source_ptr = stage1_source_offset;
    }
    
    i = i + 1;
} while (i < 5678163);
```

the value for `stage1_source_ptr` will be set anyway, it's just a matter of iterations that need to get passed until relevant code is executed.

# behaviour

in short, this stage does the following step by step:

1. set size of stage1 memory buffer to `0xbda2`.
2. increase size of stage1 memory buffer by `0x38d6`, thus resulting in a size of `0xf678`.
3. allocate a memory buffer with the calculated size. this will be the destination of stage1 which will eventualy get called as a subroutine.
4. set a pointer to stage1 source binary data to read from. it's located in the `.data` section as a big buffer of bytes.
5. copy data from stage1 static source to allocated destination, byte by byte in a loop.
6. call `VirtualProtect` on the destination stage1, with respective size, setting it's protection to `PAGE_EXECUTE_READWRITE` for later execution.
7. decrypt the entire allocated and copied stage1 using a custom [decryption method.](/extract_stage1.c#L34)
8. offset the pointer to allocated stage1 by `0x3d42`. so the first `0x3d42` bytes of stage1 should be ignored and are irrelevant.
9. jump to the address the new pointer of allocated stage1 is pointing to. from now on, execution continues from `allocated stage1 + 0x3d42`

# next: [stage1](/docs/stage1.md)