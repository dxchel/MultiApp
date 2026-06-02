PROGRAM=multiapp
OBJECTS=build/main_application.o build/browser_app.o build/fractal_app.o build/chat_app.o build/session.o
MAIN=build/main.o
TESTS=test_build/main.o test_build/main_application_tests.o test_build/browser_app_tests.o test_build/fractal_app_tests.o test_build/chat_app_tests.o

CC=g++
CFLAGS=$(shell pkg-config --cflags gtkmm-4.0 webkitgtk-6.0 gtest_main)
CFLAGS+= -Wall -Wextra -Werror -g -std=c++20

LDFLAGS=$(shell pkg-config --libs gtkmm-4.0 webkitgtk-6.0 gtest_main)

multiapp: $(OBJECTS) $(MAIN)
	$(CC) $(OBJECTS) $(MAIN) $(LDFLAGS) -o $(PROGRAM).exe

$(OBJECTS): | build

build:
	mkdir build

build/%.o : src/%.cc $(wildcard include/%.hpp)
	$(CC) $(CFLAGS) -c $< -o $@

tests: $(OBJECTS) $(TESTS)
	$(CC) $(OBJECTS) $(TESTS) $(LDFLAGS) -o $(PROGRAM)_tests.exe

$(TESTS): | test_build

test_build:
	mkdir test_build

test_build/%.o : tests/%.cc
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -rf *build *.exe 

release: CFLAGS+= -O3
release: LDFLAGS+= -s
release: multiapp
