#include "todo_list.h"

typedef enum bool_t {
  false,
  true,
} bool;

struct options_t {
  bool clear;
};

void parse_args(struct options_t* options) {
  options->clear = true;
}

int main(int argc, char *argv[]) {
  struct options_t options;
  // options.clear = false;
  parse_args(&options);

  struct todo_list_t* todos = read_todos();
  for (int i=1; i<argc; i++) {
    struct todo_t* todo = create_todo(argv[i]);
    add_todo(todos, todo);
  }
  print_todo_list(*todos);
  save_todos(*todos);
  free_todo_list(todos);
}
