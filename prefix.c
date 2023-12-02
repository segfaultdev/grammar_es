#include <prefix.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

pf_node_t *pf_nodes = NULL;
size_t pf_node_count = 0;

void pf_push(const char8_t *word, uint32_t node) {
  
}

bool pf_load(const char *path) {
  FILE *file = fopen(path, "rb");
  
  if (!file) {
    return false;
  }
  
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  
  if (size % sizeof(pf_node_t)) {
    fclose(file);
    return false;
  }
  
  pf_node_count = size / sizeof(pf_node_t);
  pf_nodes = malloc(pf_node_count * size);
  
  rewind(file);
  fread(pf_nodes, pf_node_count, sizeof(pf_node_t), file);
  
  fclose(file);
  return true;
}

uint32_t pf_next(pf_state_t *state) {
  /* TODO */
  return 0;
}
