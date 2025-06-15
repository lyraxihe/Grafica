/// @author Katya
/// @copyright () Katya

#ifndef GROUND_HEADER
#define GROUND_HEADER
#include <Color.hpp>
#include <Color_Buffer.hpp>
#include <glad/glad.h>
#include <glm.hpp>
#include "Shader.hpp"
#include <string>

namespace PracticaKatya
{
    /// @brief Esta clase gestiona el código del terreno en la escena.
    /// recibe una imágen y la transforma a coordenadas de y para aplicar la elevación y dibujarla con lineas.
    class Terrain
    {
    
    private:

        // Índices para indexar el array vbo_ids:
        enum
        {
            COORDINATES_VBO,
            TEXTURE_UVS_VBO,
            VBO_COUNT
        };
        GLuint  vao_id;
        GLuint  vbo_ids[VBO_COUNT];

        GLsizei number_of_vertices;

        static const std::string vertex_shader_code;
        static const std::string fragment_shader_code;
        static const std::string texture_path;

        Shader shader;
        GLuint shader_program_id;
        GLint  model_view_matrix_id;
        GLint  projection_matrix_id;
        GLuint texture_id;
        bool   there_is_texture;
        GLint number_of_indices;

    public:
         /// @brief inicializa el terreno con una medida pasada
         /// @param width Medida del terreno
         /// @param depth Profundidad del terreno
         /// @param x_slices Medida de lineas en x
         /// @param z_slices Medida de lineas en z
         Terrain(float width, float depth, unsigned x_slices, unsigned z_slices);
        ~Terrain();

    public:

        /// renderizado del terreno
        /// @param view_matrix Vista de la cámara
        /// @param translation Translación a aplicar al terreno
        /// @param rotation Rotación a aplicar al terreno
        /// @param scaleFactor Escalado a aplicar al terreno
        void render(glm::mat4 view_matrix, glm::vec3 translation, glm::vec3 rotation, float scaleFactor);

        /// @brief asigna el width y height
        /// Calcula la proyección y actualiza el uniform
        /// Establece el viewport
        /// @param width  El ancho de la pantalla
        /// @param height El alto de la pantalla
        void resize(int  width, int height);

        /// transforma la textura a terreno en coordenadas y
        /// @param texture_path el path de la textura a transformar a coordenadas y
        /// @return id de la textura
        GLuint create_texture_2d_monochrome(const std::string& texture_path);
    };

}


#endif
