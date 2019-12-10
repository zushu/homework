#include <iostream>
#include <string>
#include <vector>
#include "Scene.h"
#include "Matrix4.h"
#include "Helpers.h"

using namespace std;

Scene *scene;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Please run the rasterizer as:" << endl
             << "\t./rasterizer <input_file_name>" << endl;
        return 1;
    }
    else
    {
        const char *xmlPath = argv[1];

        scene = new Scene(xmlPath);

        for (int i = 0; i < scene->cameras.size(); i++)
        {
            std::cout << "camera " << i << std::endl;
            // initialize image with basic values
            scene->initializeImage(scene->cameras[i]);

            // do forward rendering pipeline operations
            scene->forwardRenderingPipeline(scene->cameras[i]);

            // generate PPM file
            scene->writeImageToPPMFile(scene->cameras[i]);

            // Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
            // Notice that os_type is not given as 1 (Ubuntu) or 2 (Windows), below call doesn't do conversion.
            // Change os_type to 1 or 2, after being sure that you have ImageMagick installed.
            scene->convertPPMToPNG(scene->cameras[i]->outputFileName, 99);
        }

        //vector<Vec3*> vertices_copy(scene->vertices.size());
        //vertices_copy = scene->copy_vertices(scene->vertices);
        //Triangle tri_transformed = scene->transform_triangle(scene->models[0]->triangles[0], model_tf_matrix);

        //std::cout << "model: " << *(scene->models[0]) << std::endl;

        //std::cout << "transformed model: " << *new_model << std::endl;

        /*
        for (int i = 0 ; i < scene->models[0]->triangles.size(); i++)
        {
            std::cout << "is culled: " << scene->triangle_is_culled(scene->models[0]->triangles[i], scene->cameras[0]->pos, scene->vertices) << std::endl;
        }

        Vec3 v0(-5, -0.5, -0.5, -1);
        Vec3 v1(2, 0, 0, -1);
        Vec3 vmin(-1, -1, -1, -1);
        Vec3 vmax(1, 1, 1, -1);

        scene->line_clipping(vmin, vmax, v0, v1);

        std::cout << "v0: " << v0 << " v1: " << v1 << std::endl;

        Vec3 v2(200, 200, 0.5, 2);
        Vec3 v3(200, 50, 0.5, 3);

        scene->line_drawing(v2, v3, scene->image);
        
        for (auto& temp: scene->image)
        {
            for (auto& cl : temp)
            {
                std::cout << cl << " ";
            }
            std::cout << "\n";
        }

        scene->writeImageToPPMFile(scene->cameras[0]);*/
        return 0;
    }
}