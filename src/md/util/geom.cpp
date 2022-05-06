#include <cmath> 
#include <glm/glm.hpp>

#include <md/util/constants.hpp>
#include <md/util/math_tools.hpp>  
#include <md/util/geom.hpp>
 

namespace clientmap {
//----class template explicit instantiation
template struct  SPoint2D<short>;
template struct  SPoint2D<int>;
template struct  SPoint2D<float>;
template struct  SPoint2D<double>;
//------------------------------------------------------------
bool lineSegIntersetionAt(const Pt2D &aP0, const Pt2D &aP1,
    const Pt2D &aQ0, const Pt2D &aQ1,
    Pt2D &interPoint)
{
    const Pt2D & a= aP0;
    const Pt2D & b = aP1;
    const Pt2D & c = aQ0;
    const Pt2D & d = aQ1;
    double area_abc = (b.x - a.x)*(c.y - b.y) - (b.y - a.y)*(c.x - b.x);
    double area_abd = (b.x - a.x)*(d.y - b.y) - (b.y - a.y)*(d.x - b.x);
    if (area_abc * area_abd >= 0) {
        return false;
    }

    double area_cda = (d.x - c.x)*(a.y - d.y) - (d.y - c.y)*(a.x - d.x);
    double area_cdb = area_abc + area_cda - area_abd;
    if (area_cda * area_cdb >= 0) {
        return false;
    }

    double t = area_cda / (area_abd - area_abc);
    interPoint.x = a.x + (b.x - a.x)*t;
    interPoint.y = a.y + (b.y - a.y)*t;
    return true;
}
//-----------------------------------------------------------

Quad2D::Quad2D()
    :mPolygon{{ {0,0},{0,0},{0,0},{0,0} }}
    , mBound(mPolygon[0], mPolygon[0])
{
 
}

Quad2D::Quad2D(const Pt2D& lt,  const Pt2D& rt,
    const Pt2D& rb,  const Pt2D& lb)
    :mPolygon{ lt,rt,rb,lb,lt }
    ,mBound(mPolygon[0], mPolygon[0])
{ 
}

Quad2D::Quad2D(const Quad2D& other) 
{ 
	*this = other;
}

Quad2D& Quad2D::operator=(const Quad2D& other)
{
    if (&other != this) {
        mPolygon = other.mPolygon ; 
		mBound = other.mBound;
		mIsDirtyBound = other.mIsDirtyBound;
    }
    return *this;
}

Quad2D::~Quad2D(){
}

const Box2D& Quad2D::getBound()
{
    if (mIsDirtyBound){
        calcBound();
        mIsDirtyBound = false;
    }
    return mBound;
}

void Quad2D::calcBound()
{
    Box2D bound(mPolygon[0], mPolygon[0]);
    bound.unite(mPolygon[1].x, mPolygon[1].y);
    bound.unite(mPolygon[2].x, mPolygon[2].y);
    bound.unite(mPolygon[3].x, mPolygon[3].y);

    mBound =  bound;
}

bool Quad2D::intersect(const Box2D& bound )
{ 
     return getBound().intersect(bound);     
}

bool Quad2D::intersect(const Pt2D &p0, const Pt2D &p1, Pt2D &interPoint)
{
    for (int i = 0; i < 4; i++) 
    {
        const Pt2D &qprime0 = mPolygon[i];
        const Pt2D &qprime1 = mPolygon[i + 1];

        Pt2D q0(qprime0 );
        Pt2D q1(qprime1 );
        if (lineSegIntersetionAt(p0, p1, q0, q1, interPoint)) 
        {
            return true;
        }
    }
    return false;
}

Pt2D  Quad2D::getPoint(int pntIndex) const
{
    assert(pntIndex >= 0 && pntIndex < 4);
    //assert(pntIndex < mPolygon.size() - 1);
    return mPolygon[pntIndex];
}
  
void Quad2D::setPointXY(int pntIndex, const Pt2D& aPt)
{
    assert(pntIndex >= 0 && pntIndex < 4);
    if (pntIndex == 0)
    {
        mPolygon[4]= aPt;
    }
    mPolygon[pntIndex] = aPt;  
    mIsDirtyBound = true;
}

Pt2D Quad2D::getCenter()
{
    return getBound().center();
}

Pt2D Quad2D::getLeftTop()
{
    return mPolygon[0];
}

Pt2D Quad2D::getRightTop()
{
    return mPolygon[1];
}

Pt2D Quad2D::getRightBottom()
{
    return mPolygon[2];
}

Pt2D Quad2D::getLeftBottom()
{
    return mPolygon[3];
}
//------------------------------------------------------------
} // namespace mbgl
