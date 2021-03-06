#pragma once
#include "core.h"
#include "geom.h"
#include "image.h"
#include "node.h"
#include "embree.h"
#include <vector>

/**
 * Manages rendering by simulating the action of a physical pinhole camera.
 */
class Camera {
  /**
   * The number of bounces at which a ray is subject to Russian Roulette
   * termination, stage 1 (less aggressive).
   */
  static constexpr int RUSSIAN_ROULETTE_DEPTH_1 = 5;
  /**
   * The number of bounces at which a ray is subject to Russian Roulette
   * termination, stage 2 (more aggressive).
   */
  static constexpr int RUSSIAN_ROULETTE_DEPTH_2 = 50;
  /**
   * The amount of memory to initially allocate for each path.
   */
  static constexpr int INITIAL_PATH_LENGTH = 50;
  /**
   * Limits any given sample to the given amount of radiance. This helps to
   * reduce "fireflies" in the output. The lower this value, the more bias will
   * be introduced into the image. For unbiased rendering, set this to
   * std::numeric_limits<float>::max().
   */
  static constexpr float BIASED_RADIANCE_CLAMPING = 50.0f;

  Embree accel; /**< The accelerator containing renderable geometry. */
  std::vector<const Geom*> emitters; /**< List of all light emitters. */

  const float focalLength; /**< The distance from the eye to the focal plane. */
  const float lensRadius; /**< The radius of the lens opening. */
  const Transform camToWorldXform; /**< Transform from camera to world space. */

  float focalPlaneUp; /**< The height of the focal plane. */
  float focalPlaneRight; /**< The width of the focal plane. */
  Vec focalPlaneOrigin; /**< The origin (corner) of the focal plane. */

  Randomness masterRng; /**< The RNG used to seed the per-row RNGs. */
  std::vector<unsigned> rowSeeds; /**< The per-row RNG seeds. */

  Image img; /**< The rendered and filtered image. */

  int iters; /** The current number of path-tracing iterations done. */

  /**
   * Traces a path starting with the given ray, and returns the sampled
   * radiance.
   *
   * @param rng                the per-thread RNG in use
   * @param r                  the ray that starts the path
   * @param sharedEyePath [in] a shared structure used to store the eye path
   *                           while tracing the sample
   * @returns                  the sampled radiance of the path
   */
   Vec trace(
     Randomness& rng,
     const Ray& r,
     std::vector<RenderVertex>& sharedEyePath
   ) const;

  /**
   * Constructs a path by randomly walking the scene from the given initial ray.
   *
   * @param rng           the per-thread RNG in use
   * @param r             the ray that starts the path
   * @param beta          the throughput at the initial point in the path
   * @param path [in,out] a shared structure used to store the constructed path
   */
  void randomWalk(
    Randomness& rng,
    Ray r,
    Vec beta,
    std::vector<RenderVertex>& path
  ) const;

  /**
   * Randomly picks a light and samples it for direct illumination. The
   * radiance returned will be scaled according to the probability of picking
   * the light.
   *
   * @param rng         the per-thread RNG in use
   * @param isect       the intersection on the target geometry that should be
   *                    illuminated
   */
  Vec uniformSampleOneLight(
    Randomness& rng,
    const Intersection& isect
  ) const;

public:
  /**
   * Constructs a camera.
   *
   * @param xform  the transformation from camera space to world space
   * @param objs   the objects to render
   * @param ww     the width of the output image, in pixels
   * @param hh     the height of the output image, in pixels
   * @param fov    the field of view (horizontal or vertical, whichever is
   *               smaller), in radians
   * @param len    the focal length of the lens
   * @param fStop the f-stop (aperture) of the lens
   */
  Camera(
    const Transform& xform,
    const std::vector<const Geom*>& objs,
    int ww,
    int hh,
    float fov = math::PI_4,
    float len = 50.0f,
    float fStop = 16.0f
  );

  /**
   * Constructs a camera from the given node.
   */
   Camera(const Node& n);

  /**
   * Renders an additional iteration of the image by path-tracing.
   * If there are existing iterations, the additional iteration will be
   * combined in a weighted manner.
   *
   * @param name    the name of the output EXR file
   */
  void renderOnce(
    std::string name
  );

  /**
   * Renders multiple additional path-tracing iterations.
   * To render infinite iterations, specify iterations = -1.
   *
   * @param name       the name of the output EXR file
   * @param iterations the number of iterations to render; if < 0, then this
   *                   function will run forever
   */
  void renderMultiple(
    std::string name,
    int iterations
  );
};
