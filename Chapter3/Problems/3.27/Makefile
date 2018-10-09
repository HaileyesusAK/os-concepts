APP_NAME = filecopy 
CC = gcc
CFLAGS = 
LFLAGS = 
INCLUDE = 

%.o: %.c %.h
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

SRCS = filecopy.c
OBJS = $(patsubst %.c, %.o, $(filter %.c, $(SRCS)))

build: $(OBJS)
	$(CC)  $^ $(LFLAGS) -o $(APP_NAME)

clean:
	rm -vf $(APP_NAME) $(OBJS)

.PHONY: clean
