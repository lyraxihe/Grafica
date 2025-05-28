
// Este código es de dominio público
// angel.rodriguez@udit.es

#include "Scene.hpp"
#include <cassert>
#include <iostream>
#include <glad/glad.h>
#include <memory>
#include <SOIL2.h>


using namespace std;
using namespace glm;

namespace udit
{

    const string Scene::vertex_shader_code =
        "#version 330\n"
        "uniform mat4 model_view_matrix;"
        "uniform mat4 projection_matrix;"
        "layout (location = 0) in vec3 vertex_coordinates;"
        "layout (location = 1) in vec2 vertex_texture_uv;"
        "out vec2 texture_uv;"
        "void main() {"
        "   gl_Position = projection_matrix * model_view_matrix * vec4(vertex_coordinates, 1.0);"
        "   texture_uv = vertex_texture_uv;"
        "}";


    const string Scene::fragment_shader_code =
        "#version 330\n"
        "uniform sampler2D sampler;"
        "in vec2 texture_uv;"
        "out vec4 fragment_color;"
        "void main() {"
        "     fragment_color = vec4(texture (sampler, texture_uv).rgb, 0.5);"
        "}";


    Scene::Scene(int width, int height)
    :
        skybox("../../../shared/assets/sky-cube-map-"), 
        object_1("../../../shared/assets/flor.obj", "../../../shared/assets/florT.jpeg"), 
        object_2("../../../shared/assets/stanford-bunny.obj", "../../../shared/assets/florT.jpeg"),
        terrain(10.f, 10.f, 50, 50),
        angle(0)
    {
        // Se establece la configuración básica:

       // glEnable     (GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glClearColor(.1f, .1f, .1f, 1.f);

        // Se compilan y se activan los shaders:
        program_id = compile_shaders();
        glUseProgram(program_id);

        model_view_matrix_id = glGetUniformLocation(program_id, "model_view_matrix");
        projection_matrix_id = glGetUniformLocation(program_id, "projection_matrix");

        resize(width, height);

        angle_around_x = angle_delta_x = 0.0;
        angle_around_y = angle_delta_y = 0.0;
        pointer_pressed = false;
    }

    Scene::~Scene()
    {
        glDeleteVertexArrays(1, &vao_id);
        glDeleteBuffers(VBO_COUNT, vbo_ids);
    }

    void Scene::update()
    {
        angle += 0.01f;

        //camera
        angle_around_x += angle_delta_x;
        angle_around_y += angle_delta_y;

        if (angle_around_x < -1.5)
        {
            angle_around_x = -1.5;
        }
        else
            if (angle_around_x > +1.5)
            {
                angle_around_x = +1.5;
            }
        glm::mat4 camera_rotation(1);
        camera_rotation = glm::rotate(camera_rotation, angle_around_y, glm::vec3(0.f, 1.f, 0.f));
        camera_rotation = glm::rotate(camera_rotation, angle_around_x, glm::vec3(1.f, 0.f, 0.f));

        glm::vec3 front = glm::normalize(glm::vec3(camera.get_target()) - glm::vec3(camera.get_location()));
        camera.set_target(0, 0, -1);
        camera.rotate(camera_rotation);
    }

    void Scene::render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //renderiza el kybox
        skybox.render(camera);
        

        //                                                                -- -- -- -- primer objeto -- -- -- --
        // acomoda la vista del modelo
        glUseProgram(program_id);
        glm::mat4 view_matrix = camera.get_transform_matrix_inverse();
        glm::mat4 model_matrix = glm::mat4(1.0f);

        //acomoda el modelo
        model_matrix = glm::translate(model_matrix, glm::vec3(0.f, -1.f, -4.5f));
        model_matrix = glm::rotate(model_matrix, 0.8f, glm::vec3(1.f, 0.f, 0.f));
        model_matrix = glm::rotate(model_matrix, angle, glm::vec3(0.f, 1.f, 0.f));

        glm::mat4 model_view_matrix = view_matrix * model_matrix;
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(model_view_matrix));

        // se renderiza
        object_1.render();


        //                                                                 -- -- -- -- segundo objeto -- -- -- --
        // Se habilita la mezcla con el color de fondo usando el canal alpha y se deshabilita la escritura en el Z-Buffer:
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // acomoda el modelo
        model_matrix = glm::mat4(1);
        model_matrix = glm::translate(model_matrix, glm::vec3(0.f, 0.f, -2.f));
       
        model_view_matrix = view_matrix * model_matrix;
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(model_view_matrix));

        // se renderiza
        object_2.render();

        // Se deshabilita la mezcla con el fondo y se restaura escritura en el Z-Buffer:
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);

        terrain.render();

    }

    void Scene::resize(int width_, int height_)
    {
        width = width_;
        height = height_;

        camera.set_ratio(float(width_) / height_);

        glm::mat4 projection_matrix = glm::perspective(20.f, GLfloat(width_) / height_, 1.f, 5000.f);

        glUniformMatrix4fv(projection_matrix_id, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        glViewport(0, 0, width_, height_);
    }

    GLuint Scene::compile_shaders()
    {
        GLint succeeded = GL_FALSE;

        // Se crean objetos para los shaders:

        GLuint   vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

        // Se carga el código de los shaders:

        const char* vertex_shaders_code[] = { vertex_shader_code.c_str() };
        const char* fragment_shaders_code[] = { fragment_shader_code.c_str() };
        const GLint    vertex_shaders_size[] = { (GLint)vertex_shader_code.size() };
        const GLint  fragment_shaders_size[] = { (GLint)fragment_shader_code.size() };

        glShaderSource(vertex_shader_id, 1, vertex_shaders_code, vertex_shaders_size);
        glShaderSource(fragment_shader_id, 1, fragment_shaders_code, fragment_shaders_size);

        // Se compilan los shaders:

        glCompileShader(vertex_shader_id);
        glCompileShader(fragment_shader_id);

        // Se comprueba que si la compilación ha tenido éxito:

        glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &succeeded);
        if (!succeeded) show_compilation_error(vertex_shader_id);

        glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &succeeded);
        if (!succeeded) show_compilation_error(fragment_shader_id);

        // Se crea un objeto para un programa:

        GLuint program_id = glCreateProgram();

        // Se cargan los shaders compilados en el programa:

        glAttachShader(program_id, vertex_shader_id);
        glAttachShader(program_id, fragment_shader_id);

        // Se linkan los shaders:

        glLinkProgram(program_id);

        // Se comprueba si el linkage ha tenido éxito:

        glGetProgramiv(program_id, GL_LINK_STATUS, &succeeded);
        if (!succeeded) show_linkage_error(program_id);

        // Se liberan los shaders compilados una vez se han linkado:

        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);

        return (program_id);
    }

    void Scene::on_drag(int pointer_x, int pointer_y)
    {
        if (pointer_pressed)
        {
            angle_delta_x = 0.025f * float(last_pointer_y - pointer_y) / float(height);
            angle_delta_y = 0.025f * float(last_pointer_x - pointer_x) / float(width);

        }

    }

    void Scene::on_click(int pointer_x, int pointer_y, bool down)
    {
        if ((pointer_pressed = down) == true)
        {
            last_pointer_x = pointer_x;
            last_pointer_y = pointer_y;
        }
        else
        {
            angle_delta_x = angle_delta_y = 0.0;
        }

    }

    void Scene::move_camera(char key)
    {
        // Define una velocidad para el movimiento (ajústala según lo necesites)
        float moveSpeed = 0.1f;

        // Calcula el vector "front" de la cámara: (target - location) normalizado.
        // Notarás que en tu cámara, "move" suma una traslación a location y target.
        glm::vec3 front = glm::normalize(glm::vec3(camera.get_target()) - glm::vec3(camera.get_location()));

        // Calcula el vector "right" usando el up mundial (0,1,0). El vector right es:
        glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

        // En base a la tecla, calcula el vector de traslación:
        glm::vec3 translation(0.0f);
        switch (key)
        {
        case 'w':
            translation = front * moveSpeed;
            break;
        case 's':
            translation = -front * moveSpeed;
            break;
        case 'a':
            translation = -right * moveSpeed;
            break;
        case 'd':
            translation = right * moveSpeed;
            break;
        default:
            break;
        }

        // Llama a la función move de la cámara
        camera.move(translation);
    }

    void Scene::show_linkage_error(GLuint program_id)
    {
        string info_log;
        GLint  info_log_length;

        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);

        info_log.resize(info_log_length);

        glGetProgramInfoLog(program_id, info_log_length, NULL, &info_log.front());

        cerr << info_log.c_str() << endl;

#ifdef _MSC_VER
        //OutputDebugStringA (info_log.c_str ());
#endif

        assert(false);
    }

    void Scene::show_compilation_error(GLuint shader_id)
    {
        string info_log;
        GLint  info_log_length;

        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

        info_log.resize(info_log_length);

        glGetShaderInfoLog(shader_id, info_log_length, NULL, &info_log.front());

        cerr << info_log.c_str() << endl;

        #ifdef _MSC_VER
            //OutputDebugStringA (info_log.c_str ());
        #endif

        assert(false);
    }

}

