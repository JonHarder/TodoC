#include "todo_list.h"

#include <libgen.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

struct options_t {
  bool clear;
  char* task;
  bool list_tasks;
  int update_id;
  int delete_id;
};

void print_help(char* name) {
  printf("Usage: %s [arguments]\n", name);
  printf("   Or: %s <task>\n", name);
  printf("\n");
  printf("Arguments:\n");
  printf("  -c\t\t\tClear the current todo list of all tasks (can't be undone)\n");
  printf("  -d <id>\t\tDelete task in position <id>\n");
  printf("  -h\t\t\tPrint this help\n");
  printf("  -l\t\t\tList tasks\n");
  printf("  -t <task>\t\tAdd the task <task> to the list\n");
  printf("  -u <id>\t\tUpdate the task number <id> to the next status\n");
}

void parse_args(int *argc, char **argv[], struct options_t* options) {
  int opt;
  while((opt = getopt(*argc, *argv, "cd:hlt:u:")) != -1) {
    switch (opt) {
    case 'c': // mutually exclusive
      options->clear = true;
      break;
    case 't': // mutually exclusive
      options->task = optarg;
      break;
    case 'd': // mutually exclusive
      options->delete_id = atoi(optarg);
      break;
    case 'h':
      print_help(basename(*argv[0]));
      exit(0);
    case 'u':
      options->update_id = atoi(optarg);
      break;
    case 'l':
    case '?':
    default:
      options->list_tasks = true;
      break;
    }
  }
  *argc -= optind;
  *argv += optind;
}

int main(int argc, char *argv[]) {
  struct options_t options = {
    .clear = false,
    .task = "",
    .delete_id = -1,
    .update_id = -1,
    .list_tasks = false
  };
  parse_args(&argc, &argv, &options);

  struct todo_list_t* todos = read_todos();

  if (argc > 0) {
    // positional args still remaining.
    // treat implicitly as tasks to add to list.
    for (int i=0; i<argc; i++) {
      struct todo_t* todo = create_todo(argv[i]);
      add_todo(todos, todo);
      print_todo_list(*todos);
      save_todos(*todos);
    }
  }


  if (options.clear) {
    free_todo_list(todos);
    todos = empty_todo_list();
    save_todos(*todos);
    printf("Tasks cleared.\n");
  }

  if (options.task[0] != '\0') {
    struct todo_t* todo = create_todo(options.task);
    add_todo(todos, todo);
    save_todos(*todos);
  }

  if (options.update_id != -1) {
    update_todo(todos, options.update_id-1);
    save_todos(*todos);
  }

  if (options.delete_id != -1) {
    // TODO, how to handle removal of elements from growable list?
    // I'd like to avoid having to shift all the memory after the
    // element, but mayke that can't be avoided.
    printf("Not Implimented!\n");
  }

  print_todo_list(*todos);

  free_todo_list(todos);
}
