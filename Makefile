CC = cc
LIBRARY_NAME = icemu
TARGET_LIB = $(LIBRARY_NAME).so
TEST_TARGET = test_icemu

CFLAGS = -Wall -fPIC
LDFLAGS = -shared

SRCS = icemu.c
OBJS = $(SRCS:%.c=%.o)
TEST_SRCS = tests.c
TEST_OBJS = $(TEST_SRCS:%.c=%.o)

.PHONY: all
all: $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $(.TARGET) $(.ALLSRC)

.c.o:
	$(CC) $(CFLAGS) -c -o $(.TARGET) $(.IMPSRC)

.PHONY: clean
clean:
	rm -rf $(OBJS) $(TARGET_LIB) $(TEST_OBJS) $(TEST_TARGET)

$(TEST_TARGET): $(OBJS) $(TEST_OBJS)
	$(CC) -o $(.TARGET) $(.ALLSRC)

.PHONY: test
test: $(TEST_TARGET)
	./$(TEST_TARGET)
