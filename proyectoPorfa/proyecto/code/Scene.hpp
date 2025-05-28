
// Este código es de dominio público
// angel.rodriguez@udit.es

#ifndef SCENE_HEADER
#define SCENE_HEADER

    #include <glad/glad.h>
    #include <glm.hpp>
    #include <string>
    #include "Object.hpp"
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

            Object object_1;
            Object object_2;

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
            GLuint program_id;

        public:

            Scene(int width, int height);
           ~Scene();

            void   update();
            void   render();

            void   on_drag     (int pointer_x, int pointer_y);
            void   on_click    (int pointer_x, int pointer_y, bool down);
            void   move_camera (char key);



         private:
            void   resize                 (int width, int height);
            GLuint compile_shaders        ();
            void   show_linkage_error     (GLuint program_id);
            void   show_compilation_error (GLuint  shader_id);

        };

    }

#endif
