CC = cc
LIBRARY_NAME = icemu
TARGET_LIB = $(LIBRARY_NAME).so
TEST_TARGET = test_icemu

CFLAGS = -Wall -fPIC
LDFLAGS = -shared

SRCS = pin.c chip.c shiftregister.c decoder.c util.c
OBJS = $(addprefix src/, $(SRCS:%.c=%.o))
TEST_SRCS = main.c shiftregister.c decoder.c circuit.c
TEST_OBJS = $(addprefix tests/, $(TEST_SRCS:%.c=%.o))

.PHONY: all
all: $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf $(OBJS) $(TARGET_LIB) $(TEST_OBJS) $(TEST_TARGET)

$(TEST_TARGET): $(OBJS) $(TEST_OBJS)
	$(CC) -o $@ $^

.PHONY: test
test: $(TEST_TARGET)
	./$(TEST_TARGET)
