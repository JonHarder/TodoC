#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include "todo_list.h"
#include "config.h"

struct todo_list_t* empty_todo_list() {
  struct todo_list_t* todos = malloc(sizeof(struct todo_list_t));
  todos->todos = malloc(sizeof(struct todo_t) * DEFAULT_TODO_LIST_CAPACITY);
  todos->capacity = DEFAULT_TODO_LIST_CAPACITY;
  todos->len = 0;
  return todos;
}

void grow_todo_list(struct todo_list_t* todos) {
  todos->capacity *= LIST_SCALE_FACTOR;
  struct todo_t** mem = realloc(todos->todos, todos->capacity * sizeof(struct todo_t));
  if(mem == NULL) {
      perror("realloc():");
      exit(1);
  }
  todos->todos = mem;
}

void add_todo(struct todo_list_t* todos, struct todo_t* todo) {
  if(todos->len >= todos->capacity) {
    grow_todo_list(todos);
  }
  todos->todos[todos->len] = todo;
  todos->len += 1;
}

void free_todo(struct todo_t* todo) {
  free(todo->task);
  free(todo->added);
  free(todo);
}

void free_todo_list(struct todo_list_t* todos) {
  for (int i = 0; i<todos->len; i++) {
    free_todo(todos->todos[i]);
  }
  free(todos->todos);
  free(todos);
}

void print_todo_list(struct todo_list_t todos) {
  for (int i=0; i<todos.len; i++) {
    if(i+1 < 10) {
      printf("%i.  ", i+1);
    } else {
      printf("%i. ", i+1);
    }
    print_todo(*todos.todos[i]);
  }
}

int save_todos(struct todo_list_t todos) {
  char config_dir[64];
  if(get_config_dir(config_dir)) {
    printf("error: get_config_dir()\n");
    return 1;
  }
  struct stat s = {0};
  // get info on the directory
  if(stat(config_dir, &s)) {
    // if stat() returns non-zero, check what the issue was
    if(errno == ENOENT) {
      // if the directory doesn't exist, make it
      mkdir(config_dir, 0700);
    } else {
      // for all other errors, just bail.
      perror("stat()");
      return 1;
    }
  }

  // re-stat() in case the directory was created and the stat struct
  // needs to be updated
  stat(config_dir, &s);
  if(!S_ISDIR(s.st_mode)) {
    printf("%s exists but is not a directory. bailing\n", config_dir);
    return 1;
  }

  char config_file[64];
  if(get_config_file(config_file)) {
    printf("error: get_config_file()\n");
    return 1;
  }
  FILE* f = fopen(config_file, "w");
  char buf[100];
  for(int i=0; i<todos.len; i++) {
    struct todo_t* todo = todos.todos[i];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", todo->added);
    fprintf(f, "%i|%s|%s\n", todo->state, todo->task, buf);
  }
  fclose(f);
  return 0;
}

struct todo_list_t* read_todos() {
  char config_file[64];
  if(get_config_file(config_file)) {
    printf("error: get_config_file()\n");
    return NULL;
  }
  FILE* f = fopen(config_file, "r");
  if(f == NULL) {
    perror("fopen config_file:");
    return NULL;
  }
  struct todo_list_t* todos = empty_todo_list();
  char* line = NULL;
  size_t len = 0;
  while (getline(&line, &len, f) != -1) {
    struct todo_t* todo = read_todo(line);
    add_todo(todos, todo);
  }
  if(line) {
    free(line);
  }
  return todos;
}
