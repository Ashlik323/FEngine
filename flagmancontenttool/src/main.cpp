
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <json.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/* Model file structure
Header:
  -Name size DEPRECATED has no sense
  -Name DEPRECATED
  -Model type
Primitives:(Each primitive is it's own material)
  -header (either 16 byte long or 24)
     -Contains Vertex positions, normals, coordinates, indices memory size, and possibly size of rig affiliation and weights
  -contents, contains the data types listed above
(OPTIONALLY) Rig info:
  -Not added yet
  End of file:
  Amount of primitives and their respective positions in the file
*/

//VERY IMPORTANT NOTE: only works with the models exported by blender, due to accessor byteOffset not being present here(That functionality could easily be added though)


void gltfop(std::string& path){
    std::ifstream GLTFJson(path);
    nlohmann::json jdata = nlohmann::json::parse(GLTFJson);

    std::string BPath = "./import/";
    BPath.append(jdata["buffers"][0]["uri"].get<std::string>());
    std::ifstream GLTFBuffer(BPath, std::ios::binary);

    for (int meshnum = 0; meshnum < jdata["meshes"].size(); meshnum++){
        int primCount = jdata["meshes"][meshnum]["primitives"].size();
        int primPositions[primCount];
        int primSizes[primCount];

        std::fstream outputfile;
        std::string MeshName = jdata["meshes"][meshnum]["name"];
        //std::string MeshName2 = jdata["meshes"][meshnum]["name"];
        //int MeshNameSize = sizeof(MeshName2);
        int MeshType = 0;
        std::string outpath = "./export/";
        MeshName.append(".fmodel");
        outputfile.open(outpath.append(MeshName), std::ios::out | std::ios::binary);

        //outputfile.write(reinterpret_cast<char*>(&MeshNameSize), 4);
        //outputfile.write(reinterpret_cast<char*>(&MeshName2), MeshNameSize);
        outputfile.write(reinterpret_cast<char*>(&MeshType), 4);

        for (int primnum = 0; primnum < primCount; primnum++){

            int Vposloc = jdata["meshes"][meshnum]["primitives"][primnum]["attributes"]["POSITION"].get<int>();
            int VposBufferloc = jdata["accessors"][ Vposloc ]["bufferView"];

            int Vnormalloc = jdata["meshes"][meshnum]["primitives"][primnum]["attributes"]["NORMAL"].get<int>();
            int VnormalBufferloc = jdata["accessors"][ Vnormalloc ]["bufferView"];

            int Vtexcoordloc = jdata["meshes"][meshnum]["primitives"][primnum]["attributes"]["TEXCOORD_0"].get<int>();
            int VtexcoordBufferloc = jdata["accessors"][ Vtexcoordloc ]["bufferView"];

            int indicesloc = jdata["meshes"][meshnum]["primitives"][primnum]["indices"].get<int>();
            int indicesBufferloc = jdata["accessors"][ indicesloc ]["bufferView"];


            int VposLength = jdata["accessors"][Vposloc]["count"].get<int>()*12;
            int VnormalLength = jdata["accessors"][Vnormalloc]["count"].get<int>()*12;
            int VtexcoordLength = jdata["accessors"][Vtexcoordloc]["count"].get<int>()*8;
            int indicesLength = jdata["accessors"][ indicesloc ]["count"].get<int>()*2;

            int VposOffset = jdata["bufferViews"][ VposBufferloc ]["byteOffset"];
            int VnormalOffset = jdata["bufferViews"][ VnormalBufferloc ]["byteOffset"];
            int VtexcoordOffset = jdata["bufferViews"][ VtexcoordBufferloc ]["byteOffset"];
            int indicesOffset = jdata["bufferViews"][ indicesBufferloc ]["byteOffset"];

            char charbufferP[VposLength];
            char charbufferN[VnormalLength];
            char charbufferT[VtexcoordLength];
            char charbufferI[indicesLength];


            GLTFBuffer.seekg(VposOffset);
            GLTFBuffer.read(charbufferP, VposLength);
            GLTFBuffer.seekg(VnormalOffset);
            GLTFBuffer.read(charbufferN, VnormalLength);
            GLTFBuffer.seekg(VtexcoordOffset);
            GLTFBuffer.read(charbufferT, VtexcoordLength);
            GLTFBuffer.seekg(indicesOffset);
            GLTFBuffer.read(charbufferI, indicesLength);

            outputfile.write(reinterpret_cast<char*>(&VposLength), 4);
            outputfile.write(reinterpret_cast<char*>(&VnormalLength), 4);
            outputfile.write(reinterpret_cast<char*>(&VtexcoordLength), 4);
            outputfile.write(reinterpret_cast<char*>(&indicesLength), 4);

            outputfile.write(charbufferP, VposLength);
            outputfile.write(charbufferN, VnormalLength);
            outputfile.write(charbufferT, VtexcoordLength);
            outputfile.write(charbufferI, indicesLength);


            primSizes[primnum] = VposLength+VnormalLength+VtexcoordLength+indicesLength+16;//I had an additional = here for some reason???
            if(primnum == 0){
                primPositions[primnum] = 4;
            }
            else{
                primPositions[primnum] = primPositions[primnum-1]+primSizes[primnum-1];
            }
        }

        outputfile.write(reinterpret_cast<char*>(primPositions), sizeof(primPositions));
        outputfile.write(reinterpret_cast<char*>(&primCount), 4);

        outputfile.close();
    }
    /*
    std::ifstream check("./export/Cube.fmodel", std::ios::binary);
    std::vector<float> cpos;
    int cpossize;
    check.seekg(4);
    check.read(reinterpret_cast<char*>(&cpossize), 4);
    check.seekg(20);
    std::cout << cpossize << std::endl;
    cpos.resize(cpossize/4);
    check.read(reinterpret_cast<char*>(cpos.data()), cpossize);
    for (int i=0; i<(cpos.size()-3); i=(i+3)){
        std::cout << cpos.at(i) << " " << cpos.at(i+1) << " " << cpos.at(i+2) << " : "<< i << " " << i+1 << " " << i+2 << std::endl;
    }
    */

}

void tgaop(std::string& path){
    int width;
    int height;
    int channels;
    //stbi_set_flip_vertically_on_load(true);
    unsigned char* img = stbi_load(path.c_str(), &width, &height, &channels, 3);
    std::cout << channels << " " << width << " " << height << std::endl;
    std::fstream outputfile;
    int imgtype = 0;
    std::string name = path;
    name.erase(0, 9);
    name.pop_back();
    name.pop_back();
    name.pop_back();
    name.append("ftex");
    std::string newpath = "./export/";
    outputfile.open(newpath.append(name), std::ios::out | std::ios::binary);

    outputfile.write(reinterpret_cast<char*>(&imgtype), 4);
    outputfile.write(reinterpret_cast<char*>(&width), 4);
    outputfile.write(reinterpret_cast<char*>(&height), 4);
    for (int i = 0; i<(width*height*3); i++){
        //std::cout << *(img+i);
        outputfile.write(reinterpret_cast<char*>((img+i)), 1);

    }
    outputfile.close();
}

int main()
{
    // Define the directory path to list files from
    std::filesystem::path directorypath = "./import/";
    std::string GLTFfiletype = ".gltf";
    // To check if the directory exists or not
    for (const auto& entry :
             std::filesystem::directory_iterator(directorypath)) {
            // Output the path of the file or subdirectory
            std::string pathf = entry.path().u8string();
            std::size_t checkfindgltf = pathf.find(GLTFfiletype);
            std::size_t checkfindTGA = pathf.find(".tga");
            if (checkfindgltf != std::string::npos){
                std::cout << "File gltf: " << entry.path() << std::endl;
                gltfop(pathf);
            }
            if (checkfindTGA != std::string::npos){
                std::cout << "File tga: " << entry.path() << std::endl;
                tgaop(pathf);
            }
        }

    return 0;
}
