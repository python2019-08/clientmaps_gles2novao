#include <assert.h>
#include "md/util/constants.hpp"
#include "md/drawengine/drawengine.h"
#include "md/mapengine/style/style4map.hpp"  
#include "md/mapengine/mapengine.h"
#include "md/md_global.h" 
#include "md/md_map.h"
namespace clientmap {

CMDMap::CMDMap() 
{
    // 创建 CDrawEngine对象
    m_drawEngine = std::make_unique<CDrawEngine>();

    // 创建 CMapEngine对象，并注册到m_drawEngine上
    m_mapEngine = std::make_shared<CMapEngine>(*m_drawEngine);
    m_drawEngine->addScene( m_mapEngine );
}

CMDMap::~CMDMap()
{
    done();
}

int CMDMap::init(std::shared_ptr<IDrawDevice> aDrawDevice,
    double             aPixelRatio,
	const std::string& aRootPath,
	const std::string& aBaseUrl,
	const std::string& aAccessToken
    )
{
    assert(m_drawEngine && m_mapEngine);
	CMdGlobal::unique.init(aPixelRatio,aRootPath, aBaseUrl, aAccessToken);

    m_drawEngine->init(aDrawDevice, aPixelRatio );

    m_mapEngine->init(  );
 
    return 0;
}

void CMDMap::done()
{

}

void CMDMap::setStyle(const std::string&  aStyleName)
{
    if (m_mapEngine) 
    {
        m_mapEngine->setStyle(aStyleName);


        auto mapStyle = m_mapEngine->getStyle();
        if (mapStyle)
        {
            this->jumpTo(CameraOptions().withCenter(mapStyle->_lngLat)
                .withZoom(mapStyle->_zoom)
            );
        }
    }
}

void CMDMap::setStyleString(const std::string&  aStyleString)
{ 
    if (m_mapEngine) {
        m_mapEngine->setStyleString(aStyleString);
    }
}

void CMDMap::setDefaultFboId(uint32_t aFboId)
{
    if (m_drawEngine) {
        m_drawEngine->setDefaultFboId(aFboId);
    }
}

void CMDMap::resize(int32_t _w, int32_t _h)
{
    if (m_drawEngine) {
        m_drawEngine->resize(_w,  _h);
    } 
} 

void CMDMap::requestDrawing()
{
    if (m_drawEngine) {
        m_drawEngine->requestDrawing();
    }
}
void CMDMap::drawFrame()
{
    if (m_drawEngine) {
        m_drawEngine->drawFrame();
    }    
}
//--------gesture ---------------
void
CMDMap::cancelTransitions()
{
    m_drawEngine->cancelTransitions();
}
/*!
    Tells the map rendering engine that there is currently a gesture in \a progress. This
    affects how the map renders labels, as it will use different texture filters if a gesture
    is ongoing.
*/
void
CMDMap::setGestureInProgress(bool inProgress)
{
    m_drawEngine->setGestureInProgress(inProgress);
}

bool
CMDMap::isGestureInProgress() const
{
    return m_drawEngine->isGestureInProgress();
}

bool
CMDMap::isRotating() const
{
    return m_drawEngine->isRotating();
}

bool
CMDMap::isScaling() const
{
    return m_drawEngine->isScaling();
}

bool
CMDMap::isPanning() const
{
    return m_drawEngine->isPanning();
}

// Camera
CameraOptions CMDMap::getCameraOptions() const
{
    return m_drawEngine->getCameraOptions();
}

void CMDMap::jumpTo(const CameraOptions& aCamera)
{
    m_drawEngine->jumpTo(aCamera);
}

void CMDMap::returnToDefaultPos()
{ 
    auto mapStyle = m_mapEngine->getStyle();
    if (mapStyle)
    {
        clientmap::CameraOptions  camOption;
        camOption.withCenter(mapStyle->_lngLat);
        camOption.withZoom(mapStyle->_zoom);
        camOption.withBearing(0);
        camOption.withPitch(0);

        m_drawEngine->jumpTo(camOption);
    }    
}


void CMDMap::easeTo(const CameraOptions& aCamera, 
                    const AnimationOptions& aAnim)
{
    m_drawEngine->easeTo(aCamera, aAnim);
}
void CMDMap::flyTo(const CameraOptions& aCamera,
    const AnimationOptions& aAnim)
{
    m_drawEngine->flyTo(aCamera, aAnim);
}


void CMDMap::moveBy(const  ScreenPt& aPt,
    const AnimationOptions& aAnim)
{
    m_drawEngine->moveBy(aPt, aAnim);
}

void CMDMap::scaleBy(double aScale, 
    const  ScreenPt& aAnchor,
    const AnimationOptions& aAnim)
{
    m_drawEngine->scaleBy(aScale, aAnchor, aAnim);
}

void CMDMap::zoomBy(double aDiffZ,  
    const AnimationOptions& aAnim  )
{
    m_drawEngine->zoomBy(aDiffZ, aAnim);
}

void CMDMap::pitchBy(double aPitch,
    const AnimationOptions& aAnim )
{
    m_drawEngine->pitchBy(aPitch, aAnim);
}
void CMDMap::rotateBy(const  ScreenPt& aP1,
    const  ScreenPt& aP2,
    const AnimationOptions& aAnim
)
{
    m_drawEngine->rotateBy(aP1, aP2, aAnim);
}

void CMDMap::rotateBy_deg(const  double aDegDiff,
    const AnimationOptions& animation)
{
    m_drawEngine->rotateBy_deg(aDegDiff, animation);
}

//------------------------------------------
}//namespace md----------------------------- 