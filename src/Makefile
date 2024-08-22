# Compilation
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu11

# Debug
CFLAGS += -Werror
CFLAGS  += -fsanitize=address -fno-omit-frame-pointer
LDFLAGS += -fsanitize=address

# Libs
CFLAGS  += $(shell pkg-config --cflags sdl2)
LDFLAGS += $(shell pkg-config --libs sdl2)
CFLAGS  += $(shell pkg-config --cflags SDL2_image)
LDFLAGS += $(shell pkg-config --libs SDL2_image)
CFLAGS  += $(shell pkg-config --cflags SDL2_ttf)
LDFLAGS += $(shell pkg-config --libs SDL2_ttf)

LDFLAGS += -lm # math library

# .c source files	
LOOP_SRCS = loop.c
MENU_SRCS = main_menu.c
BACK_SRCS = $(wildcard back/*.c)
FRONT_SRCS = $(wildcard front/*.c)
TEST_SRCS = $(wildcard test/*.c) 

# compiled files
BACK_OBJS = $(BACK_SRCS:.c=.o)
FRONT_OBJS = $(FRONT_SRCS:.c=.o)
# obj files needed for the game 
LOOP_OBJS = $(LOOP_SRCS:.c=.o) $(BACK_OBJS) $(FRONT_OBJS)
MENU_OBJS = $(MENU_SRCS:.c=.o) $(BACK_OBJS) $(FRONT_OBJS)
# tests
TEST_OBJS = $(TEST_SRCS:.c=.o)

# Header files
BACK_HEADERS = $(wildcard back/*.h)
FRONT_HEADERS = $(wildcard front/*.h)

# binaries
# binaries
LOOP_TARGET = loop
MENU_TARGET = main_menu

TEST_BIN = $(TEST_SRCS:.c=)

.PHONY: all clean test run_tests

all: $(LOOP_TARGET) $(MENU_TARGET)

$(LOOP_TARGET): $(LOOP_OBJS) 
	$(CC) $(CFLAGS) $(LOOP_OBJS) -o $@ $(LDFLAGS)

$(MENU_TARGET): $(MENU_OBJS) 
	$(CC) $(CFLAGS) $(MENU_OBJS) -o $@ $(LDFLAGS)

# Compile .c files to .o files
%.o: %.c $(BACK_HEADERS) $(FRONT_HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@


# Test target
test: bin_tests
# run the tests
	@total_tests=0; \
	passed_tests=0; \
	for test_bin in $(TEST_BIN); do \
		total_tests=$$((total_tests + 1)); \
		echo "\nExécution de $${test_bin}."; \
		./$$test_bin; \
		if [ $$? -ne 0 ]; then \
			echo "Échec de $$test_bin !"; \
		else \
			echo "Réussite de $$test_bin."; \
			passed_tests=$$((passed_tests + 1)); \
		fi; \
	done; \
	echo "\nRésultat des tests unitaires : $$passed_tests / $$total_tests tests réussis."

# link test binaries
bin_tests: $(TEST_OBJS) $(BACK_OBJS) $(FRONT_OBJS)
		for test_bin in $(TEST_BIN); do \
		$(CC) $(LDFLAGS) $${test_bin}.o $(BACK_OBJS) $(FRONT_OBJS) -o $$test_bin; \
	done

clean:
	rm -fr $(LOOP_TARGET) $(MENU_TARGET) $(LOOP_OBJS) $(MENU_OBJS) $(TEST_OBJS) $(TEST_BIN)