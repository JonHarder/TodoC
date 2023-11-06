#include <stdio.h>
#include <stdlib.h>

// Populates the provided char buffer with the config file path.
// Must be allocated witn at least 64 bytes of memory.
// returns non-zero if an error was encountered.
int get_config_file(char* buffer) {
  char* home = getenv("HOME");
  if(home == NULL) {
    printf("HOME environment variable not set, unable to locate config directory.\n");
    return 1;
  }
  snprintf(buffer, 64, "%s/.config/todo/todos", home);
  return 0;
}

// Returns a char pointer to the configuration file.
// The caller assumes responsibility to free the pointer
// when done.
int get_config_dir(char* buffer) {
  char* home = getenv("HOME");
  if(home == NULL) {
    printf("HOME environment variable not set, unable to locate config directory.\n");
    return 1;
  }
  snprintf(buffer, 64, "%s/.config/todo", home);
  return 0;
}
