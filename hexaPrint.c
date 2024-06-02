#include <stdio.h>
#include <stdlib.h>


// Function to print the hex representation of a buffer
void PrintHex(const unsigned char *buffer, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);////////CHANGEEEEEEEEEEEEEE????????????????????
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("ERROR: cannot open file");
        return 1;
    }
    unsigned char buffer[16];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        PrintHex(buffer, bytesRead);
    }
    fclose(file);
    return 0;
}






