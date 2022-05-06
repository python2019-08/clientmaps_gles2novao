#pragma once

#include <cstdint>
#include <cmath>
#include <functional>

#include <md/drawengine/camera_options.hpp>
//#include <md/map/projection_mode.hpp>
#include <md/drawengine/transform_state.hpp>
#include <md/util/chrono.hpp>
#include <md/util/geo.hpp>
#include <md/util/cpp_misc.hpp>
 
namespace clientmap {
// Transform类主要用于处理手势及手势动画
class Transform : private  NonCopyable {
public:
    Transform(TransformState &state_) 
    :  mState(state_) 
    {

    }
 
    // Map view
    void resize(int32_t  _width, int32_t _height);
    LonLat getMapCenter(LonLat::WrapMode = LonLat::Wrapped) const;

    // Bounds
    //void setLatLngBounds( LatLngBounds);
    void setZoomRange(const double aMin, const double aMax);

    // Camera
    /** Returns the current camera options. */
    CameraOptions getCameraOptions() const;

    /** Instantaneously, synchronously applies the given camera options. */
    void jumpTo(const CameraOptions&);
    /** Asynchronously transitions all specified camera options linearly along
        an optional time curve. However, center coordinate is not transitioned
        linearly as, instead, ground speed is kept linear.*/
    void easeTo(const CameraOptions&, const AnimationOptions& = {});
    /** Asynchronously zooms out, pans, and zooms back into the given camera
        along a great circle, as though the viewer is riding a supersonic
        jetcopter.
        Parameter linearZoomInterpolation: when true, there is no additional
        zooming out as zoom is linearly interpolated from current to given
        camera zoom. This is used for easeTo.*/
    void flyTo(const CameraOptions&, 
               const AnimationOptions& = {}, 
               bool linearZoomInterpolation = false
               );

    // Position

    /** Pans the map by the given amount.
        @param offset The distance to pan the map by, measured in pixels from
            top to bottom and from left to right. */
    void moveBy(const  ScreenPt& offset,
                const AnimationOptions& = {});

    // Bearing
    void rotateBy(const  ScreenPt& first,
                  const  ScreenPt& second,
                  const AnimationOptions& = {}
                  );
    void rotateBy_deg(const  double aDegDiff, const AnimationOptions& animation);
    // Transitions
    bool inTransition() const;
    void updateTransitions(const  TimePoint& now);
     TimePoint getTransitionStart() const {
        return m_transitionStart; 
    }
     Duration getTransitionDuration() const {
        return m_transitionDuration; 
    }
    void cancelTransitions();

    //// State
    bool isChanging() const;
    bool isRotating() const;
    void setRotatingInProgress(bool val) {
        rotating = val;
    }
    bool isScaling() const;
    void setScalingInProgress(bool val) {
        scaling = val;
    }
    bool isPanning() const;
    void setPanningInProgress(bool val) {
        panning = val;
    }
    bool isGestureInProgress() const;
    void setGestureInProgress(bool val);


    // Conversion and projection
    ScreenPt 
    latLngToScreenPt(const  LonLat&) const;
    LonLat 
    screenPtToLatLng(const  ScreenPt&,
                       LonLat::WrapMode =  LonLat::Wrapped
                       ) const;
private:
    void startTransition(const CameraOptions&,
                         const AnimationOptions&,
                         const std::function<void(double)>&,
                         const  Duration&);

    // We don't want to show horizon: limit max pitch based on edge insets.
    double getMaxPitchForViewCenterOffset(const  ScreenPt& aViewCtOffset) const;

     TimePoint m_transitionStart;
     Duration  m_transitionDuration;
    std::function<bool(const  TimePoint)> m_transitionFrameFn;
    std::function<void()>                        m_transitionFinishFn;
private:
    //MapObserver& observer;
    TransformState& mState;

    // animation state
    //bool pitching = false;//in platform\glfw\glfw_view.hpp
    bool rotating = false;
    bool scaling = false;
    bool panning = false;
    bool gestureInProgress = false;
};

} // namespace clientmap
