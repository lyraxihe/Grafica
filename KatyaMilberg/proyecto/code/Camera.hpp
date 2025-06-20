/// @author Katya
/// @copyright () Katya

#ifndef CAMERA_HEADER
#define CAMERA_HEADER

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>


namespace PracticaKatya
{
    /// @brief Esta clase gestiona el código de la cámara.
    /// Movimiento y visión.

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
        /// @brief inicializa la cámara
        /// @param ratio El valor de radio
        Camera(float ratio = 1.f)
        {
            reset(60.f, 0.1f, 1000.f, ratio);
        }

        /// @brief inicializa la cámara
        /// @param near_z El valor de cercanía
        /// @param far_z  El valor de lejanía
        /// @param ratio  El valor de radio (1.f)
        Camera(float near_z, float far_z, float ratio = 1.f)
        {
            reset(60.f, near_z, far_z, ratio);
        }

        /// @brief inicializa la cámara
        /// @param fov_degrees El valor en grados del fov
        /// @param near_z      El valor de cercanía
        /// @param far_z       El valor de lejanía
        /// @param ratio       El valor de radio
        Camera(float fov_degrees, float near_z, float far_z, float ratio)
        {
            reset(fov_degrees, near_z, far_z, ratio);
        }

    public:
        /// Retorna el fov de la cámara
        /// @return El fov de la cámara
        float         get_fov()     const { return fov; }

        /// Retorna el near_z de la cámara
        /// @return El near_z de la cámara
        float         get_near_z()  const { return near_z; }

        /// Retorna el far_z de la cámara
        /// @return El far_z de la cámara
        float         get_far_z()   const { return far_z; }

        /// Retorna el radio de la cámara
        /// @return El ratio de la cámara
        float         get_ratio()   const { return ratio; }

        /// Retorna la rotación "X" de la cámara
        /// @return La rotation_x de la cámara
        float		  getRotation_x() const { return    rotation_x; }

        /// Retorna la rotation "Y" de la cámara
        /// @return La rotation_y de la cámara
        float		  getRotation_y() const { return    rotation_y; }

        /// Retorna las coordenadas en tipo point de la cámara
        /// @return Location de la cámara
        const Point& get_location() const { return location; }

    public:
        /// Asigna el fov de la cámara
        /// @param new_fov El fov a asignar para la cámara
        void set_fov(float new_fov) { fov = new_fov;    calculate_projection_matrix(); }

        /// Asigna el near_z de la cámara
        /// @param new_near_z El near_z a asignar para la cámara
        void set_near_z(float new_near_z) { near_z = new_near_z; calculate_projection_matrix(); }

        /// Asigna el far_z de la cámara
        /// @param new_far_z El far_z a asignar para la cámara
        void set_far_z(float new_far_z) { far_z = new_far_z;  calculate_projection_matrix(); }

        /// Asigna el ratio de la cámara
        /// @param new_ratio El ratio a asignar para la cámara
        void set_ratio(float new_ratio) { ratio = new_ratio;  calculate_projection_matrix(); }

        /// Asigna la location de la cámara
        /// @param x El location[0] a asignar para la cámara
        /// @param y El location[1] a asignar para la cámara
        /// @param z El location[2] a asignar para la cámara
        void set_location(float x, float y, float z) { location[0] = x; location[1] = y; location[2] = z; }
        
        /// Asigna la rotation_x de la cámara y recalcula la projection matrix
        /// @param newRotation_x La rotation_x a asignar para la cámara
        void setRotation_x(float newRotation_x) { rotation_x = newRotation_x; calculate_projection_matrix(); }
        
        /// Asigna la rotation_y de la cámara y recalcula la projection matrix
        /// @param newRotation_y La rotation_x a asignar para la cámara
        void setRotation_y(float newRotation_y) { rotation_y = newRotation_y; calculate_projection_matrix(); }
        
        /// Asigna la el fov, near_z, far_z, ratio, location, rotation_x, rotation_y y recalcula la projection matrix
        /// @param new_fov El fov a asignar para la cámara
        /// @param new_near_z El near_z a asignar para la cámara
        /// @param new_far_z El far_z a asignar para la cámara
        /// @param new_ratio El ratio a asignar para la cámara
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

        /// Suma valor a la location
        /// @param translation La translación a aplicar a la cámara
        void move(const glm::vec3& translation)
        {
            location += glm::vec4(translation, 1.f);
        }
        /// Suma valor a la rotación "x" e "y"
        /// @param deltaRotation_x La rotación en x a aplicar a la cámara
        /// @param deltaRotation_y La rotación en y a aplicar a la cámara
        void rotate(float deltaRotation_x, float deltaRotation_y)
        {
            rotation_x += deltaRotation_x;
            rotation_y += deltaRotation_y;

            if (rotation_x > glm::radians(89.f)) rotation_x = glm::radians(89.f);
            if (rotation_x < glm::radians(-89.f)) rotation_x = glm::radians(-89.f);
        }

        /// Retorna la projection_matrix de la cámara
        /// @return La projection_matrix de la cámara
        const glm::mat4& get_projection_matrix() const
        {
            return projection_matrix;
        }

        /// Retorna el transform matrix de la cámara
        /// @return El transform matrix de la cámara
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

        /// Calcula la matriz de proyección 
        void calculate_projection_matrix()
        {
            projection_matrix = glm::perspective(glm::radians(fov), ratio, near_z, far_z);
        }

    };

}

#endif
