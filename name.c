#include <name.h>

#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

n_name_t *n_names = NULL;
size_t n_name_count = 0, n_name_limit = 0;

int n_test_name(const void *a_void, const void *b_void) {
  const n_name_t *a = a_void, *b = b_void;
  
  for (size_t i = 0; a->text[i] || b->text[i]; i++) {
    if (a->text[i] != b->text[i]) {
      char8_t a_char = a->text[i], b_char = b->text[i];
      
      if (a_char == 0xB1) {
        a_char = 0x91;
      } else {
        a_char = toupper(a_char);
      }
      
      if (b_char == 0xB1) {
        b_char = 0x91;
      } else {
        b_char = toupper(b_char);
      }
      
      return ((int)(a_char) - (int)(b_char));
    }
  }
  
  return 0;
}

n_name_t *n_find_name(const char8_t *text) {
  const n_name_t name = (n_name_t) {.text = text};
  
  return bsearch(
    &name, n_names, n_name_count, sizeof(n_name_t), n_test_name
  );
}

void n_push_name(const char8_t *text, char8_t gender) {
  n_name_t *name = n_find_name(text);
  
  if (name->gender != gender) {
    name->gender = 'c';
    return;
  }
  
  
}
