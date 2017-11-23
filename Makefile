CC = cc
LIBRARY_NAME = icemu
TARGET_LIB = lib$(LIBRARY_NAME).a
TEST_TARGET = test_icemu

CFLAGS = -Wall

SRCS = pin.c chip.c shiftregister.c decoder.c util.c
OBJS = $(addprefix src/, $(SRCS:%.c=%.o))
TEST_SRCS = main.c shiftregister.c decoder.c circuit.c
TEST_OBJS = $(addprefix tests/, $(TEST_SRCS:%.c=%.o))

.PHONY: all
all: $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	ar rcs $@ $^

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf $(OBJS) $(TARGET_LIB) $(TEST_OBJS) $(TEST_TARGET)

$(TEST_TARGET): $(TARGET_LIB) $(TEST_OBJS)
	$(CC) -static -L. -o $@ $(TEST_OBJS) -l$(LIBRARY_NAME) 

.PHONY: test
test: $(TEST_TARGET)
	./$(TEST_TARGET)
