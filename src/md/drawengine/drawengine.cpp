#include <assert.h>
#include "md/util/merc_prj.hpp"
#include "md/util/logtool.hpp"

#include "platformbase/gl_interface.h" 


#include "md/drawengine/glpainters/glpainters.h"
#include "md/drawengine/drawcontext.h"

#include "md/drawengine/drawengine.h"


namespace clientmap {
 
//////////////////////////////////////////////////////////
// CDrawEngine
//////////////////////////////////////////////////////////
CDrawEngine::CDrawEngine()
    :mTrans(mTransState)
{
     
}

CDrawEngine::~CDrawEngine()
{
    done();
}

 
bool CDrawEngine::init(std::shared_ptr<IDrawDevice> _drawDevice,
    double             _pixelRatio )
{
    bool ret = true; 
    if (!_drawDevice ||
        (_pixelRatio <= 0.0) )
    {
        assert(0);
        ret = false;
    }
    else {
       m_drawDevice = _drawDevice;        
       mTransState.setPixelRatio(_pixelRatio);

       m_renderQueued.clear();// clear

       mPainters= std::make_unique<GlPainters>( );
    }

    return ret;
}

 
void CDrawEngine::done()
{ 

}

	// 重置
bool CDrawEngine::restore()
{
    bool ret = true;

    return ret;
}
 

// 重置资源
void CDrawEngine::resetResource()
{

}

void CDrawEngine::setViewSize(int32_t _w, int32_t _h)
{
    std::lock_guard<std::mutex>  lck(m_transCS);
    mTrans.resize(_w, _h);
}

void CDrawEngine::getViewSize(int32_t& _w, int32_t & _h)  
{
    std::lock_guard<std::mutex>  lck(m_transCS);
    _w = mTransState.viewWidth( );
    _h = mTransState.viewHeight();
}


void CDrawEngine::setDefaultFboId(uint32_t aFboId)
{ 
    std::lock_guard<std::mutex>  lck(m_transCS);
    mTransState.setDefaultFboId(aFboId);
}

void CDrawEngine::setScreenPixelRatio(double _pxRatio)
{
    std::lock_guard<std::mutex>  lck(m_transCS);
    mTransState.setPixelRatio(_pxRatio);
}

void CDrawEngine::resize(int32_t _w, int32_t _h)
{
    std::lock_guard<std::mutex>  lck(m_transCS);
    mTrans.resize(_w, _h);
}

void CDrawEngine::setFirstRender()
{

}

void CDrawEngine::requestDrawing()
{
    if (!m_renderQueued.test_and_set()) // test, then set
    {
        std::shared_ptr<IDrawDevice> drawDevice = m_drawDevice.lock();
        if (drawDevice) {
            drawDevice->triggerDraw();
        }            
    } 
}
 
bool CDrawEngine::drawFrame()
{
    bool ret = true;
    m_renderQueued.clear();// clear
 
    std::shared_ptr<IDrawDevice> drawDevice = m_drawDevice.lock();
    if (drawDevice) 
    { 
        drawDevice->activate();//--start

        {
            std::lock_guard<std::mutex>  lck(m_transCS);
            mTransState4Draw = mTransState;
        } 



        {          
            CPaintContext dc{
            mTransState4Draw,
            Clock::now()
            };

            _beginDrawFrame(dc);
            _drawFrame(dc);
            _endDrawFrame(dc);
        }
        drawDevice->deactivate();//--stop  
    }
    return ret;
}


// 获取空闲状态
bool CDrawEngine::isIdle() const
{
    bool ret = true;

    return ret;
}

// 添加渲染器
void CDrawEngine::addScene(DrawSceneBaseSP aScenePtr)
{
    std::lock_guard<std::mutex> lck(m_sceneArrCS);
    m_sceneArr.push_back(aScenePtr);
}
// 移除渲染器
void CDrawEngine::rmScene(DrawSceneBaseSP aScenePtr)
{
    std::lock_guard<std::mutex> lck(m_sceneArrCS);

    auto  itr= m_sceneArr.begin();
    for (; itr != m_sceneArr.end(); itr++)
    {
        auto sceneSP = *itr;
        if (sceneSP == aScenePtr)// found aScenePtr
        {
            // 找到后，删除它
            m_sceneArr.erase(itr);
            break;
        }
    }
}

void CDrawEngine::_beginDrawFrame(const CPaintContext& aDC)
{
    std::lock_guard<std::mutex> lck(m_sceneArrCS);

    auto  itr = m_sceneArr.begin();
    for (; itr != m_sceneArr.end(); itr++)
    {
        auto sceneSP = *itr;
        sceneSP->onBeginDrawFrame( *mPainters, aDC);
    }
}
void CDrawEngine::_drawFrame(const CPaintContext& aDC)
{
    md::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    auto& cam =const_cast<TransformState&>(aDC._transState);
    md::glViewport(0, 0,cam.viewWidth(), cam.viewHeight() );

    

    
    std::lock_guard<std::mutex> lck(m_sceneArrCS); 
    Log::Debug("CDrawEngine::_drawFrame----start\n");
    auto  itr = m_sceneArr.begin();
    for (; itr != m_sceneArr.end(); itr++)
    {
        auto sceneSP = *itr;
        sceneSP->onDrawFrame(*mPainters, aDC);
    }
    Log::Debug("CDrawEngine::_drawFrame----end\n");
}
void CDrawEngine::_endDrawFrame(const CPaintContext& aDC)
{
    std::lock_guard<std::mutex> lck(m_sceneArrCS);

    auto  itr = m_sceneArr.begin();
    for (; itr != m_sceneArr.end(); itr++)
    {
        auto sceneSP = *itr;
        sceneSP->onEndDrawFrame(*mPainters, aDC);
    }
}
//------------------------------------------

void CDrawEngine::getMapCenter(double& _lon, double& _lat) const
{
     LonLat mapCt= mTrans.getMapCenter();
    _lon = mapCt.lng();
    _lat = mapCt.lat();
}

// 缩放级别(Zoom Level)  
int32_t CDrawEngine::getZoom() const
{
    return mTransState.getZoomi();
}

 
double CDrawEngine::getScale() const
{
    double z =  mTransState.getZoom();
    return MercPrj::scale4zoom(z);
}

 
double CDrawEngine::getBearing() const
{
    double roll = mTransState.getRotateZ(); 

    return glm::mod(-roll, 360.0);
}

// 倾斜角度(rotate X) 视线垂直地图为0度 角度单位
double CDrawEngine::getPitch() const
{
    return mTransState.getRotateX();
}
 
//-------------------手势相关---------------
#pragma mark - Transitions

void
CDrawEngine::cancelTransitions()
{
    mTrans.cancelTransitions();
    requestDrawing();
}

void
CDrawEngine::setGestureInProgress(bool inProgress)
{
    mTrans.setGestureInProgress(inProgress);
    requestDrawing();
}

bool
CDrawEngine::isGestureInProgress() const
{
    return mTrans.isGestureInProgress();
}

bool
CDrawEngine::isRotating() const
{
    return mTrans.isRotating();
}

bool
CDrawEngine::isScaling() const
{
    return mTrans.isScaling();
}

bool
CDrawEngine::isPanning() const
{
    return mTrans.isPanning();
}

#pragma mark -

CameraOptions
CDrawEngine::getCameraOptions( ) const
{
    return mTrans.getCameraOptions();
}

void
CDrawEngine::jumpTo(const CameraOptions& camera)
{
    mCameraMutated = true;
    mTrans.jumpTo(camera);
    requestDrawing();
}

void
CDrawEngine::easeTo(const CameraOptions& camera, 
    const AnimationOptions& animation)
{
    mCameraMutated = true;
    mTrans.easeTo(camera, animation);
    requestDrawing();
}

void
CDrawEngine::flyTo(const CameraOptions& camera, 
    const AnimationOptions& animation)
{
    mCameraMutated = true;
    mTrans.flyTo(camera, animation);
    requestDrawing();
}

void
CDrawEngine::moveBy(const  ScreenPt& point,
    const AnimationOptions& animation
)
{
    mCameraMutated = true;
    mTrans.moveBy(point, animation);
    requestDrawing();
}

void
CDrawEngine::pitchBy(double pitch, 
    const AnimationOptions& animation)
{
    double pitchF = mTransState.getRotateX()  - pitch;

    
    if (animation.duration <= 0.0) {
        jumpTo(CameraOptions().withPitch(pitchF));
    }
    else {
        easeTo(CameraOptions().withPitch(pitchF), animation);
    }
}

void
CDrawEngine::scaleBy(double scale,
    const  ScreenPt& aAnchor,
    const AnimationOptions& animation)
{    
    LonLat  scaleAnchor;
    scaleAnchor = mTrans.screenPtToLatLng(aAnchor);

    double deltaZ = MercPrj::zoom4scale(scale);
    double zoom = mTransState.getZoom() + deltaZ;
    if (animation.duration <= 0.0) {
        jumpTo(CameraOptions().withCenter(scaleAnchor).withZoom(zoom));
    }
    else {
        easeTo(CameraOptions().withCenter(scaleAnchor).withZoom(zoom), animation);
    }
    
}

void
CDrawEngine::zoomBy(double aDeltaZ, const AnimationOptions& aAnim)
{     
    double zoom = mTransState.getZoom() + aDeltaZ;

    if (aAnim.duration <= 0.0) {
        jumpTo(CameraOptions().withZoom(zoom));
    }
    else {
        easeTo(CameraOptions().withZoom(zoom), aAnim);
    }
}

void
CDrawEngine::rotateBy(const  ScreenPt& first,
    const  ScreenPt& second,
    const AnimationOptions& animation)
{
    mCameraMutated = true;
    mTrans.rotateBy(first, second, animation);
    requestDrawing();
}

void CDrawEngine::rotateBy_deg(const  double aDegDiff,
    const AnimationOptions& animation)
{
    mCameraMutated = true;
    mTrans.rotateBy_deg(aDegDiff, animation);
    requestDrawing();
}
#if 0
CameraOptions
CDrawEngine::cameraForLatLngBounds(const LatLngBounds& bounds,
    const EdgeInsets& padding,
    const optional<double>& bearing,
    const optional<double>& pitch) const
{
    return cameraForLatLngs(
        {
            bounds.northwest(),
            bounds.southwest(),
            bounds.southeast(),
            bounds.northeast(),
        },
        padding,
        bearing,
        pitch);
}

CameraOptions
cameraForLatLngs(const std::vector<LonLat>& latLngs,
    const Transform& transform,
    const EdgeInsets& padding)
{
    if (latLngs.empty())
    {
        return {};
    }
    Size size = transform.getState().getSize();
    // Calculate the bounds of the possibly rotated shape with respect to the viewport.
    ScreenPt nePixel = { -INFINITY, -INFINITY };
    ScreenPt swPixel = { INFINITY, INFINITY };
    for (LonLat latLng : latLngs) {
        ScreenPt pixel = transform.latLngToScreenPt(latLng);
        swPixel.x = std::min(swPixel.x, pixel.x);
        nePixel.x = std::max(nePixel.x, pixel.x);
        swPixel.y = std::min(swPixel.y, pixel.y);
        nePixel.y = std::max(nePixel.y, pixel.y);
    }
    double width = nePixel.x - swPixel.x;
    double height = nePixel.y - swPixel.y;

    // Calculate the zoom level.
    double minScale = INFINITY;
    if (width > 0 || height > 0) {
        double scaleX = double(size.width) / width;
        double scaleY = double(size.height) / height;
        scaleX -= (padding.left() + padding.right()) / width;
        scaleY -= (padding.top() + padding.bottom()) / height;
        minScale = util::min(scaleX, scaleY);
    }

    double zoom = transform.getZoom();
    if (minScale > 0) {
        zoom = util::cm_clamp(zoom + util::log2(minScale), transform.getState().getMinZoom(), transform.getState().getMaxZoom());
    }
    else {
        Log::Error(Event::General, 
            "Unable to calculate appropriate zoom level for bounds."
            " Vertical or horizontal padding is greater "
            "than map's height or width.");
    }

    // Calculate the center point of a virtual bounds that is extended in all directions by padding.
    ScreenPt centerPixel = nePixel + swPixel;
    centerPixel /= 2.0;

    return CameraOptions()
        .withCenter(transform.screenPtToLatLng(centerPixel))
        .withPadding(padding)
        .withZoom(zoom);
}

CameraOptions 
Map::cameraForLatLngs(const std::vector<LonLat>& latLngs,
    const EdgeInsets& padding,
    const optional<double>& bearing,
    const optional<double>& pitch) const {
    if (!bearing && !pitch) {
        return mbgl::cameraForLatLngs(latLngs, impl->transform, padding);
    }

    Transform transform(impl->transform.getState());

    if (bearing || pitch) {
        transform.jumpTo(CameraOptions().withBearing(bearing).withPitch(pitch));
    }

    return mbgl::cameraForLatLngs(latLngs, transform, padding)
        .withBearing(-transform.getBearing() * util::RAD2DEG)
        .withPitch(transform.getPitch() * util::RAD2DEG);
}

CameraOptions Map::cameraForGeometry(const Geometry<double>& geometry,
    const EdgeInsets& padding,
    const optional<double>& bearing,
    const optional<double>& pitch) const {
    std::vector<LonLat> latLngs;
    forEachPoint(geometry, [&](const Point<double>& pt) {
        latLngs.emplace_back(pt.y, pt.x);
    });
    return cameraForLatLngs(latLngs, padding, bearing, pitch);
}

LatLngBounds Map::latLngBoundsForCamera(const CameraOptions& camera) const 
{
    Transform shallow{ impl->transform.getState() };
    Size size = shallow.getState().getSize();

    shallow.jumpTo(camera);
    return LatLngBounds::hull(
        shallow.screenPtToLatLng({}),
        shallow.screenPtToLatLng({ double(size.width), double(size.height) })
    );
}

LatLngBounds 
Map::latLngBoundsForCameraUnwrapped(const CameraOptions& camera) const 
{
    Transform shallow{ impl->transform.getState() };
    Size size = shallow.getState().getSize();

    shallow.jumpTo(camera);
    LonLat nw = shallow.screenPtToLatLng({});
    LonLat se = shallow.screenPtToLatLng({ double(size.width), double(size.height) });
    LonLat ne = shallow.screenPtToLatLng({ double(size.width), 0.0 });
    LonLat sw = shallow.screenPtToLatLng({ 0.0, double(size.height) });
    LonLat center = shallow.screenPtToLatLng({ double(size.width) / 2, double(size.height) / 2 });
    nw.unwrapForShortestPath(center);
    se.unwrapForShortestPath(center);
    ne.unwrapForShortestPath(center);
    sw.unwrapForShortestPath(center);
    LatLngBounds bounds = LatLngBounds::hull(nw, se);
    bounds.extend(ne);
    bounds.extend(sw);
    bounds.extend(center);
    return bounds;
}
#endif

//------------------------------------------------------
}// namespace clientmap 

 