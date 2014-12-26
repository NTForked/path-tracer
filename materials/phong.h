#pragma once
#include "bsdf.h"

namespace materials {

  /**
   * A glossy reflective material using Phong reflectance.
   * Note that Phong reflectance is not physically plausible.
   */
  class Phong : public BSDF {
  protected:
    virtual Vec sampleBSDF(
      Randomness& rng,
      const Vec& incoming,
      Vec* outgoingOut,
      float* probabilityOut
    ) const override;

    virtual Vec evalBSDF(
      const Vec& incoming,
      const Vec& outgoing
    ) const override;

    /** Cached scaling term in the Phong BRDF. */
    const Vec scaleBRDF;

    /** Cached scaling term in the PDF. */
    const float scaleProb;

    /** Cached inverse exponent term. */
    const float invExponent;

  public:
    /** The Phong exponent of the material. */
    const float exponent;

    /** The color of the material. */
    const Vec color;

    /**
     * Constructs a Phong material.
     *
     * @param e the Phong exponent (e >= 1); 1 = almost perfectly diffuse,
     *          ~1000 = almost perfectly specular, lower values are more glossy
     * @param c the color of the material
     */
    Phong(float e, Vec c = Vec(1, 1, 1));
  };

}
