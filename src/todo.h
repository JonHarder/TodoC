#include <stdbool.h>

enum todo_state {
  TODO,
  IN_PROGRESS,
  DONE,
  TODO_STATE_MAX,
};

struct todo_t {
  // owned reference to the allocated memory of 'task'.
  char* task;
  // owned reference to the allocated memory of 'added'
  struct tm* added;
  enum todo_state state;
};

struct todo_t*
create_todo(char* task);

void
print_todo(struct todo_t todo, bool plain);

struct todo_t*
read_todo(char* str);
