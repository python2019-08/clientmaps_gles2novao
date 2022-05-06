#ifndef CMDMap_H_INCLUDED
#define CMDMap_H_INCLUDED
#include <memory>
#include "md/drawengine/camera_options.hpp" 

namespace clientmap {
class IDrawDevice;
class CDrawEngine;
class CMapEngine;
 
class CMDMap final
{
public:
    CMDMap();
    ~CMDMap();

    int  init(std::shared_ptr<IDrawDevice> _drawDevice,
        double             _pixelRatio,
        const std::string& aRootPath,        
        const std::string& aBaseUrl,
        const std::string& aAccessToken
        );
    void done();

    //@Param aStyleName style file name  
    void setStyle(const std::string&  aStyleName);
    void setStyleString(const std::string&  aStyleString);

    //
    void setDefaultFboId(uint32_t _fboId);
    void resize(int32_t _w, int32_t _h); 
  
    void requestDrawing();
    void drawFrame();

    // Transition
    void cancelTransitions();
    void setGestureInProgress(bool);
    bool isGestureInProgress() const;
    // È±¸öisPitching()º¯Êý
    bool isRotating() const;
    bool isScaling() const;
    bool isPanning() const;

    // Camera
    CameraOptions getCameraOptions( ) const;

    void returnToDefaultPos();
    void jumpTo(const CameraOptions&);
    void easeTo(const CameraOptions&, const AnimationOptions&);
    void flyTo(const CameraOptions&,
        const AnimationOptions&);


    void  moveBy(const  ScreenPt&,
            const AnimationOptions& = {});
    void scaleBy(double scale, const  ScreenPt& aAnchor,
            const AnimationOptions& animation = {});
    void zoomBy(double aDiffZ, const AnimationOptions& aAnim = {});
    void pitchBy(double pitch,
            const AnimationOptions& animation = {});
    void rotateBy(const  ScreenPt& first,
            const  ScreenPt& second,
            const AnimationOptions& = {}  );
    void rotateBy_deg(const  double aDegDiff,
        const AnimationOptions& animation);

private:
    std::unique_ptr<CDrawEngine> m_drawEngine;

    std::shared_ptr<CMapEngine>  m_mapEngine;
};

} // namespace clientmap 
#endif // #ifndef CMDMap_H_INCLUDED
