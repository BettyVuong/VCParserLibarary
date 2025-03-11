CC = gcc
CFLAGS = -Wall -std=c11 -g -fpic
LDFLAGS= -Lbin/
INC = include/
SRC = src/
BIN = bin/
LBIS = -Lbin/ -llist -lvcparser

parser: libvcparser.so

test: libvcparser.so mainTst.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN)test $(BIN)mainTst.o $(BIN)libvcparser.so

mainTst.o: $(SRC)mainTst.c $(INC)LinkedListAPI.h $(INC)VCParser.h $(INC)VCHelper.h
	$(CC)  -I$(INC)  $(CFLAGS) -c $(SRC)mainTst.c -o $(BIN)mainTst.o

libvcparser.so: LinkedListAPI.o VCParser.o VCHelper.o
	$(CC) -shared -o $(BIN)libvcparser.so $(BIN)LinkedListAPI.o $(BIN)VCParser.o $(BIN)VCHelper.o -L$(BIN)

liblist.so: LinkedListAPI.o
	$(CC) -shared -o $(BIN)liblist.so $(BIN)LinkedListAPI.o

LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	$(CC) $(CFLAGS) -I$(INC) -c $(SRC)LinkedListAPI.c -o $(BIN)LinkedListAPI.o

VCParser.o: $(SRC)VCParser.c $(INC)LinkedListAPI.h $(INC)VCHelper.h $(INC)VCParser.h
	$(CC)  -I$(INC)  $(CFLAGS) -c  $(SRC)VCParser.c -o $(BIN)VCParser.o

VCHelper.o: $(SRC)VCHelper.c $(INC)LinkedListAPI.h $(INC)VCHelper.h $(INC)VCParser.h
	$(CC)  -I$(INC)  $(CFLAGS) -c  $(SRC)VCHelper.c -o $(BIN)VCHelper.o

clean:
	rm -rf *.o *.so $(BIN)*.o $(BIN)*.so $(BIN)test