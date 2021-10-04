#pragma once

#include "./Cartesian3.h"
#include "./diredge.h"

#include <vector>
#include <string>

namespace io
{
    std::vector<Cartesian3> readTriangleSoup(std::string);
    bool writeTriangleSoup(std::string, std::vector<Cartesian3>);
}
