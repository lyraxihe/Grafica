
// Este código es de dominio público
// angel.rodriguez@udit.es

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

    class Terrain
    {
    private:
        // typedef Color_Buffer< Monochrome8 > Color_Buffer;
    
    private:
        // Índices para indexar el array vbo_ids:

        enum
        {
            COORDINATES_VBO,
            TEXTURE_UVS_VBO,
            VBO_COUNT
        };

    private:

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

    public:

         Terrain(float width, float depth, unsigned x_slices, unsigned z_slices);
        ~Terrain();

    public:

        void render(glm::mat4 view_matrix, glm::vec3 translation, glm::vec3 rotation, float scaleFactor);
        void resize(int  width, int height);
        GLuint create_texture_2d_monochrome(const std::string& texture_path);
    };

}


#endif
