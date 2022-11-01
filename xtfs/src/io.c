#include "io.h"

void write_file(FILE* fp, long int offset, char* buffer, size_t size) {
    fseek(fp, offset, SEEK_SET);
    fwrite(buffer, 1, size, fp);
}

void read_file(FILE* fp, long int offset, char* buffer, size_t size) {
    fseek(fp, offset, SEEK_SET);
    fread(buffer, 1, size, fp);
}