#include "fileload.h"


void loadmesh(std::string modelfile, returndata* data, int primpos){//ig the data info gets destroyed here, so not using double pointers for now
    std::cout << modelfile;
    std::ifstream file(modelfile);
    file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);

    unsigned int primammount;
    unsigned int offset;
    file.seekg(length-4);
    file.read(reinterpret_cast<char*>(&primammount), 4);
    file.seekg(length-4-((primammount-primpos)*4));
    file.read(reinterpret_cast<char*>(&offset), 4);
    std::cout << offset;
    unsigned int vposSize;
    unsigned int vcoordSize;
    unsigned int indicesSize;

    std::cout << "loading file" << std::endl;

    file.seekg(offset);
    file.read(reinterpret_cast<char*>(&vposSize), 4);
    file.seekg(8+offset);
    file.read(reinterpret_cast<char*>(&vcoordSize), 4);
    file.seekg(12+offset);
    file.read(reinterpret_cast<char*>(&indicesSize), 4);
    file.seekg(16+offset);//if something is wrong, try removing 1 from every seekg value please

    std::cout << vposSize << std::endl;

    float Pos[vposSize/4];
    file.read(reinterpret_cast<char*>(&Pos), vposSize);
    file.seekg(16+offset+vposSize);

    float Norm[vposSize/4];
    file.read(reinterpret_cast<char*>(&Norm), vposSize);
    file.seekg(16+offset+vposSize*2);

    float Coord[vposSize/6];
    file.read(reinterpret_cast<char*>(&Coord), vcoordSize);
    file.seekg(16+offset+vposSize*2+vcoordSize);
    std::cout<<indicesSize;

    unsigned short indices[indicesSize/2];
    file.read(reinterpret_cast<char*>(&indices), indicesSize);

    unsigned int newindices[indicesSize/2];
    for(int i=0; i<indicesSize/2; i++){
        newindices[i] = static_cast<unsigned int>(indices[i]);
    }

    std::cout << std::endl;
    std::cout << sizeof(Norm) << std::endl;

    vertexdata vdata[vposSize/12];
    std::cout << sizeof(vdata) << std::endl;
    for(int i=0; i<vposSize/4; i=(i+3)){
        /*
        vdata[i] = Pos[i];
        vdata[i+1] = Pos[i+1];
        vdata[i+2] = Pos[i+2];

        vdata[i+3] = Norm[i];
        vdata[i+4] = Norm[i+1];
        vdata[i+5] = Norm[i+2];

        vdata[i+6] = Coord[i];
        vdata[i+7] = Coord[i+1];
        */
        //std::cout << sizeof(vdata)/32 << " " << vposSize/4 << std::endl;
        //std::cout << Pos[i] << " " << Pos[i+1] << " " << Pos[i+2] << " : "<< i << " " << i+1 << " " << i+2 << std::endl;
        vdata[i/3].vertexpos = {Pos[i], Pos[i+1], Pos[i+2]};
        vdata[i/3].normals = {Norm[i], Norm[i+1], Norm[i+2]};
        //vdata[i].texcoord = {Coord[i], Coord[i+1]};
    }
    for(int i=0; i<vcoordSize/4; i=(i+2)){
        vdata[i/2].texcoord = {Coord[i], Coord[i+1]};
    }

    returndata res = {std::vector<vertexdata>(vdata, vdata+sizeof(vdata)/sizeof(vertexdata)), std::vector<unsigned int>(newindices, newindices+sizeof(newindices)/4)};

    *data = res;

    std::cout << "done importing" << std::endl;



    /*
    for(int i = 0; i<(*ipointer).size(); i++){
        std::cout << (*ipointer).at(i) << " " << (*ipointer).at(i+1) << " " << (*ipointer).at(i+2) << std::endl;
    }
    */
    file.close();
}

std::vector<unsigned char> loadimage(std::string imagefile, unsigned int* width, unsigned int* height){
    std::ifstream file(imagefile);
    std::vector<unsigned char> ret;
    file.seekg(4);//implement image types later

    unsigned int w, h;

    file.read(reinterpret_cast<char*>(&w), 4);
    file.seekg(8);
    file.read(reinterpret_cast<char*>(&h), 4);
    file.seekg(12);

    ret.resize(w*h*3);
    file.read(reinterpret_cast<char*>(ret.data()), (w*h*3));

    *width = w;
    *height = h;

    file.close();
    return ret;
}

void loadfilecontents(std::string File, std::string* contents){
    std::ifstream file(File);
    file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);

    char c[length+1];
    file.read(reinterpret_cast<char*>(&c), length);
    c[length] = 0;
    *contents = c;

    file.close();
}
