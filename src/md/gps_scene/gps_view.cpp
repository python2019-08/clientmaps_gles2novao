
#include <string> 

#include "glm/gtc/matrix_transform.hpp"
#include "md/drawengine/drawengine.h"

#include "md/gps_scene/gps_view.h" 

 
namespace clientmap {
    
    
    GpsView::GpsView( ) 
    {
        _circleRadius = 128;
        _circleColor = { 25, 114, 160, 209 };
    }
    
    GpsView::~GpsView() 
    {
    }
 
    void GpsView::draw(const GlPainters& aPainter,
        const CPaintContext& aDC)
    {

    }

    void GpsView::_drawCircle(const   GlPainters& aPainter,
        const CPaintContext& aDC,
        float               height)
    {
    
    }
  
} /* namespace clientmap */
