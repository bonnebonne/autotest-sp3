CC = g++
CFLAGS  = -g -Wall

FILES = testsuite.cpp main.cpp
EX = tester

$(EX):
	$(CC) $(CFLAGS) -o $(EX) $(FILES)

clean:
	$(RM) $(EX)
