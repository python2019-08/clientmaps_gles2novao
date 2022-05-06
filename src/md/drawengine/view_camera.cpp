
#include <cmath>
 
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/epsilon.hpp" 

#include <md/util/math_tools.hpp>
#include "md/util/logtool.hpp"
#include <md/util/geo.hpp> 
#include "md/util/stops_def.hpp"
#include "md/util/merc_prj.hpp"
#include "md/drawengine/tilecover_rasterize.h"
#include <cmath>
#include <algorithm>


#include <md/util/tile_cover_bound.hpp>
 
#include "md/drawengine/view_camera.h"

#define MAX_LOD 6
#define CM_ROTATE_X_MAX 85.0 
#define CM_FOVY_MAX     85.0 
#define VIEW_WIDTH_MIN  2
#define VIEW_WIDTH_MAX  4096

#define BpxWorldBound_MinX   -536870912.0
#define BpxWorldBound_MaxX  536870912.0
#define BpxWorldBound_MinY   -536870912.0
#define BpxWorldBound_MaxY  536870912.0

namespace clientmap {

namespace {// Sky Config Data 
TStyleExp3D s_skyConfig = { {
        //zoom, {{rotX, skyHRatio_max, fovY}}
        { 0.0, {{30.2, 0.0, 35}} },
        {10.0, {{40.2, 0.0, 35}} },
        {11.0, {{45.2, 0.0, 35}} },
        {14.0, {{46.2, 0.0, 35}} },
        {15.0, {{59.2, 0.090, 35}} },
        {16.0, {{69.8, 0.168, 35}} },
        {17.0, {{73.8, 0.208, 35}} },
        {18.0, {{76.8, 0.248, 35}} },
    } };
}//END OF namespace{// Sky Config Data 

double invLodFunc(double d) {
    return exp2(d) - 1.0;
}
//----------------------------------------
ViewCamera::ViewCamera( ) : 
    m_vpWidth(800),
    m_vpHeight(600),
    m_roll(0.0),
    m_pitch(0.0),
    m_zoom(14.0),
    m_minZoom(2.5),
    m_maxZoom(22.0),
    m_obliqueAxis(0, 1)
{ 
    m_constraint.setLimitsX(-MercPrj::WORLD_HALF_WIDTH_BPX, MercPrj::WORLD_HALF_WIDTH_BPX);// +
    m_constraint.setLimitsY(-MercPrj::WORLD_HALF_WIDTH_BPX, MercPrj::WORLD_HALF_WIDTH_BPX);

    setViewSize(800, 600,false);
    setZoom(14.0, false);
    setMapCenterLonLat({ 116.37530842, 39.90924172 });
}
//
//ViewCamera::ViewCamera(const ViewCamera& other)
//{
//	*this = other;
//}
//
//ViewCamera&
//ViewCamera::operator=(const ViewCamera& aOther)
//{
//	if (this != &aOther) {
//		m_constraint = aOther.m_constraint;
//		m_defaultFboId = aOther.m_defaultFboId;
//		m_pixelRatio = aOther.m_pixelRatio;
//		m_vpWidth = aOther.m_vpWidth;
//		m_vpHeight = aOther.m_vpHeight;
//		m_roll = aOther.m_roll;
//		m_pitch = aOther.m_pitch;
//		m_zoom = aOther.m_zoom;
//		m_minZoom = aOther.m_minZoom;
//		m_maxZoom = aOther.m_maxZoom;
//		m_constrainToWorldBounds = aOther.m_constrainToWorldBounds;
//		m_type = aOther.m_type;
//		m_pos = aOther.m_pos;
//		m_pos_curZoomPx = aOther.m_pos_curZoomPx;
//		m_eye = aOther.m_eye;
//		m_obliqueAxis = aOther.m_obliqueAxis;
//		m_vanishingPoint = aOther.m_vanishingPoint;
//		_visEdgeInset = aOther._visEdgeInset;
//		 
//		_update();
//		_updateGroundBound();
//	}
//
//	return *this;
//}

ViewCamera::~ViewCamera()
{

}

void ViewCamera::setDefaultFboId(uint32_t  aDefaultFboId)
{
    m_defaultFboId = aDefaultFboId;
}

uint32_t ViewCamera::defaultFboId()
{
    return m_defaultFboId;
}

void ViewCamera::setPixelRatio(double aPixelsPerPoint, bool aIsToUpdate)
{
    if (std::isnan(aPixelsPerPoint) || std::isnan(aPixelsPerPoint)) {
        return;
    }
    m_pixelRatio = aPixelsPerPoint;

    if (aIsToUpdate)
    {
        _update();
    }     
}

double ViewCamera::pixelRatio() const 
{
    return m_pixelRatio;
}
 
float ViewCamera::pixelsPerMeter() const
{
    return MercPrj::pixelsPerMeterAtZoom(m_zoom);
}

 
void ViewCamera::setViewSize(int aWidth, int aHeight, bool aIsToUpdate)
{
    m_vpWidth  = glm::clamp(aWidth,  VIEW_WIDTH_MIN, VIEW_WIDTH_MAX);
    m_vpHeight = glm::clamp(aHeight, VIEW_WIDTH_MIN, VIEW_WIDTH_MAX);

    if (aIsToUpdate) {
        _update();
    }   
}
 
float ViewCamera::viewWidth() const 
{
    return m_vpWidth;
}

float ViewCamera::viewHeight() const 
{
    return m_vpHeight;
}

ScreenPt ViewCamera::getViewCenter() const 
{
    return ScreenPt( (double)m_vpWidth * 0.5, (double)m_vpHeight * 0.5);
}

void ViewCamera::setCameraOptions(const CameraOptions& aCamera,
    bool aIsToUpdate )
{

}

CameraOptions ViewCamera::getCameraOptions()
{
    Pt2D  ctMpx = { getMapCenter().x, getMapCenter().y };
    LonLat ct = MercPrj::LngLatFromPx(ctMpx, BASE_ZOOM, true);

    return CameraOptions()
        .withCenter(ct)
        .withViewCenterOffset({ 0,0 })
        .withZoom(getZoom())
        .withBearing(-m_roll * RAD2DEG)//???
        .withPitch(m_pitch *  RAD2DEG);
}


void ViewCamera::setMapCenter(double _x, double _y, bool aIsUpdate)
{
    // Wrap horizontal position around the 180th meridian, 
    // which corresponds to [-MercPrj::WORLD_HALF_WIDTH_BPX , 
    //                       MercPrj::WORLD_HALF_WIDTH_BPX)bpx.
    m_pos.x = cm_wrap(_x, -MercPrj::WORLD_HALF_WIDTH_BPX,
        MercPrj::WORLD_HALF_WIDTH_BPX);
    // Clamp vertical position to the span of the map,
    // which is [-WORLD_HALF_ , MercPrj::WORLD_HALF_)bpx.
    m_pos.y = glm::clamp(_y, -MercPrj::WORLD_HALF_WIDTH_BPX,
        MercPrj::WORLD_HALF_WIDTH_BPX);


    if (m_constrainToWorldBounds) {
        applyWorldBounds();
    }

    if (aIsUpdate)
    {
        _update();
    }
}

void ViewCamera::setMapCenter(const glm::dvec2 pos, bool aIsUpdate)
{
    setMapCenter(pos.x, pos.y, aIsUpdate);
}

const glm::dvec3& ViewCamera::getMapCenterRef()
{
    return m_pos;
}

Pt3D ViewCamera::getMapCenter() const
{
    return { m_pos.x,m_pos.y,m_pos.z };
}

Pt2D ViewCamera::getMapCenterPx() const
{
    auto px = MercPrj::px4bpx({ m_pos.x, m_pos.y }, m_zoom);
    return px;
}

void ViewCamera::setMapCenterLonLat(const LonLat& aCenter, bool aIsUpdate)
{
    auto bpx = MercPrj::pxFromLngLat({ aCenter.mLon, aCenter.mLat }, BASE_ZOOM);
    setMapCenter(bpx.x, bpx.y, aIsUpdate);
}

LonLat ViewCamera::getMapCenterLonLat() const
{
    auto center = MercPrj::LngLatFromPx({ m_pos.x, m_pos.y }, BASE_ZOOM);
    return center;
}


void ViewCamera::setVanishingPoint(float x, float y, bool aIsUpdate)
{
    m_vanishingPoint = { x, y };
    if (aIsUpdate) {
        _update();
    }
}

auto ViewCamera::vanishingPoint() const
{
    return m_vanishingPoint;
}

void ViewCamera::setZoomRange(const double aMin, const double aMax,
    bool aIsUpdate)
{
    if (glm::isnan(aMin) || glm::isinf(aMin) ||
        glm::isnan(aMax) || glm::isinf(aMax))
    {
        return;
    }

    double minZoom = glm::clamp(aMin, MIN_ZOOM, MAX_ZOOM);
    double maxZoom = glm::clamp(aMax, MIN_ZOOM, MAX_ZOOM);
    if (minZoom > maxZoom) {
        std::swap(minZoom, maxZoom);
    }

    // Constrain scale to avoid zooming out far enough to show
    // off-world areas on the Y axis.
    const double ratioY = ((double)m_vpHeight) / MercPrj::tileWIDTH;
    double minScale = glm::max(MercPrj::scale4zoom(minZoom), ratioY);
    minZoom = MercPrj::zoom4scale(minScale);
    if (minZoom > maxZoom) {
        return;
    }

    m_minZoom = minZoom;
    m_maxZoom = maxZoom;

    // Set the current zoom again to validate it.
    setZoom(m_zoom, aIsUpdate);
}

double ViewCamera::getMinZoom() const {
    return m_minZoom;
}

// Get the maximum zoom level.
double ViewCamera::getMaxZoom() const {
    return m_maxZoom;
}

 
void ViewCamera::setZoom(double aZoom, bool aIsToUpdate )
{
    if (glm::isnan(aZoom)  || glm::isinf(aZoom) ||  
        glm::epsilonEqual(aZoom, m_zoom, glm::epsilon<double>() ) 
        )
    {
        return;
    }

    // ensure zoom value is allowed
    m_zoom = glm::clamp(aZoom, m_minZoom, m_maxZoom);
          
    if (m_constrainToWorldBounds) {
        applyWorldBounds();
    }

    if (aIsToUpdate) {
        _update();
    }
}

double ViewCamera::getZoom() const // zoom level in double-type 
{
    //m_zoom = glm::clamp(m_zoom, 0.0, MAX_ZOOM);

    return m_zoom;
}

int ViewCamera::getZoomi() const {
    //return static_cast<int>(m_zoom);

    int  floor = (int)m_zoom;
    int  z = (m_zoom - floor > 0.65) ? floor + 1 : floor;
    return glm::clamp(z, 0, (int)BASE_ZOOM);
}
 

void ViewCamera::setRotateX(double aPitch, bool aIsUpdate)
{
    if (glm::isnan(aPitch) || glm::isinf(aPitch) ||
        glm::epsilonEqual(aPitch, m_pitch, glm::epsilon<double>())
        )
    {
        return;
    }

    m_pitch = glm::clamp<double>(aPitch, 0.0, CM_ROTATE_X_MAX);

    if (aIsUpdate)
    {
        _update();
    }
}

double ViewCamera::getRotateX() const
{
    return m_pitch;
}

void ViewCamera::setRotateZ(double aRoll,  bool aIsUpdate)
{ 
    if (glm::isnan(aRoll) || glm::isinf(aRoll) || 
        glm::epsilonEqual(aRoll, m_roll, glm::epsilon<double>())
        )
    {
        return;
    }    
    m_roll = cm_wrap(aRoll, 0.0, 360.0 );// wrap to [0,360)degrees 
      
    if (aIsUpdate)
    {
        _update();
    }
}

double ViewCamera::getRotateZ() const
{
    return m_roll;
}


void ViewCamera::setConstrainToWorldBounds(bool constrainToWorldBounds)
{
    m_constrainToWorldBounds = constrainToWorldBounds;
    if (m_constrainToWorldBounds)
    {
        applyWorldBounds();
    }

}


glm::mat4 ViewCamera::getMMatS() const
{
    return m_mMatS;
}

glm::mat4 ViewCamera::getVMat() const {
    return m_vMat;
}

// Get the transformation from view space into screen space.
glm::mat4 ViewCamera::getPMat() const {
    return m_pMat;
}

// Get the combined view and projection transformation.
glm::mat4 ViewCamera::getVPMat() const {
    return m_vpMat;
}

glm::mat4 ViewCamera::getMVPMat() const
{
    return m_mvpMat;
}

// Get the normal matrix; transforms surface normals from model space to camera space.
glm::mat3 ViewCamera::getNormalMat() const {
    return m_normalMatrix;
}
glm::mat3 ViewCamera::getInvNormalMat() const {
    return m_invNormalMatrix;
}

glm::mat4  ViewCamera::getVMatHUD() const
{
    return m_vMatHUD;
}
glm::mat4 ViewCamera::getPMatHUD() const
{
    return m_pMatHUD;
}


glm::vec2 ViewCamera::normalizedWindowCoordinates(float _x, float _y) const 
{
    return { _x / m_vpWidth, 1.0 - _y / m_vpHeight };
}


glm::dvec2 ViewCamera::bpx2RelativeBpx(double aAbsX, double aAbsY) const
{
    double dx = aAbsX - m_pos.x;
    double dy = aAbsY - m_pos.y;
    // If the position is closer when wrapped around the 180th meridian, 
    // then wrap it.
    cm_wrap(dx, -MercPrj::WORLD_HALF_WIDTH_BPX,
                 MercPrj::WORLD_HALF_WIDTH_BPX);

    return { dx, dy };
}


glm::dvec3 ViewCamera::bpx2RelativeBpx(const Pt3D& aPt) const
{
    return { aPt.x - m_pos.x,aPt.y - m_pos.y, aPt.z - 0 };
}


template <>
Pt2D ViewCamera::bpx2RelativeBpxX<Pt2D>(const Pt2D &coord) const
{
    auto tmp = Pt3D(coord.x, coord.y);
    return Pt2D(tmp.x - m_pos.x, tmp.y - m_pos.y);
}

void ViewCamera::bpx2RelativeBpx(Box2D& aBound) const
{
    aBound.set(aBound.mMinX - m_pos.x,
        aBound.mMinY - m_pos.y,
        aBound.mMaxX - m_pos.x,
        aBound.mMaxY - m_pos.y);
}

glm::dvec3 ViewCamera::relativeBpx2Bpx(const Pt3D& coor) const
{
    double orgx = m_pos.x;
    double orgy = m_pos.y;

    return { coor.x + orgx, coor.y + orgy, coor.z };
}


 
double ViewCamera::_rotateXAtCurZoom() const
{
    double rotXMax = 0;
    _skyCfgAtZoom(m_zoom, &rotXMax, nullptr, nullptr);

    return glm::clamp<double>(m_pitch, 0.0, rotXMax);
}


double ViewCamera::_fovYAtCurZoom() const
{
    double curZoomFovY = 0;
    _skyCfgAtZoom(m_zoom, nullptr, nullptr, &curZoomFovY);

    return curZoomFovY;
}


void ViewCamera::_update() 
{  
    _updateMatrices(); // Resets dirty flag 

    _updateGroundBound();
}
  

void ViewCamera::_updateMatrices() 
{
#if 0 
    // find dimensions of tiles in world space at new zoom level
    float worldTileSize = MercPrj::WORLD_WIDTH_BPX * exp2(-m_zoom);
    // viewport height in world space is such that each tile is [m_pixelsPerTile]
    // px square in screen space
    float screenTileSize = MercPrj::tileWIDTH * m_pixelRatio;  
    double pxGeoWidth = worldTileSize / screenTileSize; 
    geoHeight = (float)m_vpHeight * pxGeoWidth;
#else
    float worldTileSize = glm::pow(2.0, BASE_ZOOM - m_zoom)*MercPrj::tileWIDTH;
#endif


    m_mMatT = glm::translate(glm::dmat4(1.0), glm::dvec3(-m_pos.x, -m_pos.y, 0));
    // GroundPx = m_mMatS * glm::dvec4(RelativeBpx.x,RelativeBpx.y,RelativeBpx.z,1.0);
    // GroundPx is px coordinate at m_zoom(ctb named GL3D coordinate).
    double deltaS = glm::pow(2.0, m_zoom - BASE_ZOOM);//deltaScale 
    m_mMatS = glm::scale(glm::dmat4(1.0), glm::dvec3(deltaS, deltaS, deltaS));
    m_invMMatS = glm::inverse(m_mMatS);
    //----------------------------
    // set vertical field-of-view
    double fovy = glm::radians( _fovYAtCurZoom() );
    // the ratio of m_vpWidth to m_vpHeight
    double aspect = (double)m_vpWidth / (double)m_vpHeight;
    // we assume portrait orientation by default, so in landscape
    // mode we scale the vertical FOV such that the wider dimension
    // gets the intended FOV
    if (m_vpWidth > m_vpHeight) {
        fovy /= aspect;
    }

    // set camera z to produce desired viewable area
    double eyeZ = ((double)m_vpHeight) * 0.5 / tan(fovy * 0.5);

    double rotX = _rotateXAtCurZoom( );
    rotX = glm::radians(rotX);
    double rotZ = glm::radians(m_roll);
#if 0
    m_eye = glm::dvec3(0.f, 0.f, eyeZ);
    m_eye = glm::rotateX(m_eye, rotX);
    m_eye = glm::rotateZ(m_eye, rotZ);

    glm::dvec3 at = { 0.f, 0.f, 0.f };

    glm::dvec3 up = glm::dvec3(0.f, 1.f, 0.f);
    up = glm::rotateX(up, rotX);
    up = glm::rotateZ(up, rotZ);
#else
    m_eye = glm::rotateZ(glm::rotateX(glm::dvec3(0.f, 0.f, eyeZ), rotX), rotZ);
    glm::dvec3 at = { 0.f, 0.f, 0.f };
    glm::dvec3 up = glm::rotateZ(glm::rotateX(glm::dvec3(0.f, 1.f, 0.f), rotX), rotZ);
#endif

    // Generate view matrix
    m_vMat = glm::lookAt(m_eye, at, up);

    //----------------------------
    double maxTileDistance = worldTileSize * invLodFunc(MAX_LOD + 1);
    double zNear = eyeZ / 50.f;
    double zFar = 1;
    double hw = 0.5 * m_vpWidth;
    double hh = 0.5 * m_vpHeight;

    if (m_type == CameraType::perspective)
    {
        //----Generate perspective projection matrix 
        zFar = 2. * eyeZ / std::max(0., cos(rotX + 0.5 * fovy));//MNOTE:???
        zFar = std::min(zFar, maxTileDistance);
        m_pMat = glm::perspective(fovy, aspect, zNear, zFar);
        // Adjust for vanishing point.
        m_pMat[2][0] -= m_vanishingPoint.x / viewWidth();
        m_pMat[2][1] -= m_vanishingPoint.y / viewHeight();
    }
    else if (m_type == CameraType::isometric ||
        m_type == CameraType::flat)
    {
        //----Generate isometric projection matrix 
        zFar = 2. * (eyeZ + hh * std::abs(tan(rotX)));
        zFar = std::min(zFar, maxTileDistance);
        m_pMat = glm::ortho(-hw, hw, -hh, hh, zNear, zFar);
    } 

    if (m_type == CameraType::isometric) {
        glm::dmat4 shear = m_vMat;

        // Add the oblique projection scaling factors to the shear matrix
        shear[2][0] += m_obliqueAxis.x;
        shear[2][1] += m_obliqueAxis.y;

        // Remove the view from the shear matrix so we don't apply it two times
        shear *= glm::inverse(m_vMat);

        // Inject the shear in the projection matrix
        m_pMat *= shear;
    }
     
    //----------------------------
    m_vpMat = m_pMat * m_vMat;
    m_invVpMat = glm::inverse(m_vpMat);

    m_mvpMat = m_pMat * m_vMat * m_mMatS;
    m_invMvpMat = glm::inverse(m_mvpMat);

    m_mvpMatAll = m_pMat * m_vMat * m_mMatS * m_mMatT;
    m_invMvpMatAll = glm::inverse(m_mvpMatAll);
    //----------------------------
    // The matrix that transforms normals from world space to camera space is
    // the transpose of the inverse of the view matrix,but since our view matrix
    // is orthonormal transposing is equivalent to inverting, so the normal
    // matrix is just the original view matrix (cropped to the top-left 3 rows 
    // and columns, since we're applying it to 3d vectors)
    m_normalMatrix = glm::mat3(m_vMat);
    m_invNormalMatrix = glm::inverse(m_normalMatrix);
     
    //----------------------------
    // Screen space orthographic projection matrix, top left origin, y pointing down
    //glm::ortho(0.f, (float)aCamera.viewWidth(), 
    //     (float)aCamera.viewHeight(), 0.f,    -1.f, 1.f )
    m_pMatHUD = glm::ortho(0.0,  (double)m_vpWidth, 
                           (double)m_vpHeight, 0.0, -1.0, 10000.0); 
    m_vMatHUD = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5000));
}


void ViewCamera::_updateGroundBound()
{
    auto sh = getSkyHeight();
    bool bIntersect = true;

    double aGpxX,   aGpxY;
    double ret = screen2GroundPx(0, sh,  aGpxX,   aGpxY);    

    auto tl = bpxFromScreen(0, sh , bIntersect);
    auto tr = bpxFromScreen(m_vpWidth, sh, bIntersect);
    auto bl = bpxFromScreen(0, m_vpHeight, bIntersect);
    auto br = bpxFromScreen(m_vpWidth, m_vpHeight, bIntersect);

    m_coveredGroundBound.setPointXY(0, { tl.x, tl.y });
    m_coveredGroundBound.setPointXY(1, { tr.x, tr.y });
    m_coveredGroundBound.setPointXY(2, { br.x, br.y });
    m_coveredGroundBound.setPointXY(3, { bl.x, bl.y });
}


void ViewCamera::applyWorldBounds()
{
    // Approximate the view diameter in pixels by taking the maximum dimension.
    double viewDiameterPixels = std::fmax(viewWidth(), viewHeight()) / pixelRatio();

    //------------constraint m_zoom to WorldBounds
    // Approximate the minimum zoom that keeps with view span within the 
    // drawable projection area. [1]
    double worldBoundsMinZoom =
        std::log(viewDiameterPixels / MercPrj::tileWIDTH + 2) / std::log(2);

    if (m_zoom < worldBoundsMinZoom) {
        m_zoom = worldBoundsMinZoom;
    }

    //-------------constraint m_pos to WorldBounds
    // Constrain by moving map center to keep view in bounds.
    //m_constraint.setRadius(0.5 * viewDiameterPixels / pixelsPerMeter());//raw code
    m_constraint.setRadius(0.5 * viewDiameterPixels);
    m_pos.x = m_constraint.getConstrainedX(m_pos.x);
    m_pos.y = m_constraint.getConstrainedY(m_pos.y);
}


void ViewCamera::_skyCfgAtZoom(double  aZoom,
    double* aRotXMax,
    double* aSkyRatioMax,
    double* aFovy) const
{
    if (aRotXMax || aSkyRatioMax || aFovy)
    {
        double z = glm::clamp(aZoom, 0.0, MAX_ZOOM);
        auto   cfgAtZ = s_skyConfig.evaluate(z);

        if (aRotXMax)
        {
            double rotXMax = cfgAtZ.at(0);//  maxPitch at aZoom 
            *aRotXMax = glm::clamp(rotXMax, 0.0, CM_ROTATE_X_MAX);
        }

        if (aSkyRatioMax)
        {
            double skyRatioMax = cfgAtZ.at(1);//  maxPitch at aZoom 
            *aSkyRatioMax = glm::clamp(skyRatioMax, 0.0, 1.0);
        }

        if (aFovy)
        {
            double fovYMax = cfgAtZ.at(2);
            *aFovy = glm::clamp(fovYMax, 0.0, CM_FOVY_MAX);
        }
    }
}


double ViewCamera::_rotateXMax_global() const
{
    return CM_ROTATE_X_MAX;
}


double ViewCamera::_focalLength() const
{
    return fieldOfViewToFocalLength(_fovYAtCurZoom());
}

float ViewCamera::focalLengthToFieldOfView(float length)
{
    return 2.f * atanf(1.f / length);
}

float ViewCamera::fieldOfViewToFocalLength(float aDegrees)
{
    double radians = glm::radians(aDegrees);
    return 1.f / tanf(radians / 2.f);
}



ScreenPt ViewCamera::screenFromBpx(const Pt3D& aBpx) 
{
    bool outsideViewport = false;
    glm::vec2 screenPt =_bpxToScreenPos(aBpx.x, aBpx.y, outsideViewport);
    return { screenPt.x,screenPt.y };
}

Pt3D
ViewCamera::bpxFromScreen(double sx, double sy, bool& aIsIntersection)
{
    double dx = sx, dy = sy;
    double distance = screen2GroundPx(sx, sy, dx, dy);//not raw code
    aIsIntersection = (distance >= 0);


    glm::dvec4 relat = m_invMMatS * glm::dvec4{ dx, dy,0, 1 };
    Pt3D bpt(relat.x + m_pos.x, relat.y + m_pos.y, 0);
    return bpt;
}
 
//  screenToGroundPlane
double ViewCamera::screen2GroundPx(double _screenX, double _screenY,
    double& aGpxX, double& aGpxY) const
{
    // Cast a ray and find its intersection with the z = 0 plane,
    // following the technique described here: 
    // http://antongerdelan.net/opengl/raycasting.html

    glm::dvec4 target_clip = { 2. * _screenX / m_vpWidth - 1.,
                               1. - 2. * _screenY / m_vpHeight,
                               -1.,  1. };
    glm::dvec4 target_world = m_invVpMat * target_clip;//raw code 
    target_world /= target_world.w;

    glm::dvec4 origin_world;
    switch (m_type) {
    case CameraType::perspective:
        origin_world = glm::dvec4(m_eye, 1);
        break;
    case CameraType::isometric:
    case CameraType::flat:
        origin_world = m_invVpMat * (target_clip * glm::dvec4(1, 1, 0, 1));//raw code 
        break;
    }

    glm::dvec4 ray_world = target_world - origin_world;

    double t = 0; // Distance along ray to ground plane
    if (ray_world.z != 0.f) {
        t = -origin_world.z / ray_world.z;
    }

    ray_world *= std::abs(t);

    // Determine the maximum distance from the view position at which tiles 
    // can be drawn; If the projected point is farther than this maximum or 
    // if the point is above the horizon (t < 0) then we set the distance 
    // of the point to always be this maximum distance.
    double maxTileDistance = invLodFunc(MAX_LOD + 1) * 2.0
        * MercPrj::WORLD_HALF_WIDTH_BPX
        * pow(2, -m_zoom);
    double rayDistanceXY = sqrt(ray_world.x * ray_world.x
        + ray_world.y * ray_world.y);
    if (rayDistanceXY > maxTileDistance || t < 0) {
        ray_world *= maxTileDistance / rayDistanceXY;
    }

    //MNOTE: (aGpxX,aGpxY) is  px at cur zoom.
    aGpxX = ray_world.x + origin_world.x;
    aGpxY = ray_world.y + origin_world.y;

    return t;
}

/* screen coordinate to GroundPx coordinate, method 2. */
void ViewCamera::screen2GroundPx_2(double aSx, double aSy,
    double& aGx, double& aGy) const
{ 
    // The coordinate system origin of screen coordinates "(aSx, aSy)" 
    // is in the "upper-left" corner of the window;
    // The coordinate system of point "win" is actually the coordinate 
    // system specified by glViewport(), and the origin of glViewport() 
    // coordinate system is in the "lower-left" corner of the window;
    // 点win的坐标系实际是glViewPort指定的坐标系，而glViewPort坐标系原点在 
    // 窗口左下角 ;
    // win.z == 1.0，经过 unProject变换后得到的物体坐标位于 远侧裁剪平面上。
    // win.z == 0.0，经过 unProject变换后得到的物体坐标位于 近侧裁剪平面上。
    double winZ = 1.0;//0.99970531f
    glm::dvec3 win = glm::dvec3(aSx, m_vpHeight - aSy, winZ);
    glm::vec4  viewport = glm::dvec4(0, 0, m_vpWidth, m_vpHeight);
    glm::dvec3 obj = glm::unProject(win, m_vMat, m_pMat, viewport);

    // Find the intersection of ray(m_eye -> obj) and plane.
    // 求射线"m_eye -> obj"与平面plane的交点。
    // 
    // 射线ray的起点是m_eye，方向是从m_eye指向obj；
    // 平面plane由平面上的1点(0,0,0)和平面的法线向量(0,1,0)表示。
    glm::dvec3  rayDir = obj - m_eye;
    rayDir =glm::normalize(rayDir);
    glm::dvec3 rayHitPt;
    bool bHit = rayCast(m_eye, rayDir, 
        glm::dvec3(0, 0, 0), glm::dvec3(0, 1, 0), rayHitPt);

    aGx = (obj.x);
    aGy = (obj.z);
}
/// Computes the clip coordinates from position in 
/// world space and a model view matrix 
glm::vec4 ViewCamera::worldToClipSpace(const glm::mat4& mvp, 
                                       const glm::vec4& worldPos)
{
    return mvp * worldPos;
}

/// Return true if the given clip space point is behind 
/// the camera plane. 
bool ViewCamera::clipSpaceIsBehindCamera(const glm::vec4& clip)
{
    return clip.w < 0;
}

/// Return the given clip space point converted to Normalized Device Coordinates.
glm::vec3 ViewCamera::clipSpaceToNdc(const glm::vec4& clip)
{
    return glm::vec3(clip) / clip.w;
}

/// Return the given point in Normalized Device Coordinates converted to screen space.
glm::vec2 ViewCamera::ndcToScreenSpace(const glm::vec3& ndc, 
                                       const glm::vec2& screenSize)
{
    return glm::vec2(1 + ndc.x, 1 - ndc.y) * screenSize * 0.5f;
}


glm::vec2 ViewCamera::groundPx2Screen(double aXGpx, double aYGpx,
    bool& aOutsideViewport,
    bool  aClipToViewport)
{ 
    glm::dvec2 gpx = { aXGpx, aYGpx };
    // 
    glm::vec4 worldPosition(gpx, 0.0, 1.0);
    glm::vec4 clip = worldToClipSpace(m_vpMat, worldPosition);//raw code 
    glm::vec3 ndc = clipSpaceToNdc(clip);
    aOutsideViewport = clipSpaceIsBehindCamera(clip) ||
        abs(ndc.x) > 1 || abs(ndc.y) > 1;

    if (aOutsideViewport && aClipToViewport)
    {
        // Get direction to the point and determine the point on the 
        // screen edge in that direction.
        glm::vec4 worldDirection(gpx, 0, 0);
        glm::vec4 clipDirection = worldToClipSpace(m_vpMat, worldDirection);//raw code 
        ndc = glm::vec3(clipDirection) / glm::max(abs(clipDirection.x), abs(clipDirection.y));
    }

    glm::vec2 screenSize(m_vpWidth, m_vpHeight);
    glm::vec2 screenPosition = ndcToScreenSpace(ndc, screenSize);
    return screenPosition;
}


/*  3D坐标转为屏幕坐标 GL3DToScreen*/
void ViewCamera::groundPx2Screen_2(double aGx, double aGy, double aGz,
    double& aSx, double& aSy)const
{ 
    // 把物体坐标obj转换成 glViewPort指定的坐标系中的坐标win;
    // 点win所用的glViewPort坐标系的原点在窗口左下角（即点win的坐标系实际是
    // glViewPort指定的坐标系） .
    glm::dvec3 obj(aGx, aGy, aGz);
    glm::vec4  viewport = glm::dvec4(0, 0, m_vpWidth, m_vpHeight);
    glm::dvec3 win = glm::project(obj, m_vMat, m_pMat, viewport);

    // 把glViewPort指定的坐标系中的坐标转换成屏幕坐标ScreenPt。
    // ScreenPt坐标点(aGx,aGy)所在的屏幕坐标系原点在窗口左上角；
    aSx = win.x;
    aSy = m_vpHeight - win.y;
}


glm::vec2 ViewCamera::_bpxToScreenPos(double aXBpx, double aYBpx,
    bool& aOutsideViewport,
    bool  aClipToViewport)
{
    // absolute bpx coordinate to relative Bpx coordinate 
    glm::dvec2 relativeBpx = bpx2RelativeBpx( aXBpx, aYBpx );

    // 
    glm::vec4 worldPosition(relativeBpx, 0.0, 1.0);
    //glm::vec4 clip = worldToClipSpace(m_vpMat, worldPosition);//raw code
    glm::vec4 clip = worldToClipSpace(m_mvpMat, worldPosition);
    glm::vec3 ndc = clipSpaceToNdc(clip);
    aOutsideViewport = clipSpaceIsBehindCamera(clip) ||
        abs(ndc.x) > 1 || abs(ndc.y) > 1;

    if (aOutsideViewport && aClipToViewport)
    {
        // Get direction to the point and determine the point on the 
        // screen edge in that direction.
        glm::vec4 worldDirection(relativeBpx, 0, 0);
        //glm::vec4 clipDirection = worldToClipSpace(m_vpMat, worldDirection);//raw code
        glm::vec4 clipDirection = worldToClipSpace(m_mvpMat, worldDirection);
        ndc = glm::vec3(clipDirection) / glm::max(  abs(clipDirection.x), 
                                                    abs(clipDirection.y) );
    }

    glm::vec2 screenSize(m_vpWidth, m_vpHeight);
    glm::vec2 screenPosition = ndcToScreenSpace(ndc, screenSize);
    return screenPosition;
}


bool ViewCamera::rayCast(
    const glm::dvec3&  aRayStart, // Ray starting point
    const glm::dvec3&  aRayDir, // Ray direction vector,射线方向向量；
    const glm::dvec3&  aPlanePt, // A point on a plane,平面上的某个点；
    const glm::dvec3&  aPlaneNormal,  // Plane normal vector,平面法向量；
    glm::dvec3& aRayCastHitPt
)const
{    
    double d = -glm::dot(aPlaneNormal, aPlanePt);;

    double numer = glm::dot(aPlaneNormal,aRayStart) + d;
    double denom = glm::dot(aPlaneNormal,aRayDir);
    //glm::epsilonEqual(denom,0.0, glm::epsilon<double>());
    if (denom == 0)  // normal is orthogonal to vector, cant intersect
    {
        aRayCastHitPt =  glm::dvec3(0, 0, 0);
        return false;
    }


    double fDist = -(numer / denom);
    aRayCastHitPt = aRayStart + aRayDir * fDist;
    return true;
}


glm::vec2 
ViewCamera::lngLatToScreenPos(double aLng, double aLat, 
                            bool& aOutsideViewport, 
                            bool  aClipToViewport) 
{ 
    Pt2D absoluteBpx = MercPrj::pxFromLngLat({ aLng, aLat },BASE_ZOOM);

    return _bpxToScreenPos(absoluteBpx.x, absoluteBpx.y,
        aOutsideViewport, aClipToViewport);
}

LonLat 
ViewCamera::screenPosToLngLat(float _x, float _y, bool& _intersection) 
{ 
    double dx = _x, dy = _y;
    double distance = screen2GroundPx(_x, _y, dx, dy);
    _intersection = (distance >= 0);


    glm::dvec4 relat = m_invMMatS * glm::dvec4{ dx, dy,0, 1 };
    Pt2D bpt(relat.x + m_pos.x, relat.y + m_pos.y );
    LonLat lngLat = MercPrj::LngLatFromPx(bpt, BASE_ZOOM);
    return lngLat.wrapped();
}

glm::dvec3 ViewCamera::bpx2GroundPx(const Pt3D& aBPt)
{ 
    glm::dvec3 relat = bpx2RelativeBpx(aBPt);

    glm::dvec4 groundPx = m_mMatS * glm::dvec4(relat,1.0);
    return glm::dvec3(groundPx); //{ groundPx.x ,groundPx.y,groundPx.z };
}

Pt3D ViewCamera::groundPx2Bpx(const glm::dvec3& aGpt)
{
    glm::dvec4 relat= m_invMMatS * glm::dvec4(aGpt, 1.0);
    auto bpt = relativeBpx2Bpx({ relat.x,relat.y,relat.z });
    return { bpt.x,bpt.y,bpt.z };
}

//------------------------
void ViewCamera::setEdgePaddingRatio(float tv, float bv,
    float lv, float rv)
{
    auto t = glm::clamp<float>(tv,      0.0,1.0); // ratio
    auto b = glm::clamp<float>((1 - bv),0.0, 1.0);
    auto l = glm::clamp<float>(lv,      0.0, 1.0);
    auto r = glm::clamp<float>((1 - rv),0.0, 1.0);
    _visEdgeInset = glm::vec4(t, b, l, r);
}


GeoBox ViewCamera::getScrBoundVisible() const 
{
    auto w = viewWidth();
    auto h = viewHeight();

    auto t = int(_visEdgeInset.x * h);
    auto b = int(_visEdgeInset.y * h);
    auto l = int(_visEdgeInset.z * w);
    auto r = int(_visEdgeInset.w * w);

    return GeoBox(l, t, r, b);
}


Pt3D ViewCamera::roundToBoundVisible(double x, double y)
{
    auto const &bound = getScrBoundVisible();

    auto t = std::max(int(bound.mMinY), getSkyHeight());
    auto b = int(bound.mMaxY);
    auto l = int(bound.mMinX);
    auto r = int(bound.mMaxX);

    Quad2D scrQard;
    scrQard.setPointXY(0, { (double)l, (double)t });
    scrQard.setPointXY(1, { (double)l, (double)b });
    scrQard.setPointXY(2, { (double)r, (double)b });
    scrQard.setPointXY(3, { (double)r, (double)t });

    glm::dvec2 center(bound.center().x, bound.center().y);
    glm::dvec2 vec = glm::dvec2(x, y) - center;
    vec = glm::normalize(vec);
    vec *= 10000;
    auto dest = center + vec;

    Pt2D interPoint;
    scrQard.intersect({ center.x , center.y }, { dest.x , dest.y },
        interPoint);

    return Pt3D(interPoint.x, interPoint.y, 0);
}

Quad2D  ViewCamera::getGroundBound()const
{
    return m_coveredGroundBound;
}

double ViewCamera::getSkyBoxHeightRatio() 
{ 
    double curZoom =  getZoom() ;  
    double rotXMax = 0;
    double skyRatioMax = 0;
    double fovy = 45.0;
    _skyCfgAtZoom(curZoom, &rotXMax, &skyRatioMax, &fovy);
    if (skyRatioMax <= 0.0 || rotXMax <= 0.0) {
        return 0.0;
    }
     
    double  rotXCur = glm::clamp<double>(m_pitch, 0.0, rotXMax);;
    double  r = rotXCur / rotXMax;
    double  finalRatio = r * skyRatioMax;
    //
    double minRatio = getMinSkyHRatio(rotXCur, fovy);
    finalRatio = (finalRatio >= minRatio) ? finalRatio : minRatio;

    //printf("Zoom=%d rotX=%f  minSkyHiRatio=%f\n", 
    //          getZoomi(),rotXCur,minRatio);

    return finalRatio;
}

// 计算当前倾角下 最小的天空高度
double ViewCamera::getMinSkyHRatio(double  aRotXDeg, double aCurFoy)
{
    double       rotX = aRotXDeg;//_rotateXAtCurZoom() ;

    double aspect = m_vpWidth * 1.0 / m_vpHeight;
    double halfFovy = aCurFoy * 0.5;

    double minRatio = 0.0;
    if (90.0 - rotX < halfFovy)
    {
        double notSkyDeg = 90.0 - rotX - 1.5;
        double tanHalfFovy = tan(glm::radians(halfFovy));// complete sky 
        minRatio = tanHalfFovy - tan(glm::radians(notSkyDeg));
        minRatio /= (tanHalfFovy * 2.0);
    }

    return  minRatio;
}


int ViewCamera::getSkyHeight()
{ 
    double  skyRatio = getSkyBoxHeightRatio();



    return (int)glm::round(m_vpHeight * skyRatio);
}


int ViewCamera::getCoveredTiles_01(std::vector<TileID>& aTiles)
{
    int curZoomi = getZoomi();
 
    auto lt = m_coveredGroundBound.getLeftTop();
    auto rt = m_coveredGroundBound.getRightTop();
    auto rb = m_coveredGroundBound.getRightBottom();
    auto lb = m_coveredGroundBound.getLeftBottom();
    auto ct = m_coveredGroundBound.getCenter();
    lt.x = glm::clamp<double>(lt.x, BpxWorldBound_MinX, BpxWorldBound_MaxX);
    rt.x = glm::clamp<double>(rt.x, BpxWorldBound_MinX, BpxWorldBound_MaxX);
    rb.x = glm::clamp<double>(rb.x, BpxWorldBound_MinX, BpxWorldBound_MaxX);
    lb.x = glm::clamp<double>(lb.x, BpxWorldBound_MinX, BpxWorldBound_MaxX);
    ct.x = glm::clamp<double>(ct.x, BpxWorldBound_MinX, BpxWorldBound_MaxX);
    lt.y = glm::clamp<double>(lt.y, BpxWorldBound_MinY, BpxWorldBound_MaxY);
    rt.y = glm::clamp<double>(rt.y, BpxWorldBound_MinY, BpxWorldBound_MaxY);
    rb.y = glm::clamp<double>(rb.y, BpxWorldBound_MinY, BpxWorldBound_MaxY);
    lb.y = glm::clamp<double>(lb.y, BpxWorldBound_MinY, BpxWorldBound_MaxY);
    ct.y = glm::clamp<double>(ct.y, BpxWorldBound_MinY, BpxWorldBound_MaxY);

    const std::array<Pt2D, 5> quad{ lt,rt,rb,lb,ct };
    getCoveredTilesbyBpxQuad(quad, curZoomi, aTiles);
 
    printf("Lv=%d tiles.size = %d\n", curZoomi, aTiles.size());
    return aTiles.size();
}

//#define Frustum_CULL 1
int ViewCamera::getCoveredTiles_02(std::vector<TileID>& aTiles)
{
    int curZoomi = getZoomi();
    Log::Debug("ViewCamera::getCoveredTiles_02...start\n");
    Box2D bbox = m_coveredGroundBound.getBound();
    bbox.mMinX = glm::clamp<double>(bbox.mMinX, BpxWorldBound_MinX, BpxWorldBound_MaxX);
    bbox.mMaxX = glm::clamp<double>(bbox.mMaxX, BpxWorldBound_MinX, BpxWorldBound_MaxX);
    bbox.mMinY = glm::clamp<double>(bbox.mMinY, BpxWorldBound_MinY, BpxWorldBound_MaxY);
    bbox.mMaxY = glm::clamp<double>(bbox.mMaxY, BpxWorldBound_MinY, BpxWorldBound_MaxY);

    std::vector<TileID>  tiles;
    getCoveredTilesByBpxBbox(bbox, curZoomi, tiles);

#ifdef   Frustum_CULL      
    double worldSize = MercPrj::worldPxWidth(curZoomi);
    //
    Pt2D mapCtPx = MercPrj::px4bpx({ m_pos.x,m_pos.y }, curZoomi);
    glm::dmat4  mMatT = glm::translate(glm::dmat4(1),
        glm::dvec3(-mapCtPx.x, -mapCtPx.y, 0.0));
    glm::dmat4 mvp = m_pMat * m_vMat * mMatT;
    glm::dmat4 invMvp = glm::inverse(mvp);
    //
    Frustum frustum = Frustum::fromInvProjMatrix(invMvp, 1, 1);

    //-- for debug ,start
    std::array<float, 16> p;
    p[0] = m_pMat[0][0];
    p[1] = m_pMat[0][1];
    p[2] = m_pMat[0][2];
    p[3] = m_pMat[0][3];
    p[4] = m_pMat[1][0];
    p[5] = m_pMat[1][1];
    p[6] = m_pMat[1][2];
    p[7] = m_pMat[1][3];
    p[8]  = m_pMat[2][0];
    p[9]  = m_pMat[2][1];
    p[10] = m_pMat[2][2];
    p[11] = m_pMat[2][3];
    p[12]  = m_pMat[3][0];
    p[13]  = m_pMat[3][1];
    p[14] = m_pMat[3][2];
    p[15] = m_pMat[3][3];

    glm::dmat4 mvGlm = m_vMat * mMatT;
    std::array<float, 16> mv;
    mv[0] = mvGlm[0][0];
    mv[1] = mvGlm[0][1];
    mv[2] = mvGlm[0][2];
    mv[3] = mvGlm[0][3];
    mv[4] = mvGlm[1][0];
    mv[5] = mvGlm[1][1];
    mv[6] = mvGlm[1][2];
    mv[7] = mvGlm[1][3];
    mv[8] = mvGlm[2][0];
    mv[9] = mvGlm[2][1];
    mv[10] = mvGlm[2][2];
    mv[11] = mvGlm[2][3];
    mv[12] = mvGlm[3][0];
    mv[13] = mvGlm[3][1];
    mv[14] = mvGlm[3][2];
    mv[15] = mvGlm[3][3];
    std::array<std::array<float, 4>, 6>  frustumPlanes;
    Frustum::calcFrustumPlanes(p, mv, frustumPlanes);
    //-- for debug ,end
#endif //#ifdef   Frustum_CULL 
    //glm::dvec4 relBpx= m_mMatT * glm::dvec4(m_pos.x,m_pos.y,0,1.0);
    Box2D viewBBox(0, 0,  m_vpWidth, m_vpHeight);
    for (int i = 0; i < (int)tiles.size(); i++)
    {
        TileID& tileId = tiles[i];
        Box2D tileBBox = MercPrj::tileBoundsInBpx(tileId);
        
        ScreenPt bl = this->screenFromBpx({ tileBBox.mMinX,tileBBox.mMinY, 0 });
        ScreenPt br = this->screenFromBpx({ tileBBox.mMaxX,tileBBox.mMinY, 0 });
        ScreenPt tr = this->screenFromBpx({ tileBBox.mMaxX,tileBBox.mMaxY, 0 });
        ScreenPt tl = this->screenFromBpx({ tileBBox.mMinX,tileBBox.mMaxY, 0 });
        Quad2D   qd(tl, tr, br, bl);
        Box2D    tileScrnBBox= qd.getBound();
        if (viewBBox.intersect(tileScrnBBox))
        {
            aTiles.push_back(tileId);
            Log::Debug("s2bTid %d %d %d\n", tileId.z, tileId.x, tileId.y);
        }

    #ifdef   Frustum_CULL  
        Box2D tileBBox1 = MercPrj::tileBoundsInPx(tileId);
        //AABB  ab({ tileId.x, tileId.y, 0.0 },
        //    { tileId.x+1, tileId.y+1, 0.0 });
        AABB  ab({ tileBBox1.mMinX, tileBBox1.mMinY, 0.0 },
            { tileBBox1.mMaxX, tileBBox1.mMaxY, 0.0 });
        IntersectionResult intersectRet = frustum.intersects(ab);
        if (intersectRet != IntersectionResult::Separate)
        {
            //aTiles.push_back(tileId);
            Log::Debug("frustumTid %d %d %d\n", tileId.z, tileId.x, tileId.y);
        }
    #endif //#ifdef   Frustum_CULL 
    }  
    Log::Debug("ViewCamera::getCoveredTiles_02...end..Lv=%d tiles.size = %d\n",
        curZoomi, aTiles.size());
    return aTiles.size();
}
 


int ViewCamera::getCoveredTiles(std::vector<TileID>& aTiles)
{
    return getCoveredTiles_02(aTiles);
}

void ViewCamera::getVisibleTiles(std::vector<TileID>& aTiles) const
{
#if 0
    int zoom = getZoomi();
    int maxTileIndex = 1 << zoom;

    // Bounds of view trapezoid in world space (i.e. view 
    // frustum projected onto z = 0 plane)
    glm::dvec2 viewBL = { 0.f,       m_vpHeight }; // bottom left
    glm::dvec2 viewBR = { m_vpWidth, m_vpHeight }; // bottom right
    glm::dvec2 viewTR = { m_vpWidth, 0.f        }; // top right
    glm::dvec2 viewTL = { 0.f,       0.f        }; // top left

    double t0 = _screenToGroundPlane(viewBL.x, viewBL.y, viewBL.x, viewBL.y);
    double t1 = _screenToGroundPlane(viewBR.x, viewBR.y, viewBR.x, viewBR.y);
    double t2 = _screenToGroundPlane(viewTR.x, viewTR.y, viewTR.x, viewTR.y);
    double t3 = _screenToGroundPlane(viewTL.x, viewTL.y, viewTL.x, viewTL.y);

    // if all of our raycasts have a negative intersection distance, we have no area to cover
    if (t0 < .0 && t1 < 0. && t2 < 0. && t3 < 0.) {
        return;
    }

    // Transformation from world space to tile space
    //double hc = MercPrj::EARTH_HALF_CIRCUMFERENCE_METERS;// old code
    double hc = MercPrj::WORLD_HALF_WIDTH_BPX;
    //double invTileSize = double(maxTileIndex) / MercPrj::EARTH_CIRCUMFERENCE_METERS;// old code
    double invTileSize = double(maxTileIndex) / MercPrj::WORLD_WIDTH_BPX;
    //glm::dvec2 tileSpaceOrigin(-hc, hc);// old code
    glm::dvec2 tileSpaceOrigin(-hc, -hc);
    //glm::dvec2 tileSpaceAxes(invTileSize, -invTileSize);// old code
    glm::dvec2 tileSpaceAxes(invTileSize, invTileSize);

    // Bounds of view trapezoid in tile space
    glm::dvec2 a = (glm::dvec2(viewBL.x + m_pos.x, viewBL.y + m_pos.y) - tileSpaceOrigin) * tileSpaceAxes;
    glm::dvec2 b = (glm::dvec2(viewBR.x + m_pos.x, viewBR.y + m_pos.y) - tileSpaceOrigin) * tileSpaceAxes;
    glm::dvec2 c = (glm::dvec2(viewTR.x + m_pos.x, viewTR.y + m_pos.y) - tileSpaceOrigin) * tileSpaceAxes;
    glm::dvec2 d = (glm::dvec2(viewTL.x + m_pos.x, viewTL.y + m_pos.y) - tileSpaceOrigin) * tileSpaceAxes;

    // Location of the view center in tile space
    glm::dvec2 e = (glm::dvec2(m_pos.x + m_eye.x, m_pos.y + m_eye.y) - tileSpaceOrigin) * tileSpaceAxes;

    static const int imax = std::numeric_limits<int>::max();
    static const int imin = std::numeric_limits<int>::min();

    // Scan options - avoid heap allocation for std::function
    // [1] http://www.drdobbs.com/cpp/efficient-use-of-lambda-expressions-and/232500059?pgno=2
    struct ScanParams {
        explicit ScanParams(int _zoom, int _maxZoom) 
        : zoom(_zoom), maxZoom(_maxZoom) 
        {}

        int zoom;
        int maxZoom;

        // Distance thresholds in tile space for levels of detail:
        // Element [n] in each array is the minimum tile index at which level-of-detail n
        // should be applied in that direction.
        int x_limit_pos[MAX_LOD] = { imax };
        int x_limit_neg[MAX_LOD] = { imin };
        int y_limit_pos[MAX_LOD] = { imax };
        int y_limit_neg[MAX_LOD] = { imin };

        glm::ivec4 last = glm::ivec4{-1};
    };

    ScanParams opt{ zoom, static_cast<int>(m_maxZoom) };

    if (m_type == CameraType::perspective) 
    {
        // Determine zoom reduction for tiles far from the center of view
        //double tilesAtFullZoom = std::max(m_width, m_height) * invTileSize * 0.5;//old code
        double longer = std::max(m_vpWidth, m_vpHeight);
        double pxGeoW = MercPrj::pxGeoWidth(m_zoom);
        double tilesAtFullZoom = (longer * pxGeoW) * invTileSize * 0.5;
        double viewCenterX = (m_pos.x + hc) * invTileSize;
        double viewCenterY = (m_pos.y - hc) * -invTileSize;

        for (int i = 0; i < MAX_LOD; i++) 
        {
            int j = i + 1;
            double r = invLodFunc(i) + tilesAtFullZoom;
            opt.x_limit_neg[i] = ((int(viewCenterX - r) >> j) - 1) << j;
            opt.y_limit_pos[i] = ((int(viewCenterY + r) >> j) + 1) << j;
            opt.y_limit_neg[i] = ((int(viewCenterY - r) >> j) - 1) << j;
            opt.x_limit_pos[i] = ((int(viewCenterX + r) >> j) + 1) << j;
        }
    }

    Rasterize::ScanCallback s = [&opt, &aTiles](int x, int y)
    {

        int lod = 0;
        while (lod < MAX_LOD && x >= opt.x_limit_pos[lod]) 
        { lod++; }
        while (lod < MAX_LOD && x <  opt.x_limit_neg[lod]) 
        { lod++; }
        while (lod < MAX_LOD && y >= opt.y_limit_pos[lod]) 
        { lod++; }
        while (lod < MAX_LOD && y <  opt.y_limit_neg[lod]) 
        { lod++; }

        x >>= lod;
        y >>= lod;

        glm::ivec4 tile;
        tile.z = glm::clamp((opt.zoom - lod), 0, opt.maxZoom);

        // Wrap x to the range [0, (1 << z))
        tile.x = x & ((1 << tile.z) - 1);
        tile.y = y;

        if (tile != opt.last) {
            opt.last = tile;

            aTiles.push_back(TileID(tile.z,tile.x, tile.y));
        }
    };

    // Rasterize view trapezoid into tiles
    Rasterize::scanTriangle(a, b, c, 0, maxTileIndex, s);
    Rasterize::scanTriangle(c, d, a, 0, maxTileIndex, s);

    // Rasterize the area bounded by the point under the view center 
    // and the two nearest corners of the view trapezoid. This is 
    // necessary to not cull any geometry with height in these tiles
    // (which should remain visible, even though the base of the tile is not).
    Rasterize::scanTriangle(a, b, e, 0, maxTileIndex, s);
#endif
}

}// namespace clientmap

// [1]
// The maximum visible span horizontally is the span covered by 2^z - 2 tiles.
// We consider one tile to be effectively not visible because at the 180th
// meridian it will be drawn on one side or the other, not half on both 
// sides. Tile coverage is calculated from the floor() of our zoom value 
// but here we operate on the continuous zoom value, so we remove one more 
// tile to under-approximate the coverage. Algebraically we get:
// (span of view in meters) = (view diameter in pixels) 
//            * (earth circumference) / ((tile size in pixels) * 2^z)
// If we substitute the desired view span at the minimum zoom:
// (span of view in meters) = (earth circumference) * (2^z - 2) / 2^z
// We can solve for the minimum zoom:
// z = log2((view diameter in pixels) / (tile size in pixels) + 2)
