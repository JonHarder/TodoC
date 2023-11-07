todo: src/*.c
	mkdir -p build && clang src/main.c src/todo.c src/todo_list.c src/config.c -o build/todo
