
// Este código es de dominio público
// angel.rodriguez@udit.es

#ifndef CAMERA_HEADER
#define CAMERA_HEADER

#include <glm.hpp>                          // vec3, vec4, ivec4, mat4
#include <gtc/matrix_transform.hpp>         // translate, rotate, scale, perspective


namespace PracticaKatya
{

    class Camera
    {
        using Point = glm::vec4;
        using Vector = glm::vec4;
        using Matrix44 = glm::mat4;

    private:

        float    fov;
        float    near_z;
        float    far_z;
        float    ratio;

        Point    location;
        float    rotation_x;
        float    rotation_y;

        Matrix44 projection_matrix;

    public:

        Camera(float ratio = 1.f)
        {
            reset(60.f, 0.1f, 1000.f, ratio);
        }

        Camera(float near_z, float far_z, float ratio = 1.f)
        {
            reset(60.f, near_z, far_z, ratio);
        }

        Camera(float fov_degrees, float near_z, float far_z, float ratio)
        {
            reset(fov_degrees, near_z, far_z, ratio);
        }

    public:

        float         get_fov()     const { return fov; }
        float         get_near_z()  const { return near_z; }
        float         get_far_z()   const { return far_z; }
        float         get_ratio()   const { return ratio; }
        float		  getRotation_x() const { return    rotation_x; }
        float		  getRotation_y() const { return    rotation_y; }

        const Point& get_location() const { return location; }

    public:

        void set_fov(float new_fov) { fov = new_fov;    calculate_projection_matrix(); }
        void set_near_z(float new_near_z) { near_z = new_near_z; calculate_projection_matrix(); }
        void set_far_z(float new_far_z) { far_z = new_far_z;  calculate_projection_matrix(); }
        void set_ratio(float new_ratio) { ratio = new_ratio;  calculate_projection_matrix(); }

        void set_location(float x, float y, float z) { location[0] = x; location[1] = y; location[2] = z; }
        void setRotation_x(float newRotation_x) { rotation_x = newRotation_x; calculate_projection_matrix(); }
        void setRotation_y(float newRotation_y) { rotation_y = newRotation_y; calculate_projection_matrix(); }
        
        void reset(float new_fov, float new_near_z, float new_far_z, float new_ratio)
        {
            set_fov(new_fov);
            set_near_z(new_near_z);
            set_far_z(new_far_z);
            set_ratio(new_ratio);
            set_location(0.f, 0.f, 0.f);
            setRotation_x(0.f);
            setRotation_y(0.f);
            calculate_projection_matrix();
        }

    public:

        void move(const glm::vec3& translation)
        {
            location += glm::vec4(translation, 1.f);
        }

        void rotate(float deltaRotation_x, float deltaRotation_y)
        {
            rotation_x += deltaRotation_x;
            rotation_y += deltaRotation_y;

            if (rotation_x > glm::radians(89.f)) rotation_x = glm::radians(89.f);
            if (rotation_x < glm::radians(-89.f)) rotation_x = glm::radians(-89.f);
        }

    public:

        const glm::mat4& get_projection_matrix() const
        {
            return projection_matrix;
        }

        glm::mat4 get_transform_matrix_inverse() const
        {
            glm::vec3 direction
            (
                cos(rotation_x) * sin(rotation_y),
                sin(rotation_x),
                cos(rotation_x) * cos(rotation_y)
            );
            
            glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), direction));
            glm::vec3 up = glm::normalize(glm::cross(direction, right));

            return glm::lookAt(glm::vec3(location), glm::vec3(location) + direction, up);
        }

    private:

        void calculate_projection_matrix()
        {
            projection_matrix = glm::perspective(glm::radians(fov), ratio, near_z, far_z);
        }

    };

}

#endif
