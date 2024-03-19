#include <group.h>
#include <name.h>
#include <word.h>

#include <inttypes.h>
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
      "- `words <word table> <name table>`: Generates a new 'words.bin',\n"
      "  given word and name tables.\n"
      "- `syntax <file>`: Splits into words, phrases and clauses a file,\n"
      "  or stdin if '-' was passed.\n"
      "- `test`: Run all the available test cases.\n",
      
      argv[0]
    );
  } else if (!strcmp(argv[1], "words")) {
    if (argc < 4) {
      errorf("Missing arguments, try `%1$s help`.\n", argv[0]);
      return 1;
    }
    
    FILE *file = fopen(argv[2], "r");
    
    if (!file) {
      errorf("Could not open '%s'.\n", argv[0], argv[2]);
      return 1;
    }
    
    if (!n_load(argv[3])) {
      errorf("Could not open '%s'.\n", argv[0], argv[3]);
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
      
      char8_t *word      = strtok(buffer, u8"\t\n");
      char8_t *root      = strtok(NULL, u8"\t\n");
      char8_t *flags     = strtok(NULL, u8"\t\n");
      size_t   frequency = strtoumax(strtok(NULL, u8"\t\n"), NULL, 0);
      
      if (flags[0] == 'E' || flags[0] == 'F' || flags[0] == 'U') {
        /* Ignore certain word types. */
        continue;
      }
      
      if (flags[0] == 'K') {
        word = root;
        root = u8"";
        
        /* TODO: Use name database to find most likely gender
          (ftp://ftp.heise.de/pub/ct/listings/0717-182.zip) */
        
        flags[0] = 'N';
        flags[2] = 's';
      }
      
      size_t index = w_push_word(word, root, flags, frequency);
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
      .stack_head = 0,
      .unread_head = 0,
      
      .index = 0,
    };
    
    FILE *file = stdin;
    
    if (strcmp(argv[2], "-") && (file = fopen(argv[2], "r"))) {
      errorf("Could not open '%s'.\n", argv[0], argv[2]);
      return 1;
    }
    
    size_t *words = malloc(sizeof(size_t));
    size_t count = 0;
    
    while (true) {
      int     key   = fgetc(file);
      char8_t key_8 = (key == EOF) ? u8'\0' : (char8_t)(key);
      
      size_t index = w_read(&state, key_8);
      
      if (index) {
        /* printf("(%u => '%s')\n", index, w_blobs + w_words[index - 1].word_blob); */
        
        words = realloc(words, (count + 2) * sizeof(size_t));
        words[count++] = index;
      }
      
      if (key == EOF) {
        break;
      }
    }
    
    words[count] = 0;
    
    
    
    free(words);
  }
  
  return 0;
}
