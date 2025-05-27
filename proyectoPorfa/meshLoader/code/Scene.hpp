
// Este código es de dominio público
// angel.rodriguez@udit.es

#ifndef SCENE_HEADER
#define SCENE_HEADER

    #include <glad/glad.h>
    #include <glm.hpp>
    #include <string>
    #include "Camera.hpp"
    #include "Skybox.hpp"   

    namespace udit
    {

        using glm::vec3;

        class Scene
        {
        private:

            enum
            {
                COORDINATES_VBO,
                TEXCOORDS_VBO,
                INDICES_EBO,
                VBO_COUNT
            };



            static const std::string   vertex_shader_code;
            static const std::string fragment_shader_code;

            GLuint  vbo_ids[VBO_COUNT];
            GLuint  vao_id;

            GLsizei number_of_indices;

            GLint   model_view_matrix_id;
            GLint   projection_matrix_id;

            float   angle;

            Camera camera;
            Skybox skybox;

            int    width;
            int    height;

            float  angle_around_x;
            float  angle_around_y;
            float  angle_delta_x;
            float  angle_delta_y;

            bool   pointer_pressed;
            int    last_pointer_x;
            int    last_pointer_y;


        public:
            GLuint program_id;
            static const std::string texture_path;
            GLuint texture_id;
            bool there_is_texture;


            Scene(int width, int height);
           ~Scene();

            void   update ();
            void   render ();
            void   resize (int width, int height);
            void on_drag(int pointer_x, int pointer_y);
            void on_click(int pointer_x, int pointer_y, bool down);
            void move_camera_by_key(char key);

        private:

            GLuint compile_shaders        ();
            void   show_compilation_error (GLuint  shader_id);
            void   show_linkage_error     (GLuint program_id);
            void   load_mesh              (const std::string & mesh_file_path);
            GLuint create_texture_2d(const std::string& texture_path);

        };

    }

#endif
