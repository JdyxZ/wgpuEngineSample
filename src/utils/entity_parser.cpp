// Headers
#include "entity_parser.hpp"
#include "hittables/mesh.hpp"
#include "hittables/surface.hpp"
#include "math/matrix.hpp"
#include "materials/material.hpp"
#include "materials/texture.hpp"
#include "graphics/color.hpp"
#include "hittables/triangle.hpp"
#include "hittables/hittable_list.hpp"
#include "graphics/texture.h"

// Usings
using Raytracing::Mesh;
using Raytracing::Matrix44;
using Raytracing::ImageTexture;
using Raytracing::Lambertian;
using Raytracing::color;
using Raytracing::normal;

vector<shared_ptr<Mesh>> parse_nodes(const vector<Node*>& nodes)
{
    vector<shared_ptr<Mesh>> meshes;

    for (auto node : nodes)
    {
        auto node_meshes = parse_node(node);

        if (!node_meshes.empty())
            meshes.insert(meshes.end(), node_meshes.begin(), node_meshes.end());
    }

    return meshes;
}

vector<shared_ptr<Mesh>> parse_node(Node* node)
{
    vector<shared_ptr<Mesh>> meshes;

    std::function<void(Node*)> parse = [&](Node* node)
    {
        MeshInstance3D* scene_mesh = dynamic_cast<MeshInstance3D*>(node);

        if (scene_mesh)
        {
            // Name
            string name = scene_mesh->get_name();

            // Surfaces
            shared_ptr<hittable_list> surfaces = make_shared<hittable_list>();

            for (auto surface : scene_mesh->get_surfaces())
            {
                auto new_surface = parse_surface(surface);
                surfaces->add(new_surface);
            }

            // Model Matrix
            auto model = make_shared<Matrix44>(scene_mesh->get_global_model());

            // Mesh
            auto new_mesh = make_shared<Mesh>(name, surfaces, model);
            meshes.push_back(new_mesh);
        }

        if (!node->get_children().empty())
        {
            for (auto child : node->get_children())
            {
                parse(child);
            }
        }
    };

    parse(node);

    return meshes;
}

shared_ptr<Raytracing::Surface> parse_surface(Surface* surface)
{
    // Material
    shared_ptr<Raytracing::Material> parsed_material;
    Material* material = surface->get_material();
    Texture* diffuse_texture = material->get_diffuse_texture();

    // Load diffuse texture
    if (diffuse_texture)
    {
        sTextureData& texture_data = diffuse_texture->get_texture_data();
        auto texture = make_shared<ImageTexture>(texture_data);
        parsed_material = make_shared<Lambertian>(texture);
    }
    // If there is no diffuse texture, create one
    else
    {
        const color albedo = color(material->get_color());
        parsed_material = make_shared<Lambertian>(albedo);
    }

    // Triangles
    shared_ptr<hittable_list> triangles = make_shared<hittable_list>();
    sSurfaceData& surface_data = surface->get_surface_data();

    // Iterate vertices

    if (surface_data.indices.empty())
    {
        auto triangle_vertices = vector<vertex>(3);
        triangles->reserve(surface_data.vertices.size() / 3);

        for (size_t i = 0; i < surface_data.vertices.size(); i++)
        {
            // Create and add triangle
            if (i != 0 && i % 3 == 0)
            {
                auto triangle = make_shared<Triangle>(triangle_vertices[0], triangle_vertices[1], triangle_vertices[2], parsed_material);
                triangles->add(triangle);
            }

            // Create vertex
            vertex v;

            // Position
            v.position = point3(surface_data.vertices[i]);

            // Normal
            v.normal = !surface_data.normals.empty() ? std::optional<normal>(surface_data.normals[i]) : std::nullopt;

            // Texture coordinates
            v.uv = !surface_data.uvs.empty() ? std::optional<pair<double, double>>(make_pair(surface_data.uvs[i].x, surface_data.uvs[i].y)) : std::nullopt;

            // Color
            v.color = !surface_data.colors.empty() ? std::optional<color>(surface_data.colors[i]) : std::nullopt;

            triangle_vertices[i % 3] = v;
        }
    }
    else
    {
        auto triangle_vertices = vector<vertex>(3);
        triangles->reserve(surface_data.vertices.size() / 3);

        for (size_t i = 0; i < surface_data.indices.size(); i++)
        {
            // Create and add triangle
            if (i != 0 && i % 3 == 0)
            {
                auto triangle = make_shared<Triangle>(triangle_vertices[0], triangle_vertices[1], triangle_vertices[2], parsed_material);
                triangles->add(triangle);
            }

            // Create vertex
            vertex v;

            size_t surface_index = surface_data.indices[i];

            // Position
            v.position = point3(surface_data.vertices[surface_index]);

            // Normal
            v.normal = !surface_data.normals.empty() ? std::optional<normal>(surface_data.normals[surface_index]) : std::nullopt;

            // Texture coordinates
            v.uv = !surface_data.uvs.empty() ? std::optional<pair<double, double>>(make_pair(surface_data.uvs[surface_index].x, surface_data.uvs[surface_index].y)) : std::nullopt;

            // Color
            v.color = !surface_data.colors.empty() ? std::optional<color>(surface_data.colors[surface_index]) : std::nullopt;

            triangle_vertices[i % 3] = v;
        }
    }

    // Parsed surface
    auto parsed_surface = make_shared<Raytracing::Surface>(triangles, parsed_material);

    return parsed_surface;
}
