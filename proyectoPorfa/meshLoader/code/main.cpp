
// Este código es de dominio público
// angel.rodriguez@udit.es

#include "Scene.hpp"
#include <Window.hpp>

using udit::Scene;
using udit::Window;

int main (int , char * [])
{
    constexpr unsigned viewport_width  = 1024;
    constexpr unsigned viewport_height =  576;

    Window window
    (
        "OpenGL example",
        Window::Position::CENTERED,
        Window::Position::CENTERED,
        viewport_width,
        viewport_height, 
        { 3, 3 }
    );
    
    Scene scene(viewport_width, viewport_height);

    bool exit = false;
    int  mouse_x = 0;
    int  mouse_y = 0;
    bool button_down = false;

    do
    {
        // Se procesan los eventos acumulados:

        SDL_Event event;

        while (SDL_PollEvent (&event) > 0)
        {
            switch (event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                {
                    auto   buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
                    auto left_down = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);

                    if (left_down && not button_down) scene.on_click(mouse_x, mouse_y, button_down = true);

                    break;
                }

                case SDL_MOUSEBUTTONUP:
                {
                    auto   buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
                    auto left_down = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);

                    if (not left_down && button_down) scene.on_click(mouse_x, mouse_y, button_down = false);

                    break;
                }

                case SDL_MOUSEMOTION:
                {
                    SDL_GetMouseState(&mouse_x, &mouse_y);

                    scene.on_drag(mouse_x, mouse_y);

                    break;
                }

                case SDL_KEYDOWN:  // Aquí detectamos la pulsación de una tecla.
                {
                    // Define una velocidad de movimiento (puedes ajustarla según tu escena)
                    const float moveSpeed = 0.1f;
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_w:
                        // Mover hacia adelante, suponiendo que avance en el eje z negativo.
                        scene.move_camera(glm::vec3(0.0f, 0.0f, -moveSpeed));
                        break;
                    case SDLK_s:
                        // Mover hacia atrás.
                        scene.move_camera(glm::vec3(0.0f, 0.0f, moveSpeed));
                        break;
                    case SDLK_a:
                        // Mover hacia la izquierda.
                        scene.move_camera(glm::vec3(-moveSpeed, 0.0f, 0.0f));
                        break;
                    case SDLK_d:
                        // Mover hacia la derecha.
                        scene.move_camera(glm::vec3(moveSpeed, 0.0f, 0.0f));
                        break;
                    }
                    break;
                }

                case SDL_QUIT:
                {
                    exit = true;
                }
            }
        }

        // Se actualiza la escena:

        scene.update ();

        // Se redibuja la escena:

        scene.render ();

        // Se actualiza el contenido de la ventana:

        window.swap_buffers ();
    }
    while (not exit);

    SDL_Quit ();

    return 0;
}
