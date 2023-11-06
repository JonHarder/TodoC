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

void test_fails() {
  TestStart("test_fails");
  assert(1 == 2);
  TestEnd();
}

int main(int argc, char *argv[]) {

  num_tests = 0;
  tests_passed = 0;
  done = 0;
  // BEGIN TESTS
  test_empty_list();
  test_fails();
  // END TESTS
  printf("Total tests passed: %d\n", tests_passed);
  return !(tests_passed == num_tests);
}
