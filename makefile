LDLIBS = -lraylib -lGL -lpthread -lm
LDLIBS += -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor

pong: main.c
	gcc -o pong main.c $(LDLIBS)