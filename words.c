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
  
  w_nodes[w_node_count++] = (w_node_t) {key, 0, 0, 0};
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
          size_t child = w_push_node_single(*word);
          w_nodes[node].child = child;
        }
        
        node = w_nodes[node].child - 1;
      }
    } else {
      if (!w_nodes[node].sibling) {
        size_t sibling = w_push_node_single(*word);
        w_nodes[node].sibling = sibling;
      }
      
      node = w_nodes[node].sibling - 1;
    }
  }
  
  if (!w_nodes[node].index) {
    w_nodes[node].index = index;
    return;
  }
  
  /* TODO */
}

size_t w_push_word(char8_t type, const char8_t *word, const char8_t *root) {
  if (w_word_count == w_word_limit) {
    w_word_limit += 262144;
    w_words = realloc(w_words, w_word_limit * sizeof(w_word_t));
  }
  
  w_words[w_word_count++] = (w_word_t) {
    .type = type,
    
    .next = 0,
    
    .word_blob = w_push_blob(word),
    .root_blob = w_push_blob(root),
  };
  
  return w_word_count;
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

size_t w_next(w_state_t *state) {
  /* TODO */
  return 0;
}
