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
  todo->due = NULL;
  
  return todo;
}

void print_todo(struct todo_t todo, bool plain) {
  char time_str[64];
  strftime(time_str, sizeof(time_str), "%c", todo.added);
  char due_str[64];
  strftime(due_str, sizeof(due_str), "%c", todo.due);
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
  case TODO_STATE_MAX:
    printf("STATE SET TO TODO_STATE_MAX! This shouldn't be possible\n");
    exit(1);
  }
  
  if (plain) {
    if (todo.due != NULL) {
      printf("%s %-35sCreated: %s, due: %s\n", state_str, todo.task, time_str, due_str);
    } else {
      printf("%s %-35sCreated: %s\n", state_str, todo.task, time_str);
    }
  } else {
    if (todo.due != NULL) {
      printf("%s %-35s\e[3mCreated: %s\e[0m, due: %s\n", state_str, todo.task, time_str, due_str);
    } else {
      printf("%s %-35s\e[3mCreated: %s\e[0m\n", state_str, todo.task, time_str);
    }
  }
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
  char due[21];
  sscanf(str, "%i|%[^|]|%[^|]|%s", &state, task, added, due);
  
  struct tm* time = parse_iso8601_time(added);
  struct tm* due_time = parse_iso8601_time(due);
  struct todo_t* todo = create_todo(task);
  todo->added = time;
  todo->state = state;
  todo->due = due_time;
  return todo;
}
