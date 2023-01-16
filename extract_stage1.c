#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

const int STAGE1_SIZE = 0xF678;
const int STAGE1_OFFSET = 0x2e170;
const int STAGE1_SLICE = 0x3d42;

typedef struct {
    uint32_t upper;
    uint32_t lower;
} encblock_t;

void decrypt_block(encblock_t* block) {
    const int salt_a = 0x93a00cac;
    const int salt_b = 0x7afac68e;
    const int salt_c = 0xbd437644;
    int salt_d = -0x3910c8e0;
    const uint32_t salt_e = 0x9e3779b9;
    const int salt_f = 0x0b84b2b5;

    for(int i = 0; i < 32; i++) {
        int lower_recipe = (block->upper >> 5) + salt_f ^ (block->upper << 4) + salt_c ^ salt_d + block->upper;
        block->lower -= lower_recipe;

        int upper_recipe = (block->lower * 0x10 + salt_a ^ salt_d + block->lower ^ (block->lower >> 5) + salt_b);
        block->upper -= upper_recipe;

        salt_d -= salt_e;
    }
}

void decrypt_stage1(char* stage1) {
    uint32_t total_blocks = STAGE1_SIZE / sizeof(encblock_t);
    encblock_t* curblock = (encblock_t*)stage1;

    for(uint32_t iblock = 0; iblock < total_blocks; iblock++) {
        decrypt_block(curblock);

        curblock++;
    }
}

int main(int argc, const char** argv) {
    const char* stage0_filename = "stage0.bin";
    const char* stage1_filename = "stage1.bin";

    if(argc > 1) {
        stage0_filename = argv[1];
    }

    if(argc > 2) {
        stage1_filename = argv[2];
    }

    FILE* stage0_file = fopen(stage0_filename, "rb");
    fseek(stage0_file, 0, SEEK_END);

    size_t stage0_filesize = ftell(stage0_file);
    rewind(stage0_file);

    char* stage0 = malloc(stage0_filesize);
    fread(stage0, sizeof(char), stage0_filesize, stage0_file);

    fclose(stage0_file);

    char* stage1 = malloc(STAGE1_SIZE);
    memcpy(stage1, &stage0[STAGE1_OFFSET], STAGE1_SIZE);

    decrypt_stage1(stage1);

    FILE* stage1_file = fopen(stage1_filename, "wb");
    fwrite(&stage1[STAGE1_SLICE], sizeof(char), STAGE1_SIZE - STAGE1_SLICE, stage1_file);

    fclose(stage1_file);

    free(stage0);
    free(stage1);
    
    return 0;
}
