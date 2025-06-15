/// @author Katya
/// @copyright () Katya

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

        /// @brief Esta clase gestiona el código para crear y renderizar una escena.
        /// La escena contiene cámara,objetos, terreno, skybox, animaciones, luz y postproceso bloom
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
            GLuint uboLights;


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
            /// inicializa la escena cargando los mesh y textura de objetos, terreno y skybox
            /// @param width ancho de la escena
            /// @param height alto de la escena
            Scene(int width, int height);
           ~Scene();

            /// update que se actualiza constantemente
            void   update();

            /// renderiza los objetos, el skybox y terreno dentro de la escena, pasandoles valores de renderizado y aplicando bloom
            void   render();

            /// rota la cámara
            /// @param pointer_x número para localizar puntero en x
            /// @param pointer_y número para localizar puntero en y
            void   on_drag     (int pointer_x, int pointer_y);

            /// comienza a tomar lo valores del puntero
            /// @param pointer_x número para localizar puntero en x
            /// @param pointer_y número para localizar puntero en y
            /// @param down detecta si se está haciendo click
            void   on_click    (int pointer_x, int pointer_y, bool down);



         private:
            /// @brief asigna el width y height
            /// ejecuta set ratio de la cámara
            /// Establece el viewport
            /// Ejecuta la función resize() de los objetos
            /// @param width  El ancho de la pantalla
            /// @param height El alto de la pantalla
            void   resize           (int width_, int height_);

            /// Crea lightData con los valores para configurar la luz y hace bind con uboLights
            void   configure_light  ();

        };

    }

#endif
