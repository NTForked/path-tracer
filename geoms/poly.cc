#include "poly.h"

geoms::Poly::Poly(
  const Point* a,
  const Point* b,
  const Point* c,
  const Material* m,
  const AreaLight* l
) : Geom(m, l), pt0(a), pt1(b), pt2(c) {}

geoms::Poly::Poly(const geoms::Poly& other)
  : Geom(other.mat, other.light),
    pt0(other.pt0), pt1(other.pt1), pt2(other.pt2) {}

bool geoms::Poly::intersect(const Ray& r, Intersection* isectOut) const {
  // Uses the Moller-Trumbore intersection algorithm.
  // See <http://en.wikipedia.org/wiki/
  // M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm> for more info.

  const Vec edge1 = pt1->position - pt0->position;
  const Vec edge2 = pt2->position - pt0->position;

  const Vec p = r.direction.cross(edge2);
  const float det = edge1.dot(p);

  if (math::isNearlyZero(det)) {
    return false; // No hit on plane.
  }

  const float invDet = 1.0f / det;
  const Vec t = r.origin - pt0->position;

  const float u = t.dot(p) * invDet;
  if (u < 0.0f || u > 1.0f) {
    return false; // In plane but not triangle.
  }

  const Vec q = t.cross(edge1);
  const float v = r.direction.dot(q) * invDet;
  if (v < 0.0f || (u + v) > 1.0f) {
    return false; // In plane but not triangle.
  }

  const float dist = edge2.dot(q) * invDet;
  if (!math::isPositive(dist)) {
    return false; // In triangle but behind us.
  }

  const float w = 1.0f - u - v;

  Intersection isect(
    /* position */
      r.at(dist),
    /* normal */
      (w * pt0->normal + u * pt1->normal + v * pt2->normal).normalized(),
    /* incomingRay */
      r,
    /* geom */
      this,
    /* distance */
      dist
  );

  *isectOut = isect;
  return true;
}

bool geoms::Poly::intersectShadow(const Ray& r, float maxDist) const {
  // Uses the Moller-Trumbore intersection algorithm.
  // See <http://en.wikipedia.org/wiki/
  // M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm> for more info.

  const Vec edge1 = pt1->position - pt0->position;
  const Vec edge2 = pt2->position - pt0->position;

  const Vec p = r.direction.cross(edge2);
  const float det = edge1.dot(p);

  if (math::isNearlyZero(det)) {
    return false; // No hit on plane.
  }

  const float invDet = 1.0f / det;
  const Vec t = r.origin - pt0->position;

  const float u = t.dot(p) * invDet;
  if (u < 0.0f || u > 1.0f) {
    return false; // In plane but not triangle.
  }

  const Vec q = t.cross(edge1);
  const float v = r.direction.dot(q) * invDet;
  if (v < 0.0f || (u + v) > 1.0f) {
    return false; // In plane but not triangle.
  }

  const float dist = edge2.dot(q) * invDet;
  if (!math::isPositive(dist) || !math::isPositive(maxDist - dist)) {
    return false; // In triangle but not in the right range.
  }

  return true;
}

BBox geoms::Poly::boundBox() const {
  BBox b(pt0->position, pt1->position);
  b.expand(pt2->position);

  return b;
}
