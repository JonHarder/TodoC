#include<assert.h>
#include<stdio.h>

#include "../src/todo_list.h"

#ifdef assert
#undef assert
#endif
#define assert(x) (rslt = rslt && (x))

static int rslt;

// test suite main variables
static int done;
static int num_tests;
static int tests_passed;


//  utility function
void TestStart(char *name) {
    num_tests++;
    rslt = 1;
    printf("-- Testing %s ... ",name);
}

//  utility function
void TestEnd() {
    if (rslt) tests_passed++;
    printf("%s\n", rslt ? "success" : "fail");
}

void test_empty_list() {
  TestStart("test_empty_list");
  struct todo_list_t* list = empty_todo_list();
  assert(list->len == 0);
  free_todo_list(list);
  TestEnd();
}

void test_adding_to_empty_list_increments_len() {
  TestStart("test_adding_to_empty_list_increments_len");
  struct todo_list_t* list = empty_todo_list();
  size_t initial_len = list->len;
  struct todo_t* todo = create_todo("a task");
  add_todo(list, todo);
  assert(initial_len+1 == list->len);
  TestEnd();
}

int main(int argc, char *argv[]) {

  num_tests = 0;
  tests_passed = 0;
  done = 0;
  // BEGIN TESTS
  test_empty_list();
  test_adding_to_empty_list_increments_len();
  // END TESTS
  printf("Total tests passed: %d\n", tests_passed);
  return !(tests_passed == num_tests);
}
