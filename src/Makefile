CC = clang
CFLAGS = -Wall -Werror -g -Og
LDFLAGS = -rcs
HEADERS = config.h mlpt.h
OBJS = mlpt.o
TARGET = libmlpt.a
TEST_SRCS = test.c mlpt_2.c 
TEST_TARGET = test_suite

all: $(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET) && rm -f $(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRCS)
	$(CC) $(CFLAGS) $(TEST_SRCS) -o $(TEST_TARGET)

$(TARGET): $(OBJS)
	ar $(LDFLAGS) $@ $(OBJS)

*.o: *.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@

clean:
	rm -f $(TARGET) $(OBJS)
