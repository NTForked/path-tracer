#include "mesh.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "../debug.h"

geoms::Mesh::Mesh(Vec o, Material* m, AreaLight* l)
  : Geom(m, l), points(), faces(), origin(o) {}

bool geoms::Mesh::readPolyModel(std::string name) {
  // Create an instance of the Importer class
  Assimp::Importer importer;

  // And have it read the given file with some example postprocessing.
  const aiScene* scene = importer.ReadFile(
    name,
    aiProcess_Triangulate
    | aiProcess_JoinIdenticalVertices
    | aiProcess_SortByPType
    | aiProcess_GenNormals
    | aiProcess_PreTransformVertices
    | aiProcess_ValidateDataStructure
  );

  // If the import failed, report it
  if (!scene) {
    std::cerr << "Importer error: " << importer.GetErrorString() << "\n";
    return false;
  }

  // Now we can access the file's contents.
  if (scene->mNumMeshes > 0) {
    // Process first mesh only right now.
    // TODO: process multiple meshes.
    aiMesh* mesh = scene->mMeshes[0];

    if (!mesh->HasPositions()) {
      std::cerr << "Error: no vertex positions on the mesh.\n";
      return false;
    }

    if (!mesh->HasNormals()) {
      std::cerr << "Error: no vertex normals on the mesh.\n";
      return false;
    }

    // We have something to work with! Clear the existing data.
    clear();

    // Add points.
    for (size_t i = 0; i < mesh->mNumVertices; ++i) {
      aiVector3D thisPos = mesh->mVertices[i];
      aiVector3D thisNorm = mesh->mNormals[i];

      geoms::Poly::Point thisPoint;
      thisPoint.position = Vec(thisPos.x, thisPos.y, thisPos.z) + origin;
      thisPoint.normal =
        Vec(thisNorm.x, thisNorm.y, thisNorm.z).normalized();

      points.push_back(thisPoint);
    }

    // Add faces.
    for (size_t i = 0; i < mesh->mNumFaces; ++i) {
      aiFace face = mesh->mFaces[i];

      // Only add the triangles (we should have a triangulated mesh).
      if (face.mNumIndices == 3) {
        geoms::Poly thisPoly(
          mem::ID(face.mIndices[0]),
          mem::ID(face.mIndices[1]),
          mem::ID(face.mIndices[2]),
          &points,
          mat,
          light
        );

        faces.push_back(thisPoly);
      }
    }
  }

  // We're done. Everything will be cleaned up by the importer destructor.
  return true;
}

void geoms::Mesh::clear() {
  points.clear();
  faces.clear();
}

bool geoms::Mesh::intersect(
  const Ray& /* r */,
  Intersection* /* isectOut */
) const {
  return debug::shouldNotReach(false);
}

bool geoms::Mesh::intersectShadow(
  const Ray& /* r */,
  float /* maxDist */
) const {
  return debug::shouldNotReach(false);
}

BBox geoms::Mesh::bounds() const {
  return debug::shouldNotReach(BBox());
}

Vec geoms::Mesh::samplePoint(Randomness& /* rng */) const {
  return debug::shouldNotReach(Vec(0, 0, 0));
}

float geoms::Mesh::area() const {
  return debug::shouldNotReach(0.0f);
}

void geoms::Mesh::refine(std::vector<const Geom*>& refined) const {
  for (const Poly& p : faces) {
    refined.push_back(&p);
  }
}