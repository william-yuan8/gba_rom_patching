#include <stdio.h>
#include <string.h>
#include "utilities.h"

#define GBA_sav_size 131072

unsigned char bytes[GBA_sav_size];

int main(int argc, char *argv[]) {
    char *fn = "curr.sav";
    char *ofn = "hacked.sav";
    FILE *ptr = fopen(fn, "rb");
    fread(bytes, sizeof(bytes), 1, ptr);
    fclose(ptr);

    /* Prepare for ROM hacking */
    init_half(bytes);

    if (argc > 1) {
        size_t len = strlen(argv[1]);
        if (len > 0 && len < 8) {
            change_name(argv[1], bytes, len);
        } else {
            printf("Name must be 0-7 characters in length\n");
        }
    }
    
    FILE *optr = fopen(ofn, "wb");
    fwrite(bytes, sizeof(bytes), 1, optr);
    fclose(optr);
    return 0;
    
}