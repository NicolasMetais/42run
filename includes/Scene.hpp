#pragma once
#include <iostream>
#include <vector>


struct Scene {
	std::string name;

	std::vector<int> rootNodes; //liste des nodes de departs
};