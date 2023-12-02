#include <prefix.h>

#include <string.h>
#include <stdio.h>

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
      "- `prefix <file>`: Generates a new 'prefix.bin', given a file.\n"
      "- `syntax <file>`: Parses a file, or stdin if '-' was passed.\n",
      
      argv[0]
    );
  } else if (!strcmp(argv[1], "prefix")) {
    if (argc <= 2) {
      errorf("Missing arguments, try `%1$s help`.\n", argv[0]);
      return 1;
    }
    
    /* TODO: Generate 'prefix.bin'. */
  } else if (!strcmp(argv[1], "syntax")) {
    if (argc <= 2) {
      errorf("Missing arguments, try `%1$s help`.\n", argv[0]);
      return 1;
    }
    
    if (!pf_load("prefix.bin")) {
      errorf("'prefix.bin' not generated, try `%1$s help`.\n", argv[0]);
      return 1;
    }
    
    pf_state_t state = (pf_state_t) {
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
      uint32_t word = pf_next(&state);
      
      if (!word) {
        break;
      }
      
      printf("(%u)", word);
    }
    
    printf("\n");
  }
  
  return 0;
}
