#include "rendergl.h"

int idcount;

extern std::string MATPATH;
extern std::string SHADERPATH;
extern std::string TEXTUREPATH;

glm::mat4x4 camtransform; //add system for multiple cameras later
glm::mat4x4 camperspective;



std::vector<DynMeshRenderInfo> DynRQueue;

std::vector<ShaderRenderInfo> ShaderQueue;

std::vector<GLuint> vertexbuffers;

std::vector<ImageRenderInfo> images;
std::vector<ShaderRenderInfo> shaders;

unsigned int IRI = 0, SRI = 0, MRI = 0;

ShaderRenderInfo* render_Shader_findbyid(unsigned int fid){
    for (int i = 0; i < ShaderQueue.size(); i++){

        if (ShaderQueue.at(i).id == fid){
            return &ShaderQueue.at(i);
        }
    }
    return NULL;
}

ImageRenderInfo& loadtexture(unsigned int width, unsigned int height, unsigned char* contents, unsigned int type){
    ImageRenderInfo r;

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);//ADD MORE ACTIVE TEXTURES
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, contents);//maybe make a system for color channel amount optimization NOTE: not needed i think?
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    r.id = ++IRI;
    r.imgid = texture;
    images.push_back(r);
    return images.at(images.size()-1);
}

ShaderRenderInfo& loadshaders(std::string vertexShaderCode, std::string fragmentShaderCode){
    GLint status;

    ShaderRenderInfo RI;

    //char vscontents[vslength+1];
    //vsfile.read(reinterpret_cast<char*>(&vscontents), vslength);
    //vscontents[vslength] = 0;
    //const char* vssource = vscontents;

    const char* v = vertexShaderCode.c_str();
    const char* f = fragmentShaderCode.c_str();

    GLuint vertexS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexS, 1, &v, NULL);
    glCompileShader(vertexS);

    GLuint fragS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragS, 1, &f, NULL);
    glCompileShader(fragS);

    GLuint SID = glCreateProgram();

    glAttachShader(SID, vertexS);
    glAttachShader(SID, fragS);

    glLinkProgram(SID);

    //glDeleteShader(vertexS);
    //glDeleteShader(fragS);

    glUseProgram(SID);

    RI.id = ++SRI;
    RI.progid = SID;

    ShaderQueue.push_back(RI);
    return ShaderQueue.at(ShaderQueue.size()-1);
}

void render_Camera_change_transform(glm::mat4x4 ctransform){
    camtransform = ctransform;
};

void render_Camera_change_perspective(glm::mat4x4 ptransform){
    camperspective = ptransform;
};

DynMeshRenderInfo& render_DynamicMesh_add(std::vector<vertexdata> data, std::vector<unsigned int> inddata){

    DynMeshRenderInfo RI;

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    GLuint VBO;
    GLuint EBO;
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    glBindVertexArray(VAO);

    //for(int i =0; i<data.data.size()/8; i++)
    //    std::cout << *(data.data.data()+i) << " " << *(data.data.data()+i+1) << " " << *(data.data.data()+i+2) << std::endl;
    //std::this_thread::sleep_for(std::chrono::seconds(200));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, inddata.size()*4, inddata.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(vertexdata), data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

    std::cout << "EvO: "<<EBO << std::endl;
    std::cout << "VAO: "<< VAO << std::endl;


    RI.id = ++idcount;
    RI.ebo = EBO;
    RI.vao = VAO;
    RI.Icount = inddata.size();
    RI.mtransform = {1,0,0,0,
                     0,1,0,0,
                     0,0,1,0,
                     0,0,0,1
    };
    vertexbuffers.push_back(VBO);

    DynRQueue.push_back(RI);

    return DynRQueue.at(DynRQueue.size()-1);
}

void render_Shader_add_mesh(DynMeshRenderInfo& mesh, ShaderRenderInfo& SP){
    SP.MeshesQueue.push_back(&mesh);
}

void render_DynamicMesh_change_transform(DynMeshRenderInfo& mesh, glm::mat4x4 meshtransform){
    mesh.mtransform = meshtransform; //possible pointer bug here
};

void render_DynamicMesh_change_query(DynMeshRenderInfo& mesh, unsigned int status){
    mesh.enablequery = status;
}

void render_DynamicMesh_add_imagebind(DynMeshRenderInfo& mesh, ImageRenderInfo& image){
    std::cout << "changed" << std::endl;
    mesh.texturesid.push_back(&image);
}
int l = 0;

void render_tick(GLFWwindow** window){
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //std::cout<<"cleared"<<std::endl;
    for (int sc = 0; sc < ShaderQueue.size(); sc++){
        glUseProgram(ShaderQueue.at(sc).progid);

        for(int meshc = 0; meshc < ShaderQueue.at(sc).MeshesQueue.size(); meshc++){
            if (ShaderQueue.at(sc).MeshesQueue.at(meshc) != nullptr){
                for (int t=0; t<ShaderQueue.at(sc).MeshesQueue.at(meshc)->texturesid.size() && t<10; t++){
                    //glActiveTexture(GL_TEXTURE0 + t);
                    //glBindTexture(GL_TEXTURE_2D, ShaderQueue.at(sc).MeshesQueue.at(meshc)->texturesid.at(t)->imgid);
                    std::cout << glGetError() << std::endl;
                    std::cout << ShaderQueue.at(sc).MeshesQueue.at(meshc)->texturesid.size() << std::endl;
                }
                glActiveTexture(GL_TEXTURE0);
                std::cout << glGetError() << std::endl;

                glBindVertexArray(ShaderQueue.at(sc).MeshesQueue.at(meshc)->vao);
                std::cout << glGetError() << std::endl;

                glm::mat4x4 mc = camperspective*camtransform*ShaderQueue.at(sc).MeshesQueue.at(meshc)->mtransform;
                GLuint TUPos = glGetUniformLocation(ShaderQueue.at(sc).progid, "transform");
                glUniformMatrix4fv(TUPos, 1, GL_FALSE, glm::value_ptr(mc));
                std::cout << glGetError() << std::endl;

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ShaderQueue.at(sc).MeshesQueue.at(meshc)->ebo);
                std::cout << glGetError() << std::endl;

                glDrawElements(GL_TRIANGLES, ShaderQueue.at(sc).MeshesQueue.at(meshc)->Icount, GL_UNSIGNED_INT, NULL);
                std::cout << glGetError() << std::endl;

                std::cout << "finished drawing" << std::endl;
            }
            else{
                std::cout << "ERR" << std::endl;
            }
        }
    }
    std::cout << "new frame" << std::endl;
        /* Swap front and back buffers */
    glfwSwapBuffers(*window);
    //std::cout<<"swapped"<<std::endl;
};

void render_deinit(){
    for (int sc = 0; sc < ShaderQueue.size(); sc++){
        glUseProgram(ShaderQueue.at(sc).progid);
        std::cout << glGetError() << std::endl;
        for(int meshc = 0; meshc < ShaderQueue.at(sc).MeshesQueue.size(); meshc++){

            for (int t=0; t<ShaderQueue.at(sc).MeshesQueue.at(meshc)->texturesid.size(); t++){
                glDeleteTextures(1, &ShaderQueue.at(sc).MeshesQueue.at(meshc)->texturesid.at(t)->imgid);;
            }

            glDeleteBuffers(1, &ShaderQueue.at(sc).MeshesQueue.at(meshc)->ebo);
            glDeleteVertexArrays(1, &ShaderQueue.at(sc).MeshesQueue.at(meshc)->vao);
        }
    }
    for (int sc = 0; sc < vertexbuffers.size(); sc++){
        glDeleteBuffers(1, &vertexbuffers.at(sc));
    }
};

/*
void RStream(){
    while (IsRunning){
            for (int i=0; i<RQueue.size(); i++){
                MeshRInfo MInstance = RQueue.at(i);
                for (int t=0; t<MInstance.Textures.size(); t++){
                    glActiveTexture(GL_TEXTURE0 + t);
                    glBindTexture(GL_TEXTURE_2D, *MInstance.Textures.at(t));
                }
                glActiveTexture(GL_TEXTURE0);
                glUseProgram(*MInstance.SID);//possible errors here
                glBindVertexArray(*MInstance.VAO);

                GLuint TUPos = glGetUniformLocation(*MInstance.SID, "transform");
                glUniformMatrix4fv(TUPos, 1, GL_FALSE, glm::value_ptr(*MInstance.Transformation));

                glDrawElements(GL_TRIANGLES, *MInstance.ICount, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
    }
}



void InitRender(){
    std::thread RThread(RStream);
}

void AddToRQueue(MeshRInfo MeshInfo){
    RQueue.push_back(MeshInfo);
}
*/
