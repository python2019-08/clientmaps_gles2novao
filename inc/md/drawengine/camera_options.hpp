#pragma once
#include <functional>
#include <md/util/geo.hpp>
#include <md/util/chrono.hpp> 
  
namespace clientmap {
 
/** Various options for describing the viewpoint of a map. All fields are
    optional.
    Anchor and center points are mutually exclusive, with preference for the
    center point when both are set.
    */
struct CameraOptions 
{
    CameraOptions& withCenter(const  LonLat& o)
    { 
        center = o; 
        m_center_set = true;
        return *this; 
    }
    CameraOptions& withViewCenterOffset(const  ScreenPt& o)
    { 
        m_viewCenterOffset = o;
        m_viewCenterOffset_set = true;
        return *this; 
    } 
    CameraOptions& withZoom(const double  o) 
    { 
        zoom = o; 
        m_zoom_set = true;
        return *this; 
    }
    CameraOptions& withBearing(const double  o) 
    { 
        bearing = o; 
        m_bearing_set = true;
        return *this; 
    }
    CameraOptions& withPitch(const double  o) 
    { 
        pitch = o; 
        m_pitch_set = true;
        return *this; 
    }

    /** Coordinate at the center of the map. */
    LonLat  center;
    bool    m_center_set = false;

    /** Padding around the interior of the view that affects the frame of
        reference for `center`. */
    ScreenPt  m_viewCenterOffset;
    bool      m_viewCenterOffset_set = false;
  
    /** Zero-based zoom level. Constrained to the minimum and maximum zoom
        levels. */
    double zoom  = 0;
    bool   m_zoom_set = false;

    /** Bearing, measured in degrees from true north. Wrapped to [0, 360). */
    double  bearing = 0;
    bool    m_bearing_set = false;

    /** Pitch toward the horizon measured in degrees , with 0 deg resulting in a
        two-dimensional map. */
    double pitch = 0;
    bool   m_pitch_set = false;

    friend /*constexpr*/ bool
        operator==(const CameraOptions& a,
            const CameraOptions& b)
    {
        return a.center == b.center 
            && a.m_center_set == b.m_center_set 
            && a.m_viewCenterOffset == b.m_viewCenterOffset
            && a.m_viewCenterOffset_set == b.m_viewCenterOffset_set
            && a.zoom == b.zoom
            && a.m_zoom_set == b.m_zoom_set
            && a.bearing == b.bearing
            && a.m_bearing_set == b.m_bearing_set
            && a.pitch == b.pitch
            && a.m_pitch_set == b.m_pitch_set ;
    }
    friend /*constexpr*/ bool
        operator!=(const CameraOptions& a,
            const CameraOptions& b)
    {
        return !(a == b);
    }
};




/** Various options for describing a transition between viewpoints with
    animation. All fields are optional; the default values depend on how this
    struct is used. */
struct AnimationOptions 
{
    /** Time to animate to the viewpoint defined herein. */
     double  duration =  0.0;

    /** Average velocity of a flyTo() transition, measured in screenfuls per
        second, assuming a linear timing curve.

        A <i>screenful</i> is the visible span in pixels. It does not correspond
        to a fixed physical distance but rather varies by zoom level. */
    double velocity = -1;

    /** Zero-based zoom level at the peak of the flyTo() transition’s flight
        path. */
    double minZoom = -1;
 
    /** A function that is called on each frame of the transition, just before a
        screen update, except on the last frame. The first parameter indicates
        the elapsed time as a percentage of the duration. */
    std::function<void(double)> transitionFrameFn ;

    /** A function that is called once on the last frame of the transition, just
        before the corresponding screen update. */
    std::function<void()> transitionFinishFn;

    /** Creates an animation with no options specified. */
    AnimationOptions() = default;

    /** Creates an animation with the specified duration. */
    AnimationOptions( double d)
        : duration(d) 
	{}
};

} // namespace md
