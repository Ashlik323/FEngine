#ifndef FFILELOAD
#define FFILELOAD

#include <glm/glm.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>


struct vertexdata{
    glm::vec3 vertexpos;
    glm::vec3 normals;
    glm::vec2 texcoord;
};

struct returndata{//temporary permanent solution
    std::vector<vertexdata> data;
    std::vector<unsigned int> inddata;
};

//add multimaterial support
void loadmesh(std::string modelfile, returndata* data, int primpos);

std::vector<unsigned char> loadimage(std::string imagefile, unsigned int* width, unsigned int* height);

void loadfilecontents(std::string File, std::string* contents);

#endif // FFILELOAD
