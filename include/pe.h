#include <stdint.h>
#include <stdio.h>

char *get_entry_point_hex_string(FILE *file, uint32_t len);
FILE *open_file(char *filename);
int get_string_with_offset(FILE *file, uint32_t len, uint32_t offset,
                           char *buffer);
