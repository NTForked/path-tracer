#pragma once
#include "core.h"
#include "geom.h"
#include "material.h"
#include "kdtree.h"

/**
 * A diffuse area light that causes radiance to be emitted from a piece of
 * solid geometry.
 */
class AreaLight {
public:
  const Vec color; /**< The color of the light emitted. */

  /**
   * Constructs a light that emits the specified color.
   */
  AreaLight(Vec c);

  /**
   * Computes the direct illumination from a random point on a piece of solid
   * geometry (the emitter) onto another piece of geometry (the reflector) at
   * the specified intersection point.
   *
   * @param rng         the per-thread RNG in use
   * @param incoming    the ray coming into the intersection on the target
   *                    geometry (on the reflector)
   * @param isect       the intersection on the target geometry that should be
   *                    illuminated
   * @param mat         the material of the target geometry being illuminated
   * @param emissionObj the object doing the illuminating (the emitter)
   * @param kdt         the k-d tree containing all geometry in the scene
   */
  Vec directIlluminate(
    Randomness& rng,
    const Ray& incoming,
    const Intersection& isect,
    const Material* mat,
    const Geom* emissionObj,
    const KDTree& kdt
  ) const;
};