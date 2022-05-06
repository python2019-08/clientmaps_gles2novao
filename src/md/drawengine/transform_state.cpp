#include <md/util/math_tools.hpp>
#include <md/util/tile_id.hpp>
#include <md/util/constants.hpp>
#include <md/util/interpolate.hpp>
#include <md/util/logtool.hpp>
#include <md/util/stops_def.hpp>

#include <md/drawengine/transform_state.hpp>

#include <cmath>
#include <algorithm>
 
#include <md/util/geo.hpp>
#include <md/util/merc_prj.hpp>
#include <md/util/tile_cover_bound.hpp>

#include "glm/gtc/matrix_transform.hpp"
 

#define VIEW_WIDTH_MIN 2
 
 
#define CM_ROTATE_X_MAX 85.0 

#define CM_TILE_COUNT_MAX 256
 
#define CLAMP_ZOOM(x)    CM_CLAMP(x, 0, 22)
#define CLAMP2_0_1(x)    CM_CLAMP(x, 0, 1)
 

namespace clientmap { 
    
namespace{// Sky Config Data 
    TStyleExp3D s_skyConfig = {{
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
    //------------------------
    XformState::XformState( ) 
    {
        mViewWidth = 600;
        mViewHeight = 800; 
        mRotX = 0;
        mRotZ = 0;
        mZoomMax = BASE_ZOOM;
        mZoomMin = 1.1;

        Pt2D bpxPt=MercPrj::pxFromLngLat(
            { 116.3333333,39.93333333 }, BASE_ZOOM);
        mMapCntr = { bpxPt.x,bpxPt.y, 0};
        mZoom     = 5.0; 
   
        _makeWorldBoundInBpx();

        _update();
    }

    XformState::XformState(const XformState& other) 
    {
        *this = other;
    }

    XformState& 
    XformState::operator=(const XformState& other) 
    {
        if (this == &other) {
            return *this;
        }

        mViewWidth = other.mViewWidth;
        mViewHeight = other.mViewHeight; 
        mMapCntr = other.mMapCntr;
        mZoom = other.mZoom;
        mRotX = other.mRotX;
        mRotZ = other.mRotZ;
        mZoomMax = other.mZoomMax;
        mZoomMin = other.mZoomMin;
        _visEdgeInset = other._visEdgeInset;

        m_PMat = other.m_PMat;
        _update();
        _updateGroundBound();
         

        return *this;
    }

    XformState::~XformState() 
    {
         
    }
     

    void XformState::setDefaultFboId(uint32_t  aDefaultFboId)
    {
        mDefaultFboId = aDefaultFboId;
    }

    void XformState::setPixelRatio(double aPixelRatio)
    {
        mPixelRatio = aPixelRatio;
    }

    void XformState::setViewSize(int width, int height)
    {
        auto zoom = getZoom();

        this->mViewWidth = cm_max<int>(VIEW_WIDTH_MIN, width);
        this->mViewHeight= cm_max<int>(VIEW_WIDTH_MIN, height);
        m_aspect = (float)mViewWidth / (float)mViewHeight;

        setZoom(zoom);
    }

    // Euler-Angle
    //------------------------
    double XformState::getRotateZ() const {
        return mRotZ;
    }

    double XformState::getRotateX() const {
        return mRotX;
    }

    double XformState::_rotateXMax_global() const
    {
        return CM_ROTATE_X_MAX;
    }

    double XformState::_rotateXMax_atCurZoom() const { 
        double zoom = CLAMP_ZOOM(getZoom());
        auto valAtZ = s_skyConfig.evaluate(zoom);
        return valAtZ.at(0);
    }
     

    void XformState::setRotateX(double angle) 
    {
        if (angle == mRotX) {
            return;
        }

        mRotX = CM_CLAMP(angle, 0.0, CM_ROTATE_X_MAX);
        _update();    
    }

    void XformState::setRotateZ(double angle) 
    {
        if (glm::isnan(angle) || glm::isinf(angle) ||
            angle == mRotZ)
        {
            return;
        }

        mRotZ = cm_wrap(angle, 0.0, 360.0);// wrap to [0,360)degrees 

        _update(); 
    }

    // zoom
    //------------------------
    int XformState::getZoomi() const 
    {
        int  floor = (int)mZoom;
        int  z = (mZoom - floor > 0.65) ? floor + 1 : floor;
        z =  CM_CLAMP(z, 0, BASE_ZOOM);

        return z;
    }

    double XformState::getZoom() const {
        return mZoom;
    }

    void XformState::setZoom(double aZoom) 
    {   
        if (glm::isnan(aZoom) || glm::isinf(aZoom)|| 
            mZoom == aZoom)
        {
            return;
        }          

        mZoom = cm_clamp(aZoom,mZoomMin, mZoomMax);
        _update();
    }


    double XformState::getZoomFromFocalLen(double aFocalLen) const
    {
        // calc focalLenAtBaseZoom( EyeToCenterDistance at BaseZoom )
        auto valAtZ = s_skyConfig.evaluate( (float)getZoomi() );
        double curHalfFovY = valAtZ.at(2) * 0.5;
        double hnh = glm::tan( glm::radians(curHalfFovY) );         
        auto focalLenAtBaseZoom = 0.5 * mViewHeight / hnh;

        // _distance is focalLen at mZoom
        return BASE_ZOOM - glm::log2(aFocalLen / focalLenAtBaseZoom);
    }
     
    // FocalLen ,指的是相机到观察点(屏幕中心)距离
    double XformState::getFocalLen(double aZoom) const
    {        
        double curHalfFovY = _fovYAtZoom(aZoom) * 0.5;
        double hnh = glm::tan(glm::radians(curHalfFovY));// half near-plane height 
        auto focalLenAtBaseZoom = mViewHeight *0.5 / hnh;

        double zoom = glm::clamp(aZoom, MIN_ZOOM, MAX_ZOOM);
        return glm::exp2(BASE_ZOOM - zoom) * focalLenAtBaseZoom;         
    }

    // LookAt
    //------------------------
    Pt3D XformState::getMapCenter() const 
    {
        auto coord = Pt3D(mMapCntr.x, mMapCntr.y, 0);
        return coord;
    }

    void XformState::setMapCenter(double geoX, double geoY) {
        if (std::isnan(geoX) || std::isnan(geoY)) {
            return;
        }

        LonLat ctLL = MercPrj::LngLatFromPx({ geoX,geoY }, BASE_ZOOM);
        if (ctLL.mLat > 85)
        {
            printf("dddd");
        }

        auto dest = Pt3D(geoX, geoY);

        auto suitCoord = _constrainCoord(dest);
        if (suitCoord == mMapCntr) {
            return;
        }
    
        mMapCntr = suitCoord;
        _update();
    }

    // Zoom
    //------------------------
    double XformState::getMaxZoom() const {
        return mZoomMax;
    }
     

    double XformState::getMinZoom() const {
        return mZoomMin;
    }
     

    void XformState::setZoomRange(const double aMin, const double aMax)
    {
        double minZoom = cm_clamp(aMin, MIN_ZOOM, MAX_ZOOM);
        double maxZoom = cm_clamp(aMax, MIN_ZOOM, MAX_ZOOM);
        if (minZoom > maxZoom) {
            std::swap(minZoom, maxZoom);
        }

        // Constrain scale to avoid zooming out far enough to show
        // off-world areas on the Y axis.
        const double ratioY = ((double)mViewHeight) / MercPrj::tileWIDTH;
        double minScale = cm_max( MercPrj::scale4zoom(minZoom), ratioY);
        minZoom = MercPrj::zoom4scale( minScale );
        if (minZoom > maxZoom) {
            return;
        }

        mZoomMin = minZoom;
        mZoomMax = maxZoom;
    }

    // Size
    //------------------------
    int XformState::viewWidth() const {
        return mViewWidth;
    }

    int XformState::viewHeight() const {
        return mViewHeight;
    }
     

    ScreenPt XformState::getViewCenter() const {
        return ScreenPt(mViewWidth / 2.0, mViewHeight / 2.0);
    }

    CameraOptions XformState::getCameraOptions()
    {
        Pt2D  ctMpx = {getMapCenter().x, getMapCenter().y };
        LonLat ct= MercPrj::LngLatFromPx(ctMpx,BASE_ZOOM, true);
        return CameraOptions()
            .withCenter(ct)
            .withViewCenterOffset({0,0})
            .withZoom(getZoom())
            .withBearing( -mRotZ * RAD2DEG)//???
            .withPitch( mRotX *  RAD2DEG);
    }

    // Pt3D Transform
    //------------------------
    ScreenPt XformState::screenFromBpx(Pt3D coor) const 
    {
        double rotX = _rotateXAtCurZoom();

        auto wtoeMat = glm::translate(glm::dmat4(1), 
                        glm::dvec3(-mMapCntr.x, -mMapCntr.y, 0));
        auto npp = m_PMat * m_VMat * wtoeMat 
                   * glm::dvec4(coor.x, coor.y, coor.z, 1);

        auto hsw = mViewWidth / 2;
        auto hsh = mViewHeight / 2;

        npp /= npp.w;
        auto x = (1 + npp.x) * hsw;
        auto y = (1 - npp.y) * hsh;

        return ScreenPt(x, y);
    }


    Pt3D 
    XformState::bpxFromScreen(double sx, double sy, bool& bIntersect) const
    { 
        auto hsw = mViewWidth / 2;// 屏幕宽度的一半
        auto hsh = mViewHeight / 2;// 屏幕高度的一半

        double curHalfFovY = _fovYAtZoom(getZoom() ) * 0.5;
        auto hnh = glm::tan(glm::radians(curHalfFovY)); // half the height of near plne
        auto hnw = hnh * mViewWidth / mViewHeight;// half the width of near plne

        auto tInCm = glm::dvec3((sx / hsw - 1)*hnw, 
                        (1 - sy / hsh)*hnh, -1);// 近裁剪面上的点A(原点在近裁剪面中心)
        auto tdist = glm::length(tInCm);// 点A到眼睛的距离

        auto ctowMat = glm::translate(glm::dmat4(1), 
                                glm::dvec3(mMapCntr.x, mMapCntr.y, 0)
                            ) * m_invVMat;
        auto tInWd = ctowMat * glm::dvec4(tInCm, 1); // 点A的世界坐标
        auto cInWd = ctowMat * glm::dvec4(0, 0, 0, 1);// 眼睛的世界坐标
        auto nTVec = glm::normalize(tInWd - cInWd);

        auto ground = cInWd + 
            nTVec * tdist * cInWd.z / (cInWd.z - tInWd.z);//--- 

        auto coor = Pt3D(ground.x, ground.y, 0);
        return coor;
    }

    glm::dvec3 XformState::bpx2RelativeBpx(const Pt3D& coor) const
    {
        auto tmp = coor;
        glm::dvec3  relat={ tmp.x - mMapCntr.x,
                            tmp.y - mMapCntr.y,
                            tmp.z - mMapCntr.z };
        return relat;
    }


    template <>
    Pt2D XformState::bpx2RelativeBpxX<Pt2D>(const Pt2D &coord) const 
    {
        auto tmp = Pt3D(coord.x, coord.y);
        return Pt2D(tmp.x - mMapCntr.x, tmp.y - mMapCntr.y);
    }
     
    void XformState::bpx2RelativeBpx(Box2D& aBound) const
    { 
        aBound.set(aBound.mMinX - mMapCntr.x,
                aBound.mMinY - mMapCntr.y,
                aBound.mMaxX - mMapCntr.x,
                aBound.mMaxY - mMapCntr.y);
    }

    void XformState::relativeBpx2Bpx(Pt3D& coor) const 
    {
        double orgx = mMapCntr.x;
        double orgy = mMapCntr.y;

        coor.x = (coor.x + orgx);
        coor.y=  (coor.y + orgy);
    }

     

    // GroundBound
    //------------------------
    Quad2D XformState::getGroundBound()const
    {  
        return  mOutGroundBound;
    }

    Quad2D XformState::getGroundBoundVisible()
    {
        auto bound = getScrBoundVisible();

        auto t = std::max(int(bound.mMinY), getSkyHeight());
        auto b = int(bound.mMaxY );
        auto l = int(bound.mMinX);
        auto r = int(bound.mMaxX);

        t = std::max(t, getSkyHeight());

        bool intersect = true;
        auto lt = bpxFromScreen(l, t, intersect);
        auto lb = bpxFromScreen(l, b, intersect);
        auto rb = bpxFromScreen(r, b, intersect);
        auto rt = bpxFromScreen(r, t, intersect);

        Quad2D quard;
        quard.setPointXY(0, { lt.x, lt.y });
        quard.setPointXY(1, { lb.x, lb.y });
        quard.setPointXY(2, { rb.x, rb.y });
        quard.setPointXY(3, { rt.x, rt.y });
        return quard;
    }

    GeoBox XformState::getScrBoundVisible() const {
        auto w = viewWidth();
        auto h = viewHeight();

        auto t = int(_visEdgeInset.x * h);
        auto b = int(_visEdgeInset.y * h);
        auto l = int(_visEdgeInset.z * w);
        auto r = int(_visEdgeInset.w * w);

        return GeoBox(l, t, r, b);
    }

    Pt3D XformState::roundToBoundVisible(double x, double y) 
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

    bool XformState::intersect(TileID tile) {
        Box2D  bound = MercPrj::tileBoundsInBpx( tile);
        return mOutGroundBound.intersect(bound );
    }

    bool XformState::intersect(const Box2D& bound) {
        return mOutGroundBound.intersect(bound );
    }

    bool XformState::intersect(Pt3D coord) {
        return mOutGroundBound.contains(Pt2D(coord.x, coord.y));
    }

    // Screen-Visible
    //------------------------
    void XformState::setEdgePaddingRatio(float tv, float bv,  
        float lv,  float rv) 
    {
        auto t = CLAMP2_0_1(tv); // ratio
        auto b = CLAMP2_0_1((1 - bv));
        auto l = CLAMP2_0_1(lv);
        auto r = CLAMP2_0_1((1 - rv));
        _visEdgeInset = glm::vec4(t, b, l, r);
    }
     
    // SkyBox
    //------------------------
    // 计算当前倾角下 最小的天空高度
    double XformState::getMinSkyHRatio(double   aCurRotXDeg)
    {
        double       rotX = aCurRotXDeg;//_rotateXAtCurZoom() ;

        double aspect = mViewWidth * 1.0 / mViewHeight;
        double halfFovy = _fovYAtZoom( getZoom() ) * 0.5;

        double minRatio = 0.0;
        if (90.0 - rotX < halfFovy)
        {
            double tanHalfFovy = tan(glm::radians(halfFovy));
            minRatio = tanHalfFovy - tan(glm::radians(90.0 - rotX - 1.2));
            minRatio /= (tanHalfFovy * 2.0);
        }

        return  minRatio;
    }

    double XformState::getSkyBoxHeightRatio() {
        if (getZoomi() < 15) {
            return 0;
        }

        // 计算((curLayer - 15.0)/(BASE_ZOOM -15.0)) * maxRatio
        double curZoom = CLAMP_ZOOM(getZoom());
        auto valAtZ = s_skyConfig.evaluate(curZoom) ;
        double curLayerMaxHRate = valAtZ.at(1);

        //
        double rotateMax = _rotateXMax_atCurZoom();
        if (rotateMax <= 0) {
            return 0;
        }

        double  rotXCur = _rotateXAtCurZoom();
        double  r = rotXCur / rotateMax;
        double  finalRatio = r * curLayerMaxHRate;


        double minRatio = getMinSkyHRatio(rotXCur);
        finalRatio = (finalRatio >= minRatio) ? finalRatio : minRatio;

        return finalRatio;
    }

    int XformState::getSkyHeight() 
    {
        auto rotateMax = _rotateXMax_atCurZoom();
        if (rotateMax <= 0) {
            return 0;
        }

        double  finalRatio = getSkyBoxHeightRatio();

        //printf("Lv=%d rotX=%f  minSkyHiRatio=%f\n", getZoomi(),rotX,minSkyHiRatio);

        return (int)glm::round(mViewHeight * finalRatio);
    }

    double XformState::_fovYAtZoom(double aZoom) const
    { 
        double zoom = CLAMP_ZOOM(aZoom);
        auto valAtZ = s_skyConfig.evaluate(zoom);
        double curZoomFovY = valAtZ.at(2);
        return curZoomFovY;
    }

    void  XformState::_makeWorldBoundInBpx()
    {
        Pt2D minBpxPt=MercPrj::pxFromLngLat(
            { -180, -LATITUDE_MAX }, BASE_ZOOM);
        Pt2D maxBpxPt = MercPrj::pxFromLngLat(
            { 180,  LATITUDE_MAX }, BASE_ZOOM);
        mBpxWorldBound ={ minBpxPt, maxBpxPt };
    }

    // Tiles
    //------------------------
    int XformState::getCoveredTiles(std::vector<TileID>& aTiles) 
    {   
        int curZoomi = getZoomi();
    #if 0
        auto lt = mOutGroundBound.getLeftTop();
        auto rt = mOutGroundBound.getRightTop();
        auto rb = mOutGroundBound.getRightBottom();
        auto lb = mOutGroundBound.getLeftBottom();
        auto ct = mOutGroundBound.getCenter();
        lt.x = glm::clamp(lt.x, mBpxWorldBound.mMinX, mBpxWorldBound.mMaxX);
        rt.x = glm::clamp(rt.x, mBpxWorldBound.mMinX, mBpxWorldBound.mMaxX);
        rb.x = glm::clamp(rb.x, mBpxWorldBound.mMinX, mBpxWorldBound.mMaxX);
        lb.x = glm::clamp(lb.x, mBpxWorldBound.mMinX, mBpxWorldBound.mMaxX);
        ct.x = glm::clamp(ct.x, mBpxWorldBound.mMinX, mBpxWorldBound.mMaxX);
        lt.y = glm::clamp(lt.y, mBpxWorldBound.mMinY, mBpxWorldBound.mMaxY);
        rt.y = glm::clamp(rt.y, mBpxWorldBound.mMinY, mBpxWorldBound.mMaxY);
        rb.y = glm::clamp(rb.y, mBpxWorldBound.mMinY, mBpxWorldBound.mMaxY);
        lb.y = glm::clamp(lb.y, mBpxWorldBound.mMinY, mBpxWorldBound.mMaxY);
        ct.y = glm::clamp(ct.y, mBpxWorldBound.mMinY, mBpxWorldBound.mMaxY);

        const std::array<Pt2D, 5> quad{lt,rt,rb,lb,ct};        
        getCoveredTilesbyBpxQuad(quad, curZoomi, aTiles);
    #else
        Box2D bbox = mOutGroundBound.getBound();
        bbox.mMinX = glm::clamp(bbox.mMinX, mBpxWorldBound.mMinX, mBpxWorldBound.mMaxX);
        bbox.mMaxX = glm::clamp(bbox.mMaxX, mBpxWorldBound.mMinX, mBpxWorldBound.mMaxX);
        bbox.mMinY = glm::clamp(bbox.mMinY, mBpxWorldBound.mMinY, mBpxWorldBound.mMaxY);
        bbox.mMaxY = glm::clamp(bbox.mMaxY, mBpxWorldBound.mMinY, mBpxWorldBound.mMaxY);

        getCoveredTilesByBpxBbox(bbox, curZoomi, aTiles);

    #endif
        printf("Lv=%d tiles.size = %d\n", curZoomi, aTiles.size());
        return aTiles.size();
    } 

    //  
    //------------------------
    glm::mat4 XformState::getVMat() const 
    {
        return m_VMat;
    }

    glm::mat4 XformState::getPMat() const 
    {
        return m_PMat;
    }

    glm::mat4 XformState::getVPMat() const 
    {
        return m_PMat * m_VMat;
    }

    glm::mat4 XformState::getVMatHUD() const 
    {
        return glm::translate(glm::mat4(1), glm::vec3(0, 0, -5000));
    }

    glm::mat4 XformState::getPMatHUD() const 
    {
        return glm::ortho<float>(0, mViewWidth, mViewHeight, 0, 0, 10000);
    }

    // Misc
    //------------------------ 
    double XformState::getScaleFactor(Pt3D pos) {
        auto d1 = _distToPointByScrCoord(mViewWidth / 2, (getSkyHeight() + mViewHeight) / 2);
        auto d2 = _distToPointByGeoCoord(pos.x, pos.y, pos.z);
        return d1 / d2;
    }

    double XformState::getScreenCenterScaleFactor(Pt3D pos) 
    {
        auto d1 = _distToPointByScrCoord(mViewWidth / 2, mViewHeight / 2);
        auto d2 = _distToPointByGeoCoord(pos.x, pos.y, pos.z);
        return d1 / d2;
    }
     

    ///* 三维旋转变换，参数：旋转轴（由点p1和p2定义）和旋转角度（aDegrees）*/
    //static glm::dmat4 
    //rotate3D(glm::dmat4& finalM,  
    //    glm::dvec3 p1,  glm::dvec3 p2,  double aDegrees)
    //{
    //    /* 设置旋转轴的矢量 */
    //    double vx = (p2.x - p1.x);
    //    double vy = (p2.y - p1.y);
    //    double vz = (p2.z - p1.z);
    //    /*通过平移-旋转-平移复合变换序列完成任意轴的旋转（注意OpenGL中的反序表示）*/

    //    // (3)移动p1到原始位置
    //    finalM = glm::translate(finalM, glm::dvec3(p1.x, p1.y, p1.z));
    //    // (2)关于通过坐标原点的坐标轴旋转
    //    finalM = glm::rotate(finalM, glm::radians(aDegrees), glm::dvec3(vx, vy, vz));
    //    // (1)移动p1到原点位置
    //    finalM = glm::translate(finalM, glm::dvec3(-p1.x, -p1.y, -p1.z));

    //    //
    //    return  finalM;
    //}

    // Private
    //------------------------
    void XformState::_update() 
    {
        auto aspect = ((double)mViewWidth) / ((double)mViewHeight);
        //  auto fovy = glm::atan(NEAR_PLANE_HALF_WIDTH()/aspect) * 2;
        double fovy = glm::radians( _fovYAtZoom(getZoom() ) );
        m_PMat = glm::infinitePerspective(fovy, aspect, 1.0);
          
        auto rotx = _rotateXAtCurZoom();

        // 
        double distance = getFocalLen(mZoom);
        m_VMat = glm::dmat4(1);
        m_VMat = glm::translate(m_VMat, glm::dvec3(0, 0, -distance));
        m_VMat = glm::rotate(m_VMat, glm::radians(-rotx), glm::dvec3(1, 0, 0));
        m_VMat = glm::rotate(m_VMat, glm::radians(-mRotZ), glm::dvec3(0, 0, 1));


        m_invVMat = glm::dmat4(1);
        m_invVMat = glm::rotate(m_invVMat, glm::radians(mRotZ), glm::dvec3(0, 0, 1));
        m_invVMat = glm::rotate(m_invVMat, glm::radians(rotx), glm::dvec3(1, 0, 0));
        m_invVMat = glm::translate(m_invVMat, glm::dvec3(0, 0, distance));
        //-----------------------
        _updateGroundBound();

        //--------------dbg,start
        double zoom = getZoom();
        double curFovY = _fovYAtZoom(zoom);

        double       rotX = _rotateXAtCurZoom();
        double  rotXRatio = rotX / _rotateXMax_atCurZoom();
        double  minSkyHRatio = getMinSkyHRatio(rotX);
        double  maxSkyHiRatio = getSkyBoxHeightRatio();
        double  finalSkyHRatio = rotXRatio * maxSkyHiRatio;
        //TestInfo("SdkLog", "L=%f scrnWH(%d,%d) fovY=%f rotX(%f,%f,Rate=%f) skyHRate(min=%f max=%f %f)\n",
        //    layer, mViewWidth, mViewHeight, curFovY, mRotX, rotX, rotXRatio,
        //    minSkyHRatio, maxSkyHiRatio, finalSkyHRatio);
        printf("L=%f scrnWH(%d,%d) fovY=%f rotX(%f,%f,Rate=%f) skyHRate(min=%f max=%f %f)\n",
            zoom, mViewWidth, mViewHeight, curFovY, mRotX, rotX, rotXRatio,
            minSkyHRatio, maxSkyHiRatio, finalSkyHRatio);
        //--------------dbg,end

    }

    void XformState::_updateGroundBound() 
    {
        auto sh = getSkyHeight();

        bool intersect = true;
        auto tl = bpxFromScreen(0, sh , intersect);
        auto tr = bpxFromScreen(mViewWidth, sh , intersect);
        auto bl = bpxFromScreen(0, mViewHeight ,intersect);
        auto br = bpxFromScreen(mViewWidth, mViewHeight, intersect);

        mOutGroundBound.setPointXY(0, { tl.x, tl.y });
        mOutGroundBound.setPointXY(1, { tr.x, tr.y });
        mOutGroundBound.setPointXY(2, { br.x, br.y });
        mOutGroundBound.setPointXY(3, { bl.x, bl.y });
    }

    double XformState::_distToPointByScrCoord(int sx, int sy) 
    {
        auto hsw = mViewWidth * 0.5;
        auto hsh = mViewHeight * 0.5;

        double curHalfFovY = _fovYAtZoom(getZoom() ) * 0.5;
        auto hnh = glm::tan(glm::radians(curHalfFovY)); // 近裁剪面宽度的一半
        auto hnw = hnh * mViewWidth / mViewHeight;

        auto tInCm = glm::dvec3((sx / hsw - 1)*hnw, (1 - sy / hsh)*hnh, -1);
        auto tdist = glm::length(tInCm);

        auto ctowMat = glm::translate(glm::dmat4(1), 
            glm::dvec3(mMapCntr.x, mMapCntr.y, 0) ) * m_invVMat;
        auto tInWd = ctowMat * glm::dvec4(tInCm, 1);
        auto cInWd = ctowMat * glm::dvec4(0, 0, 0, 1);

        return tdist * cInWd.z / (cInWd.z - tInWd.z);
    }

    double XformState::_distToPointByGeoCoord(double x, double y, double z) {
        auto ctowMat = glm::translate(glm::dmat4(1), 
                   glm::dvec3(mMapCntr.x, mMapCntr.y, 0))  * m_invVMat;
        auto cInWd = ctowMat * glm::dvec4(0, 0, 0, 1);
        return  glm::length((glm::dvec4(x, y, z, 1) - cInWd));
    }

    Pt3D XformState::_constrainCoord(const Pt3D &coord) const 
    {
        auto retCoord = coord;

        auto hRadius = mViewWidth   / 2;
        auto left = coord.x - hRadius;
        auto right= coord.x + hRadius;

        auto vRadius = mViewHeight / 2;
        auto top    = coord.y + vRadius;
        auto bottom = coord.y - vRadius;

        if (left < mBpxWorldBound.mMinX) {
            retCoord.x = (mBpxWorldBound.mMinX + hRadius);
        }
        else if (right > mBpxWorldBound.mMaxX) {
            retCoord.x = (mBpxWorldBound.mMaxX - hRadius);
        }

        if (top > mBpxWorldBound.mMaxY) {
            retCoord.y = (mBpxWorldBound.mMaxY - vRadius);
        }
        else if (bottom < mBpxWorldBound.mMinY) {
            retCoord.y = (mBpxWorldBound.mMinY + vRadius);
        }

        return retCoord;
    }     

    double XformState::_rotateXAtCurZoom()const 
    {
        double mx = _rotateXMax_atCurZoom(); 
        return CM_CLAMP(mRotX, 0, mx);
    }     
}
