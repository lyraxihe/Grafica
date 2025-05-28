#ifndef OBJECT_HEADER
#define OBJECT_HEADER

#include <glad/glad.h>
#include <glm.hpp>
#include <string>


namespace udit
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

        GLuint  vbo_ids[VBO_COUNT];
        GLuint  vao_id;

        GLsizei number_of_indices;
        GLuint texture_id;
        bool there_is_texture;

    public:

        Object (const std::string mesh_file_path, const std::string texture_path);
        ~Object();

        void   render            ();
        void   load_mesh         (const std::string& mesh_file_path);
        GLuint create_texture_2d (const std::string& texture_path);
    };

}

#endif