#ifndef __PREFIX_H__
#define __PREFIX_H__

#include <stdint.h>
#include <stdio.h>
#include <uchar.h>

#define PF_STACK_LIMIT 256

typedef struct pf_state_t pf_state_t;
typedef struct pf_node_t pf_node_t;

struct pf_state_t {
  FILE *file;
  
  uint32_t stack[PF_STACK_LIMIT];
  uint32_t head;
};

struct pf_node_t {
  char8_t key;
  uint32_t value;
  
  uint32_t sibling, child;
} __attribute__((packed));

extern pf_node_t *pf_nodes;
extern size_t pf_node_count;

void pf_push(const char8_t *word, uint32_t node);
bool pf_load(const char *path);

uint32_t pf_next(pf_state_t *state);

#endif
