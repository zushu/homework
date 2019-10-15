#include "Camera.h"

Camera::Camera(int id,                      // Id of the camera
               const char* imageName,       // Name of the output PPM file 
               const Vector3f& pos,         // Camera position
               const Vector3f& gaze,        // Camera gaze direction
               const Vector3f& up,          // Camera up direction
               const ImagePlane& imgPlane)  // Image plane parameters
               : id(id)
{

      //this->imageName = new char[strlen(imageName)];
      strcpy(this->imageName, imageName);
      this->pos = pos;
      this->gaze = gaze;
      this->up = up;
      this->w = gaze * (-1);
      this->u = up.cross_product(w);
      this->imgPlane.left = imgPlane.left;
      this->imgPlane.right = imgPlane.right;
      this->imgPlane.bottom = imgPlane.bottom;
      this->imgPlane.top = imgPlane.top;
      this->imgPlane.distance = imgPlane.distance;
      this->imgPlane.nx = imgPlane.nx;
      this->imgPlane.ny = imgPlane.ny;
}

/* Takes coordinate of an image pixel as row and col, and
 * returns the ray going through that pixel. 
 */
Ray Camera::getPrimaryRay(int col, int row) const
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */

     Ray result;
     result.origin = pos;
     float s_u = (row + 0.5) * (imgPlane.right - imgPlane.left) / imgPlane.nx;
     float s_v = (col + 0.5) * (imgPlane.top - imgPlane.bottom) / imgPlane.ny;
     Vector3f m = pos + gaze * imgPlane.distance;
     Vector3f q = m + u * imgPlane.left + up * imgPlane.top;
     Vector3f s = q + u * s_u - up * s_v;
     result.direction = s - pos;
     return result;


}

