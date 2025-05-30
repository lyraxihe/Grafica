#ifndef OBJECT_HEADER
#define OBJECT_HEADER

#include <glad/glad.h>
#include <glm.hpp>
#include <string>
#include "Shader.hpp"


namespace PracticaKatya
{

    class Object
    {
    private:

         //Índices para indexar el array vbo_ids:
        enum
        {
            COORDINATES_VBO,
            TEXCOORDS_VBO,
            INDICES_EBO,
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

        Object (const std::string mesh_file_path, const std::string texture_path);
        ~Object();

        void render(glm::mat4 view_matrix, glm::vec3 translation, float angle, glm::vec3 rotation, float scaleFactor);
        void update();
        void resize(int width, int height);

    private:

        void   load_mesh         (const std::string& mesh_file_path);
        GLuint create_texture_2d (const std::string& texture_path);
    };

}

#endif