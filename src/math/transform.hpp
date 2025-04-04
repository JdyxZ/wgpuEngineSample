// Headers
#include "core/core.hpp"
#include "core/framework.hpp"

// Forward declarations
struct Quaternion;
class vec3;

// Namespace forward declarations
namespace Raytracing
{
    class Matrix44;
}

namespace Raytracing
{
    struct Transform
    {
    public:
	    Transform();
	    Transform(const vec3& translation, const Quaternion& rotation, const vec3& scailing);

	    shared_ptr<Transform> inverse();

	    void set_translation(const vec3& translation);
	    void set_rotation(const Quaternion& rotation);
        void set_rotation(vec3 axis, const double& angle);
	    void set_scaling(const vec3& scailing);

	    void set_recompute(bool recompute);
	    bool needs_recompute() const;

	    void cache_model();
	    shared_ptr<Raytracing::Matrix44> get_model();

	     static const shared_ptr<Raytracing::Matrix44> transform_matrix(const vec3& translation = vec3(0), vec3 axis = x_axis, const double& angle = 0, const vec3& scailing = vec3(1));

    private:
	    bool recompute = false;
	    shared_ptr<vec3> translation;
	    shared_ptr<Quaternion> rotation;
	    shared_ptr<vec3> scaling;
	    shared_ptr<Raytracing::Matrix44> model;

	    shared_ptr<Raytracing::Matrix44> compute_model();
    };
}
