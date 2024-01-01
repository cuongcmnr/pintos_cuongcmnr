#include <stdio.h>
int main() {
    FILE *file = fopen("self", "w");
    if (file == NULL) {
        printf("Failed to open file for writing.\n");
        return 1;
    } else {
        printf("Successfully opened file for writing.\n");
        fclose(file);
        return 0;
    }
}