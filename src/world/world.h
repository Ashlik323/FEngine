#ifndef FWORLD_H
#define FWORLD_H

#include <utils/fileload.h>
#include <json.hpp>
#include <string>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>

#include <world/classes/meshdynamic.h>

void world_load(std::string mapname);

void world_init();

#endif