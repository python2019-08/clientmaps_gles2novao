#pragma once
#include <functional>
#include <memory>
#include "glm/mat4x4.hpp"
#include "glm/vec4.hpp"
#include "glm/vec3.hpp"

#include "md/util/merc_prj.hpp"
#include "md/util/geo.hpp"
#include "md/util/tile_id.hpp" 

#include "md/drawengine/camera_options.hpp"
#include "md/drawengine/viewcam_Constraint.h"

namespace clientmap {

enum class CameraType : uint8_t {
    perspective = 0,
    isometric,
    flat,
};


// ViewCamera
// 1. Set/get camera options
// 2. Update the matrices 
// 3. Coordinate transform  
// 4. Determines which tiles are visible in the current view

class ViewCamera {
    friend class Transform;
public:
    ViewCamera( );

	//ViewCamera(const ViewCamera& other);
	//ViewCamera& operator=(const ViewCamera& other);

	~ViewCamera();
    //-------------------------------------
    void setDefaultFboId(uint32_t aFboId);
    uint32_t defaultFboId();
    
    // Set/Get the ratio of hardware pixels to logical pixels. Default is 1.0.
    void   setPixelRatio(double aPixelsPerPoint,bool aIsToUpdate=true);
    double pixelRatio() const;
    float  pixelsPerMeter() const;//+

    // Set the size of the viewable area in pixel.
    void  setViewSize(int _width, int _height, bool aIsToUpdate = true);
    float viewWidth() const;
    float viewHeight() const;
    ScreenPt  getViewCenter() const;

    //----------
    void setCameraOptions(const CameraOptions& aCamera, bool aIsToUpdate = true);
    CameraOptions getCameraOptions();

    
    // Set the position of the view within the world in BPX.
    void setMapCenter(double _x, double _y, bool aIsUpdate = true);
    void setMapCenter(const glm::dvec2 pos, bool aIsUpdate = true);
    // Get the position of the view in projection units (z is the 
    // effective 'height' ).
    const glm::dvec3& getMapCenterRef();
    Pt3D getMapCenter() const; // bpx
    Pt2D getMapCenterPx() const;//px

    // Set the position of the view within the world in LonLat.
    void setMapCenterLonLat(const LonLat& aCt, bool aIsUpdate = true);
    // Get the coordinates of the point at the center of the view.
    LonLat getMapCenterLonLat() const;

    //-------------//+
    void setVanishingPoint(float x, float y, bool aIsUpdate = true);//+
    auto vanishingPoint() const;//+

    //-------------
    // Set the minimum zoom level and the maximum zoom level.
    // Clamped to >=0 and <= 22.0.
    void setZoomRange(const double aMin, const  double aMax, 
        bool aIsUpdate = true);
    // Get the minimum zoom level.
    double getMinZoom() const;
    // Get the maximum zoom level.
    double getMaxZoom() const;

    // Set the zoom level of the view.
    void setZoom(double aZ, bool aIsUpdate = true);
    // Get the current zoom.  zoom level in double-type  
    double getZoom() const;
    // Get the current zoom truncated to an integer. 
    // This is the zoom used to determine visible tiles.
    int getZoomi() const;

    //------------- 
    // Set the pitch angle of the view in degrees. Default is 0.
    void   setRotateX(double aDegrees, bool aIsUpdate = true);
    // Get the current pitch angle in degrees.
    double getRotateX() const;


    // Set the roll angle of the view in radians. Default is 0.
    void   setRotateZ(double aDeg, bool aIsUpdate = true);
    // Get the current roll angle in radians.
    double getRotateZ() const;     
      
    //-------------
    // Whether to constrain visible area to the projected bounds of the world.
    void setConstrainToWorldBounds(bool constrainToWorldBounds);// +

    //-------------Matrix
    //
    glm::mat4 getMMatS() const;
    // Get the transformation from global space into view (camera) space;
    // Due to precision limits, this does not contain the translation of 
    // the view from the global origin (you must apply that separately).
    glm::mat4  getVMat() const;

    // Get the transformation from view space into screen space.
    glm::mat4 getPMat() const;

    // Get the combined view and projection transformation.
    glm::mat4 getVPMat() const;
    //
    glm::mat4 getMVPMat() const;

    // Get the normal matrix; transforms surface normals from model space to camera space.
    glm::mat3 getNormalMat() const;
    glm::mat3 getInvNormalMat() const;

    glm::mat4 getVMatHUD() const;
    glm::mat4 getPMatHUD() const;
    //---------------
    // Get the eye position in world space.
    glm::vec3 getEye() const { return m_eye; }

    // Get the window coordinates [0,1], lower left corner of the window is (0, 0).
    glm::vec2 normalizedWindowCoordinates(float _x, float _y) const; //-    

 
    //--------------coordinate transform
    // For a position on the map in projected meters, this returns the 
    // displacement vector *from* the view *to* that position, accounting 
    // for wrapping around the 180th meridian to get the smallest magnitude
    // displacement.
    void  bpx2RelativeBpx(Box2D& aBound) const;
    glm::dvec2 bpx2RelativeBpx(double aAbsX, double aAbsY) const;
    glm::dvec3 bpx2RelativeBpx(const Pt3D& coor) const;
    glm::dvec3 relativeBpx2Bpx(const Pt3D& coor) const;
    
    template <typename T>
    T bpx2RelativeBpxX(const T &coord) const
    {
        return T((coord.x - m_pos.x),
            (coord.y - m_pos.y),
            (coord.z));
    }

    //--// bpx Pt -> groundPx pt  -> Screen pt
    // GroundPx is pixel coordinate at current zoom,
    // and its coordinate ORIGIN is at MAP-CENTER.
    glm::dvec3 bpx2GroundPx(const Pt3D& aBPt);
    glm::vec2  groundPx2Screen(double aXGpx, double aYGpx,
        bool& aOutsideViewport, bool  aClipToViewport);
    void groundPx2Screen_2(double aGx, double aGy, double aGz,
        double& aSx, double& aSy)const;
    // screenToGroundPlane
    // Calculate the position on the ground plane (z = 0) under the 
    // given screen space coordinates,replacing the input coordinates
    // with world-space coordinates.Returns the un-normalized distance 
    // 'into the screen' to the ground plane (if < 0, intersection 
    // is behind the screen). 
    // (aGpxX,aGpxY) is GroundPx coordinate.
    double screen2GroundPx(double _screenX, double _screenY,
        double& aGpxX, double& aGpxY) const;
    void screen2GroundPx_2(double aSx, double aSy,
        double& aGx, double& aGy) const;//--glm::unUroject&&rayCast
    Pt3D groundPx2Bpx(const glm::dvec3& aBPt);
    //------
    ScreenPt screenFromBpx(const Pt3D& aBpx);
    Pt3D     bpxFromScreen(double x, double y, bool& aIsIntersection);
     
    //------
    // Get the screen position from a latitude/longitude.
    glm::vec2 lngLatToScreenPos(double lng, double lat, 
                                    bool& outsideViewport, 
                                    bool  clipToViewport = false);

    LonLat screenPosToLngLat(float x, float y, bool& intersection);
    //----
    void setEdgePaddingRatio(float top, float bottom,
        float left, float right);    
    GeoBox   getScrBoundVisible() const;
    Pt3D roundToBoundVisible(double x, double y);//??? 

    Quad2D   getGroundBound()const;
    //---------- SkyBox 
    double getSkyBoxHeightRatio();
    double getMinSkyHRatio(double   aCurRotXDeg,double aCurFoy);
    int    getSkyHeight();

    //--------------coordinate transform
    // Get the set of all tiles visible at the current position and zoom.
    void getVisibleTiles(std::vector<TileID>& aTiles) const;      
        
    int getCoveredTiles(std::vector<TileID>& aTiles);
protected: 
    int getCoveredTiles_01(std::vector<TileID>& aTiles);
    int getCoveredTiles_02(std::vector<TileID>& aTiles); 
    //------------- 
    // Update the view and projection matrices if properties have changed.
    void _update();
    void _updateMatrices(); 
    void _updateGroundBound();

    void applyWorldBounds();
    //----------------------------------------------------   
    void _skyCfgAtZoom(double aZoom, double* aRotXMax, 
        double* aSkyRatioMax, double* aFovy) const;
    double _rotateXMax_global() const;
     
    // Get the current pitch angle in degrees. 
    double _rotateXAtCurZoom() const;
    
    // Get the vertical field-of-view angle, in degrees.
    double _fovYAtCurZoom() const;

    //-------------  
    // Get the focal length that corresponds to the current vertical
    // field-of-view angle.
    double _focalLength() const;
    static float focalLengthToFieldOfView(float length);
    static float fieldOfViewToFocalLength(float radians);

    //------------  
    /// Computes the clip coordinates from position in 
    /// world space and a model view matrix 
    static glm::vec4 worldToClipSpace(const glm::mat4& mvp, const glm::vec4& worldPos);
    /// Return true if the given clip space point is behind 
    /// the camera plane. 
    static bool clipSpaceIsBehindCamera(const glm::vec4& clip);
    /// Return the given clip space point converted to Normalized Device Coordinates.
    static glm::vec3 clipSpaceToNdc(const glm::vec4& clip);
    /// Return the given point in Normalized Device Coordinates converted to screen space.
    static glm::vec2 ndcToScreenSpace(const glm::vec3& ndc, const glm::vec2& screenSize);
    ///
    glm::vec2 _bpxToScreenPos(double aXBpx, double aYBpx,
            bool& aOutsideViewport,
            bool  aClipToViewport = false);
    //----
    // Find the intersection of ray  and plane
    bool rayCast( const glm::dvec3&  aRayStart, // Ray starting point
        const glm::dvec3&  aRayDir, // Ray direction vector,射线方向向量；
        const glm::dvec3&  aPlanePt, // A point on a plane,平面上的某个点；
        const glm::dvec3&  aPlaneNormal, // Plane normal vector,平面法向量；
        glm::dvec3& aRayCastHitPt // 
    )const;
    //----------------------------------------------------   
    ViewConstraint m_constraint;

    uint32_t m_defaultFboId = 0;
    double   m_pixelRatio = 1.0f;// pixelsPerPoint
    int m_vpWidth  = 800;//  viewport width
    int m_vpHeight = 600; // viewport width 
    
    double m_roll = 0.f;
    double m_pitch = 0.f;// 是视线的倾斜角，不是ground plane的倾斜角

    double m_zoom = 14.0;//  ZoomLevel ，scale = std::pow(2, m_zoom);   
    double m_minZoom = 0.0;
    double m_maxZoom = 22.0;

    //float m_width; // world space（单位:米）中的视口宽度，即视口对应的世界坐标系的宽度
    //float m_height;
    bool m_constrainToWorldBounds = false;
    CameraType m_type = CameraType::perspective;

    glm::dvec3 m_pos; // map center; m_pos.xy in bpx
    glm::dvec3 m_pos_curZoomPx;  // not used
    glm::dvec3 m_eye; //the eye position in world space.
    glm::vec2 m_obliqueAxis;
    glm::dvec2 m_vanishingPoint;

    glm::vec4 _visEdgeInset;    
    //--------------
	Quad2D    m_coveredGroundBound;// covered Ground Bound

    glm::dmat4 m_mMatT;//-- TMat;added +
    glm::dmat4 m_mMatS;//-- SMat;added +
    glm::dmat4 m_invMMatS;//-- SMat;added +
    glm::dmat4 m_vMat; 
    glm::dmat4 m_pMat; 
    glm::dmat4 m_vpMat;
    glm::dmat4 m_invVpMat;
    glm::dmat4 m_mvpMat;
    glm::dmat4 m_invMvpMat;

    glm::dmat4 m_mvpMatAll;
    glm::dmat4 m_invMvpMatAll;


    glm::dmat3 m_normalMatrix;//-- 
    glm::dmat3 m_invNormalMatrix;// inverse mormal matrix

    // Head Up Display,平视显示器,简称HUD。
    // In UE4 blueprint, it refers to the two-dimensional object drawn
    // on the screen.在UE4蓝图中,用于在屏幕上面绘制二维UI物体。 
    glm::dmat4 m_vMatHUD;//-- Ortho Viewport matrix
    glm::dmat4 m_pMatHUD;
};

template <> 
Pt2D ViewCamera::bpx2RelativeBpxX<Pt2D>(const Pt2D& coord) const;

}//clientmap
