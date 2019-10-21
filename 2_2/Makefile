CC=g++ -O2 -fno-stack-limit -x c++ -std=c++14
APP = app

all: build_debug_app run_app

run_app:
	./$(APP)

build_debug_app: main.cpp
	$(CC) -DDEBUG main.cpp -o $(APP)

clean:
	rm -rf *.o $(APP)