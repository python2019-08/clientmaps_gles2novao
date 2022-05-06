#include <algorithm>
#include <cassert>
#include <cmath>
#include <array>
#include <md/util/logtool.hpp>

#include <md/util/tc_bound_frustum.hpp>
 
namespace clientmap {
namespace {

glm::dvec3 toVec3(const glm::dvec4& v)
{
    return glm::dvec3{v[0], v[1], v[2]};
}
 
double vec4Dot(const glm::dvec4& a, const glm::dvec4& b)
{
    return a[0] * b[0]  + a[1] * b[1] 
         + a[2] * b[2]  + a[3] * b[3];
}

template <size_t N>
static Pt2D 
ProjectPointsToAxis(const std::array<glm::dvec3, N>& aPoints,
                    const glm::dvec3& aOrigin,
                    const glm::dvec3& aAxis)
{
    double min = std::numeric_limits<double>::max();
    double max = -std::numeric_limits<double>::max();

    for (const glm::dvec3& point : aPoints)
    {
        //double projectedPoint = vec3Dot(vec3Sub(point, origin), axis);
        double projectedPoint = glm::dot((point - aOrigin) , aAxis);
        min = std::min(projectedPoint, min);
        max = std::max(projectedPoint, max);
    }

    return {min, max};
}

} // namespace
 

AABB::AABB() : min({0, 0, 0}), max({0, 0, 0}) 
{
}

AABB::AABB(const glm::dvec3& min_, const glm::dvec3& max_)
    : min(min_), max(max_) 
{
}
// gm:should named clampPoint
glm::dvec3 AABB::closestPoint(const glm::dvec3& point) const
{
    return {std::max(std::min(max[0], point[0]), min[0]),
             std::max(std::min(max[1], point[1]), min[1]),
             std::max(std::min(max[2], point[2]), min[2])};
}

glm::dvec3 AABB::distanceXYZ(const glm::dvec3& point) const
{
    //glm::dvec3 vec = vec3Sub(closestPoint(point), point);
    glm::dvec3 vec = closestPoint(point) -  point;

    vec[0] = std::abs(vec[0]);
    vec[1] = std::abs(vec[1]);
    vec[2] = std::abs(vec[2]);

    return vec;
}

AABB AABB::quadrant(int idx) const 
{
    assert(idx >= 0 && idx < 4);
    glm::dvec3 quadrantMin = min;
    glm::dvec3 quadrantMax = max;
    const double xCenter = 0.5 * (max[0] + min[0]);
    const double yCenter = 0.5 * (max[1] + min[1]);

    // This aabb is split into 4 quadrants. 
    // For each axis define in which side of the split "idx" is
    // The result for indices 0, 1, 2, 3 is: 
    //         { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 }
    const std::array<int, 4> xSplit = {{0, 1, 0, 1}};
    const std::array<int, 4> ySplit = {{0, 0, 1, 1}};

    quadrantMin[0] = xSplit[idx] ? xCenter : quadrantMin[0];
    quadrantMax[0] = xSplit[idx] ? quadrantMax[0] : xCenter;

    quadrantMin[1] = ySplit[idx] ? yCenter : quadrantMin[1];
    quadrantMax[1] = ySplit[idx] ? quadrantMax[1] : yCenter;

    return {quadrantMin, quadrantMax};
}

bool AABB::intersects(const AABB& aabb) const 
{
    if (min[0] > aabb.max[0] || aabb.min[0] > max[0]) 
        return false;
    if (min[1] > aabb.max[1] || aabb.min[1] > max[1]) 
        return false;
    if (min[2] > aabb.max[2] || aabb.min[2] > max[2]) 
        return false;
    return true;
}

bool AABB::operator==(const AABB& aabb) const 
{
    return min == aabb.min && max == aabb.max;
}

bool AABB::operator!=(const AABB& aabb) const 
{
    return !(*this == aabb);
}

// Named index values for frustum::points array
enum {
    near_tl = 0,
    near_tr = 1,
    near_br = 2,
    near_bl = 3,
    far_tl = 4,
    far_tr = 5,
    far_br = 6,
    far_bl = 7,
};

Frustum::Frustum(const std::array<glm::dvec3, 8>& points_,
                 const std::array<glm::dvec4, 6>& planes_
                 )
    : mPts(points_), m_planes(planes_)
{
    const Pt2D xBounds = ProjectPointsToAxis(
        mPts, {0, 0, 0}, {1, 0, 0});
    const Pt2D yBounds = ProjectPointsToAxis(
        mPts, {0, 0, 0}, {0, 1, 0});
    const Pt2D zBounds = ProjectPointsToAxis(
        mPts, {0, 0, 0}, {0, 0, 1});

    m_bounds = AABB({xBounds.x, yBounds.x, zBounds.x},
                    {xBounds.y, yBounds.y, zBounds.y});

    //   Precompute a set of separating axis candidates for precise 
    // intersection tests.
    //   Remaining axes not covered in basic intersection tests are: 
    // axis[] = (edges of aabb) x (edges of frustum)
    //std::array<glm::dvec3, 6> frustumEdges = { {
    //    vec3Sub(mPts[near_br], mPts[near_bl]),
    //    vec3Sub(mPts[near_tl], mPts[near_bl]),
    //    vec3Sub(mPts[far_tl],  mPts[near_tl]),
    //    vec3Sub(mPts[far_tr],  mPts[near_tr]),
    //    vec3Sub(mPts[far_br],  mPts[near_br]),
    //    vec3Sub(mPts[far_bl],  mPts[near_bl]) } };
    std::array<glm::dvec3, 6> frustumEdges = { {
    (mPts[near_br] - mPts[near_bl]),
    (mPts[near_tl] - mPts[near_bl]),
    (mPts[far_tl] -  mPts[near_tl]),
    (mPts[far_tr] -  mPts[near_tr]),
    (mPts[far_br] -  mPts[near_br]),
    (mPts[far_bl] -  mPts[near_bl]) } };

    for (size_t i = 0; i < frustumEdges.size(); i++) 
    {
        // Cross product [1, 0, 0] x [a, b, c] == [0, -c, b]
        // Cross product [0, 1, 0] x [a, b, c] == [c, 0, -a]
        const glm::dvec3 axis0 = {0.0,
                            -frustumEdges[i][2], 
                             frustumEdges[i][1]};
        const glm::dvec3 axis1 = {frustumEdges[i][2],
                             0.0, 
                            -frustumEdges[i][0]};

        m_projections[i * 2] = {axis0,
                    ProjectPointsToAxis(mPts, mPts[0], axis0) };
        m_projections[i * 2 + 1] = {axis1,
                    ProjectPointsToAxis(mPts, mPts[0], axis1) };
    }
}

Frustum Frustum::fromInvProjMatrix(const glm::dmat4& aInvProj, 
                                    double aWorldSize, 
                                    double aZoom, 
                                    bool aFlippedY) 
{
    // Define frustum corner points in normalized clip space
    std::array<glm::dvec4, 8> cornerCoords = { {
        glm::dvec4{-1.0,  1.0, -1.0, 1.0},
        glm::dvec4{ 1.0,  1.0, -1.0, 1.0},
        glm::dvec4{ 1.0, -1.0, -1.0, 1.0},
        glm::dvec4{-1.0, -1.0, -1.0, 1.0},
        glm::dvec4{-1.0,  1.0,  1.0, 1.0},
        glm::dvec4{ 1.0,  1.0,  1.0, 1.0},
        glm::dvec4{ 1.0, -1.0,  1.0, 1.0},
        glm::dvec4{-1.0, -1.0,  1.0, 1.0}
        } };

    const double scale = std::pow(2.0, aZoom);

    // Transform points to tile space
    for (auto& coord : cornerCoords) 
    {
        //matrix::transformMat4(coord, coord, aInvProj);
        coord = aInvProj * coord;
        //for (auto& component : coord) 
        //    component *= 1.0 / coord[3] / aWorldSize * scale;
        coord  *= 1.0 / coord[3] / aWorldSize * scale; 
        coord /= coord[3];
    }

    std::array<glm::ivec3, 6> frustumPlanePointIndices = {
    {
        glm::ivec3{near_bl, near_br, far_br},  // bottom
        glm::ivec3{near_tl, near_bl, far_bl},  // left
        glm::ivec3{near_br, near_tr, far_tr},  // right
        glm::ivec3{near_tl, far_tl, far_tr},   // top
        glm::ivec3{near_tl, near_tr, near_br}, // near
        glm::ivec3{far_br, far_tr, far_tl}     // far
    }};

    if (aFlippedY) 
    {
        std::for_each(frustumPlanePointIndices.begin(), 
                      frustumPlanePointIndices.end(), 
                      [](glm::ivec3& tri)
        {
            std::swap(tri[1], tri[2]);
        });
    }

    Log::Debug("fromInvProjMatrix()..frustumPlanes----start\n");
    std::array<glm::dvec4, 6> frustumPlanes;
    for (std::size_t i = 0; i < frustumPlanePointIndices.size(); i++) 
    {
        const glm::ivec3 indices = frustumPlanePointIndices[i];

        // Compute plane equation using 3 points on the plane
        //const glm::dvec3 p0 = toVec3(cornerCoords[indices[0]]);
        //const glm::dvec3 p1 = toVec3(cornerCoords[indices[1]]);
        //const glm::dvec3 p2 = toVec3(cornerCoords[indices[2]]);
        const glm::dvec3 p0 = glm::dvec3( cornerCoords[indices[0] ] );
        const glm::dvec3 p1 = glm::dvec3( cornerCoords[indices[1] ] );
        const glm::dvec3 p2 = glm::dvec3( cornerCoords[indices[2] ] );

        const glm::dvec3 a = p0 - p1;//vec3Sub(p0, p1);
        const glm::dvec3 b = p2 - p1;//vec3Sub(p2, p1);
        //const glm::dvec3 n = vec3Normalize(vec3Cross(a, b));
        const glm::dvec3 n = glm::normalize(glm::cross(a, b));

        //frustumPlanes[i] = {{n[0], n[1], n[2], -vec3Dot(n, p1)}};
        frustumPlanes[i] =  {-n[0], -n[1], -n[2], glm::dot(n,p1)} ;

        Log::Debug("%f %f %f %f\n", 
            frustumPlanes[i].x,  frustumPlanes[i].y, 
            frustumPlanes[i].z,  frustumPlanes[i].w);
    }
    Log::Debug("fromInvProjMatrix()..frustumPlanes----end\n");


    Log::Debug("fromInvProjMatrix()..frustumPoints----start\n");
    std::array<glm::dvec3, 8> frustumPoints;
    for (size_t i = 0; i < cornerCoords.size(); i++)
    {    
        //frustumPoints[i] = toVec3(cornerCoords[i]);
        frustumPoints[i] = glm::dvec3(cornerCoords[i]);
        Log::Debug("%f %f %f %f\n",
            cornerCoords[i].x, cornerCoords[i].y,
            cornerCoords[i].z, cornerCoords[i].w);
    }
    Log::Debug("fromInvProjMatrix()..frustumPoints----end\n");

    return {frustumPoints, frustumPlanes};
}

IntersectionResult 
Frustum::intersects(const AABB& aabb) const 
{
    // Execute separating axis test between two convex objects to find
    // intersections Each frustum plane together with 3 major axes 
    // define the separating axes This implementation is conservative 
    // as it's not checking all possible axes.
    // False positive rate is ~0.5% of all cases (see intersectsPrecise).
    // Note: test only 4 points as both min and max points have zero 
    // elevation
    assert(aabb.min[2] == 0.0 && aabb.max[2] == 0.0);
    IntersectionResult  ret = IntersectionResult::Intersects;
    do
    {

        if (!m_bounds.intersects(aabb))
        {
            ret = IntersectionResult::Separate;
            break;
        }


        const std::array<glm::dvec4, 4> aabbPoints = {
        {
            glm::dvec4{aabb.min[0], aabb.min[1], 0.0, 1.0},
            glm::dvec4{aabb.max[0], aabb.min[1], 0.0, 1.0},
            glm::dvec4{aabb.max[0], aabb.max[1], 0.0, 1.0},
            glm::dvec4{aabb.min[0], aabb.max[1], 0.0, 1.0},
        } };
               
        bool fullyInside = true;
        for (const glm::dvec4& plane : m_planes)
        {
            size_t pointsInside = 0;

            //pointsInside += vec4Dot(plane, aabbPoints[0]) >= 0.0;
            //pointsInside += vec4Dot(plane, aabbPoints[1]) >= 0.0;
            //pointsInside += vec4Dot(plane, aabbPoints[2]) >= 0.0;
            //pointsInside += vec4Dot(plane, aabbPoints[3]) >= 0.0;
            double in0 = glm::dot(plane, aabbPoints[0]);
            double in1 = glm::dot(plane, aabbPoints[1]);
            double in2 = glm::dot(plane, aabbPoints[2]);
            double in3 = glm::dot(plane, aabbPoints[3]);

            pointsInside += glm::dot(plane, aabbPoints[0]) >= 0.0;
            pointsInside += glm::dot(plane, aabbPoints[1]) >= 0.0;
            pointsInside += glm::dot(plane, aabbPoints[2]) >= 0.0;
            pointsInside += glm::dot(plane, aabbPoints[3]) >= 0.0;

            if (!pointsInside) {
                // Separating axis found, no intersection
                ret = IntersectionResult::Separate;
                break;
            }

            if (pointsInside != aabbPoints.size())
                fullyInside = false;
        }
        
        if (ret != IntersectionResult::Separate) 
        {
            ret = fullyInside
                ? IntersectionResult::Contains
                : IntersectionResult::Intersects;
        } 

    } while (false);

    return ret;
}

IntersectionResult 
Frustum::intersectsPrecise( const AABB& aabb, 
                            bool edgeCasesOnly) const 
{
    if (!edgeCasesOnly) 
    {
        IntersectionResult result = intersects(aabb);

        if (result == IntersectionResult::Separate) 
            return result;
    }

    const std::array<glm::dvec3, 4> aabbPoints = { {
        glm::dvec3{aabb.min[0], aabb.min[1], 0.0},
        glm::dvec3{aabb.max[0], aabb.min[1], 0.0},
        glm::dvec3{aabb.max[0], aabb.max[1], 0.0},
        glm::dvec3{aabb.min[0], aabb.max[1], 0.0}
        }};

    // For a precise SAT-test all edge cases needs to be covered
    // Projections of the frustum on separating axis 
    // candidates have been precomputed already
    for (const Projection& proj : m_projections)
    {
        Pt2D projectedAabb = 
            ProjectPointsToAxis(aabbPoints, mPts[0], proj.axis);
        const Pt2D& projectedFrustum = proj.projection;

        if (projectedFrustum.y < projectedAabb.x || 
            projectedFrustum.x > projectedAabb.y) 
        {
            return IntersectionResult::Separate;
        }
    }

    return IntersectionResult::Intersects;
}

//////////////////////////////////////////////
#if 1 
void Frustum::calcFrustumPlanes(const std::array<float,16>&     p,
                            const std::array<float, 16>&    mv,
    std::array<std::array<float, 4>, 6>& aFrustumPlanes)
{
    //float p[16];   // projection matrix
    //float mv[16];  // model-view matrix
    float mvp[16]; // model-view-projection matrix
    float t;
       
    //glGetFloatv(GL_PROJECTION_MATRIX, p);
    //glGetFloatv(GL_MODELVIEW_MATRIX, mv);

    //
    // Concatenate the projection matrix and the model-view matrix to produce
    // a combined model-view-projection matrix.
    //
    mvp[0] = mv[0] * p[0] + mv[1] * p[4] + mv[2] * p[8] + mv[3] * p[12];
    mvp[1] = mv[0] * p[1] + mv[1] * p[5] + mv[2] * p[9] + mv[3] * p[13];
    mvp[2] = mv[0] * p[2] + mv[1] * p[6] + mv[2] * p[10] + mv[3] * p[14];
    mvp[3] = mv[0] * p[3] + mv[1] * p[7] + mv[2] * p[11] + mv[3] * p[15];

    mvp[4] = mv[4] * p[0] + mv[5] * p[4] + mv[6] * p[8] + mv[7] * p[12];
    mvp[5] = mv[4] * p[1] + mv[5] * p[5] + mv[6] * p[9] + mv[7] * p[13];
    mvp[6] = mv[4] * p[2] + mv[5] * p[6] + mv[6] * p[10] + mv[7] * p[14];
    mvp[7] = mv[4] * p[3] + mv[5] * p[7] + mv[6] * p[11] + mv[7] * p[15];

    mvp[8] = mv[8] * p[0] + mv[9] * p[4] + mv[10] * p[8] + mv[11] * p[12];
    mvp[9] = mv[8] * p[1] + mv[9] * p[5] + mv[10] * p[9] + mv[11] * p[13];
    mvp[10] = mv[8] * p[2] + mv[9] * p[6] + mv[10] * p[10] + mv[11] * p[14];
    mvp[11] = mv[8] * p[3] + mv[9] * p[7] + mv[10] * p[11] + mv[11] * p[15];

    mvp[12] = mv[12] * p[0] + mv[13] * p[4] + mv[14] * p[8] + mv[15] * p[12];
    mvp[13] = mv[12] * p[1] + mv[13] * p[5] + mv[14] * p[9] + mv[15] * p[13];
    mvp[14] = mv[12] * p[2] + mv[13] * p[6] + mv[14] * p[10] + mv[15] * p[14];
    mvp[15] = mv[12] * p[3] + mv[13] * p[7] + mv[14] * p[11] + mv[15] * p[15];
     
    //
    // Extract the frustum's right clipping plane and normalize it.
    //
    aFrustumPlanes[0][0] = mvp[3] - mvp[0];
    aFrustumPlanes[0][1] = mvp[7] - mvp[4];
    aFrustumPlanes[0][2] = mvp[11] - mvp[8];
    aFrustumPlanes[0][3] = mvp[15] - mvp[12];

    t = (float)sqrt(aFrustumPlanes[0][0] * aFrustumPlanes[0][0] +
        aFrustumPlanes[0][1] * aFrustumPlanes[0][1] +
        aFrustumPlanes[0][2] * aFrustumPlanes[0][2]);

    aFrustumPlanes[0][0] /= t;
    aFrustumPlanes[0][1] /= t;
    aFrustumPlanes[0][2] /= t;
    aFrustumPlanes[0][3] /= t;

    //
    // Extract the frustum's left clipping plane and normalize it.
    //

    aFrustumPlanes[1][0] = mvp[3] + mvp[0];
    aFrustumPlanes[1][1] = mvp[7] + mvp[4];
    aFrustumPlanes[1][2] = mvp[11] + mvp[8];
    aFrustumPlanes[1][3] = mvp[15] + mvp[12];

    t = (float)sqrt(aFrustumPlanes[1][0] * aFrustumPlanes[1][0] +
        aFrustumPlanes[1][1] * aFrustumPlanes[1][1] +
        aFrustumPlanes[1][2] * aFrustumPlanes[1][2]);

    aFrustumPlanes[1][0] /= t;
    aFrustumPlanes[1][1] /= t;
    aFrustumPlanes[1][2] /= t;
    aFrustumPlanes[1][3] /= t;

    
    //
    // Extract the frustum's bottom clipping plane and normalize it.
    //
    aFrustumPlanes[2][0] = mvp[3] + mvp[1];
    aFrustumPlanes[2][1] = mvp[7] + mvp[5];
    aFrustumPlanes[2][2] = mvp[11] + mvp[9];
    aFrustumPlanes[2][3] = mvp[15] + mvp[13];

    t = (float)sqrt(aFrustumPlanes[2][0] * aFrustumPlanes[2][0] +
        aFrustumPlanes[2][1] * aFrustumPlanes[2][1] +
        aFrustumPlanes[2][2] * aFrustumPlanes[2][2]);

    aFrustumPlanes[2][0] /= t;
    aFrustumPlanes[2][1] /= t;
    aFrustumPlanes[2][2] /= t;
    aFrustumPlanes[2][3] /= t;

    //
    // Extract the frustum's top clipping plane and normalize it.
    //
    aFrustumPlanes[3][0] = mvp[3] - mvp[1];
    aFrustumPlanes[3][1] = mvp[7] - mvp[5];
    aFrustumPlanes[3][2] = mvp[11] - mvp[9];
    aFrustumPlanes[3][3] = mvp[15] - mvp[13];

    t = (float)sqrt(aFrustumPlanes[3][0] * aFrustumPlanes[3][0] +
        aFrustumPlanes[3][1] * aFrustumPlanes[3][1] +
        aFrustumPlanes[3][2] * aFrustumPlanes[3][2]);

    aFrustumPlanes[3][0] /= t;
    aFrustumPlanes[3][1] /= t;
    aFrustumPlanes[3][2] /= t;
    aFrustumPlanes[3][3] /= t;



    //
    // Extract the frustum's far clipping plane and normalize it.
    //
    aFrustumPlanes[4][0] = mvp[3] - mvp[2];
    aFrustumPlanes[4][1] = mvp[7] - mvp[6];
    aFrustumPlanes[4][2] = mvp[11] - mvp[10];
    aFrustumPlanes[4][3] = mvp[15] - mvp[14];

    t = (float)sqrt(aFrustumPlanes[4][0] * aFrustumPlanes[4][0] +
        aFrustumPlanes[4][1] * aFrustumPlanes[4][1] +
        aFrustumPlanes[4][2] * aFrustumPlanes[4][2]);

    aFrustumPlanes[4][0] /= t;
    aFrustumPlanes[4][1] /= t;
    aFrustumPlanes[4][2] /= t;
    aFrustumPlanes[4][3] /= t;
    //
    // Extract the frustum's near clipping plane and normalize it.
    //
    aFrustumPlanes[5][0] = mvp[3] + mvp[2];
    aFrustumPlanes[5][1] = mvp[7] + mvp[6];
    aFrustumPlanes[5][2] = mvp[11] + mvp[10];
    aFrustumPlanes[5][3] = mvp[15] + mvp[14];

    t = (float)sqrt(aFrustumPlanes[5][0] * aFrustumPlanes[5][0] +
        aFrustumPlanes[5][1] * aFrustumPlanes[5][1] +
        aFrustumPlanes[5][2] * aFrustumPlanes[5][2]);

    aFrustumPlanes[5][0] /= t;
    aFrustumPlanes[5][1] /= t;
    aFrustumPlanes[5][2] /= t;
    aFrustumPlanes[5][3] /= t;

    Log::Debug("calcFrustumPlanes---start\n");
    for (int k=0;k < (int)aFrustumPlanes.size();k++ )
    {
        auto& plane = aFrustumPlanes.at(k);
        Log::Debug("%f %f %f %f\n", plane[0], plane[1], plane[2], plane[3]);
    }
    Log::Debug("calcFrustumPlanes---end\n");
}

bool isBoundingSphereInFrustum(std::array<std::array<float, 4>, 6>& aFrustumPlanes,
    float x, float y, float z)
{
    for (int i = 0; i < 6; ++i)
    {
        if (aFrustumPlanes[i][0] * x +
            aFrustumPlanes[i][1] * y +
            aFrustumPlanes[i][2] * z +
            aFrustumPlanes[i][3] <= 0)
            return false;
    }

    return true;
}
#endif








////////////////////////////////////////



 
} // namespace MdU