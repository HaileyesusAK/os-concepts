APP_NAME = fibo 
CC = gcc
CFLAGS = 
LFLAGS = -lpthread
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
