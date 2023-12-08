#include <words.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

w_node_t *w_nodes = NULL;
size_t w_node_count = 0, w_node_limit = 0;

w_word_t *w_words = NULL;
size_t w_word_count = 0, w_word_limit = 0;

char8_t *w_blobs = NULL;
size_t w_blob_bytes = 0, w_blob_limit = 0;

static inline size_t w_push_node_single(char8_t key) {
  if (w_node_count == w_node_limit) {
    w_node_limit += 262144;
    w_nodes = realloc(w_nodes, w_node_limit * sizeof(w_node_t));
  }
  
  w_nodes[w_node_count++] = (w_node_t) {
    .key = key,
    .index = 0,
    
    .sibling = 0,
    .child = 0,
  };
  
  return w_node_count;
}

void w_push_node(const char8_t *word, size_t index) {
  if (!w_node_count) {
    w_push_node_single(*word);
  }
  
  size_t node = 0;
  
  while (*word) {
    if (w_nodes[node].key == *word) {
      word++;
      
      if (*word) {
        if (!w_nodes[node].child) {
          w_nodes[node].child = w_push_node_single(*word);
        }
        
        node = w_nodes[node].child - 1;
      }
    } else {
      if (!w_nodes[node].sibling) {
        w_nodes[node].sibling = w_push_node_single(*word);
      }
      
      node = w_nodes[node].sibling - 1;
    }
  }
  
  if (!w_nodes[node].index) {
    w_nodes[node].index = index;
    return;
  }
  
  size_t tail = w_nodes[node].index - 1;
  
  while (w_words[tail].next_word) {
    tail = w_words[tail].next_word - 1;
  }
  
  w_words[tail].next_word = index;
}

size_t w_push_word(const char8_t *word, const char8_t *root,
    const char8_t *flags, size_t frequency) {
  if (w_word_count == w_word_limit) {
    w_word_limit += 262144;
    w_words = realloc(w_words, w_word_limit * sizeof(w_word_t));
  }
  
  w_words[w_word_count] = (w_word_t) {
    .flags = {0},
    .frequency = frequency,
    
    .next_word = 0,
    
    .word_blob = w_push_blob(word),
    .root_blob = w_push_blob(root),
  };
  
  strncpy(w_words[w_word_count].flags, flags, 7);
  return ++w_word_count;
}

size_t w_push_blob(const char8_t *blob) {
  size_t start = w_blob_bytes;
  w_blob_bytes += strlen(blob) + 1;
  
  if (w_blob_bytes > w_blob_limit) {
    w_blob_limit = w_blob_bytes + 1048576;
    w_blobs = realloc(w_blobs, w_blob_limit);
  }
  
  strcpy(w_blobs + start, blob);
  return start;
}

bool w_load(const char *path) {
  FILE *file = fopen(path, "rb");
  
  if (!file) {
    return false;
  }
  
  uint64_t checksum = 0;
  w_header_t header;
  
  rewind(file);
  fread(&header, sizeof(w_header_t), 1, file);
  
  if (header.magic != W_MAGIC) {
    fclose(file);
    return false;
  }
  
  rewind(file);
  
  while (!feof(file)) {
    uint32_t dword = 0;
    fread(&dword, 4, 1, file);
    
    checksum += dword;
    
    while (checksum >> 32) {
      checksum = (checksum & 0xFFFFFFFF) + (checksum >> 32);
    }
  }
  
  if ((uint32_t)(checksum) != 0xFFFFFFFF) {
    fclose(file);
    return false;
  }
  
  fseek(file, sizeof(w_header_t), SEEK_SET);
  
  w_nodes = malloc((w_node_count = header.node_count) * sizeof(w_node_t));
  fread(w_nodes, sizeof(w_node_t), w_node_count, file);
  
  w_words = malloc((w_word_count = header.word_count) * sizeof(w_word_t));
  fread(w_words, sizeof(w_word_t), w_word_count, file);
  
  w_blobs = malloc(w_blob_bytes = header.blob_bytes);
  fread(w_blobs, 1, w_blob_bytes, file);
  
  fclose(file);
  return true;
}

bool w_save(const char *path) {
  FILE *file = fopen(path, "wb+");
  
  if (!file) {
    return false;
  }
  
  uint64_t checksum = 0;
  
  w_header_t header = (w_header_t) {
    .magic = W_MAGIC,
    .checksum = 0,
    
    .node_count = w_node_count,
    .word_count = w_word_count,
    .blob_bytes = w_blob_bytes,
  };
  
  rewind(file);
  fwrite(&header, sizeof(w_header_t), 1, file);
  
  fwrite(w_nodes, sizeof(w_node_t), w_node_count, file);
  fwrite(w_words, sizeof(w_word_t), w_word_count, file);
  fwrite(w_blobs, 1, w_blob_bytes, file);
  
  rewind(file);
  
  while (!feof(file)) {
    uint32_t dword = 0;
    fread(&dword, 4, 1, file);
    
    checksum += dword;
    
    while (checksum >> 32) {
      checksum = (checksum & 0xFFFFFFFF) + (checksum >> 32);
    }
  }
  
  header.checksum = ~((uint32_t)(checksum & 0xFFFFFFFF));
  
  rewind(file);
  fwrite(&header, sizeof(w_header_t), 1, file);
  
  fclose(file);
  return true;
}

size_t w_read(w_state_t *state, char8_t key) {
  while (state->unread_head) {
    size_t unread_head = state->unread_head;
    state->unread_head = 0;
    
    size_t index = w_read(state, state->unread[--unread_head]);
    state->unread_head = unread_head;
    
    if (index) {
      memmove(state->unread + 1, state->unread, state->unread_head * sizeof(size_t));
      state->unread[0] = key;
      
      state->unread_head++;
      return index;
    }
  }
  
  size_t index = 0;
  
  while (state->index < w_node_count) {
    if (w_nodes[state->index].key == key) {
      if (!w_nodes[state->index].child) {
        index = w_nodes[state->index].index;
        break;
      }
      
      state->stack[state->stack_head++] = state->index;
      state->index = w_nodes[state->index].child - 1;
      
      return 0;
    }
    
    state->index = w_nodes[state->index].sibling - 1;
  }
  
  for (size_t i = state->stack_head - 1; !index && i < state->stack_head; i--) {
    if (w_nodes[state->stack[i]].index) {
      state->unread[state->unread_head++] = key;
      
      for (size_t j = state->stack_head - 1; j > i; j--) {
        state->unread[state->unread_head++] = w_nodes[state->stack[j]].key;
      }
      
      index = w_nodes[state->stack[i]].index;
    }
  }
  
  state->stack_head = state->index = 0;
  return index;
}
