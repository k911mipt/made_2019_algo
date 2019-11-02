CC11x32=g++ -O2 -fno-stack-limit -x c++ -std=c++11 -m32
CC11=g++ -O2 -fno-stack-limit -x c++ -std=c++11
CC17=g++ -O2 -fno-stack-limit -x c++ -std=c++17
CC=$(CC11)
CCDEBUG = $(CC) -g3 -DDEBUG
APP = sort
SINGLEAPP = single
HWAPP = test

all: build run_app

mem: build_single run_single

run_app:
	./$(APP)

run_single:
	valgrind --tool=cachegrind ./$(SINGLEAPP)

build: build_hello_world build_test

build_test: task.cpp
	$(CC) task.cpp -o $(APP)

build_single: single.cpp
	$(CC) single.cpp -o $(SINGLEAPP)

build_hello_world: test.cpp
	$(CCDEBUG) test.cpp -o $(HWAPP)

clean:
	rm -rf *.o $(HWAPP) $(APP) $(SINGLEAPP)