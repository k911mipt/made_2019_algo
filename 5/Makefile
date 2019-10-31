CC=g++ -O3 -fno-stack-limit -x c++ -std=c++14
APP = sort
HWAPP = test

all: build run_app

run_app:
	./$(APP)

build: build_hello_world build_test

build_test: task.cpp
	$(CC) task.cpp -o $(APP)


build_hello_world: test.cpp
	$(CC) -DDEBUG test.cpp -o $(HWAPP)

clean:
	rm -rf *.o $(HWAPP)