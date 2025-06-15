/// @author Katya
/// @copyright () Katya

#ifndef OBJECT_HEADER
#define OBJECT_HEADER

#include <glad/glad.h>
#include <glm.hpp>
#include <string>
#include "Shader.hpp"


namespace PracticaKatya
{
    /// @brief Esta clase gestiona el código de los objetos en la escena.
    /// Les aplica su textura, los renderiza y permite moverlos, rotarlos y escalarlos.
    class Object
    {
    private:

        enum
        {
            COORDINATES_VBO,
            TEXCOORDS_VBO,
            INDICES_EBO,
            NORMALS_VBO,
            VBO_COUNT
        };

        GLuint  vao_id;
        GLuint  vbo_ids[VBO_COUNT];

        GLsizei number_of_indices;
        GLuint texture_id;
        bool there_is_texture;

        static const std::string   vertex_shader_code;
        static const std::string fragment_shader_code;

        GLuint       shader_program_id;

        GLint        model_view_matrix_id;
        GLint        projection_matrix_id;

        Shader shader;

        float  angle;

    public:

        /// @brief Inicializa el objeto 
        /// Crea la textura y la malla
        /// Configura el shader
        /// Vincula el bloque 'LightBlock' a binding point 0
        Object (const std::string mesh_file_path, const std::string texture_path);
        ~Object();

        /// @brief renderiza el objeto
        /// Aplica las modificaciónes
        /// @param view_matrix La vista desde la cámara
        /// @param translation La translación a aplicar
        /// @param angle El ángulo el cual va a rotar
        /// @param rotation el vector que indica a dónde se va a aplicar la rotación (x, y, z)
        /// @param scaleFactor El valor de escala a aplicar al objeto
        /// @param specularIntensity El valor de specular del objeto
        /// @param shininess El valor de brillo del objeto
        void render(glm::mat4 view_matrix, glm::vec3 translation, float angle, glm::vec3 rotation, float scaleFactor, float specularIntensity, float shininess);
        
        /// Se actualiza constantemente
        void update();

        /// @brief asigna el width y height
        /// Calcula la proyección y actualiza el uniform
        /// Establece el viewport
        /// @param width  El ancho de la pantalla
        /// @param height El alto de la pantalla
        void resize(int width, int height);

    private:
        /// @brief crea el mesh
        /// @param mesh_file_path El path donde se encuentre el mesh
        void   load_mesh         (const std::string& mesh_file_path);

        /// @brief crea la textura del objeto
        /// @param texture_path El path donde se encuentre la textura
        /// @return el id de la textura
        GLuint create_texture_2d (const std::string& texture_path);
    };

}

#endif