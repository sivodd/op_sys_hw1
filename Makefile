# Makefile for the smash program
CC = g++
CFLAGS = -g -Wall 
CCLINK = $(CC)
OBJS = smash.o commands.o signals.o job.o
RM = rm -f
# Creating the  executable
smash: $(OBJS)
	$(CCLINK) $(CFLAGS) -o smash $(OBJS) 
# Creating the object files
commands.o: commands.cpp commands.h job.h 
smash.o: smash.cpp commands.h job.h signals.h
signals.o: signals.cpp signals.h job.h
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*

