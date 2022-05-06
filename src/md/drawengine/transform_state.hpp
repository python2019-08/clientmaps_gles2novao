 
#ifndef MDMAP_ENGINE_TRANSFORM_STATE_H
#define MDMAP_ENGINE_TRANSFORM_STATE_H

#include <cstdint>
#include <array>
#include <limits>
#include <set>
#include "glm/glm.hpp"


#include <md/util/geo.hpp> 
#include <md/drawengine/camera_options.hpp>
#include <md/util/constants.hpp>
#include <md/util/tile_id.hpp> 

#include "md/drawengine/view_camera.h"
 
namespace clientmap { 
  
    class XformState {
        friend class Transform;

    public:
        XformState( );
        XformState(const XformState& other);
        XformState& operator=(const XformState& other);

        ~XformState();
         
        // View 
        //------------------------
        void setDefaultFboId(uint32_t aFboId);

        void setPixelRatio(double aPixelRatio);

        void setViewSize(int width, int height);
        int viewWidth() const;
        int viewHeight() const; 

        ScreenPt  getViewCenter() const;

        CameraOptions getCameraOptions();

        // LookAt
        //------------------------
        Pt3D getMapCenter() const;
        void   setMapCenter(double x, double y);

        // Zoom
        //------------------------ 
        void setZoomRange(const double aMin, const  double aMax);
        double getMaxZoom() const;
        double getMinZoom() const;
        

        void   setZoom(double zoom); 
        int    getZoomi() const;// zoom level in integer-type 
        double getZoom() const;// zoom level in double-type 

        // Rotate
        //------------------------
        void   setRotateX(double angle);
        double getRotateX() const;

        void   setRotateZ(double angle);
        double getRotateZ() const;

        //--Matrix------------------------
        glm::mat4 getVMat() const;
        glm::mat4 getPMat() const;
        glm::mat4 getVPMat() const;

        glm::mat4 getVMatHUD() const;
        glm::mat4 getPMatHUD() const;
        // Pt3D Transform
        //------------------------
        ScreenPt screenFromBpx(Pt3D coor) const;
        Pt3D bpxFromScreen(double x, double y, bool& bIntersect) const;
        
        glm::dvec3 bpx2RelativeBpx(const Pt3D& coor ) const;
        void relativeBpx2Bpx(Pt3D& coor) const;
        void bpx2RelativeBpx(Box2D& aBound ) const;

        template <typename T>
        T bpx2RelativeBpxX(const T &coord) const 
        {
            return T( (coord.x - mMapCntr.x), 
                      (coord.y - mMapCntr.y), 
                      (coord.z - mMapCntr.z));
        }
        //--------Screen-Visible
        void setEdgePaddingRatio(float top, float bottom,
            float left, float right);
        GeoBox   getScrBoundVisible() const;
        Pt3D roundToBoundVisible(double x, double y);

        //---------- SkyBox 
        double getSkyBoxHeightRatio();
        double getMinSkyHRatio(double   aCurRotXDeg);
        int    getSkyHeight();

        //---------- GroundBound 
        Quad2D getGroundBound()const ;
        Quad2D getGroundBoundVisible();

        bool intersect(TileID tile);
        bool intersect(const Box2D& bound);
        bool intersect(Pt3D coord);

        bool isRect() const { 
            return ((int)mRotZ % 90 == 0) && (mRotX == 0); 
        }
          

        // Tiles
        //------------------------
        int getCoveredTiles(std::vector<TileID>& aTiles);

        //------------------------  
        double getScaleFactor(Pt3D pos);
        double getScreenCenterScaleFactor(Pt3D pos); 

    private:
        // 相机到观察点(屏幕中心)距离:EyeToCenterDistance
        double getFocalLen(double aZoom) const;
        double getZoomFromFocalLen(double aDistance) const;// not used
  
        void _update();
        void _updateGroundBound();
         
        void     _makeWorldBoundInBpx();
        Pt3D _constrainCoord(const Pt3D &coord) const;


        double _distToPointByScrCoord(int sx, int sy);
        double _distToPointByGeoCoord(double x, double y, double z = 0);

        double _rotateXAtCurZoom() const;
        double _rotateXMax_atCurZoom() const;
        double _rotateXMax_global() const;

        double _fovYAtZoom(double aZoom) const;

    private:
        uint32_t mDefaultFboId = 0;
        double mPixelRatio = 1.0;
        // attributes determine matrix
        Pt3D mMapCntr; // 地图中心点bpx地理坐标 -- 会显示在屏幕中心
        double mZoom; // zoomLevel ，scale = std::pow(2,mZoom);       
        double mRotX;
        double mRotZ;

        int mViewWidth = 600; // view宽
        int mViewHeight= 800;// view高
        double m_aspect = 600/800;// = width / height.
        //  
        double mZoomMax;
        double mZoomMin;
        glm::vec4 _visEdgeInset;

        //  read only
        int mCurZoomi = 15; 
    private:
        glm::dmat4 m_VMat; // 引擎到相机
        glm::dmat4 m_invVMat; // 相机到引擎
        glm::dmat4 m_PMat;

        Quad2D   mOutGroundBound;
        Box2D    mBpxWorldBound;
    };
 
    template <>
    Pt2D XformState::bpx2RelativeBpxX<Pt2D>(const Pt2D &coord) const;
 
#ifdef  USE_ViewCamera_AS_TransformState
    using TransformState = ViewCamera;
#else
    using TransformState = XformState;
#endif
 }// namespace
#endif //#ifndef MDMAP_ENGINE_TRANSFORM_STATE_H
 
