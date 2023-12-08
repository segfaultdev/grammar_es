#ifndef __WORDS_H__
#define __WORDS_H__

#include <stdint.h>
#include <stdio.h>
#include <uchar.h>

#define W_MAGIC       0xB2C14970
#define W_STACK_LIMIT 256

typedef struct w_state_t w_state_t;
typedef struct w_header_t w_header_t;

typedef struct w_node_t w_node_t;
typedef struct w_word_t w_word_t;

struct w_state_t {
  size_t stack[W_STACK_LIMIT];
  size_t stack_head;
  
  char8_t unread[W_STACK_LIMIT];
  size_t unread_head;
  
  size_t index;
};

struct w_header_t {
  uint32_t magic; /* Noam Chomsky's birthday in (signed) UNIX time. */
  uint32_t checksum;
  
  uint32_t node_count;
  uint32_t word_count;
  uint32_t blob_bytes;
} __attribute__((packed));

struct w_node_t {
  uint32_t key:   8;
  uint32_t index: 24;
  
  uint32_t sibling, child;
} __attribute__((packed));

struct w_word_t {
  char8_t flags[8];
  uint32_t frequency;
  
  uint32_t next_word;
  uint32_t word_blob, root_blob;
} __attribute__((packed));

extern w_node_t *w_nodes;
extern size_t w_node_count, w_node_limit;

extern w_word_t *w_words;
extern size_t w_word_count, w_word_limit;

extern char8_t *w_blobs;
extern size_t w_blob_bytes, w_blob_limit;

void   w_push_node(const char8_t *word, size_t index);
size_t w_push_word(const char8_t *word, const char8_t *root,
    const char8_t *flags, size_t frequency);
size_t w_push_blob(const char8_t *blob);

bool w_load(const char *path);
bool w_save(const char *path);

size_t w_read(w_state_t *state, char8_t letter);

#endif
