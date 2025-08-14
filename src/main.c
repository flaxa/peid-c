#include "pe.h"
#include "signature_tree.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum { SEARCH_EP, SEARCH_FULL } search_mode_t;
typedef enum { MATCH_ALL, MATCH_FIRST } match_mode_t;

int main(int argc, char *argv[]) {

  search_mode_t search_mode = SEARCH_EP;
  match_mode_t match_mode = MATCH_ALL;
  char *binary = NULL;
  FILE *file;
  uint32_t len;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--full") == 0) {
      search_mode = SEARCH_FULL;
    } else if (strcmp(argv[i], "-1") == 0 || strcmp(argv[i], "--first") == 0) {
      match_mode = MATCH_FIRST;
    } else if ((strcmp(argv[i], "-i") == 0 ||
                strcmp(argv[i], "--input") == 0)) {
      if (i + 1 < argc) {
        binary = argv[++i];
      } else {
        fprintf(stderr, "Error: Missing file after %s\n", argv[i]);
        return 1;
      }
    } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      printf("Usage: %s [OPTIONS] -i FILE\n", argv[0]);
      printf("Defaults: search from entry point, print all matches.\n\n");
      printf("Options:\n");
      printf("  -f, --full       Search the whole file instead of from entry "
             "point\n");
      printf("  -1, --first      Stop after first match instead of printing "
             "all matches\n");
      printf("  -i, --input FILE Input file to scan (required)\n");
      printf("  -h, --help       Show this help message\n");
      return 0;
    } else {
      fprintf(stderr, "Unknown option: %s\n", argv[i]);
      return 1;
    }
  }
  if (!binary) {
    fprintf(stderr, "Error: Input file is required.\n");
    return 1;
  }
  // argv[1] is the first argument after the program name

  file = open_file(binary);
  cJSON *sig_tree;

  if (!get_tree(&sig_tree)) {
    fprintf(stderr, "Failed to load signature tree");
    exit(EXIT_FAILURE);
  }

  len = sig_tree->child->next->next->next->valueint;

  if (search_mode == SEARCH_EP) {
    char *entry_point_hex_string = get_entry_point_hex_string(file, len);
    if (match_mode == MATCH_FIRST) {
      char *packer = search_tree_recursive(sig_tree->child->next->child,
                                           entry_point_hex_string, -1);
      if (packer != NULL) {
        printf("%s\n", packer);
      }

    } else {
      search_tree_recursive_match_all(sig_tree->child->next->child,
                                      entry_point_hex_string, -1);
    }

    free(entry_point_hex_string);
  } else {
    char *file_str = malloc(len * 2 + 1);
    if (match_mode == MATCH_FIRST) {
      char *packer = NULL;
      for (int i = 0; get_string_with_offset(file, len, i, file_str); i++) {
        packer =
            search_tree_recursive(sig_tree->child->child, file_str, -1);
        if (packer != NULL) {
          break;
        }
      }

      if (packer != NULL) {
        printf("%s\n", packer);
      }

    } else {
      for (int i = 0; get_string_with_offset(file, len, i, file_str); i++) {

        search_tree_recursive_match_all(sig_tree->child->child, file_str, -1);
      }
    }
    free(file_str);
  }

  fclose(file);
  cJSON_Delete(sig_tree);
  return 0;
}
