 
namespace clientmap {
//-----------------------------------------------------------
template <typename T>
SPoint2D<T> operator+(SPoint2D<T> const& lhs, SPoint2D<T> const& rhs)
{
    return SPoint2D<T>(lhs.x + rhs.x, lhs.y + rhs.y);
}
template <typename T>
SPoint2D<T> operator+(SPoint2D<T> const& lhs, T const& rhs)
{
    return SPoint2D<T>(lhs.x + rhs, lhs.y + rhs);
}

template <typename T>
SPoint2D<T> operator-(SPoint2D<T> const& lhs, SPoint2D<T> const& rhs)
{
    return SPoint2D<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}
template <typename T>
SPoint2D<T> operator-(SPoint2D<T> const& lhs, T const& rhs)
{
    return SPoint2D<T>(lhs.x - rhs, lhs.y - rhs);
}
template <typename T>
SPoint2D<T> operator*(SPoint2D<T> const& lhs, SPoint2D<T> const& rhs)
{
    return SPoint2D<T>(lhs.x * rhs.x, lhs.y * rhs.y);
}

template <typename T>
SPoint2D<T> operator*(SPoint2D<T> const& lhs, T const& rhs)
{
    return SPoint2D<T>(lhs.x * rhs, lhs.y * rhs);
}
template <typename T>
SPoint2D<T> operator/(SPoint2D<T> const& lhs, SPoint2D<T> const& rhs)
{
    return SPoint2D<T>(lhs.x / rhs.x, lhs.y / rhs.y);
}

template <typename T>
SPoint2D<T> operator/(SPoint2D<T> const& lhs, T const& rhs)
{
    return SPoint2D<T>(lhs.x / rhs, lhs.y / rhs);
}
template <typename T>
SPoint2D<T>& operator+=(SPoint2D<T>& lhs, SPoint2D<T> const& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}
template <typename T>
SPoint2D<T>& operator+=(SPoint2D<T>& lhs, T const& rhs)
{
    lhs.x += rhs;
    lhs.y += rhs;
    return lhs;
}
template <typename T>
SPoint2D<T>& operator-=(SPoint2D<T>& lhs, SPoint2D<T> const& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}
template <typename T>
SPoint2D<T>& operator-=(SPoint2D<T>& lhs, T const& rhs)
{
    lhs.x -= rhs;
    lhs.y -= rhs;
    return lhs;
}
template <typename T>
SPoint2D<T>& operator*=(SPoint2D<T>& lhs, SPoint2D<T> const& rhs)
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    return lhs;
}

template <typename T>
SPoint2D<T>& operator*=(SPoint2D<T>& lhs, T const& rhs)
{
    lhs.x *= rhs;
    lhs.y *= rhs;
    return lhs;
}

template <typename T>
SPoint2D<T>& operator/=(SPoint2D<T>& lhs, SPoint2D<T> const& rhs)
{
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    return lhs;
}

template <typename T>
SPoint2D<T>& operator/=(SPoint2D<T>& lhs, T const& rhs)
{
    lhs.x /= rhs;
    lhs.y /= rhs;
    return lhs;
}
// member functions
template <typename T>
SPoint2D<T>& SPoint2D<T>::operator+=(const SPoint2D<T>& p1)
{
    this->x += p1.x;
    this->y += p1.y;
    return *this;
}
template <typename T>
double SPoint2D<T>::distanceTo(const SPoint2D<T>& p1) const
{
    double lx = x - p1.x;
    double ly = y - p1.y;
    return sqrt(lx*lx + ly * ly);
}

#if 0
template <class S, class T>
SPoint2D<S> convertPoint(const SPoint2D<T>& p) {
    return SPoint2D<S>(p.x, p.y);
}
#endif
//-----------------------------------------------------------
template <typename T>
inline double SPoint3D<T>::distanceTo(const SPoint3D<T>& obj) const 
{
    double dX = x - obj.x;
    double dY = y - obj.y;
    double dZ = z - obj.z;
    return sqrt(dX*dX + dY * dY + dZ * dZ);
} 
//------------------------------------------------------------
template <typename T>
void SBox2D<T>::set(T x1, T y1, T x2, T y2)
{
    this->mMinX = std::min(x1, x2);
    this->mMaxX = std::max(x1, x2);
    this->mMinY = std::min(y1, y2);
    this->mMaxY = std::max(y1, y2);
}

template <typename T>
void SBox2D<T>::unite(T x, T y)
{
    this->mMinX = std::min(this->mMinX, x);
    this->mMinY = std::min(this->mMinY, y);
    this->mMaxX = std::max(this->mMaxX, x);
    this->mMaxY = std::max(this->mMaxY, y);
}


template<class T>
void SBox2D<T>::inflate(T _x, T _y)
{
    mMinX -= _x;
    mMaxX += _x;
    mMinY -= _y;
    mMaxY += _y;
}

template<class T>
bool SBox2D<T>::deflate(T _x, T _y)
{
    if( (_x*2.0 >= width())|| (_y *2.0 >= height()) )
    {
        return false;
    }else
    {
        mMinX += _x;
        mMaxX -= _x;
        mMinY += _y;
        mMaxY -= _y;
        return true;
    }
}

template <typename T>
bool SBox2D<T>::intersect(const SBox2D<T>& other) const
{
    return !(mMinX > other.mMaxX || mMaxX<other.mMinX ||
        mMinY > other.mMaxY || mMaxY < other.mMinY);
}


template <typename T>
constexpr bool operator==(SBox2D<T> const& lhs, SBox2D<T> const& rhs)
{
    return lhs.min == rhs.min && lhs.max == rhs.max;
}

template <typename T>
constexpr bool operator!=(SBox2D<T> const& lhs, SBox2D<T> const& rhs)
{
    return lhs.min != rhs.min || lhs.max != rhs.max;
}
//------------------------------------------------------------
} //  end namespace clientmap
