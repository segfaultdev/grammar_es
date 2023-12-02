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
      "Usage: %s <command> ...\n"
      "- 'help': Prints a brief description and a list of commands.\n"
      "- 'prefix <file>': Generates a new 'prefix.bin', given a file.\n"
      "- 'parse <file>': Parses a file, or stdin if '-' was passed.\n",
      
      argv[0]
    );
  } else if (!strcmp(argv[1], "prefix")) {
    
  }
  
  return 0;
}
