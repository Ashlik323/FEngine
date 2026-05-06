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
        matfile.open(jdata["materialfile"]);
    }
    else{
        matfile.open(overridejdata["materialfile"]);
    }

    std::cout << matfile.is_open() << std::endl;

    nlohmann::json jmatdata = nlohmann::json::parse(matfile);

    unsigned int w, h;

    std::vector<unsigned char> imgdata = loadimage(jmatdata["textures"][0], &w, &h);

    std::string vScode, fScode;

    loadfilecontents(jmatdata["vertexShader"], &vScode);

    loadfilecontents(jmatdata["fragmentShader"], &fScode);

    ImageRenderInfo& tex = loadtexture(w, h, imgdata.data(), 0);
    ShaderRenderInfo& shaderprog = loadshaders(vScode, fScode);
    DynMeshRenderInfo& mesh = render_DynamicMesh_add(mdata.data, mdata.inddata);

    render_DynamicMesh_add_imagebind(mesh, tex);
    render_DynamicMesh_change_program(mesh, shaderprog);

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
        render_DynamicMesh_change_transform(mesh, glm::mat4(overridejdata["transform"]));
    }

    render_DynamicMesh_change_query(mesh, 1);
}

void dynamicmesh_tick(){
    
}