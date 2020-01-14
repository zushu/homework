#include "helper.h"
// basic math functions
#include <glm/glm.hpp>
// for M,V,P transformation matrices
#include <glm/ext.hpp>

static GLFWwindow * win = NULL;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
//GLuint idJpegTexture[2];
GLuint idJpegTexture;
GLuint idMVPMatrix;

int widthTexture, heightTexture;

GLfloat heightFactor = 10;
GLfloat cam_speed = 0;

//light position
glm::vec3 light_pos;
// eye
glm::vec3 cam_pos;
// gaze
glm::vec3 cam_gaze;
// up
glm::vec3 cam_v;
// right
glm::vec3 cam_u;

static void errorCallback(int error,
  const char * description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main(int argc, char * argv[]) {

  //printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));

  /*if (argc != 3) {
    printf("Two texture images expected!\n");
    exit(-1);
  }*/
  if (argc != 2) {
    printf("One texture image expected!\n");
    exit(-1);
  }

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  win = glfwCreateWindow(1000, 1000, "CENG477 - HW3", NULL, NULL);

  if (!win) {
    glfwTerminate();
    exit(-1);
  }
  glfwMakeContextCurrent(win);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

    glfwTerminate();
    exit(-1);
  }
  glfwSetKeyCallback(win, keyCallback);

  initShaders();
  glUseProgram(idProgramShader);

  //GLuint textures[texturec];
  //glGenTextures(2, idJpegTexture);
  //glGenTextures(1, &idJpegTexture);
/*
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, idJpegTexture[0]);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, idJpegTexture[1]);
  */
  //initTexture(argv[1], argv[2], & widthTexture, & heightTexture);
  initTexture(argv[1], & widthTexture, & heightTexture);

  GLdouble near = 0.1;
  GLdouble far = 1000;
  GLdouble fovy = 45;
  GLdouble aspect = 1;

  //light
  light_pos = glm::vec3(widthTexture/2, 100, heightTexture/2);
  // eye
  cam_pos = glm::vec3(widthTexture/2, widthTexture/10, -widthTexture/4);
  // gaze
  cam_gaze = glm::vec3(0, 0, 1);
  // up
  cam_v = glm::vec3(0, 1, 0);
  // right
  cam_u = glm::cross(cam_v, -cam_gaze);
  // center of viewport
  glm::vec3 center_of_vp = glm::vec3(cam_pos + cam_gaze * near);
 
  // modelling transform -> identity matrix
  //glm::mat4 modelling_tf = glm::mat4();
  // world to camera viewing transform
  // glm::mat4 glm::lookAt(glm::vec3 const& eye, glm::vec3 const& center, glm::vec3 const& up);
  glm::mat4 view_mat = glm::lookAt(cam_pos, center_of_vp, cam_v);
  // perspective projection transform
  // perspective (T fovy, T aspect, T near, T far)
  glm::mat4 perspective_mat = glm::perspective(fovy, aspect, near, far);
  // MVP transformation - M is indentity, skipped here
  glm::mat4 mvp_mat = perspective_mat * view_mat;
  // viewing transformation matrix for normals
  glm::mat4 normal_view_mat = glm::inverseTranspose(view_mat);

  GLint lightPositionLocation = glGetUniformLocation(idProgramShader, "lightPosition");
  glUniform3fv(lightPositionLocation, 1, &light_pos[0]);  

  // bind cam_pos to uniform cameraPosition of shaders
  // GLint glGetUniformLocation(GLuint program, const GLchar *name);
  GLint cameraPositionLocation = glGetUniformLocation(idProgramShader, "cameraPosition");
  // void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  glUniform3fv(cameraPositionLocation, 1, &cam_pos[0]);  
  // bind view_mat to uniform MV mat of shaders
  GLint view_location = glGetUniformLocation(idProgramShader, "MV"); 
  glUniformMatrix4fv(view_location, 1, GL_FALSE, &view_mat[0][0]);
  // bind mvp_mat to uniform MVP mat of shaders
  GLint mvp_location = glGetUniformLocation(idProgramShader, "MVP");
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp_mat[0][0]);
  // bind normal_view_mat to uniform normal_view_mat mat of shaders
  GLint normal_view_mat_location = glGetUniformLocation(idProgramShader, "normal_view_mat");
  glUniformMatrix4fv(normal_view_mat_location, 1, GL_FALSE, &normal_view_mat[0][0]);
  
  // initial window size
  int window_width = 1000;
  int window_height = 1000;
  glViewport(0,0, window_width, window_height);

  glm::vec3 *vertices = new glm::vec3[6 * widthTexture * heightTexture];
  int array_index = 0;
  // triangularize each pixel 
  glm::vec3 v_down_left, v_down_right, v_up_left, v_up_right;
  for (int i = 0; i < widthTexture; i++)
  {
    for (int j = 0; j < heightTexture; j++)
    {
      // four vertices of the pixel
      // The flat heightmap will be on the xz plane with the corner vertices at (0, 0, 0) and (w, 0, h).
      v_down_left = glm::vec3(i, 0, j);
      v_down_right = glm::vec3(i, 0, j+1);
      v_up_left = glm::vec3(i+1, 0, j);
      v_up_right = glm::vec3(i+1, 0, j+1);

      vertices[array_index++] = v_down_left;
      vertices[array_index++] = v_up_right;
      vertices[array_index++] = v_up_left;

      vertices[array_index++] = v_down_left;
      vertices[array_index++] = v_down_right;
      vertices[array_index++] = v_up_right;

    }
  }

  // If enabled, do depth comparisons and update the depth buffer.
  glEnable(GL_DEPTH_TEST);
  // from Week11 Texture Mapping Slide page 17
  // rgbTexture: This variable represents the texture unit index. If its value is zero it will fetch from texture unit 0. Its value is given such as glUniform1i(mySamplerLoc, 0)
  GLint samplerLocation = glGetUniformLocation(idProgramShader, "rgbTexture");
  glUniform1i(samplerLocation, 0);
  //GLint samplerLocation2 = glGetUniformLocation(idProgramShader, "height_texture");
  //glUniform1i(samplerLocation2, 1);
  // bound to be used in shaders
  GLint widthTextureLocation = glGetUniformLocation(idProgramShader, "widthTexture");
  glUniform1i(widthTextureLocation, widthTexture);
  // bound to be used in shaders
  GLint heightTextureLocation = glGetUniformLocation(idProgramShader, "heightTexture");
  glUniform1i(heightTextureLocation, heightTexture);
  // bound to be used in shaders

  GLint heightFactorLocation = glGetUniformLocation(idProgramShader, "heightFactor");
  glUniform1f(heightFactorLocation, heightFactor);

  // This variable represents the texture unit index. If its value is zero it will fetch from texture unit 0. Its value is given such as glUniform1i(mySamplerLoc, 0)

  //GLFWmonitor* glfwGetPrimaryMonitor 	( 	void  		) 	
  //This function returns the primary monitor. This is usually the monitor where elements like the task bar or global menu bar are located.
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  // const GLFWvidmode* glfwGetVideoMode 	( 	GLFWmonitor *  	monitor	) 	
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  int current_width = 0, current_height = 0;
  glfwGetWindowSize( win, &current_width, &current_height);



  int vertex_count = 6 * widthTexture * heightTexture;
  while (!glfwWindowShouldClose(win)) {

    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    cam_pos += cam_speed * cam_gaze;

    //update center_of_vp
    center_of_vp = glm::vec3(cam_pos + cam_gaze * near);
    glm::mat4 view_mat = glm::lookAt(cam_pos, center_of_vp, cam_v);
    // perspective projection transform
    // perspective (T fovy, T aspect, T near, T far)
    glm::mat4 perspective_mat = glm::perspective(fovy, aspect, near, far);
    // MVP transformation - M is indentity, skipped here
    glm::mat4 mvp_mat = perspective_mat * view_mat;
    // viewing transformation matrix for normals
    glm::mat4 normal_view_mat = glm::inverseTranspose(view_mat);

    
    //update lightPosition
    GLint lightPositionLocation = glGetUniformLocation(idProgramShader, "lightPosition");
    glUniform3fv(lightPositionLocation, 1, &light_pos[0]);  
    // bind cam_pos to uniform cameraPosition of shaders
    // GLint glGetUniformLocation(GLuint program, const GLchar *name);
    GLint cameraPositionLocation = glGetUniformLocation(idProgramShader, "cameraPosition");
    // void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    glUniform3fv(cameraPositionLocation, 1, &cam_pos[0]);  
    // bind view_mat to uniform MV mat of shaders
    GLint view_location = glGetUniformLocation(idProgramShader, "MV"); 
    glUniformMatrix4fv(view_location, 1, GL_FALSE, &view_mat[0][0]);
    // bind mvp_mat to uniform MVP mat of shaders
    GLint mvp_location = glGetUniformLocation(idProgramShader, "MVP");
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp_mat[0][0]);
    // bind normal_view_mat to uniform normal_view_mat mat of shaders
    GLint normal_view_mat_location = glGetUniformLocation(idProgramShader, "normal_view_mat");
    glUniformMatrix4fv(normal_view_mat_location, 1, GL_FALSE, &normal_view_mat[0][0]);

    glViewport(0,0, window_width, window_height);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    glDrawArrays(GL_TRIANGLES, 0, vertex_count);

    glDisableClientState(GL_VERTEX_ARRAY);
    glfwSwapBuffers(win);
    glfwPollEvents();
  }

  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}

// from recitation slides
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && (action == GLFW_PRESS || action == GLFW_REPEAT)){
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  // cam_speed   Y & H & X  
  if (key == GLFW_KEY_Y && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      cam_speed += 0.01;
  }
  if (key == GLFW_KEY_H && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      cam_speed -= 0.01;
  }
  if (key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      cam_speed = 0.0;
  }
 
  // heightFactor   R & F
  if (key == GLFW_KEY_R && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      heightFactor += 0.5;
      GLint heightFactorLocation = glGetUniformLocation(idProgramShader, "heightFactor");
      glUniform1f(heightFactorLocation, heightFactor);
  }
  if (key == GLFW_KEY_F && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      heightFactor -= 0.5;
      GLint heightFactorLocation = glGetUniformLocation(idProgramShader, "heightFactor");
      glUniform1f(heightFactorLocation, heightFactor);
  }
  
  // light_pos   T & G & arrow_keys
  if (key == GLFW_KEY_T && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      light_pos.y += 5;
      GLint lightPositionLocation = glGetUniformLocation(idProgramShader, "lightPosition");
      glUniform3fv(lightPositionLocation, 1, &light_pos[0]);
      //cout<<"T  x:"<<light_pos.x<<" y:"<<light_pos.y<<" z:"<<light_pos.z<<endl;
  }
  if (key == GLFW_KEY_G && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      light_pos.y -= 5;
      GLint lightPositionLocation = glGetUniformLocation(idProgramShader, "lightPosition");
      glUniform3fv(lightPositionLocation, 1, &light_pos[0]);
      //cout<<"G  x:"<<light_pos.x<<" y:"<<light_pos.y<<" z:"<<light_pos.z<<endl;
  }
  if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      light_pos.x += 5;
      GLint lightPositionLocation = glGetUniformLocation(idProgramShader, "lightPosition");
      glUniform3fv(lightPositionLocation, 1, &light_pos[0]);
      //cout<<"RIGHT  x:"<<light_pos.x<<" y:"<<light_pos.y<<" z:"<<light_pos.z<<endl;
  }
  if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      light_pos.x -= 5;
      GLint lightPositionLocation = glGetUniformLocation(idProgramShader, "lightPosition");
      glUniform3fv(lightPositionLocation, 1, &light_pos[0]);
      //cout<<"LEFT  x:"<<light_pos.x<<" y:"<<light_pos.y<<" z:"<<light_pos.z<<endl;
  }
  if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      light_pos.z += 5;
      GLint lightPositionLocation = glGetUniformLocation(idProgramShader, "lightPosition");
      glUniform3fv(lightPositionLocation, 1, &light_pos[0]);
      //cout<<"UP  x:"<<light_pos.x<<" y:"<<light_pos.y<<" z:"<<light_pos.z<<endl;
  }
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      light_pos.z -= 5;
      GLint lightPositionLocation = glGetUniformLocation(idProgramShader, "lightPosition");
      glUniform3fv(lightPositionLocation, 1, &light_pos[0]);
      //cout<<"DOWN  x:"<<light_pos.x<<" y:"<<light_pos.y<<" z:"<<light_pos.z<<endl;
  }

  //Pitch and yaw   W & A & S & D
  if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    cam_gaze = glm::rotate(cam_gaze, 0.05f, cam_u);
    cam_v = glm::rotate(cam_v, 0.05f, cam_u);
  }
  if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    cam_gaze = glm::rotate(cam_gaze, 0.05f, cam_v);
    cam_u = glm::rotate(cam_u, 0.05f, cam_v);
  }
  if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    cam_gaze = glm::rotate(cam_gaze, -0.05f, cam_u);
    cam_v = glm::rotate(cam_v, -0.05f, cam_u);
  }
  if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    cam_gaze = glm::rotate(cam_gaze, -0.05f, cam_v);
    cam_u = glm::rotate(cam_u, -0.05f, cam_v);
  }
}