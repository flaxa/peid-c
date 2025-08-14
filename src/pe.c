#include "pe.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAGIC_SIZE 2
#define PE_OFFSET_SIZE 4
#define OPT_HEAD_SIZE_OFFSET 20
#define NUM_SECTIONS_OFFSET 6
#define PE_OFFSET 0x3c

struct pe_struct {
  uint32_t pe_offset;
  uint16_t num_sections;
  uint16_t size_of_opt_header;
};

struct section_struct {
  uint32_t virtual_size;
  uint32_t virtual_addr;
  uint32_t raw_size;
  uint32_t raw_addr;
};

void error_on_read(char *error_msg) {
  fprintf(stderr, "%s\n", error_msg);
  exit(EXIT_FAILURE);
}

FILE *open_file(char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror(filename);
    exit(EXIT_FAILURE);
  }
  return file;
}

struct pe_struct get_pe_info(FILE *file) {
  char magic[3] = {0};
  if (fread(magic, 1, MAGIC_SIZE, file) != MAGIC_SIZE) {
    error_on_read("Failed to read signature");
  }

  if (strcmp(magic, "MZ")) {
    fprintf(stderr, "Invalid MZ signature\n");
    exit(EXIT_FAILURE);
    error_on_read("Invalid MZ signature");
  }
  if (fseek(file, PE_OFFSET, SEEK_SET) != 0) {
    error_on_read("Failed to seek to PE offset");
  }
  uint32_t pe_offset;
  if (fread(&pe_offset, PE_OFFSET_SIZE, 1, file) != 1) {
    error_on_read("Failed to read PE offset");
  }
  if (fseek(file, pe_offset, SEEK_SET) != 0) {
    error_on_read("Failed to seek to PE signature");
  }
  char pe_sig[4] = {0};
  if (fread(pe_sig, 1, 4, file) != 4) {
    error_on_read("Failed to read PE signature");
  }
  if (memcmp(pe_sig, "PE\x00\x00", 4) != 0) {
    error_on_read("Invalid PE header");
  }
  if (fseek(file, pe_offset + NUM_SECTIONS_OFFSET, SEEK_SET) != 0) {
    error_on_read("Failed to seek to number of sections");
  }
  uint16_t num_sections;
  if (fread(&num_sections, 2, 1, file) != 1) {
    error_on_read("Failed to read number of sections");
  }
  if (fseek(file, pe_offset + OPT_HEAD_SIZE_OFFSET, SEEK_SET) != 0) {
    error_on_read("Failed to seek to size of optional header");
  }
  uint16_t size_of_opt_header;
  if (fread(&size_of_opt_header, 2, 1, file) != 1) {
    error_on_read("Failed to read size of optional header");
  }
  struct pe_struct pe_info;
  pe_info.num_sections = num_sections;
  pe_info.pe_offset = pe_offset;
  pe_info.size_of_opt_header = size_of_opt_header;

  return pe_info;
}

struct section_struct get_section_info(FILE *file, struct pe_struct pe_info,
                                       uint16_t section_num) {
  struct section_struct section_info;
  if (fseek(file,
            pe_info.pe_offset + 24 + pe_info.size_of_opt_header +
                section_num * 40 + 8,
            SEEK_SET) != 0) {
    error_on_read("Failed to seek to section");
  }
  if (fread(&section_info.virtual_size, 4, 1, file) != 1) {
    error_on_read("Failed to read virtual size of section");
  }
  if (fread(&section_info.virtual_addr, 4, 1, file) != 1) {
    error_on_read("Failed to read virtual address of section");
  }
  if (fread(&section_info.raw_size, 4, 1, file) != 1) {
    error_on_read("Failed to read raw size of section");
  }
  if (fread(&section_info.raw_addr, 4, 1, file) != 1) {
    error_on_read("Failed to read raw address of section");
  }
  return section_info;
}

uint32_t get_entry_point_offset(FILE *file) {
  struct pe_struct pe_info = get_pe_info(file);
  uint32_t entry_point;
  uint32_t entry_point_raw = 0;
  struct section_struct section_info;
  if (fseek(file, pe_info.pe_offset + 40, SEEK_SET) != 0) {
    error_on_read("Failed to seek to entry point offset");
  }
  if (fread(&entry_point, 4, 1, file) != 1) {
    error_on_read("Failed to read entry point offset");
  }
  for (int i = 0; i < pe_info.num_sections; i++) {

    section_info = get_section_info(file, pe_info, i);
    if (section_info.virtual_addr <= entry_point &&
        entry_point < section_info.virtual_addr + section_info.virtual_size) {
      entry_point_raw =
          section_info.raw_addr + entry_point - section_info.virtual_addr;
    }
  }
  if (entry_point_raw == 0) {
    error_on_read("Failed to find entry point");
  }
  return entry_point_raw;
}
char *get_entry_point_hex_string(FILE *file, uint32_t len) {
  unsigned char ep_data[len];
  uint32_t ep_offset = get_entry_point_offset(file);

  if (fseek(file, ep_offset, SEEK_SET) != 0) {
    error_on_read("Failed to seek to entry point");
  }
  if (fread(ep_data, 1, len, file) != len) {
    error_on_read("Failed to read data starting at entry point");
  }

  char *ep_data_str = malloc(len * 2 + 1);
  if (!ep_data_str) {
    perror("malloc");
    return NULL;
  }

  for (uint32_t i = 0; i < len; i++) {
    sprintf(&ep_data_str[i * 2], "%02X", ep_data[i]);
  }

  ep_data_str[len * 2] = '\0';

  return ep_data_str;
}

int get_string_with_offset(FILE *file, uint32_t len, uint32_t offset,
                           char *buffer) {
  uint32_t read_size;
  unsigned char ep_data[len];
  if (fseek(file, offset, SEEK_SET) != 0) {
    error_on_read("Failed to seek to offset");
  }
  if ((read_size = fread(ep_data, 1, len, file)) < 64) {
    return 0;
  }
  for (uint32_t i = 0; i < read_size; i++) {
    sprintf(&buffer[i * 2], "%02X", ep_data[i]);
  }
  buffer[read_size * 2] = '\0';
  return 1;
}
