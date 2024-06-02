#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Task 1.a

typedef struct virus {
    unsigned short SigSize; // signature size
    char virusName[16]; // virus name (max 16 bytes)
    unsigned char* sig; // signature
} virus;


char sigFileName[256] = "signatures-L"; // Default file name

void SetSigFileName() { // Set the signature file name
    printf("Enter new signature file name: ");
    scanf("%s", sigFileName); 
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
    FILE* file = fopen(fileName, "r");
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
    fclose(file);
}


// Task 1.b

typedef struct link link;

struct link {
    link *nextVirus;
    virus *vir;
};

void list_print(link *virus_list, FILE *file) {
    link *current = virus_list;
    while (current != NULL) {
        printVirusToFile((*current).vir, file);  // Print virus details to the file
        fprintf(file, "\n");  // Add a newline after each virus data
        current = (*current).nextVirus;
    }
}

void printVirusToFile(virus *v, FILE *file) {
    fprintf(file, "Name: %s\n", (*v).virusName); //ASCII
    fprintf(file, "Signature length: %d\n", (*v).SigSize); //Decimal
    fprintf(file, "Signature: "); //Hexadecimal
    for (int i = 0; i < (*v).SigSize; ++i) {
        fprintf(file, "%02X ", (*v).sig[i]);
    }
    fprintf(file, "\n");
}

link* list_append(link *virus_list, virus *data) {
    link *new_link = (link*)malloc(sizeof(link));
    if (!new_link) {
        fprintf(stderr, "ERROR: cannot allocate memory\n");
        //exit(EXIT_FAILURE); ///?????????????????????????????
        return NULL;
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
        free((*temp).vir);  // Free virus data
        free(temp); // Free link
    }
}

link* loadSignatures(char *fileName){ 
    link *virus_list = NULL;
    FILE* file = fopen(fileName, "r");
    if (!file) {
        perror("ERROR:: cannot open file");
        return NULL;
    }
    char magicNumber[4];
    if (fread(magicNumber, 1, 4, file) != 4) {
        perror("ERROR: couldn't read the magic number");
        fclose(file);
        return NULL;
    }
    if (memcmp(magicNumber, "VIRL", 4) != 0 && memcmp(magicNumber, "VIRB", 4) != 0) {
        fprintf(stderr, "Invalid magic number\n");
        fclose(file);
        return NULL;
    }
    virus* v;
    while ((v = readVirus(file)) != NULL) {
        list_append(virus_list, v);
        free((*v).sig);
        free(v);
    }
    fclose(file);
    return virus_list;
}

//Task 1.c

void detect_virus(char *buffer, unsigned int size) {
    FILE* sigFile = fopen(sigFileName, "rb"); // Open the signature file
    if (!sigFile) {
        perror("ERROR: cannot open file");
        return;
    }
    virus* v;
    while ((v = readVirus(sigFile)) != NULL) { // Read the virus from the signature file
        for (int i = 0; i < size - (*v).SigSize; ++i) { // Iterate over the buffer
            if (memcmp(buffer + i, (*v).sig, (*v).SigSize) == 0) { // Compare the signature with the buffer
                printf("Virus detected in byte %d\n", i); // Print the detection
                printf("Virus name: %s\n", (*v).virusName); // Print the virus name
            }
        }
        free((*v).sig);
        free(v);
    }
    fclose(sigFile);
}



void menu() {
    link *virus_list = NULL;  // Initialize the list as empty
    char sigFileName[100] = "signatures-L";  // Default signatures file name

    while (1) {
        // Display menu and get user input
        int choice;
        printf("0) Set signatures file name\n1) Load signatures\n2) Print signatures\n3) Detect viruses\n4) Fix file\n5) Quit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 0:
                SetSigFileName(sigFileName);  // Set signatures file name
                break;
            case 1:
                // Load signatures into memory buffers and create linked list
                virus_list = loadSignatures(sigFileName);
                break;
            case 2:
                // Print signatures from the linked list to a file
                list_print(virus_list, stdout);  // Print to stdout for demonstration
                break;
            case 3:
                printf("Detecting viruses...\n");
                // Implement virus detection logic
                break;
            case 4:
                printf("Fixing file...\n");
                // Implement file fixing logic
                break;
            case 5:
                list_free(virus_list);  // Free memory before exiting
                exit(EXIT_SUCCESS);
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
    process(argv[1]);


    
    menu();
    return 0;
}



