#pragma once

#include <vector>
#include <string>

namespace dirops
{

int lsdir(std::string dir, std::vector<std::string>& files);

int lstexts(std::vector<std::string>& files);

}