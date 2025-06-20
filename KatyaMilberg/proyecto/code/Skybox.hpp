/// @author Katya
/// @copyright () Katya

#ifndef SKYBOX_HEADER
#define SKYBOX_HEADER

#include <vector>
#include <memory>
#include "Camera.hpp"
#include "Texture_Cube.hpp"
#include "Shader.hpp"

namespace PracticaKatya
{
    /// @brief Esta clase gestiona el código del skybox
    /// Crea el shader del skybox y lo renderiza.
    class Skybox
    {
    private:

        static const GLfloat              coordinates[];
        static const std::string   vertex_shader_code;
        static const std::string fragment_shader_code;

        GLuint       vbo_id;  // Id del VBO de las coordenadas
        GLuint       vao_id;  // Id del VAO del cubo

        GLuint       shader_program_id;

        GLint        model_view_matrix_id;
        GLint        projection_matrix_id;

        Texture_Cube texture_cube;

        Shader shader;

    public:
        /// Inicializa el skybox con una textura pasada
        /// @param texture_path el path de la textura (que luego va a aplicar sumandole 1 al nombre)
        Skybox(const std::string& texture_path);
        ~Skybox();

    public:
        /// renderiza el skybox teniendo en cuenta la ubicación de la cámara, para así dar la ilusión de infinito
        void render(const Camera& camera);

    };

}

#endif
