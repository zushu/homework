#include "defs.h"
#include "Scene.h"
#include "Camera.h"
#include "Shape.h"

Scene *pScene; // definition of the global scene variable (declared in defs.h)

int main(int argc, char *argv[])
{
	const char *xmlPath = argv[1];

    pScene = new Scene(xmlPath);

    for (int i = 0; i < 3; i++)
    {
        std::cout << " x: "<< pScene->vertices[i].x << " y: " << pScene->vertices[i].y << " z: " << pScene->vertices[i].z  << std::endl;
    }

    //pScene->renderScene();

    /*Vector3f v1(0.1, 0.2, 9.3);
    Vector3f v2(10, 20, 40);
    Vector3f v3(v1 + v2);

    std::cout << v3.x << " " << v3.y << " " << v3.z << std::endl;

    v1 = v3;
    std::cout << v1.x << " " << v1.y << " " << v1.z << std::endl;

    ImagePlane implane = {1, 2, 3, 4, 5, 6, 7};
    Camera cam(1, "cam1", Vector3f(1, 2, 3), Vector3f(0, 0, 1), Vector3f(0, 1, 0), implane);
    std::cout << "cam id: " << cam.id << " imname: " << string(cam.imageName) << std::endl;

    Ray ray1(Vector3f(1, 2, 3), Vector3f(2, 3, 4));
    Vector3f pt = ray1.getPoint(2);
    std::cout << "point with  with t=2: "<< pt.x << " " << pt.y << " " << pt.z << std::endl; 

    //float t1 = ray1.gett(Vector3f(2, 6, 5));
    //std::cout << "t: " << t1 << std::endl;

    ImagePlane implane2 = {-1, 1, -1, 1, 1, 1024, 768};
    Camera cam2(2, "cam2", Vector3f(0, 0, 0), Vector3f(0, 0, -1), Vector3f(0, 1, 0), implane2);

    Ray ray2 = cam2.getPrimaryRay(192, 256);
    std::cout << "ray1 direction" << ray2.direction.x << " " << ray2.direction.y << " " << ray2.direction.z << std::endl;
    */

    //Sphere sphere;
    ReturnVal res = (pScene->objects[0])->intersect(Ray(Vector3f(0, 0, 0), Vector3f(-0.575, 0.7, -1.7)));

    //std::cout << "nese t: " << (pScene->objects[0])->intersect(Ray(Vector3f(0, 0, 0), Vector3f(-10, -10, 10))).t  << std::endl;
    std::cout << " t: " << res.t <<  std::endl;
    if (res.intersects == true)
        std::cout << " t: " << res.t << " " << res.intersection_point.x << " " << res.intersection_point.y << " " << res.intersection_point.z << std::endl; 

	return 0;
}
