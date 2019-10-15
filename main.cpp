#include "defs.h"
#include "Scene.h"
#include "Camera.h"

Scene *pScene; // definition of the global scene variable (declared in defs.h)

int main(int argc, char *argv[])
{
	//const char *xmlPath = argv[1];

    //pScene = new Scene(xmlPath);

    //pScene->renderScene();

    Vector3f v1(0.1, 0.2, 9.3);
    Vector3f v2(10, 20, 40);
    Vector3f v3(v1 + v2);

    std::cout << v3.x << " " << v3.y << " " << v3.z << std::endl;

    v1 = v3;
    std::cout << v1.x << " " << v1.y << " " << v1.z << std::endl;

    ImagePlane implane = {1, 2, 3, 4, 5, 6, 7};
    Camera cam(1, "cam1", Vector3f(1, 2, 3), Vector3f(0, 0, 1), Vector3f(0, 1, 0), implane);
    std::cout << "cam id: " << cam.id << " imname: " << string(cam.imageName) << std::endl;
	return 0;
}
