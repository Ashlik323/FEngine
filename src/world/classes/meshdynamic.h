#ifndef FMESHDYNAMIC_H
#define FMESHDYNAMIC_H

#include <json.hpp>
#include <fstream>
#include <utils/fileload.h>
#include <render/rendergl.h>
#include <glm/glm.hpp>
#include <iostream>

void dynamicmesh_setupobj_json(nlohmann::json jdata, nlohmann::json overridejdata);

void dynamicmesh_tick();

#endif