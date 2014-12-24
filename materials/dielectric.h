#pragma once
#include "../material.h"

namespace materials {

  /**
   * A dielectric (nonconductive) material following the Fresnel equations.
   * This material provides for both reflection and refraction.
   */
  class dielectric : public material {
    /** The refractive index for a vacuum (approx. air), nVac. */
    static constexpr float IOR_VACUUM = 1.0f;

    float r0; /** The cached R(0) value for Schlick's approximation. */
    const float etaEntering; /** The refraction ratio nVac / nMaterial. */
    const float etaExiting; /** Inverse of etaEntering (nMaterial / nVac). */

  protected:
    virtual vec sampleBSDF(
      randomness& rng,
      const vec& incoming,
      vec* outgoingOut,
      float* probabilityOut
    ) const;

    virtual vec evalBSDF(
      const vec& incoming,
      const vec& outgoing
    ) const;
  
  public:
    static constexpr float IOR_GLASS = 1.5f; /**< The IOR for glass. */
    static constexpr float IOR_DIAMOND = 2.4f; /**< The IOR for diamond. */

    /** The reflection and refraction color of the material. */
    const vec color;

    /**
     * Creates a dielectric material.
     *
     * @param ior the index of reflection on the inside of the material
     * @param c   the color of the material
     */
    dielectric(float ior = IOR_GLASS, vec c = vec(1, 1, 1));
  };

}
