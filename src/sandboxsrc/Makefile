CC=gcc
OS=$(shell uname)
CFLAGS=-fPIC -Wall -DGL_EXT_PROTOTYPES -DX11 -g -I.. -I/usr/X11/include -I/usr/include/libpng12 -I/usr/include/python2.7
ifeq ($(OS), Darwin)
CFLAGS+=-I/usr/local/include/freetype2 
else
CFLAGS+=-I/usr/include/freetype2
endif
#CFLAGS+=-DGEOMETRY_SHADER
LDFLAGS=-fPIC -lGLU -lGL -lm -lX11 -L/usr/X11/lib -lfreetype -ljpeg -lpng -lpython2.7 -ldl
EXEC=sandbox
OBJS=sandbox.o x11gl.o ticktimer.o 
OBJS+=hypnomix.o mathematics.o shader.o presets.o transform.o module.o dir.o opengl.o

ALL=$(EXEC)
$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(EXEC)

.c.o:
	$(CC) $(CFLAGS) -c $< 

hypnomix.o: ../hypnomix.c
	$(CC) $(CFLAGS) -c $<

mathematics.o: ../mathematics.c
	$(CC) $(CFLAGS) -c $<

shader.o: ../shader.c
	$(CC) $(CFLAGS) -c $<

presets.o: ../presets.c
	$(CC) $(CFLAGS) -c $<

transform.o: ../transform.c
	$(CC) $(CFLAGS) -c $<

dir.o: ../dir.c
	$(CC) $(CFLAGS) -c $<

module.o:../module.c
	$(CC) $(CFLAGS) -c $<

opengl.o:../opengl.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.o
	rm ../*.o
	rm $(EXEC)
