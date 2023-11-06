#include "todo_list.h"

int main(int argc, char *argv[]) {
  struct todo_list_t* todos = read_todos();
  for (int i=1; i<argc; i++) {
    struct todo_t* todo = create_todo(argv[i]);
    add_todo(todos, todo);
  }
  print_todo_list(*todos);
  save_todos(*todos);
  free_todo_list(todos);
}
