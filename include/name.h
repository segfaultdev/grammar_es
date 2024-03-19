#ifndef __NAME_H__
#define __NAME_H__

#include <stdint.h>
#include <uchar.h>

typedef struct n_name_t n_name_t;

struct n_name_t {
  const char8_t *text;
  char8_t gender;
};

extern n_name_t *n_names;
extern size_t n_name_count, n_name_limit;

n_name_t *n_find_name(const char8_t *text);
void      n_push_name(const char8_t *text, char8_t gender);

#endif
