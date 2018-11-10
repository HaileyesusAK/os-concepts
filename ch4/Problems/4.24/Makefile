APP_NAME = calc_pie
CC = gcc
CFLAGS = 
LFLAGS = -lpthread -lm
INCLUDE = 

%.o: %.c %.h
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

SRCS = main.c
OBJS = $(patsubst %.c, %.o, $(filter %.c, $(SRCS)))

build: $(OBJS)
	$(CC)  $^ $(LFLAGS) -o $(APP_NAME)

clean:
	rm -vf $(APP_NAME) $(OBJS)

.PHONY: clean
