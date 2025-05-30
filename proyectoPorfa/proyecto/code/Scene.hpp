
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
    #include "Terrain.hpp"

    namespace PracticaKatya
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

            Object flor;
            Object conejo;

            float   angle;

            Terrain terrain;

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
            bool keys[4] = { false, false, false, false };

        public:
            Scene(int width, int height);
           ~Scene();

            void   update();
            void   render();

            void   on_drag     (int pointer_x, int pointer_y);
            void   on_click    (int pointer_x, int pointer_y, bool down);



         private:
            void   resize             (int width_, int height_);
            void   configure_material (GLuint program_id);
            void   configure_light    (GLuint program_id);

        };

    }

#endif
