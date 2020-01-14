#ifndef __HELPER__H__
#define __HELPER__H__

#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <jpeglib.h>
//#include <OpenGL/gl.h>
//#include <GL/gl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

extern GLuint idProgramShader;
extern GLuint idFragmentShader;
extern GLuint idVertexShader;
//extern GLuint idJpegTexture[2];
extern GLuint idJpegTexture;

using namespace std;

void initShaders();

GLuint initVertexShader(const string& filename);

GLuint initFragmentShader(const string& filename);

bool readDataFromFile(const string& fileName, string &data);

//void initTexture(char *filename, char* filename2, int *w, int *h);
void initTexture(char *filename, int *w, int *h);

#endif
