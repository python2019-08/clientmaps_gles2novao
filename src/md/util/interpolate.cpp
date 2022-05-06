#include <md/util/interpolate.hpp>

#include <cmath>

namespace clientmap {
 

float interpolationFactor(float base, 
    float aRangeMin, 
    float aRangeMax, 
    float z)
{
    const float zoomDiff     = aRangeMax - aRangeMin;
    const float zoomProgress = z - aRangeMin;
    if (zoomDiff == 0) {
        return 0;
    } else if (base == 1.0f) {
        return zoomProgress / zoomDiff;
    } else {
        return (std::pow(static_cast<double>(base), zoomProgress) - 1) /
               (std::pow(static_cast<double>(base), zoomDiff) - 1);
    }
}

} // namespace MdU
 
