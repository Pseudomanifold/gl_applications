CC          = g++
CCFLAGS     = -Wall -c
LIBS        = -L /usr/X11/lib -L /usr/local/lib
INCLUDES    = -I /usr/local/include
LDFLAGS     = -lglut -lGL -lGLU -lm

MYOBJECTS   = gl_particles.o vector.o particles.o
BIN         = gl_particles

$(BIN): $(MYOBJECTS) Makefile
	$(CC) $(MYOBJECTS) $(LIBS) $(LDFLAGS)  -o $(BIN)

.cpp.o:
	$(CC) $(INCLUDES) $(CCFLAGS) $<

clean:
	rm -f *.o *.core $(BIN)


