OBJS = segment.h segment.cpp

EXE = segment

CC = g++

FLAGS = -w -lSDL2 -o


all: $(OBJS)
	$(CC) $(OBJS) $(FLAGS) $(EXE)

