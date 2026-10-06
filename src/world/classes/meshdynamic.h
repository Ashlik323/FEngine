#ifndef FMESHDYNAMIC_H
#define FMESHDYNAMIC_H


#define GLM_ENABLE_EXPERIMENTAL
#include <json.hpp>
#include <fstream>
#include <utils/fileload.h>
#include <render/rendergl.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>

void dynamicmesh_setupobj_json(nlohmann::json jdata, nlohmann::json overridejdata);

void dynamicmesh_tick();

#endif