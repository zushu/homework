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
            // initialize image with basic values
            scene->initializeImage(scene->cameras[i]);

            // do forward rendering pipeline operations
            //scene->forwardRenderingPipeline(scene->cameras[i]);

            // generate PPM file
            //scene->writeImageToPPMFile(scene->cameras[i]);

            // Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
            // Notice that os_type is not given as 1 (Ubuntu) or 2 (Windows), below call doesn't do conversion.
            // Change os_type to 1 or 2, after being sure that you have ImageMagick installed.
            //scene->convertPPMToPNG(scene->cameras[i]->outputFileName, 99);
        }

        Translation tr(1, 0.1, 2, 4);
        Matrix4 tr_m = scene->translation_matrix(&tr);
        Rotation rot(1, 60.0, 1.0, 2.0, 1.0);
        Matrix4 rot_m = scene->rotation_matrix(&rot);
        std::cout << "tr: \n" << tr_m << std::endl;
        std::cout << "final rot: \n" << rot_m << std::endl;

        Matrix4 model_tf_matrix = scene->transformation_matrix_of_model(scene->models[0]);
        std::cout << "model_tf_matrix: \n" << model_tf_matrix << std::endl;

        return 0;
    }
}