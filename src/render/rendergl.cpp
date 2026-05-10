#include "rendergl.h"

int idcount;

extern std::string MATPATH;
extern std::string SHADERPATH;
extern std::string TEXTUREPATH;

glm::mat4x4 camtransform; //add system for multiple cameras later
glm::mat4x4 camperspective;



std::vector<DynMeshRenderInfo> DynRQueue;

std::vector<GLuint> vertexbuffers;

std::vector<ImageRenderInfo> images;
std::vector<ShaderRenderInfo> shaders;

unsigned int IRI = 0, SRI = 0, MRI = 0;

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

    shaders.push_back(RI);
    return shaders.at(shaders.size()-1);
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

void render_DynamicMesh_change_transform(DynMeshRenderInfo& mesh, glm::mat4x4 meshtransform){
    mesh.mtransform = meshtransform; //possible pointer bug here
};

void render_DynamicMesh_change_query(DynMeshRenderInfo& mesh, unsigned int status){
    mesh.enablequery = status;
}

void render_DynamicMesh_add_imagebind(DynMeshRenderInfo& mesh, ImageRenderInfo& image){
    mesh.texturesid.push_back(&image);
}

void render_DynamicMesh_change_program(DynMeshRenderInfo& mesh, ShaderRenderInfo& SP){
    mesh.ShaderP = &SP;
}

void render_tick(GLFWwindow** window){
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //std::cout<<"cleared"<<std::endl;
    for (int dmc = 0; dmc < DynRQueue.size(); dmc++){
        DynMeshRenderInfo& Curmesh = DynRQueue.at(dmc);//possible optimization caveat due to use of references
        if (Curmesh.enablequery==1){
        for (int t=0; t<Curmesh.texturesid.size(); t++){
            glActiveTexture(GL_TEXTURE0 + t);
            ImageRenderInfo* tex = Curmesh.texturesid.at(t);
            glBindTexture(GL_TEXTURE_2D, tex->imgid);
        }
        glActiveTexture(GL_TEXTURE0);
        glUseProgram(Curmesh.ShaderP->progid);
        glBindVertexArray(Curmesh.vao);

        std::cout << Curmesh.vao << std::endl;

        GLuint TUPos = glGetUniformLocation(Curmesh.ShaderP->progid, "transform");

        glm::mat4x4 mc = camperspective*(Curmesh.mtransform*camtransform);

        //mat4x4 mc = m4x4multiplybymat(camtransform, Curmesh.mtransform);
        //mc = m4x4transpose(mc);
        glUniformMatrix4fv(TUPos, 1, GL_FALSE, glm::value_ptr(mc));//pointer bug possible here

        //for (int i = 0; i<16; i++)
        //    std::cout << mc.e[i] << std::endl;

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Curmesh.ebo);
        glDrawElements(GL_TRIANGLES, Curmesh.Icount, GL_UNSIGNED_INT, NULL);
        std::cout << "Draw finishedd" << std::endl;
        //another buffer should be binded so I think I should not unbind them as of now
        }
    }
        /* Swap front and back buffers */
    glfwSwapBuffers(*window);
    //std::cout<<"swapped"<<std::endl;
};

void render_deinit(){
    for (int dmc = 0; dmc < DynRQueue.size(); dmc++){
        DynMeshRenderInfo& Curmesh = DynRQueue.at(dmc);//possible optimization caveat due to use of references
        for (int t=0; t<Curmesh.texturesid.size(); t++){
            glDeleteTextures(1, &Curmesh.texturesid.at(t)->imgid);
        }
        glDeleteBuffers(1, &Curmesh.ebo);
        glDeleteBuffers(1, &vertexbuffers.at(dmc));
        glDeleteVertexArrays(1, &Curmesh.vao);
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
