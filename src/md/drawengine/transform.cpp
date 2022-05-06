#include <cmath>
#include <cstdio>
#include <utility>

#include <md/util/constants.hpp>
#include <md/util/unitbezier.hpp>
#include <md/util/geo.hpp> 
#include <md/util/interpolate.hpp>
#include <md/util/chrono.hpp>

#include <md/util/merc_prj.hpp>
#include <md/util/math_tools.hpp>
#include <md/util/logtool.hpp>
#include <md/util/tile_id.hpp>

#include <md/drawengine/camera_options.hpp>

#include <md/drawengine/transform.hpp>
#define NO_ANIMATION_GESTURE  1
namespace clientmap {

/** Converts the given angle (in radians) to be numerically 
 *  close to the anchor angle, allowing it to be interpolated
 *  properly without sudden jumps. */
static double _normalizeAngle(double angle, double anchorAngle)
{
    if (std::isnan(angle) || std::isnan(anchorAngle)) {
        return 0;
    }

    angle =  cm_wrap(angle, -M_PI, M_PI);
    if (angle == -M_PI) 
        angle = M_PI;
    double diff = std::abs(angle - anchorAngle);
    if (std::abs(angle -  M2PI - anchorAngle) < diff)
    {
        angle -=  M2PI;
    }
    
    if (std::abs(angle +  M2PI - anchorAngle) < diff)
    {
        angle +=  M2PI;
    }

    return angle;
}

 

#pragma mark - Map View
 

void Transform::resize(int32_t  _wpWidth, int32_t _vpHeight)
{ 
    mState.setViewSize(_wpWidth, _vpHeight);// 设置新值
}

#pragma mark - Camera

CameraOptions 
Transform::getCameraOptions() const 
{
    return mState.getCameraOptions();
}

/**
 * Change any combination of center, zoom, bearing, and pitch, without
 * a transition. The map will retain the current values for any options
 * not included in `options`.
 */
void Transform::jumpTo(const CameraOptions& aCamera)
{
    if (aCamera.m_center_set)
    {
        Pt2D bpxPt =MercPrj::pxFromLngLat(aCamera.center.wrapped(),
                                            BASE_ZOOM);
        mState.setMapCenter(bpxPt.x, bpxPt.y);
    }

    if (aCamera.m_zoom_set)
    {
        mState.setZoom(aCamera.zoom);
    }

    if (aCamera.m_bearing_set)
    {
        mState.setRotateZ( aCamera.bearing);
    }

    if (aCamera.m_pitch_set)
    {
        mState.setRotateX(aCamera.pitch);
    }
}

/**
 * Change any combination of center, zoom, bearing, pitch and edgeInsets, with a
 * smooth animation between old and new values. The map will retain the current
 * values for any options not included in `options`.
 */
void Transform::easeTo(const CameraOptions&    camera, 
                       const AnimationOptions& animation) 
{
    // Duration duration = animation.duration; 
    //if ( mState.getLatLngBounds() ==  LatLngBounds() &&
    //    !isGestureInProgress() && 
    //    duration !=  Duration::zero()  ) 
    //{
    //    // reuse flyTo, without exaggerated animation, to achieve constant ground speed.
    //    return flyTo(camera, animation, true);
    //}

    ////---- 计算easeTo动画的目的参数
    // ScreenPt  viewCtOffset =  camera.m_viewCenterOffset_set
    //                        ? camera.m_viewCenterOffset
    //                        : mState.getCenterOffset();
  

    // LonLat startLatLng = getMapCenter( LonLat::Unwrapped);
    //const  LonLat& unwrappedLatLng = camera.m_center_set
    //           ? camera.center   :  startLatLng ;
    //const  LonLat& latLng = mState.getLatLngBounds() !=  LatLngBounds()
    //           ? unwrappedLatLng   :   unwrappedLatLng.wrapped(); // ???

    //double zoom = camera.m_zoom_set ? camera.zoom : mState.getZoom();
    //double bearing = camera.m_bearing_set ? -camera.bearing *  DEG2RAD : mState.getBearing();
    //double pitch = camera.m_pitch_set ? camera.pitch *  DEG2RAD : mState.getPitch();

    //if (std::isnan(zoom) || std::isnan(bearing) || std::isnan(pitch)) 
    //{
    //    if (animation.transitionFinishFn) {
    //        animation.transitionFinishFn();
    //    }
    //    return;
    //}

    //if (mState.getLatLngBounds() ==  LatLngBounds())
    //{
    //    if (isGestureInProgress()) 
    //    {
    //        // If gesture in progress, we transfer the wrap rounds from the 
    //        // end longitude into start, so the "scroll effect" of rounding 
    //        // the world is the same while assuring the end longitude remains 
    //        // wrapped.
    //        const double wrap = unwrappedLatLng.longitude() - latLng.longitude();
    //        startLatLng =  LonLat(startLatLng.latitude(), 
    //                                     startLatLng.longitude() - wrap);
    //    } else {
    //        // Find the shortest path otherwise.
    //        startLatLng.unwrapForShortestPath(latLng);
    //    }
    //}

    //const  Point<double> startPoint =  Projection::project(startLatLng, mState.getScale());
    //const  Point<double> endPoint =  Projection::project(latLng, mState.getScale());

    //// Constrain camera options.
    //zoom  = cm_clamp(zoom,  mState.getMinZoom(),  mState.getMaxZoom());
    //pitch = cm_clamp(pitch, mState.getMinPitch(), mState.getMaxPitch());

    //// Minimize rotation by taking the shorter path around the circle.
    //bearing = _normalizeAngle(bearing, mState.getBearing());
    //mState.setBearing(_normalizeAngle(mState.getBearing(), bearing));
    ////---- 获取easeTo的开始参数
    //const double startZoom    = mState.getZoom();
    //const double startBearing = mState.getBearing();
    //const double startPitch   = mState.getPitch();
    //setPanningInProgress(unwrappedLatLng != startLatLng);
    //setScalingInProgress(zoom != startZoom);
    //setRotatingInProgress(bearing != startBearing);

    //const  ScreenPt startViewCtOffset = mState.getCenterOffset();

    //startTransition(
    //    camera,
    //    animation,
    //    [=](double t) {
    //         Point<double> framePoint =  interpolate(startPoint, endPoint, t);
    //         LonLat frameLatLng =  Projection::unproject(framePoint,
    //                                                mState.zoomScale(startZoom) );
    //        double frameZoom =  interpolate(startZoom, zoom, t);
    //        mState.setLatLngZoom(frameLatLng, frameZoom);

    //        if (bearing != startBearing) 
    //        {
    //            double frameBearing =  interpolate(startBearing, bearing, t);
    //            frameBearing =  wrap(frameBearing, -M_PI, M_PI);

    //            mState.setBearing(frameBearing);
    //        }

    //        if (viewCtOffset != startViewCtOffset) 
    //        {
    //            // Interpolate edge insets 
    //             ScreenPt newViewCtOffset{ 
    //                   interpolate(startViewCtOffset.x, viewCtOffset.x, t),
    //                   interpolate(startViewCtOffset.y, viewCtOffset.y, t)
    //            };
    //            mState.setCenterOffset(newViewCtOffset);
    //        }

    //        double maxPitch = getMaxPitchForViewCenterOffset(mState.getCenterOffset());
    //        if (pitch != startPitch || 
    //            maxPitch < startPitch) 
    //        {
    //            mState.setPitch(std::min(maxPitch,  interpolate(startPitch, pitch, t)));
    //        }
    //    },
    //    duration);
}

/** This method implements an “optimal path” animation, as detailed in:

    Van Wijk, Jarke J.; Nuij, Wim A. A. “Smooth and efficient zooming and
        panning.” INFOVIS ’03. pp. 15–22.
        <https://www.win.tue.nl/~vanwijk/zoompan.pdf#page=5>.

    Where applicable, local variable documentation begins with the associated
    variable or function in van Wijk (2003). */
void Transform::flyTo(const CameraOptions&    camera, 
                      const AnimationOptions& animation, 
                      bool linearZoomInterpolation) 
{
    //const  ScreenPt& ctOffset = camera.m_viewCenterOffset_set
    //    ? camera.m_viewCenterOffset : mState.getCenterOffset();

    // LonLat mapCt = getMapCenter( LonLat::Unwrapped);
    //const  LonLat& latLng = camera.m_center_set
    //    ? camera.center.wrapped() : mapCt.wrapped();

    //double zoom = camera.m_zoom_set ? camera.zoom : mState.getZoom();
    //double bearing = camera.m_bearing_set ? -camera.bearing *  DEG2RAD : mState.getBearing();
    //double pitch = camera.m_pitch_set ? camera.pitch *  DEG2RAD : mState.getPitch();

    //if (std::isnan(zoom) || 
    //    std::isnan(bearing) || 
    //    std::isnan(pitch) || 
    //    mState.isEmptyViewport()) 
    //{
    //    if (animation.transitionFinishFn) {
    //        animation.transitionFinishFn();
    //    }
    //    return;
    //}

    //// Determine endpoints.
    // LonLat startLatLng = getMapCenter( LonLat::Unwrapped).wrapped();
    //startLatLng.unwrapForShortestPath(latLng);

    //const  Point<double> startPoint =  Projection::project(startLatLng, mState.getScale());
    //const  Point<double> endPoint =  Projection::project(latLng, mState.getScale());

    //// Constrain camera options.
    //zoom = cm_clamp(zoom, mState.getMinZoom(), mState.getMaxZoom());
    //pitch =  cm_clamp(pitch, mState.getMinPitch(), mState.getMaxPitch());

    //// Minimize rotation by taking the shorter path around the circle.
    //bearing = _normalizeAngle(bearing, mState.getBearing());
    //mState.setBearing(_normalizeAngle(mState.getBearing(), bearing));
    //const double startZoom = mState.scaleZoom(mState.getScale());
    //const double startBearing = mState.getBearing();
    //const double startPitch = mState.getPitch();

    ///// w₀: Initial visible span, measured in pixels at the initial scale.
    ///// Known henceforth as a <i>screenful</i>.
    ////double w0 = std::max(mState.getSize().width - padding.left() - padding.right(),
    ////                     mState.getSize().height - padding.top() - padding.bottom());
    int32_t  vpW = mState.viewWidth();
    int32_t  vpH = mState.viewHeight();
    //double w0 = std::max(vpW, vpH);
    ///// w₁: Final visible span, measured in pixels with respect to the initial
    ///// scale.
    //double w1 = w0 / mState.zoomScale(zoom - startZoom);
    ///// Length of the flight path as projected onto the ground plane, measured
    ///// in pixels from the world image origin at the initial scale.
    //double u1 = ::hypot((endPoint - startPoint).x, (endPoint - startPoint).y);

    ///** ρ: The relative amount of zooming that takes place along the flight
    //    path. A high value maximizes zooming for an exaggerated animation, while
    //    a low value minimizes zooming for something closer to easeTo().

    //    1.42 is the average value selected by participants in the user study in
    //    van Wijk (2003). A value of 6<sup>¼</sup> would be equivalent to the
    //    root mean squared average velocity, V<sub>RMS</sub>. A value of 1
    //    produces a circular motion. */
    //double rho = 1.42;
    //if (animation.minZoom >=0 || linearZoomInterpolation) 
    //{
    //    double anim_minZoom = (animation.minZoom >= 0) ? animation.minZoom : startZoom;
    //    double minZoom =  min(anim_minZoom, startZoom, zoom);
    //    minZoom =  cm_clamp(minZoom, mState.getMinZoom(), mState.getMaxZoom());
    //    /// w<sub>m</sub>: Maximum visible span, measured in pixels with respect
    //    /// to the initial scale.
    //    double wMax = w0 / mState.zoomScale(minZoom - startZoom);
    //    rho = u1 != 0 ? std::sqrt(wMax / u1 * 2) : 1.0;
    //}
    ///// ρ²
    //double rho2 = rho * rho;

    ///** rᵢ: Returns the zoom-out factor at one end of the animation.

    //    @param i 0 for the ascent or 1 for the descent. */
    //auto r = [=](double i) 
    //{
    //    /// bᵢ
    //    double b = (w1 * w1 - w0 * w0 + (i ? -1 : 1) * rho2 * rho2 * u1 * u1) 
    //              / (2 * (i ? w1 : w0) * rho2 * u1);
    //    return std::log(std::sqrt(b * b + 1) - b);
    //};

    ///// r₀: Zoom-out factor during ascent.
    //double r0 = u1 != 0 ? r(0) : INFINITY; // Silence division by 0 on sanitize bot.
    //double r1 = u1 != 0 ? r(1) : INFINITY;

    //// When u₀ = u₁, the optimal path doesn’t require both ascent and descent.
    //bool isClose = std::abs(u1) < 0.000001 || !std::isfinite(r0) || !std::isfinite(r1);

    ///** w(s): Returns the visible span on the ground, measured in pixels with
    //    respect to the initial scale.

    //    Assumes an angular field of view of 2 arctan ½ ≈ 53°. */
    //auto w = [=](double s) {
    //    return (isClose ? std::exp((w1 < w0 ? -1 : 1) * rho * s)
    //            : (std::cosh(r0) / std::cosh(r0 + rho * s)));
    //};
    ///// u(s): Returns the distance along the flight path as projected onto the
    ///// ground plane, measured in pixels from the world image origin at the
    ///// initial scale.
    //auto u = [=](double s) {
    //    return (isClose ? 0.
    //            : (w0 * (std::cosh(r0) * std::tanh(r0 + rho * s) - std::sinh(r0)) / rho2 / u1));
    //};
    ///// S: Total length of the flight path, measured in ρ-screenfuls.
    //double S = (isClose ? (std::abs(std::log(w1 / w0)) / rho)
    //            : ((r1 - r0) / rho));

    // Duration duration =  Duration::zero();
    //if (animation.duration !=  Duration::zero()) 
    //{
    //    duration = animation.duration;
    //} 
    //else
    //{
    //    /// V: Average velocity, measured in ρ-screenfuls per second.
    //    double velocity = 1.2;
    //    if (animation.velocity >= 0) {
    //        velocity = animation.velocity / rho;
    //    }

    //    duration = std::chrono::duration_cast< Duration>(
    //        std::chrono::duration<double>(S / velocity)
    //        );
    //}
    //
    //if (duration ==  Duration::zero()) 
    //{
    //    // Perform an instantaneous transition.
    //    jumpTo(camera);
    //    if (animation.transitionFinishFn) {
    //        animation.transitionFinishFn();
    //    }
    //    return;
    //}

    //const double startScale = mState.getScale();
    //setPanningInProgress(true);
    //setScalingInProgress(true);
    //setRotatingInProgress( bearing != startBearing);
    //const  ScreenPt startCtOffset = mState.getCenterOffset();

    //startTransition(
    //    camera,
    //    animation,
    //    [=](double k) 
    //    {
    //        /// s: The distance traveled along the flight path, measured in
    //        /// ρ-screenfuls.
    //        double s = k * S;
    //        double us = k == 1.0 ? 1.0 : u(s);

    //        // Calculate the current point and zoom level along the flight path.
    //         Point<double> framePoint =  interpolate(startPoint, endPoint, us);
    //        double frameZoom =
    //            linearZoomInterpolation ?  interpolate(startZoom, zoom, k) 
    //                                    : startZoom + mState.scaleZoom(1 / w(s));

    //        // Zoom can be NaN if size is empty.
    //        if (std::isnan(frameZoom)) 
    //        {
    //            frameZoom = zoom;
    //        }

    //        // Convert to geographic coordinates and set the new viewpoint.
    //         LonLat frameLatLng =  Projection::LngLatFromPx(framePoint, startScale);
    //        mState.setLatLngZoom(frameLatLng, frameZoom);
    //        if (bearing != startBearing) 
    //        {
    //            mState.setBearing( wrap( interpolate(startBearing, bearing, k), 
    //                                        -M_PI, M_PI
    //                                        )
    //                            );
    //        }

    //        if (ctOffset != startCtOffset)
    //        {
    //            // Interpolate edge insets
    //            mState.setCenterOffset(
    //                { interpolate(startCtOffset.x, ctOffset.x, k),
    //                  interpolate(startCtOffset.y, ctOffset.y, k) }
    //                                   );
    //        }
    //        double maxPitch = getMaxPitchForViewCenterOffset(mState.getCenterOffset());

    //        if (pitch != startPitch || maxPitch < startPitch) {
    //            mState.setPitch(std::min(maxPitch,  interpolate(startPitch, pitch, k)));
    //        }
    //    },
    //    duration);
}

#pragma mark - Position

void Transform::moveBy(const  ScreenPt& offset,
                        const AnimationOptions& animation) 
{
     ScreenPt centerOffset = {offset.x, offset.y};
     ScreenPt pointOnScreen = mState.getViewCenter() - centerOffset;
    // Use unwrapped LonLat to carry information about moveBy direction.
     LonLat  mapCenter= screenPtToLatLng(pointOnScreen, LonLat::Unwrapped);
 #ifdef NO_ANIMATION_GESTURE
     jumpTo(CameraOptions().withCenter(mapCenter));
 #else 
    easeTo(CameraOptions().withCenter(mapCenter), animation);
#endif
}

 LonLat Transform::getMapCenter( LonLat::WrapMode wrap) const 
{
    Pt2D  ctMpx = { mState.getMapCenter().x, mState.getMapCenter().y };
    LonLat ct = MercPrj::LngLatFromPx(ctMpx, BASE_ZOOM, true);
    Pt3D bpt = mState.getMapCenter( );
    return MercPrj::LngLatFromPx({ bpt.x,bpt.y }, BASE_ZOOM, 
        wrap == LonLat::Wrapped
        );
}
  
#pragma mark - Bounds

//void Transform::setLatLngBounds( LatLngBounds bounds) 
//{
//    if (!bounds.valid()) {
//        throw std::runtime_error("failed to set bounds: bounds are invalid");
//    }
//    mState.setLatLngBounds(bounds);
//}

void Transform::setZoomRange(const double aMin, const  double aMax)
{ 
    mState.setZoomRange(aMin, aMax);
}
 
#pragma mark - Bearing

void Transform::rotateBy(const  ScreenPt& first,
                         const  ScreenPt& second,
                         const AnimationOptions& animation) 
{
    int32_t  vpW = mState.viewWidth();
    int32_t  vpH = mState.viewHeight();
    ScreenPt center = mState.getViewCenter();
    const  ScreenPt offset = first - center;
    const double distance = std::sqrt(std::pow(2, offset.x) + std::pow(2, offset.y));

    // If the first click was too close to the center, move the 
    // center of rotation by 200 pixels  in the direction of the click.
    if (distance < 200) 
    {
        const double heightOffset = -200;
        const double rotateBearing = std::atan2(offset.y, offset.x);
        center.x = first.x + std::cos(rotateBearing) * heightOffset;
        center.y = first.y + std::sin(rotateBearing) * heightOffset;
    }

    double radDiff = cm_angleBetween(first - center, second - center);
    const double bearing = -(mState.getRotateZ()  + glm::degrees(radDiff) );
    if (animation.duration <= 0)
    {
        jumpTo(CameraOptions().withBearing(bearing));
    }
    else {
        easeTo(CameraOptions().withBearing(bearing), animation);
    }
}

void Transform::rotateBy_deg(const  double aDegDiff, 
    const AnimationOptions& animation)
{ 
    const double bearing = mState.getRotateZ() + aDegDiff;
    if (animation.duration <= 0)
    {
        jumpTo(CameraOptions().withBearing(bearing));
    }
    else {
        easeTo(CameraOptions().withBearing(bearing), animation);
    }
}
  
#pragma mark - Transition

void Transform::startTransition(const CameraOptions&    camera,
                                const AnimationOptions& animation,
                                const std::function<void(double)>& frame,
                                const  Duration& duration)
{
    if (m_transitionFinishFn) {
        m_transitionFinishFn();
    }

    bool isAnimated = duration !=  Duration::zero();  

    m_transitionStart =  Clock::now();
    m_transitionDuration = duration;

    m_transitionFrameFn = [isAnimated, animation, 
                        frame,  this](const  TimePoint now) 
    {
        float t = 1.0;
        if (isAnimated) {
            t =  std::chrono::duration<float>(now - m_transitionStart) 
                 / m_transitionDuration;
        }

        if (t >= 1.0) {
            // At t = 1.0, a DidChangeAnimated notification should be sent from finish().
            frame(1.0);

            // Indicate that we need to terminate this transition
            return true;
        } else {
             UnitBezier ease =  DEFAULT_TRANSITION_EASE;
            frame(ease.solve(t, 0.001));

            
            if (animation.transitionFrameFn) {
                animation.transitionFrameFn(t);
            }
            //observer.onCameraIsChanging();
            return false;
        }          
    };

    m_transitionFinishFn = [isAnimated, animation, this] 
    {
        setPanningInProgress(false);
        setScalingInProgress(false);
        setRotatingInProgress(false);

        if (animation.transitionFinishFn) {
            animation.transitionFinishFn();
        }
        //observer.onCameraDidChange(isAnimated 
        //        ? MapObserver::CameraChangeMode::Animated
        //        : MapObserver::CameraChangeMode::Immediate);
    };

    if (!isAnimated) {
        auto update = std::move(m_transitionFrameFn);
        auto finish = std::move(m_transitionFinishFn);

        m_transitionFrameFn = nullptr;
        m_transitionFinishFn = nullptr;

        update( Clock::now());
        finish();
    }
}

bool Transform::inTransition() const {
    return m_transitionFrameFn != nullptr;
}

void Transform::updateTransitions(const  TimePoint& now) 
{
    // Use a temporary function to ensure that the transitionFrameFn lambda is
    // called only once per update.

    // This addresses the symptoms of https://github.com/mapbox/mapbox-gl-native/issues/11180
    // where setting a shape source to nil (or similar) in the `onCameraIsChanging`
    // observer function causes `Map::Impl::onUpdate()` to be called which
    // in turn calls this function (before the current iteration has completed),
    // leading to an infinite loop. See https://github.com/mapbox/mapbox-gl-native/issues/5833
    // for a similar, related, issue.
    //
    // By temporarily nulling the `transitionFrameFn` (and then restoring it
    // after the temporary has been called) we stop this recursion.
    //
    // It's important to note that the scope of this change is stop the above
    // crashes. It doesn't address any potential deeper issue (for example
    // user error, how often and when transition callbacks are called).

    auto transition = std::move(m_transitionFrameFn);
    m_transitionFrameFn = nullptr;

    if (transition && transition(now)) 
    {
        // If the transition indicates that it is complete, then we should call
        // the finish lambda (going via a temporary as above)
        auto finish = std::move(m_transitionFinishFn);

        m_transitionFinishFn = nullptr;
        m_transitionFrameFn = nullptr;

        if (finish) {
            finish();
        }
    } 
    else if (!m_transitionFrameFn) 
    {
        // We have to check `transitionFrameFn` is nil here, since a new transition
        // may have been triggered in a user callback (from the transition call
        // above)
        m_transitionFrameFn = std::move(transition);
    }
}

void Transform::cancelTransitions() {
    if (m_transitionFinishFn) {
        m_transitionFinishFn();
    }

    m_transitionFrameFn = nullptr;
    m_transitionFinishFn = nullptr;
}



#pragma mark Conversion and projection

 ScreenPt
Transform::latLngToScreenPt(const  LonLat& latLng) const 
{
    auto bpt = MercPrj::pxFromLngLat(latLng,BASE_ZOOM);
    ScreenPt point = mState.screenFromBpx({ bpt.x, bpt.y, 0 });

    return point;
}

 LonLat 
Transform::screenPtToLatLng(const  ScreenPt& point,
     LonLat::WrapMode wrapMode) const
{
     ScreenPt flippedPoint = point;
     bool bIntersect = false;          
     auto bpt = mState.bpxFromScreen(point.x, point.y, bIntersect);
     LonLat LL = MercPrj::LngLatFromPx({ bpt.x,bpt.y }, BASE_ZOOM,         
                                wrapMode == LonLat::WrapMode::Wrapped);

    return LL;
}

double 
Transform::getMaxPitchForViewCenterOffset(const  ScreenPt& aViewCenterOffset) const
{
    double centerOffsetY = aViewCenterOffset.y;// See TransformState::getCenterOffset.

    int32_t  vpW = mState.viewWidth();
    int32_t  vpH = mState.viewHeight();
    const auto height = vpH;
    assert(height);
    // For details, see description at https://github.com/mapbox/mapbox-gl-native/pull/15195
    // The definition of half of TransformState::fov with no inset, is:
    // fov = arctan((height / 2) / (height * 1.5)).
    // We use half of fov, as it is field of view above perspective center.
    // With inset, this angle changes and 
    // tangentOfFovAboveCenterAngle = (h/2 + centerOffsetY) / (height * 1.5).
    // 1.03 is a bit extra added to prevent parallel ground to viewport clipping plane.
    const double tangentOfFovAboveCenterAngle = 1.03 * (height / 2.0 + centerOffsetY) 
                                                / (1.5 * height);
    const double fovAboveCenter = std::atan(tangentOfFovAboveCenterAngle);
    return M_PI * 0.5 - fovAboveCenter;
    // e.g. Maximum pitch of 60 degrees is when perspective center's offset 
    // from the top is 84% of screen height.
}


#pragma mark - State

bool Transform::isChanging() const
{
    return rotating || scaling || panning || gestureInProgress;
}

bool Transform::isRotating() const
{
    return rotating;
}

bool Transform::isScaling() const
{
    return scaling;
}

bool Transform::isPanning() const
{
    return panning;
}

bool Transform::isGestureInProgress() const
{
    return gestureInProgress;
}
void Transform::setGestureInProgress(bool inProgress) {
    gestureInProgress = inProgress;
}

} // namespace clientmap
