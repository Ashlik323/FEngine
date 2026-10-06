#ifndef FRENDER
#define FRENDER

#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <vector>
#include <string>
#include <glad/include/glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <json.hpp>
#include <GLFW/glfw3.h>

#include <thread>

#include <utils/fileload.h>

struct ImageRenderInfo{
    unsigned int id; //prob remove this later if not needed
    GLuint imgid;
};

struct DynMeshRenderInfo{
    unsigned int id; //prob remove this later if not needed
    unsigned int enablequery = 0;
    GLuint vao;
    GLuint ebo;
    unsigned int Icount;
    unsigned int shaderprogramid;
    glm::mat4x4 mtransform;
    std::vector<ImageRenderInfo*> texturesid;
};

struct ShaderRenderInfo{
    unsigned int id; // Program id will be assigned by a value set in the material file
    GLuint progid;
    std::vector<DynMeshRenderInfo*> MeshesQueue;
};

ShaderRenderInfo* render_Shader_findbyid(unsigned int fid);

ImageRenderInfo& loadtexture(unsigned int width, unsigned int height, unsigned char* contents, unsigned int type);

ShaderRenderInfo& loadshaders(std::string vertexShaderCode, std::string fragmentShaderCode);

void render_Camera_change_transform(glm::mat4x4 ctransform);

void render_Camera_change_perspective(glm::mat4x4 ptransform);

DynMeshRenderInfo& render_DynamicMesh_add(std::vector<vertexdata> data, std::vector<unsigned int> inddata);

void render_Shader_add_mesh(DynMeshRenderInfo& mesh, ShaderRenderInfo& SP);

void render_DynamicMesh_change_transform(DynMeshRenderInfo& mesh, glm::mat4x4 meshtransform);

void render_DynamicMesh_change_query(DynMeshRenderInfo& mesh, unsigned int status);

void render_DynamicMesh_add_imagebind(DynMeshRenderInfo& mesh, ImageRenderInfo& image);

void render_tick(GLFWwindow** window);

void render_deinit();

#endif // FRENDERSTREAM_H
