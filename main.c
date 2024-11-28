#include <stdio.h>

// size of GBA .sav files
#define GBA_sav_size 128000

// offset of save B
const int B_offset = 0xE000;

/* format of each section 
0x0000 (3968) - Data
0x0FF4 (2) - Section ID
0x0FF6 (4) - Checksum
0x0FFC (4) - Save index
*/

const int sect_id_offset = 0x0FF4;
const int checksum_offset = 0x0FF6;
const int save_index_offset = 0x0FFC;

// data size by section (used for checksum, 0 indexed)
const int sect_data_size[14] = {
    3884, 3968, 3968, 3968, 3848, 3968, 3968,
    3968, 3968, 3968, 3968, 3968, 3968, 2000
};

unsigned char bytes[GBA_sav_size];

char save = 0b1; // 0 for save A, 1 for save B

int to_int32(int i) {
    return (bytes[i]) | (bytes[i+1]<<8) | (bytes[i+2]<<16) | (bytes[i+3]<<24);
}

unsigned short checksum(int sect_len, int start) {
    // start is offset of section startd
    int chk = 0;
    for (unsigned short i=start; i<start+sect_len; i+=4) {
        // read 4 bytes into int 32
        int x = to_int32(i);
        chk += x;
    }
    return chk + (chk>>16);
}

int main(int argc, char *argv[]) {
    char *fn = "curr.sav";
    char *ofn = "hacked.sav";
    FILE *ptr = fopen(fn, "rb");
    fread(bytes, sizeof(bytes), 1, ptr);
    fclose(ptr);

    // check which save half
    if (to_int32(save_index_offset) > to_int32(save_index_offset+B_offset)) {
        save = 0b0;
        printf("Save A is active.\n");
    } else {
        save = 0b1;
        printf("Save B is active.\n");
    }
    FILE *optr = fopen(ofn, "wb");
    fclose(optr);
    return 0;
}