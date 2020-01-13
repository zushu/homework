all:
	g++ *.cpp -o main -std=c++11 -lGLEW -lglfw3 -ljpeg -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

