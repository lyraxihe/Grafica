
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
    #include "Bloom.hpp"

    namespace PracticaKatya
    {

        using glm::vec3;

        class Scene
        {
        private:

            Object flor_0;
            Object flor_1;
            Object flor_2;
            Object flor_3;

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

            Bloom bloom;


        public:
            bool keys[4] = { false, false, false, false };
            struct LightBlockData
            {
                glm::vec4 lightPosition;
                glm::vec3 lightColor;
                float ambientIntensity;
                float diffuseIntensity;
            };

        public:
            Scene(int width, int height);
           ~Scene();

            void   update();
            void   render();

            void   on_drag     (int pointer_x, int pointer_y);
            void   on_click    (int pointer_x, int pointer_y, bool down);



         private:
            void   resize           (int width_, int height_);
            void   configure_light  ();

        };

    }

#endif
