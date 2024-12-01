#include <stdlib.h>
#include <stdio.h>

// size of GBA .sav files
const int GBA_sav_size = 128000;
// offset of save B
const int B_offset = 0xE000;

const int char_codes[52] = {
0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 
0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1,
0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE
};

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

int save = 0x0000; // 0x0000 for save A, 0xE000 for save B
int save_index = -1;
int shuffle_offset = 0; // offset of section 0 is (0 indexed)

/* Convert 4 bytes to int 32 */
int to_int32(unsigned char bytes[], int i) {
    return (bytes[i]) | (bytes[i+1]<<8) | (bytes[i+2]<<16) | (bytes[i+3]<<24);
}

void write_int32(unsigned char bytes[], int x, int loc) {
    for (int i=0; i<4; i++) {
        bytes[loc+i] = x>>(8*i);
    }
}

int get_section_offset(int section) {
    return save+(section+save_index)%14*0x1000;
}

/* Calculates checksum of given section */
unsigned short checksum(unsigned char bytes[], int section) {
    int start = get_section_offset(section);
    int sect_len = sect_data_size[section];
    int chk = 0;
    for (int i=start; i<start+sect_len; i+=4) {
        int x = to_int32(bytes, i);
        chk += x;
    }
    return chk + (chk>>16);
}

/* Determine which save half is currently in use and set save index and base offset accordingly */
void init_half(unsigned char bytes[]) {
    int a = to_int32(bytes, save_index_offset);
    int b = to_int32(bytes, save_index_offset+B_offset);
    if (a > b) {
        save = 0x000;
        save_index = a;
        printf("Save A is active\n");
    } else {
        save = 0xE000;
        save_index = b;
        printf("Save B is active\n");
    }
}

void validate_checksum(unsigned char bytes[], int section) {
    int offset = get_section_offset(section);
    unsigned short chk = checksum(bytes, 0);
    int chk_loc = offset + 0x0FF6;
    bytes[chk_loc] = chk;
    bytes[chk_loc+1] = chk >> 8;
}

void change_name(char *new, unsigned char bytes[], size_t len) {
    int section_offset = get_section_offset(0);
    for (int i=0; i<len; i++) {
        char c = new[i];
        if (c >= 'A' && c <= 'Z') c = char_codes[c-'A'];
        else if (c >= 'a' && c <= 'z') c = char_codes[26+c-'a'];
        else {
            printf("Invalid characters in name. Exiting...");
            exit(0);
        }
        bytes[i+section_offset] = c;
    }
    for (int i=len; i<8; i++) {
        bytes[i+section_offset] = 0xFF;
    }
    // write the checksum into the right place
    validate_checksum(bytes, 0);
    printf("Name changed successfully.\n");
}