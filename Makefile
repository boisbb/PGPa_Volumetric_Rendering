#CC specifies which compiler we're using
CC = g++

#CFLAGS specifies the additional compilation options we're using
CFLAGS = -g -lm # -fsanitize=address -fsanitize=undefined

#LFLAGS specifies the libraries we're linking against
LFLAGS = -lassimp -lglfw -lGL -lGLU -lrt -lm -ldl -lXrandr -lXinerama -lXcursor -lXext -lXrender -lXfixes -lX11 -lpthread -lxcb -lXau -lXdmcp -lGLEW -lGLU -lGL -lm -ldl -lXdamage -lXxf86vm -lXfixes -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

SOURCES=$(wildcard src/*.cpp)
SOURCES+=$(wildcard src/internal/*.cpp)
SOURCES+=$(wildcard src/models/*.cpp)
SOURCES+=$(wildcard src/vendor/*/*.cpp)
OBJECTS=$(patsubst src/%.cpp, src/%.o, $(SOURCES))

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $^

app: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LFLAGS)

run:
	./app

clean:
	rm $(wildcard src/*.o) $(wildcard src/models/*.o) app imgui.ini
