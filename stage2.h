#include <stdint.h>
#include <stdlib.h>

#pragma warning(disable: 4146)

const uintptr_t STAGE2_DECHEADER_OFFSET = 0x326b7;
const uintptr_t STAGE2_ENCRYPTED_BLOCK_OFFSET = 0x326f4;

#pragma pack(1)

typedef struct {
    uint32_t data_size;
    uint32_t initial_key;
    unsigned char is_compressed;
    uint32_t alloc_data_size;
} decheader_t;

#pragma pack()

unsigned int decrypt_code_block(decheader_t* header, unsigned char* block) {
    uint32_t current_key = header->initial_key;
    uint32_t key_recipe = current_key;
    for(uint32_t b = 0; b < header->data_size; b++) {
        key_recipe = 0x343fd * key_recipe + 0x269ec3;
        current_key = (key_recipe >> 16) & 0x7FFF;
        
        block[b] ^= current_key;
    }

    return current_key;
}

uint32_t decompress_code_block(unsigned char* compressed, uint32_t compressed_size, unsigned char* decompressed, uint32_t* decompressed_size) {
    int v5;
    unsigned char* v6;
    unsigned char* v7;
    unsigned int v8;
    unsigned int v9;
    unsigned int v10;
    int v11;
    unsigned char* v12;
    unsigned int v13;
    unsigned char* v14;
    int v15;
    unsigned int v16;
    unsigned char* v17;
    unsigned char* v18;
    unsigned char* v19;
    unsigned char* v20;
    unsigned int v21;
    unsigned char* v22;

    v22 = &compressed[compressed_size];
    v5 = 0;
    *decompressed_size = 0;
    v6 = decompressed;
    v7 = compressed;
    if (*compressed > 0x11u)
    {
        v8 = *compressed - 0x11;
        v7 = compressed + 1;
        if (v8 < 4)
            goto LABEL_22;
        do
        {
            *v6++ = *v7++;
            --v8;
        } while (v8);
        goto STEP2;
    }
LABEL_5:
    v9 = *v7++;
    if (v9 < 0x10)
    {
        if (!v9)
        {
            while (!*v7)
            {
                v9 += 255;
                ++v7;
            }
            v9 += *v7++ + 15;
        }
        *(uint32_t*)v6 = *(uint32_t*)v7;
        v6 += 4;
        v7 += 4;
        v10 = v9 - 1;
        if (v10)
        {
            if (v10 < 4)
            {
                do
                {
                    *v6++ = *v7++;
                    --v10;
                } while (v10);
            }
            else
            {
                do
                {
                    *(uint32_t*)v6 = *(uint32_t*)v7;
                    v10 -= 4;
                    v6 += 4;
                    v7 += 4;
                } while (v10 >= 4);
                for (; v10; --v10)
                    *v6++ = *v7++;
            }
        }
    STEP2:
        v9 = *v7++;
        if (v9 < 0x10)
        {
            v11 = (*v7 >> 2) + (v9 << 6) + 1793;
            *v6 = v6[-v11];
            v6[1] = v6[-v11 + 1];
            ++v7;
            v6[2] = v6[-v11 + 2];
            v6 += 3;
        LABEL_20:
            v5 = v11;
            goto LABEL_21;
        }
    }
    while (1)
    {
        if (v9 >= 0x40)
        {
            v13 = v9 & 0x1F;
            if (v13 < 28)
            {
                v15 = (*v7++ >> 2) + (v13 << 6) + 1;
                v14 = &v6[-v15];
                v5 = v15;
            }
            else
            {
                v14 = &v6[-v5];
            }
            v16 = (v9 >> 5) - 1;
        LABEL_31:
            *v6 = *v14;
            v6[1] = v14[1];
            v6 += 2;
            v17 = v14 + 2;
            do
            {
                *v6++ = *v17++;
                --v16;
            } while (v16);
            goto LABEL_21;
        }
        if (v9 < 0x20)
            break;
        v16 = v9 & 0x1F;
        if (!v16)
        {
            while (!*v7)
            {
                v16 += 255;
                ++v7;
            }
            v16 += *v7++ + 31;
        }
        v14 = &v6[-(v7[1] >> 2) - 1 + -64 * *v7];
        v5 = (v7[1] >> 2) + (*v7 << 6) + 1;
        v7 += 2;
    LABEL_49:
        if (v16 < 6 || v6 - v14 < 4)
            goto LABEL_31;
        *(uint32_t*)v6 = *(uint32_t*)v14;
        v6 += 4;
        v20 = v14 + 4;
        v21 = v16 - 2;
        do
        {
            *(uint32_t*)v6 = *(uint32_t*)v20;
            v21 -= 4;
            v6 += 4;
            v20 += 4;
        } while (v21 >= 4);
        for (; v21; --v21)
            *v6++ = *v20++;
    LABEL_21:
        v8 = *(v7 - 1) & 3;
        if ((*(v7 - 1) & 3) == 0)
            goto LABEL_5;
    LABEL_22:
        *v6++ = *v7;
        v12 = v7 + 1;
        if (v8 > 1)
        {
            *v6++ = *v12++;
            if (v8 > 2)
                *v6++ = *v12++;
        }
        v9 = *v12;
        v7 = v12 + 1;
    }
    if (v9 < 0x10)
    {
        v11 = (*v7 >> 2) + (v9 << 6) + 1;
        *v6 = v6[-v11];
        ++v7;
        v6[1] = v6[-v11 + 1];
        v6 += 2;
        goto LABEL_20;
    }
    v18 = &v6[-2048 * (v9 & 8)];
    v16 = v9 & 7;
    if (!v16)
    {
        while (!*v7)
        {
            v16 += 255;
            ++v7;
        }
        v16 += *v7++ + 7;
    }
    v19 = &v18[-(v7[1] >> 2) + -64 * *v7];
    v7 += 2;
    if (v19 != v6)
    {
        v14 = v19 - 0x4000;
        v5 = v6 - v14;
        goto LABEL_49;
    }
    *decompressed_size = v6 - decompressed;
    return v7 != v22
        ? (((unsigned int)&v7[-(uint64_t)(unsigned int)v22] >> 32) & 0xFFFFFFFC) - 4
        : 0;
}

unsigned char* prepare_stage2(decheader_t* header, unsigned char* block) {
    unsigned char* result = block;

    decrypt_code_block(header, block);

    if(header->is_compressed) {
        unsigned char* decompressed = (unsigned char*)malloc(header->alloc_data_size);

        uint32_t decompressed_data_size = 0;
        decompress_code_block(block, header->data_size, decompressed, &decompressed_data_size);

        result = decompressed;

        header->data_size = decompressed_data_size;
    }

    return result;
}
