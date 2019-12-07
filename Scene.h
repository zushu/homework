#ifndef _SCENE_H_
#define _SCENE_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "Camera.h"
#include "Color.h"
#include "Model.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Triangle.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix4.h"

using namespace std;

class Scene
{
public:
	Color backgroundColor;
	bool cullingEnabled;
	int projectionType;

	vector< vector<Color> > image;
	vector< Camera* > cameras;
	vector< Vec3* > vertices;
	vector< Color* > colorsOfVertices;
	vector< Scaling* > scalings;
	vector< Rotation* > rotations;
	vector< Translation* > translations;
	vector< Model* > models;

	Scene(const char *xmlPath);

	void initializeImage(Camera* camera);
	void forwardRenderingPipeline(Camera* camera);
	int makeBetweenZeroAnd255(double value);
	void writeImageToPPMFile(Camera* camera);
	void convertPPMToPNG(string ppmFileName, int osType);

	// helpers
	Matrix4 translation_matrix(Translation* tr);
	Matrix4 rotation_matrix(Rotation* rot);
	Matrix4 scaling_matrix(Scaling* sc);
	Matrix4 transformation_matrix_of_model(Model* model);
	Triangle transform_triangle(Triangle triangle, Matrix4 tf_matrix, vector<Vec3*>&  vertices_copy);
	Model* transform_model(Model* model, Matrix4 tf_matrix, Vec3 camera_pos, vector<Vec3*>&  vertices_copy);


	Matrix4 camera_transformation(Camera* camera);
	Matrix4 projection_transformation(Camera* camera,int projection_type);
	Matrix4 viewport_transformation(int nx, int ny);
	vector< Vec3* > copy_vertices(vector< Vec3* > vertices);

	bool triangle_is_culled(Triangle triangle, Vec3 camera_pos, vector<Vec3*>&  vertices_copy);
	bool visible(float den, float num, float& tE, float& tL);
	void line_clipping(Vec3 vmin, Vec3 vmax, Vec3& v0, Vec3& v1);

	void line_drawing(Vec3 v0, Vec3 v1, vector< vector<Color> >& image_copy);


};

#endif
