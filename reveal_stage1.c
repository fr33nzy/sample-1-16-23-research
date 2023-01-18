#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "stage1.h"

int main(int argc, const char** argv) {
    const char* stage0_filename = "stage0.bin";
    const char* stage0_revealed_filename = "stage0.revealed.bin";

    if(argc > 1) {
        stage0_filename = argv[1];
    }

    if(argc > 2) {
        stage0_revealed_filename = argv[2];
    }

    FILE* stage0_file = fopen(stage0_filename, "rb");
    fseek(stage0_file, 0, SEEK_END);

    size_t stage0_filesize = ftell(stage0_file);
    rewind(stage0_file);

    unsigned char* stage0 = (unsigned char*)malloc(stage0_filesize);
    fread(stage0, sizeof(unsigned char), stage0_filesize, stage0_file);

    fclose(stage0_file);

    unsigned char* stage1 = (unsigned char*)malloc(STAGE1_SIZE);
    memcpy(stage1, &stage0[STAGE1_OFFSET], STAGE1_SIZE);

    decrypt_stage1(stage1);

    memcpy(&stage0[STAGE1_OFFSET], stage1, STAGE1_SIZE);

    FILE* stage0_revealed_file = fopen(stage0_revealed_filename, "wb");
    fwrite(stage0, sizeof(unsigned char), stage0_filesize, stage0_revealed_file);

    fclose(stage0_revealed_file);

    free(stage0);
    free(stage1);
    
    return 0;
}
