#include <words.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <uchar.h>

#define errorf(format, ...) fprintf(stderr, "%s: " format, __VA_ARGS__)

int main(int argc, char const *argv[]) {
  if (argc <= 1) {
    errorf("Missing arguments, try `%1$s help`.\n", argv[0]);
    return 1;
  }
  
  if (!strcmp(argv[1], "help")) {
    printf(
      "Grammatical model implementation, by Julio Meroño Sáez.\n\n"
      "Usage: `%s <command> ...`\n"
      "- `help`: Prints a brief description and a list of commands.\n"
      "- `words <file>`: Generates a new 'words.bin', given a CSV file.\n"
      "- `syntax <file>`: Parses a file, or stdin if '-' was passed.\n",
      
      argv[0]
    );
  } else if (!strcmp(argv[1], "words")) {
    if (argc <= 2) {
      errorf("Missing arguments, try `%1$s help`.\n", argv[0]);
      return 1;
    }
    
    FILE *file = fopen(argv[2], "r");
    
    if (!file) {
      errorf("Could not open '%s'.\n", argv[2]);
      return 1;
    }
    
    char8_t *buffer = NULL;
    
    while (!feof(file)) {
      size_t length;
      
      if (getline((char **)(&buffer), &length, file) < 0) {
        break;
      }
      
      if (length < 11) {
        /* An entry cannot be shorter than 11 characters. */
        continue;
      }
      
      char8_t *word  = strtok(buffer, u8"\t\n");
      char8_t *root  = strtok(buffer, u8"\t\n");
      char8_t *flags = strtok(buffer, u8"\t\n");
      
      size_t index = w_push_word(flags[0], word, root);
      w_push_node(word, index);
    }
    
    if (buffer) {
      free(buffer);
    }
    
    printf(
      "'%s' -> 'words.bin'\n"
      " - Node count: %lu\n"
      " - Word count: %lu\n"
      " - Blob bytes: %lu\n",
      argv[2],
      w_node_count,
      w_word_count,
      w_blob_bytes
    );
    
    if (!w_save("words.bin")) {
      errorf("Could not open 'words.bin'.\n", argv[0]);
      return 1;
    }
  } else if (!strcmp(argv[1], "syntax")) {
    if (argc <= 2) {
      errorf("Missing arguments, try `%1$s help`.\n", argv[0]);
      return 1;
    }
    
    if (!w_load("words.bin")) {
      errorf("Could not open 'words.bin', try `%1$s help`.\n", argv[0]);
      return 1;
    }
    
    w_state_t state = (w_state_t) {
      .file = (strcmp(argv[2], "-") ? fopen(argv[2], "r") : stdin),
      
      .stack = {0},
      .head = 0,
    };
    
    if (!state.file) {
      errorf("Could not open '%s'.\n", argv[2]);
      return 1;
    }
    
    /* TODO: Do something useful with the produced words. */
    
    while (true) {
      size_t word = w_next(&state);
      
      if (!word) {
        break;
      }
      
      printf("(%u)", word);
    }
    
    printf("\n");
  }
  
  return 0;
}
