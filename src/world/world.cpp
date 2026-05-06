#include "world.h"


void world_load(std::string mapname){
    std::ifstream mapfile(mapname);
    nlohmann::json jdata;

    if (mapfile.is_open()){
        std::cout << "map opened" << std::endl;
        jdata = nlohmann::json::parse(mapfile);
    }
    else{
        std::cout << "map is not open" << std::endl;
    }

    for (int i=0; i<jdata["objects"].size(); i++){
        nlohmann::json objjdata;
        nlohmann::json objoverridejdata;

        if (jdata["objects"][i]["datatype"]==1){
            std::ifstream objfile(jdata["objects"][i]["data"]);
            objjdata = nlohmann::json::parse(objfile);
            objfile.close();
        }
        else
        {
            objjdata = jdata["objects"][i]["data"];
        }

        if (jdata["objects"][i]["overridestat"]==1){
            objoverridejdata = jdata["objects"][i]["overridedata"];
        }


        switch(jdata["objects"][i]["classid"].get<int>()){
            case 1:
                dynamicmesh_setupobj_json(objjdata, objoverridejdata);
                break;
            default:
                std::cout<< "invalid object" << std::endl;
        }
    }
    mapfile.close();
};

void world_tick(){
    //std::cout << "tick" << std::endl;
};

float lasttime;
float lasttick = 0;

void world_init(){

    while (1){
        auto currenttime = clock();
        lasttick+= ((float)currenttime-lasttime) / CLOCKS_PER_SEC;
        if (lasttick>=(1.0/60.0)){
            lasttick = 0;
            //std::cout << ((float)currenttime-lasttime) / CLOCKS_PER_SEC << std::endl;
            world_tick();
            //do the tick things
        }
        lasttime = (float)currenttime;
    }
}