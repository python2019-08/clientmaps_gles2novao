 
#ifndef GPSVIEW_H_INCLUDED
#define GPSVIEW_H_INCLUDED

#include <memory>
#include <mutex>

#include "md/util/color.hpp" 
#include "md/util/chrono.hpp" 
#include "md/util/geom.hpp" 
 
namespace clientmap {
class CDrawEngine; 
class GlPainters;
class CPaintContext;
    
class GpsView {
public:
	GpsView( );
	~GpsView( );
    
public:
	void draw(const GlPainters& aPainter, 
             const CPaintContext& aDC);
     
private:
	 void _drawCircle(const   GlPainters& aPainter,
                      const   CPaintContext& aDC,
                      float               height);  
    
private:
    std::mutex  _lock;
      
    ColorI   _circleColor = 0;
    float    _circleRadius = 0; 

	double _dirAngle = 0;     // Gps direction 
     
};

} /* namespace mapengine */

#endif // !GPSVIEW_H_INCLUDED 
