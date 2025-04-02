// Namespace forward declarations
namespace Raytracing
{
    class Scene;
    class Camera;
    class ImageWriter;
}

namespace Raytracing
{
	void book1_final_scene_creation(Scene& scene, bool blur_motion = false);
	void book1_final_scene(Scene& scene, Camera& camera, ImageWriter& image);
	void bouncing_spheres(Scene& scene, Camera& camera, ImageWriter& image);
	void checkered_spheres(Scene& scene, Camera& camera, ImageWriter& image);
	void earth(Scene& scene, Camera& camera, ImageWriter& image);
	void perlin_spheres(Scene& scene, Camera& camera, ImageWriter& image);
	void quads_scene(Scene& scene, Camera& camera, ImageWriter& image);
	void simple_light(Scene& scene, Camera& camera, ImageWriter& image);
	void cornell_box(Scene& scene, Camera& camera, ImageWriter& image);
	void cornell_smoke(Scene& scene, Camera& camera, ImageWriter& image);
	void book2_final_scene(Scene& scene, Camera& camera, ImageWriter& image);
	void obj_test(Scene& scene, Camera& camera, ImageWriter& image);
}
