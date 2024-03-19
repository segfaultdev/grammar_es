#ifndef __GROUP_H__
#define __GROUP_H__

#include <word.h>

#include <stdint.h>

typedef struct g_group_t g_group_t;

struct g_group_t {
  enum {
    g_node_clause,
    g_node_phrase,
    g_node_word,
  } type;
  
  union {
    /* (clause) */
    /* (phrase) */
    w_word_t word;
  };
  
  const size_t *next;
  bool is_head;
  
  g_group_t *siblings, *childs;
  size_t count;
};

g_group_t g_parse_SA_phrase(const size_t *next);
g_group_t g_parse_SR_phrase(const size_t *next);
g_group_t g_parse_SN_phrase(const size_t *next);
g_group_t g_parse_SV_phrase(const size_t *next);
g_group_t g_parse_clause(const size_t *next);

bool g_valid(const g_group_t *group);
void g_print(const g_group_t *group);

void g_clean(g_group_t *group);
void g_merge(g_group_t *group);

#endif
