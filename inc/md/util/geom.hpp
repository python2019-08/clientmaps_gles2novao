#pragma once
#include <cmath>
#include <algorithm> // std::min 
#include <stdexcept>
#include <vector>
#include <array>
//#include <md/util/constants.hpp>
// 因为这个接口文件可能会向外提供，所以不用glm中的类或结构体（glm太大了）。
namespace clientmap {
//===============================================================
// “SPoint2D”首字母S是struct的首字母。
template <typename T>
struct SPoint2D
{
    using coordinate_type = T;

    constexpr SPoint2D() : x(), y() { }
    constexpr SPoint2D(T x_, T y_) : x(x_), y(y_) { }

    friend bool operator==(SPoint2D<T> const& lhs, SPoint2D<T> const& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
    friend bool operator!=(SPoint2D<T> const& lhs, SPoint2D<T> const& rhs)
    {
        return !(lhs == rhs);
    }
    SPoint2D& operator+=(const SPoint2D& p1);
    double distanceTo(const SPoint2D& p1) const;
    inline double distanceToOrigin() const {
        return  sqrt(x*x + y * y);
    }
    //----------------------
    T x;
    T y;
};
//-------------------------------------------        
template <typename T>
struct SPoint3D
{
    using coordinate_type = T;

    constexpr SPoint3D() : x(), y(), z() {    }
    constexpr SPoint3D(T x_, T y_) : x(x_), y(y_), z(0) { }
    SPoint3D(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {  }

    void clear() {
        this->x = this->y = this->z = 0;
    }
    friend bool 
	operator==(SPoint3D<T> const& lhs, SPoint3D<T> const& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
    friend bool 
	operator!=(SPoint3D<T> const& lhs, SPoint3D<T> const& rhs)
    {
        return !(lhs == rhs);
    }

    inline double distanceTo(const SPoint3D<T>& obj) const;
    inline double distanceToOrigin() const {
        return  sqrt(x*x + y * y + z * z);
    }
    //----------------------------
    T x;
    T y;
    T z;
};
//===============================================================
template <typename T>
struct SBox2D
{
    using point_type = SPoint2D<T>;

    constexpr SBox2D(point_type const& min_, point_type const& max_)
        : mMinX(min_.x), mMinY(min_.y)
        , mMaxX(max_.x), mMaxY(max_.y)
    {
    }

    SBox2D() : mMinX(), mMinY(), mMaxX(), mMaxY() { }
    SBox2D(T x1, T y1, T x2, T y2)
    {
        set(x1, y1, x2, y2);
    }

    void set(T x1, T y1, T x2, T y2);

    T width() const {
        return mMaxX - mMinX;
    }

    T height() const {
        return mMaxY - mMinY;
    }
    bool isEmpty()const {
        return ((mMaxX <= mMinX) || (mMaxY <= mMinY));
    }

    point_type center() const {
        return { (mMaxX + mMinX) * 0.5, (mMaxY + mMinY) * 0.5 };
    }
    point_type southWest() const {
        return { mMinX, mMinY };
    }
    point_type northEast() const {
        return { mMaxX, mMaxY };
    }
    point_type southEast() const {
        return { mMaxX, mMinY };
    }
    point_type northWest() const {
        return { mMinX, mMaxY };
    }

    bool containsX(T x) const {
        return x >= mMinX && x <= mMaxX;
    }
    bool containsY(T y) const {
        return y >= mMinY && y <= mMaxY;
    }
    bool contains(T x, T y) const {
        return containsX(x) && containsY(y);
    }
    void unite(T x, T y);// 取并集
    void inflate(T _x, T _y);
    bool deflate(T _x, T _y);
    bool intersect(const SBox2D& other) const;

    T  mMinX;
    T  mMinY;
    T  mMaxX;
    T  mMaxY;
};
}// namespace clientmap

// template member functions implementation{
#include "md/util/geom_inl.hpp"
// template member functions implementation}


namespace clientmap{
typedef SPoint3D<double>  Pt3D;
//===============================================================
using Pt2Ds = SPoint2D<short>;
using LineStr2Ds = std::vector<Pt2Ds>;
using LineRing2Ds = std::vector<Pt2Ds>;
using Polygon2Ds = std::vector<LineRing2Ds>;

using Pt2Di = SPoint2D<int>;
using LineStr2Di = std::vector<Pt2Di>;
using LineRing2Di = std::vector<Pt2Di>;
using Polygon2Di = std::vector<LineRing2Di>;

using Pt2Df = SPoint2D<float>;
using LineStr2Df = std::vector<Pt2Df>;
using LineRing2Df = std::vector<Pt2Df>;
using Polygon2Df = std::vector<LineRing2Df>;

using Pt2D = SPoint2D<double>;
using LineStr2D = std::vector<Pt2D>;
using LineRing2D = std::vector<Pt2D>;
using Polygon2D = std::vector<LineRing2D>;
bool lineSegIntersetionAt(const Pt2D &aP0, const Pt2D &aP1,
    const Pt2D &aQ0, const Pt2D &aQ1, Pt2D &interPoint);
//===============================================================
using Box2Di     = SBox2D<int>;  
using Box2Df    = SBox2D<float>;
using Box2D     = SBox2D<double>;
//===============================================================
using GeoPt      = Pt2D;
using GeoLineStr = LineStr2D;
using GeoBox     = Box2D;

using ScreenPt      = Pt2D;
using ScreenLineStr = LineStr2D;
using ScreenBox     = Box2D;
//===============================================================
// many operation of 
class Quad2D {
public:
    Quad2D();
    Quad2D(const Quad2D& other);
    Quad2D(const Pt2D& lt, const Pt2D& rt,
            const Pt2D& rb, const Pt2D& lb);

    virtual ~Quad2D();

    const Box2D& getBound();

    bool intersect(const Box2D& bound);

    // 判断线段是否与四边形相交，并求出交点
    bool intersect(const Pt2D &p1, const Pt2D &p2, Pt2D &interPoint);

    // 判断点是否在四边形内
    bool contains(const Pt2D &point) {
        return getBound().contains(point.x, point.y);
    }

    Pt2D getPoint(int aPtIdx) const;
    void setPointXY(int aPtIdx, const Pt2D& aPt);
    Pt2D getLeftTop();
    Pt2D getRightTop();
    Pt2D getRightBottom();
    Pt2D getLeftBottom();
    Pt2D getCenter();

    Quad2D& operator=(const Quad2D& other);     
private:
    void calcBound();
    // lt,rt,rb,lb --> 0,1,2,3
    std::array<Pt2D,5> mPolygon;
    Box2D              mBound;// the bound of mPolygon
    bool               mIsDirtyBound =true;
};

//===============================================================
} // MdU//namespace md
