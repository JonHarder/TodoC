#include "todo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Returns a reference to: struct Todo_t.
// the caller then takes ownership of the memory allocation.
// When done, call: free_todo(..).
struct todo_t* create_todo(char* task) {
  struct todo_t* todo = malloc(sizeof(struct todo_t));

  size_t len = strlen(task);
  char* new_task = malloc(len);
  strcpy(new_task, task);
  todo->task = new_task;
  time_t now = time(NULL);
  struct tm* time = malloc(sizeof(struct tm));
  localtime_r(&now, time);
  todo->added = time;
  todo->state = TODO;
  
  return todo;
}

void print_todo(struct todo_t todo) {
  char time_str[64];
  strftime(time_str, sizeof(time_str), "%c", todo.added);
  char state_str[8];
  switch(todo.state) {
  case TODO:
    strcpy(state_str, "[ ]\0");
    break;
  case IN_PROGRESS:
    strcpy(state_str, "[-]\0");
    break;
  case DONE:
    strcpy(state_str, "[X]\0");
    break;
  }
  
  printf("%s %-45s\e[3mCreated: %s\e[0m\n", state_str, todo.task, time_str);
}

struct tm* parse_iso8601_time(char* str) {
  struct tm* time = malloc(sizeof(struct tm));
  strptime(str, "%FT%T", time);
  return time;
}

struct todo_t* read_todo(char* str) {
    enum todo_state state;
    char task[64];
    char added[21];
    sscanf(str, "%i|%[^|]|%s", &state, task, added);

    struct tm* time = parse_iso8601_time(added);
    struct todo_t* todo = create_todo(task);
    todo->added = time;
    todo->state = state;
    return todo;
}
