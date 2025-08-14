#include "signature_tree.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include "db.h"


int get_tree(cJSON **json) {
  *json = cJSON_Parse((const char *)userdb_txt_json);
  if (*json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      printf("Error: %s\n", error_ptr);
    }
    cJSON_Delete(*json);
    return 0;
  }
  return 1;
}

char *search_tree(cJSON *json, char *search_term) {
  int search_term_index = -1;
  char sub_search_term[3];
  int len = (int)strlen(search_term);

  printf("%d\n", len);
  sub_search_term[2] = '\0';

  printf("%s\n", json->string);

  while (strcmp(json->string, "value")) {

    search_term_index++;
    if ((2 * search_term_index + 1) >= len) {
      return "search_term too short";
    }
    sub_search_term[0] = search_term[2 * search_term_index];
    sub_search_term[1] = search_term[2 * search_term_index + 1];
    while (strcmp(json->string, sub_search_term)) {
      if (json->next == NULL)
        return "next was null";
      json = json->next;
    }
    if (json->child == NULL)
      return "child was null";

    json = json->child;
  }
  return json->valuestring;
}

char *search_tree_recursive(cJSON *json, char *search_term,
                            int search_term_index) {
  char sub_search_term[3] = {0};
  int len = (int)strlen(search_term);
  char *result;

  if (json == NULL) {
    return NULL;
  }

  search_term_index++;
  if ((2 * search_term_index + 1) >= len) {
    return NULL;
  }

  sub_search_term[0] = search_term[2 * search_term_index];
  sub_search_term[1] = search_term[2 * search_term_index + 1];

  do {
        
    if (!strcmp(json->string, "value")) {
      return json->valuestring;
    }
    if (!strcmp(json->string, sub_search_term) || !strcmp(json->string, "??") ||
        (json->string[0] == '?' && json->string[1] == sub_search_term[1]) ||
        (json->string[1] == '?' && json->string[0] == sub_search_term[0])) {


      if ((result = search_tree_recursive(json->child, search_term,
                                          search_term_index)) != NULL) {
        return result;
      }
    }

    json = json->next;
  } while (json != NULL);
  return NULL;
}
void search_tree_recursive_match_all(cJSON *json, char *search_term,
                                     int search_term_index) {
  char sub_search_term[3] = {0};
  int len = (int)strlen(search_term);

  if (json == NULL) {
    return;
  }

  search_term_index++;
  if ((2 * search_term_index + 1) >= len) {
    return;
  }

  sub_search_term[0] = search_term[2 * search_term_index];
  sub_search_term[1] = search_term[2 * search_term_index + 1];

  do {
    if (!strcmp(json->string, sub_search_term) || !strcmp(json->string, "??") ||
        (json->string[0] == '?' && json->string[1] == sub_search_term[1]) ||
        (json->string[1] == '?' && json->string[0] == sub_search_term[0])) {

      if (json->child != NULL) {

        search_tree_recursive_match_all(json->child, search_term,
                                        search_term_index);
      }
    }

    if (!strcmp(json->string, "value")) {
      printf("%s\n", json->valuestring);
    }
    json = json->next;
  } while (json != NULL);
  return;
}
