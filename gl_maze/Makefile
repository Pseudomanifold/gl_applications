CC          = g++
CCFLAGS     = -c
LIBS        = -L /usr/X11/lib -L /usr/local/lib
INCLUDES    = -I /usr/local/include
LDFLAGS     = -lglut -lGL -lGLU -lm

MYOBJECTS   = gl_maze.o texture.o
BIN         = gl_maze

$(BIN): $(MYOBJECTS) Makefile
	$(CC) $(MYOBJECTS) $(LIBS) $(LDFLAGS)  -o $(BIN)

.cpp.o:
	$(CC) $(INCLUDES) $(CCFLAGS) $<

clean:
	rm -f *.o *.core $(BIN)


