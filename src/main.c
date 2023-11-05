#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

enum todo_state {
  TODO,
  IN_PROGRESS,
  DONE,
};

struct todo_t {
  // owned reference to the allocated memory of 'task'.
  char* task;
  // owned reference to the allocated memory of 'added'
  struct tm* added;
  enum todo_state state;
};

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

struct todo_list_t {
  size_t len;
  size_t capacity;
  struct todo_t** todos;
};

#define DEFAULT_TODO_LIST_CAPACITY 5
#define LIST_SCALE_FACTOR 2

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
    printf("freeing todo item %i out of %zu\n", i+1, todos->len);
    free_todo(todos->todos[i]);
    printf("freed   todo item %i\n", i+1);
  }
  free(todos->todos);
  printf("freed   all todos. freeing list itself now.\n");
  free(todos);
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
    strcpy(state_str, "[/]\0");
    break;
  case DONE:
    strcpy(state_str, "[X]\0");
    break;
  }
  
  printf("%s %-45s\e[3mCreated: %s\e[0m\n", state_str, todo.task, time_str);
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
