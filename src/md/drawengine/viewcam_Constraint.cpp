#include "md/drawengine/viewcam_Constraint.h"

namespace clientmap {

void ViewConstraint::setLimitsY(double yMin, double yMax) 
{
    m_yMin = yMin;
    m_yMax = yMax;
}

void ViewConstraint::setLimitsX(double xMin, double xMax) 
{
    m_xMin = xMin;
    m_xMax = xMax;
}

void ViewConstraint::setRadius(double r) {
    m_radius = r;
}

auto ViewConstraint::getConstrainedX(double x) -> double {
    return constrain(x, m_radius, m_xMin, m_xMax);
}

auto ViewConstraint::getConstrainedY(double y) -> double {
    return constrain(y, m_radius, m_yMin, m_yMax);
}

auto ViewConstraint::getConstrainedScale() -> double {

    double xScale = 1.0, yScale = 1.0;
    double xRange = m_xMax - m_xMin;
    double yRange = m_yMax - m_yMin;
    double diameter = 2.0 * m_radius;

    if (diameter > yRange) { 
        yScale = yRange / diameter; 
    }

    if (diameter > xRange) { 
        xScale = xRange / diameter; 
    }

    return std::fmin(xScale, yScale);
}

auto ViewConstraint::constrain(double pos,  double radius, 
    double min,  double max) -> double 
{ 
    if ( (max < pos + radius) &&  (pos - radius < min) ) 
    {
        return 0.5 * (max + min);
    } else if (max < pos + radius) { 
        return max - radius;
    }  else if (pos - radius < min) { 
        return min + radius;
    }

    return pos;

}

}//END OF [namespace clientmap]
