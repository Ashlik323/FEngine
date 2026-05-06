#ifndef FRENDER
#define FRENDER

#include <iostream>
#include <vector>
#include <string>
#include <glad/include/glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <json.hpp>
#include <GLFW/glfw3.h>

#include <thread>

#include <utils/fileload.h>

struct ImageRenderInfo{
    unsigned int id; //prob remove this later if not needed
    GLuint imgid;
};

struct ShaderRenderInfo{
    unsigned int id; //prob remove this later if not needed
    GLuint progid;
};

struct DynMeshRenderInfo{
    unsigned int id; //prob remove this later if not needed
    unsigned int enablequery = 0;
    GLuint vao;
    GLuint ebo;
    glm::mat4x4 mtransform;
    ShaderRenderInfo* ShaderP;
    unsigned int Icount;
    std::vector<ImageRenderInfo*> texturesid;
};

ImageRenderInfo& loadtexture(unsigned int width, unsigned int height, unsigned char* contents, unsigned int type);

ShaderRenderInfo& loadshaders(std::string vertexShaderCode, std::string fragmentShaderCode);

void render_Camera_change_transform(glm::mat4x4 ctransform);

void render_Camera_change_perspective(glm::mat4x4 ptransform);

DynMeshRenderInfo& render_DynamicMesh_add(std::vector<vertexdata> data, std::vector<unsigned int> inddata);

void render_DynamicMesh_change_transform(DynMeshRenderInfo& mesh, glm::mat4x4 meshtransform);

void render_DynamicMesh_change_query(DynMeshRenderInfo& mesh, unsigned int status);

void render_DynamicMesh_add_imagebind(DynMeshRenderInfo& mesh, ImageRenderInfo& image);

void render_DynamicMesh_change_program(DynMeshRenderInfo& mesh, ShaderRenderInfo& SP);

void render_tick(GLFWwindow** window);

void render_deinit();

#endif // FRENDERSTREAM_H
