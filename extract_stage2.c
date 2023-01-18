#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "stage2.h"

int main(int argc, const char** argv) {
    const char* stage0_revealed_filename = "stage0.revealed.bin";
    const char* stage2_filename = "stage2.bin";

    if(argc > 1) {
        stage0_revealed_filename = argv[1];
    }

    if(argc > 2) {
        stage2_filename = argv[2];
    }

    FILE* stage0_revealed_file = fopen(stage0_revealed_filename, "rb");
    fseek(stage0_revealed_file, 0, SEEK_END);

    size_t stage0_revealed_filesize = ftell(stage0_revealed_file);
    rewind(stage0_revealed_file);

    unsigned char* stage0_revealed = (unsigned char*)malloc(stage0_revealed_filesize);
    fread(stage0_revealed, sizeof(unsigned char), stage0_revealed_filesize, stage0_revealed_file);

    fclose(stage0_revealed_file);

    decheader_t* header = (decheader_t*)&stage0_revealed[STAGE2_DECHEADER_OFFSET];

    unsigned char* stage2_encrypted_block = &stage0_revealed[STAGE2_ENCRYPTED_BLOCK_OFFSET];
    unsigned char* stage2_decrypted_block = prepare_stage2(header, stage2_encrypted_block);

    FILE* stage2_file = fopen(stage2_filename, "wb");
    fwrite(stage2_decrypted_block, sizeof(unsigned char), header->data_size, stage2_file);

    fclose(stage2_file);

    free(stage0_revealed);

    if(stage2_encrypted_block != stage2_decrypted_block) {
        free(stage2_decrypted_block);   
    }
    
    return 0;
}
