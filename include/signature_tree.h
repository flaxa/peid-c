#include "cJSON.h"

#define FILE_SIZE 2267648

int get_tree(cJSON **json);
char *search_tree(cJSON *json, char *search_term);
char *search_tree_recursive(cJSON *json, char *search_term, int search_term_index);
void search_tree_recursive_match_all(cJSON *json, char *search_term, int search_term_index);

