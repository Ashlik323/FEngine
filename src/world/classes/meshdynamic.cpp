#include "meshdynamic.h"

void dynamicmesh_setupobj_json(nlohmann::json jdata, nlohmann::json overridejdata){
    returndata mdata;

    if (overridejdata["meshfile"].is_null()){
        loadmesh(jdata["meshfile"], &mdata, 0);
    }
    else{
        loadmesh(overridejdata["meshfile"], &mdata, 0);
    }

    std::ifstream matfile;

    if (overridejdata["materialfile"].is_null()){
        std::string fl = jdata["materialfile"];
        matfile.open(fl);
    }
    else{
        std::string fl = overridejdata["materialfile"];
        matfile.open(fl);
    }

    std::cout << matfile.is_open() << std::endl;

    nlohmann::json jmatdata = nlohmann::json::parse(matfile);

    matfile.close();

    unsigned int w, h;

    std::vector<unsigned char> imgdata = loadimage(jmatdata["textures"][0], &w, &h);

    std::cout << "width, height: " << w << " " << h << std::endl;
    for (int i = 0; i<imgdata.size(); i++){
        //std::cout << imgdata.at(i);
        if (*(reinterpret_cast<int*>(&imgdata.at(i))) == 0){
           // std::cout << "zero";
        }

    }
    std::cout << imgdata.size();

    ShaderRenderInfo* shaderp = render_Shader_findbyid(jmatdata["programid"].get<unsigned int>());

    ImageRenderInfo& tex = loadtexture(w, h, imgdata.data(), 0);

    DynMeshRenderInfo& mesh = render_DynamicMesh_add(mdata.data, mdata.inddata);

    if (shaderp == NULL){
        std::cout << "Shader not found" << jmatdata["programid"].get<unsigned int>() << std::endl;
        std::string vScode, fScode;
        loadfilecontents(jmatdata["vertexShader"], &vScode);
        loadfilecontents(jmatdata["fragmentShader"], &fScode);
        vScode.pop_back();
        fScode.pop_back();
        ShaderRenderInfo& shaderprog = loadshaders(vScode, fScode);
        shaderprog.id = jmatdata["programid"].get<unsigned int>();
        render_Shader_add_mesh(mesh, shaderprog);
    }
    else{
        std::cout << "Shader found" << jmatdata["programid"].get<unsigned int>() << std::endl;
        render_Shader_add_mesh(mesh, *shaderp);
    }

    render_DynamicMesh_add_imagebind(mesh, tex);

    if (overridejdata["v3euler"].is_null()){
        if(jdata["v3euler"].is_null()){
            if (overridejdata["transform"].is_null()){
                render_DynamicMesh_change_transform(mesh, glm::mat4(jdata["transform"][0].get<float>(),
                                                                    jdata["transform"][1].get<float>(),
                                                                    jdata["transform"][2].get<float>(),
                                                                    jdata["transform"][3].get<float>(),
                                                                    jdata["transform"][4].get<float>(),
                                                                    jdata["transform"][5].get<float>(),
                                                                    jdata["transform"][6].get<float>(),
                                                                    jdata["transform"][7].get<float>(),
                                                                    jdata["transform"][8].get<float>(),
                                                                    jdata["transform"][9].get<float>(),
                                                                    jdata["transform"][10].get<float>(),
                                                                    jdata["transform"][11].get<float>(),
                                                                    jdata["transform"][12].get<float>(),
                                                                    jdata["transform"][13].get<float>(),
                                                                    jdata["transform"][14].get<float>(),
                                                                    jdata["transform"][15].get<float>()));
            }
            else{
                render_DynamicMesh_change_transform(mesh, glm::mat4(overridejdata["transform"][0].get<float>(),
                                                                    overridejdata["transform"][1].get<float>(),
                                                                    overridejdata["transform"][2].get<float>(),
                                                                    overridejdata["transform"][3].get<float>(),
                                                                    overridejdata["transform"][4].get<float>(),
                                                                    overridejdata["transform"][5].get<float>(),
                                                                    overridejdata["transform"][6].get<float>(),
                                                                    overridejdata["transform"][7].get<float>(),
                                                                    overridejdata["transform"][8].get<float>(),
                                                                    overridejdata["transform"][9].get<float>(),
                                                                    overridejdata["transform"][10].get<float>(),
                                                                    overridejdata["transform"][11].get<float>(),
                                                                    overridejdata["transform"][12].get<float>(),
                                                                    overridejdata["transform"][13].get<float>(),
                                                                    overridejdata["transform"][14].get<float>(),
                                                                    overridejdata["transform"][15].get<float>()));
            }
        }
        else{
            render_DynamicMesh_change_transform(mesh, glm::translate(glm::eulerAngleYXZ(jdata["v3euler"][3].get<float>(),
                                                                                        jdata["v3euler"][4].get<float>(),
                                                                                        jdata["v3euler"][5].get<float>()),
                                                                     glm::vec3(jdata["v3euler"][0].get<float>(),
                                                                               jdata["v3euler"][1].get<float>(),
                                                                               jdata["v3euler"][2].get<float>())));
        }
    }
    else{
        std::cout << "ORIENTATION: "<< glm::to_string(glm::eulerAngleYXZ(overridejdata["v3euler"][3].get<float>(),
                                                                        overridejdata["v3euler"][4].get<float>(),
                                                                        overridejdata["v3euler"][5].get<float>())) << std::endl;

        std::cout << "TRANSFORM: "<< glm::translate(glm::eulerAngleYXZ(overridejdata["v3euler"][3].get<float>(),
                                                                        overridejdata["v3euler"][4].get<float>(),
                                                                        overridejdata["v3euler"][5].get<float>()),
                                                                 glm::vec3(overridejdata["v3euler"][0].get<float>(),
                                                                           overridejdata["v3euler"][1].get<float>(),
                                                                           overridejdata["v3euler"][2].get<float>())) << std::endl;

        render_DynamicMesh_change_transform(mesh, glm::translate(glm::eulerAngleYXZ(overridejdata["v3euler"][3].get<float>(),
                                                                                    overridejdata["v3euler"][4].get<float>(),
                                                                                    overridejdata["v3euler"][5].get<float>()),
                                                                 glm::vec3(overridejdata["v3euler"][0].get<float>(),
                                                                           overridejdata["v3euler"][1].get<float>(),
                                                                           overridejdata["v3euler"][2].get<float>())));
    }


    render_DynamicMesh_change_query(mesh, 1);
}

void dynamicmesh_tick(){
    
}