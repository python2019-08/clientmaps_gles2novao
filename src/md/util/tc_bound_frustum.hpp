// Frustum Culling
#pragma once
#include <array>

#include <md/util/geo.hpp>
#include <glm/glm.hpp> 

namespace clientmap {


enum class IntersectionResult : int 
{
    Separate,
    Intersects,
    Contains,
};

class AABB 
{
public:
    AABB();
    AABB(const glm::dvec3& min_, const glm::dvec3& max_);

    // gm:should named clampPoint
    glm::dvec3 closestPoint(const glm::dvec3& point) const;

    // Computes the shortest manhattan distance to the provided point
    glm::dvec3 distanceXYZ(const glm::dvec3& point) const;

    // Creates an aabb covering one quadrant of the aabb on XZ-plane.
    AABB quadrant(int idx) const;

    bool intersects(const AABB& aabb) const;

    bool operator==(const AABB& aabb) const;
    bool operator!=(const AABB& aabb) const;

    glm::dvec3 min;
    glm::dvec3 max;
};

class Frustum 
{
public:
    Frustum(const std::array<glm::dvec3, 8>& points_,
            const std::array<glm::dvec4, 6>& planes_);

    static Frustum fromInvProjMatrix(const glm::dmat4& invProj,
                                     double worldSize, 
                                     double zoom, 
                                     bool flippedY = false
                                     );
    static void calcFrustumPlanes(const std::array<float, 16>&     p,
        const std::array<float, 16>&    mv,
        std::array<std::array<float, 4>, 6>& aFrustumPlanes);
    // Performs conservative intersection test using 
    // separating axis theorem.
    // Some accuracy is traded for better performance. 
    // False positive rate is < 1%
    IntersectionResult intersects(const AABB& aabb) const;

    // Performs precise intersection test using separating axis theorem.
    // It is possible run only edge cases that were not covered 
    // in intersects()
    IntersectionResult 
    intersectsPrecise(const AABB& aabb, 
                                    bool edgeCasesOnly = false
                                    ) const;

    const std::array<glm::dvec3, 8>& getPoints() const {
        return mPts;
    }
    const std::array<glm::dvec4, 6>& getPlanes() const {
        return m_planes;
    }

private:

    struct Projection {
        glm::dvec3 axis;
        Pt2D  projection;
    };

    AABB m_bounds;
    std::array<glm::dvec3, 8> mPts;
    std::array<glm::dvec4, 6> m_planes;
    std::array<Projection, 12> m_projections;
};
 
} // namespace md