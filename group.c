#include <group.h>

#include <stdint.h>

g_group_t g_parse_SA_phrase(const size_t *next);
g_group_t g_parse_SR_phrase(const size_t *next);
g_group_t g_parse_SN_phrase(const size_t *next);
g_group_t g_parse_SV_phrase(const size_t *next);
g_group_t g_parse_clause(const size_t *next);

bool g_valid(const g_group_t *group) {
  /* TODO: Super-duper fancy logic goes here. */
  return true;
}

void g_print(const g_group_t *group) {
  
}

void g_clean(g_group_t *group) {
  /* TODO: Magic. */
}

void g_merge(g_group_t *group) {
  /* TODO: HOW AAAAAAAAAA. */
}
