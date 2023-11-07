#include <stdlib.h>
#include "todo.h"

struct todo_list_t {
  size_t len;
  size_t capacity;
  struct todo_t** todos;
};

#define DEFAULT_TODO_LIST_CAPACITY 5
#define LIST_SCALE_FACTOR 2

struct todo_list_t*
empty_todo_list();

void
grow_todo_list(struct todo_list_t* todos);

void
add_todo(struct todo_list_t* todos, struct todo_t* todo);

void
free_todo(struct todo_t* todo);

void
free_todo_list(struct todo_list_t* todos);

void
print_todo_list(struct todo_list_t todos);

int
save_todos(struct todo_list_t todos);

struct todo_list_t*
read_todos();

void clear_todos();
