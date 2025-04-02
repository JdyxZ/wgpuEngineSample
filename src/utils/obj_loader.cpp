// Internal Headers
#include "core/core.hpp"
#include "obj_loader.hpp"
#include "math/vec3.hpp"
#include "hittables/mesh.hpp"
#include "hittables/surface.hpp"
#include "hittables/triangle.hpp"
#include "hittables/hittable_list.hpp"
#include "materials/material.hpp"
#include "materials/texture.hpp"

// Macros
#define TINYOBJLOADER_IMPLEMENTATION

// External Headers
#include "tiny_obj_loader.h"

// Usings
using Raytracing::Mesh;
using Raytracing::Surface;
using Raytracing::Material;
using Raytracing::Lambertian;
using Raytracing::ImageTexture;
using Raytracing::color;

shared_ptr<Mesh> load_obj(const string& filename)
{
    // Create tiny obj reader object
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    // Define pahts
    auto obj_path = models_path + filename;
    auto obj_path_fs = fs::path(obj_path);

    // Try to load obj file data into reader object
    if (!reader.ParseFromFile(obj_path, reader_config))
    {
        if (!reader.Error().empty())
            Logger::error("TinyObjReader", reader.Error());

        return nullptr;
    }

    // Output loading warnings
    if (!reader.Warning().empty())
        Logger::warn("TinyObjReader", reader.Warning());

    // Obj data
    auto& attrib = reader.GetAttrib(); // Vertex data (position, normal, color, texture coordinates)
    auto& shapes = reader.GetShapes(); // Submeshes inside the obj
    auto& materials = reader.GetMaterials(); // Materials included in the obj

    // Mesh vars
    shared_ptr<hittable_list> surfaces = make_shared<hittable_list>();
    vector<string> texture_names;
    vector<string> material_names;

    // Loop over shapes (surfaces)
    for (size_t s = 0; s < shapes.size(); s++)
    {
        // Create new shape
        shared_ptr<Surface> surface;
        shared_ptr<Material> material;
        shared_ptr<hittable_list> triangles = make_shared<hittable_list>();

        if (!materials.empty())
        {
            // Get shape material id
            uint32_t material_id = shapes[s].mesh.material_ids[0];
            material_id = material_id == -1 ? 0 : material_id;

            // If there is no diffuse texture, create one
            if (materials[material_id].diffuse_texname.empty())
            {
                const color albedo = color(static_cast<double>(materials[material_id].diffuse[0]),
                    static_cast<double>(materials[material_id].diffuse[1]),
                    static_cast<double>(materials[material_id].diffuse[2]));

                material = make_shared<Lambertian>(albedo);
            }
            // Else, load diffuse texture
            else
            {
                auto texture_name = materials[material_id].diffuse_texname;
                auto texture = make_shared<ImageTexture>(obj_path_fs.parent_path().string() + "/" + texture_name);
                material = make_shared<Lambertian>(texture);
                texture_names.push_back(texture_name);
            }
        }

        size_t index_offset = 0;

        // Loop over faces (primitives / polygon)
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            // Number of vertices per face
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Ignore non-triangle primitives
            if (fv != 3)
                continue;

            // Create empty triangle
            shared_ptr<Triangle> triangle;
            vertex vertices[3];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                // Vertex data pointer
                vertex& current_vertex = vertices[v];

                // Vertex position
                tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
                current_vertex.position = point3(vx, vy, vz);

                // If normal_index is negative, there is no normal data
                if (idx.normal_index >= 0)
                {
                    // Vertex normal
                    tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                    tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                    tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                    current_vertex.normal = point3(nx, ny, nz);
                }

                // If texcoord_index is negative, there is no texture coordinate data
                if (idx.texcoord_index >= 0)
                {
                    // Vertex texture coordinate
                    tinyobj::real_t u = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    tinyobj::real_t v = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                    current_vertex.uv = make_pair(u, v);
                }

                // Vertex color
                tinyobj::real_t red = attrib.colors[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t green = attrib.colors[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t blue = attrib.colors[3 * size_t(idx.vertex_index) + 2];
                current_vertex.color = color(red, green, blue);
            }

            // Create and add triangle
            triangle = make_shared<Triangle>(vertices[0], vertices[1], vertices[2], material);
            triangles->add(triangle);

            index_offset += fv;

            // per-face material
            // shapes[s].mesh.material_ids[f];
        }

        // Create and add surface
        surface = make_shared<Surface>(triangles, material);
        surfaces->add(surface);
    }

    // Create mesh
    auto mesh = make_shared<Mesh>(filename, surfaces, material_names, texture_names);

    return mesh;
}
