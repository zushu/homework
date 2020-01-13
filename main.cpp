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
GLuint idJpegTexture;
GLuint idMVPMatrix;

int widthTexture, heightTexture;

static void errorCallback(int error,
  const char * description) {
  fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char * argv[]) {

  //printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));

  if (argc != 2) {
    printf("Only one texture image expected!\n");
    exit(-1);
  }

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
    exit(-1);
  }

  //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  win = glfwCreateWindow(600, 600, "CENG477 - HW3", NULL, NULL);

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

  initShaders();
  glUseProgram(idProgramShader);
  initTexture(argv[1], & widthTexture, & heightTexture);

  GLdouble near = 0.1;
  GLdouble far = 1000;
  GLdouble fovy = 45;
  GLdouble aspect = 1;

  // eye
  glm::vec3 cam_pos = glm::vec3(widthTexture/2, widthTexture/10, -widthTexture/4);
  // gaze
  glm::vec3 cam_gaze = glm::vec3(0, 0, 1);
  // up
  glm::vec3 cam_v = glm::vec3(0, 1, 0);
  // right
  glm::vec3 cam_u = glm::cross(cam_v, -cam_gaze);
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





  while (!glfwWindowShouldClose(win)) {
    glfwSwapBuffers(win);
    glfwPollEvents();
  }

  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}