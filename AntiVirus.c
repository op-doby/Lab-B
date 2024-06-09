#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


// Task 1.a

typedef struct virus {
    unsigned short SigSize; // signature size
    char virusName[16]; // virus name (max 16 bytes)
    unsigned char* sig; // signature
} virus;


char sigFileName[256] = "signatures-L"; // Default file name
unsigned int fileSize = 0; // Default file size
FILE* file;
#define min(a, b) ((a) < (b) ? (a) : (b))

void SetSigFileName() {
    char newSigFileName[256];
    printf("Enter new signature file name: ");
    scanf("%255s", newSigFileName); // Limit input to avoid buffer overflow

    // Attempt to rename the file
    if (rename(sigFileName, newSigFileName) != 0) {
        perror("ERROR: could not rename file");
    } else {
        // Update the global variable with the new file name
        strncpy(sigFileName, newSigFileName, sizeof(newSigFileName));
        sigFileName[sizeof(sigFileName) - 1] = '\0';  // Ensure null termination
        printf("File renamed successfully to %s\n", sigFileName);
    }
}

virus* readVirus(FILE* file) {
    virus* v = (virus*)malloc(sizeof(virus)); // Allocate memory for the virus struct
    if (!v) 
        return NULL; // Allocation failed

    if (fread(v, 1, 18, file) != 18) { // Couldn't read all the bytes that we needed to read
        free(v);
        return NULL;
    }
    (*v).sig = (unsigned char*)malloc((*v).SigSize); // Allocate memory for the signature
    if (!(*v).sig) { // Allocation failed
        free(v);
        return NULL;
    }
    if (fread((*v).sig, 1, (*v).SigSize, file) != (*v).SigSize) { // Trying to read the signature
        free((*v).sig);
        free(v);
        return NULL;
    }
    return v;
}

void printVirus(virus* v) {
    printf("Name: %s\n", (*v).virusName); //ASCII
    printf("Signature length: %d\n", (*v).SigSize); //Decimal
    printf("Signature: "); //Hexadecimal
    for (int i = 0; i < (*v).SigSize; ++i) {
        printf("%02X ", (*v).sig[i]);
    }
    printf("\n");
}

void process(const char* fileName){
    file = fopen(fileName, "r");
    if (!file) {
        perror("ERROR:: cannot open file");
        return;
    }
    char magicNumber[4];
    if (fread(magicNumber, 1, 4, file) != 4) {
        perror("ERROR: couldn't read the magic number");
        fclose(file);
        return;
    }
    if (memcmp(magicNumber, "VIRL", 4) != 0 && memcmp(magicNumber, "VIRB", 4) != 0) {
        fprintf(stderr, "Invalid magic number\n");
        fclose(file);
        return;
    }
    virus* v;
    while ((v = readVirus(file)) != NULL) {
        printVirus(v);
        free((*v).sig);
        free(v);
    }
    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
}


// Task 1.b

typedef struct link link;

struct link {
    link *nextVirus;
    virus *vir;
};

void printVirusToFile(virus *v, FILE *file) {
    fprintf(file, "Name: %s\n", (*v).virusName); //ASCII
    fprintf(file, "Signature length: %d\n", (*v).SigSize); //Decimal
    fprintf(file, "Signature: "); //Hexadecimal
    for (int i = 0; i < (*v).SigSize; ++i) {
        fprintf(file, "%02X ", (*v).sig[i]);
    }
    fprintf(file, "\n");
}

void list_print(link *virus_list, FILE *file) {
    link *current = virus_list;
    while (current != NULL) {
        printVirusToFile((*current).vir, file);  // Print virus details to the file
        fprintf(file, "\n");  // Add a newline after each virus data
        current = (*current).nextVirus;
    }
}

link* list_append(link *virus_list, virus *data) {
    link *new_link = (link*)malloc(sizeof(link));
    if (!new_link) {
        fprintf(stderr, "ERROR: cannot allocate memory\n");
        exit(1); // Exit the program
    }
    (*new_link).vir = data;
    (*new_link).nextVirus = virus_list;  // Add new link at the beginning of the list
    return new_link;  // Return pointer to the new first link in the list
}

void list_free(link *virus_list) {
    link *current = virus_list;
    while (current != NULL) {
        link *temp = current;
        current = (*current).nextVirus;
        free((*(*temp).vir).sig); // Free signature data;
        free((*temp).vir);  // Free virus data
        free(temp); // Free link
    }
}

link* loadSignatures(char *fileName){ 
    link *virus_list = NULL;
    FILE* file1 = fopen(fileName, "r");
    if (!file1) {
        perror("ERROR:: cannot open file");
        return NULL;
    }
    char magicNumber[4];
    if (fread(magicNumber, 1, 4, file1) != 4) {
        perror("ERROR: couldn't read the magic number");
        fclose(file1);
        return NULL;
    }
    if (memcmp(magicNumber, "VIRL", 4) != 0 && memcmp(magicNumber, "VIRB", 4) != 0) {
        fprintf(stderr, "Invalid magic number\n");
        fclose(file1);
        return NULL;
    }
    virus* v;
    while ((v = readVirus(file1)) != NULL) 
        virus_list = list_append(virus_list, v);
    fclose(file1);
    return virus_list;
}

//Task 1.c

void detect_virus(char *buffer, unsigned int size, link *virus_list) {
    for (link *current = virus_list; current != NULL; current = (*current).nextVirus) {
        virus *v = (*current).vir;
        for (unsigned int i = 0; i <= size - (*v).SigSize; ++i) { // Iterate over the buffer to find the virus signature while making sure that we dont go out of bounds
            if (memcmp(buffer + i, (*v).sig, (*v).SigSize) == 0) {
                printf("Virus detected at byte %u\n", i);
                printf("Virus name: %s\n", (*v).virusName);
                printf("Virus signature size: %u\n", (*v).SigSize);
                printf("\n");
            }
        }
    }
}


//Task 2.b
void neutralize_virus(char *fileName, int signatureOffset) {
    FILE *file = fopen(fileName, "rb+");
    if (!file) {
        perror("ERROR: cannot open suspected file");
        return;
    }

    if (fseek(file, signatureOffset, SEEK_SET) != 0) {
        perror("ERROR: fseek failed");
        fclose(file);
        return;
    }

    unsigned char retInstruction = 0xC3;
    if (fwrite(&retInstruction, 1, 1, file) != 1) {
        perror("ERROR: fwrite failed");
    }

    fclose(file);
}


void menu(char* virusFile) {
    size_t size;
    int choice;
    virus *v;
    link *virus_list = NULL;  // Initialize the list as empty
    char *buffer = (char *)malloc(10000); // Allocate memory for the buffer
    if (buffer == NULL) {
        perror("ERROR: couldn't allocate memory");
        exit(1);
    }

    while (1) {
        int choice;
        printf("0) Set signatures file name\n1) Load signatures\n2) Print signatures\n3) Detect viruses\n4) Fix file\n5) Quit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            // Clear invalid input
            while (getchar() != '\n');
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        switch (choice) {
            case 0:
                // Set signatures file name (Assuming a function is implemented for this)
                SetSigFileName();
                break;
            case 1:
                // Load signatures
                list_free(virus_list);  // Free the old list before loading the new one
                virus_list = loadSignatures(sigFileName);
                break;
            case 2:
                // Print signatures
                list_print(virus_list, stdout);
                break;
            case 3:
                {
                // Detect viruses
                FILE *file = fopen(virusFile, "rb");
                if (!file) {
                    perror("ERROR: cannot open suspected file");
                    break;
                }

                size = fread(buffer, 1, 10000, file);
                fclose(file);
                detect_virus(buffer, size, virus_list);
                break;
                }
            case 4:
            {
                // Fix file
                FILE *file = fopen(virusFile, "rb+");
                if (!file) {
                    perror("ERROR: cannot open suspected file");
                    break;
                }

                size = fread(buffer, 1, 10000, file);
                fclose(file);

                for (link *current = virus_list; current != NULL; current = (*current).nextVirus) {
                    v = (*current).vir;
                    for (unsigned int i = 0; i <= size - (*v).SigSize; ++i) {
                        if (memcmp(buffer + i, (*v).sig, (*v).SigSize) == 0) {
                            printf("Neutralizing virus at byte %u\n", i);
                            neutralize_virus(virusFile, i);
                        }
                    }
                }
                break;
            }
            case 5:
            {
                // Quit
                list_free(virus_list);
                free(buffer);
                fclose(file);
                exit(0);
                break;
            }
            default:
                printf("Invalid choice\n");
        }
    }
}


// main for all tasks
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }
    char *virusFile = argv[1];
    process(sigFileName);
    menu(virusFile);
    return 0;
} 







