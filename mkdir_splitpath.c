#include "types.h"

extern struct NODE* root;
extern struct NODE* cwd;

//make directory
void mkdir(char* pathName) {
    char baseName[64]; // To hold the name of the new directory
    char dirName[64];  // To hold the directory path
    struct NODE* parent = splitPath(pathName, baseName, dirName);

    // Check for empty pathName
    if (strcmp(pathName, "/") == 0 || strlen(pathName) == 0) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    // Check if parent is NULL (invalid parent directory)
    if (parent == NULL) {
        return; // splitPath already printed an error message
    }

    // Check if the directory already exists
    struct NODE* temp = parent->childPtr;
    while (temp != NULL) {
        if (strcmp(temp->name, baseName) == 0) {
            printf("MKDIR ERROR: directory %s already exists\n", baseName);
            return; // Directory already exists
        }
        temp = temp->siblingPtr; // Move to the next sibling
    }

    // Create a new directory node
    struct NODE* newDir = (struct NODE*)malloc(sizeof(struct NODE));
    if (newDir == NULL) {
        perror("Failed to allocate memory");
        return; // Handle memory allocation failure
    }

    // Set the fields of the new directory node
    strcpy(newDir->name, baseName);
    newDir->fileType = 'D'; // Set fileType to 'D' for directory
    newDir->childPtr = NULL; // Initialize childPtr to NULL
    newDir->siblingPtr = NULL; // Initialize siblingPtr to NULL
    newDir->parentPtr = parent; // Set the parent pointer

    // Add the new directory to the parent's children
    if (parent->childPtr == NULL) {
        parent->childPtr = newDir; // If first child
    } else {
        // Find the last sibling and add the new directory
        struct NODE* lastSibling = parent->childPtr;
        while (lastSibling->siblingPtr != NULL) {
            lastSibling = lastSibling->siblingPtr; // Traverse to the last sibling
        }
        lastSibling->siblingPtr = newDir; // Link the new directory as a sibling
    }

    // Print success message
    printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
}

struct NODE* splitPath(char* pathName, char* baseName, char* dirName) {
    // Initialize dirName and baseName
    strcpy(dirName, ""); // Start with empty dirName

    // Handle root case
    if (strcmp(pathName, "/") == 0) {
        strcpy(baseName, ""); // Base name is empty for root
        return root; // Return root as the current directory
    }

    // Find the last '/' in pathName
    char* lastSlash = strrchr(pathName, '/');

    // If lastSlash is not found, it means the pathName is a file in current dir
    if (lastSlash == NULL) {
        strcpy(baseName, pathName); // Set baseName to pathName
        return cwd; // Return current working directory
    }

    // Set dirName and baseName
    strncpy(dirName, pathName, lastSlash - pathName); // Copy path up to the last '/'
    dirName[lastSlash - pathName] = '\0'; // Null terminate the dirName
    strcpy(baseName, lastSlash + 1); // Copy everything after the last '/'

    // Traverse the directory tree using dirName
    struct NODE* current = root;
    char* token = strtok(dirName, "/");

    while (token != NULL) {
        struct NODE* temp = current->childPtr;
        int found = 0;

        // Look for the next directory in the child nodes
        while (temp != NULL) {
            if (strcmp(temp->name, token) == 0) {
                current = temp; // Move to the next directory
                found = 1;
                break;
            }
            temp = temp->siblingPtr; // Move to the next sibling
        }

        // If the directory was not found, print error and return NULL
        if (!found) {
            printf("ERROR: directory %s does not exist\n", token);
            return NULL; // Return NULL if the directory doesn't exist
        }

        // Get the next token
        token = strtok(NULL, "/");
    }

    return current; // Return the last found directory node
}
