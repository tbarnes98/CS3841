#include <stdio.h>

int main(int argc, char* argv[])
{
    int rows, columns;
    FILE* input = fopen(argv[1], "r");
    fscanf(input, "%d", &rows);
    fscanf(input, "%d", &columns);
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < columns; j++) {
            int value;
            fscanf(input, "%d", &value);
        }
    }
    fclose(input);
}
