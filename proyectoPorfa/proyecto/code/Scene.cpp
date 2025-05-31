
// Este código es de dominio público
// angel.rodriguez@udit.es

#include "Scene.hpp"
#include <cassert>
#include <iostream>
#include <glad/glad.h>
#include <memory>
#include <SOIL2.h>
#include <gtc/type_ptr.hpp>


using namespace std;
using namespace glm;

namespace PracticaKatya
{

    Scene::Scene(int width, int height)
    :
        skybox("../../../shared/assets/sky-cube-map-"), 
        flor_0("../../../shared/assets/flor_0.obj", "../../../shared/assets/flor_0T.jpeg"), 
        flor_1("../../../shared/assets/flor_1.obj", "../../../shared/assets/flor_1T.png"),
        flor_2("../../../shared/assets/flor_2.obj", "../../../shared/assets/flor_2T.png"),
        flor_3("../../../shared/assets/flor_3.obj", "../../../shared/assets/flor_3T.png"),
        terrain(10.f, 10.f, 800, 800),
        angle(0)
    {
        // Se establece la configuración básica:
       //glEnable     (GL_CULL_FACE);
        glDisable(GL_CULL_FACE);

        glEnable(GL_DEPTH_TEST);
        glClearColor(.1f, .1f, .1f, 1.f);

        resize(width, height);
        pointer_pressed = false;

        //configuración de luces
        GLuint uboLights;
        glGenBuffers(1, &uboLights);
        glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
        LightBlockData lightData;
        lightData.lightPosition = glm::vec4(1.f, 10.f, 4.f, 1.f);
        lightData.lightColor = glm::vec3(1.f, 1.f, 1.f);
        lightData.ambientIntensity = 0.4f;
        lightData.diffuseIntensity = 0.8f;
        glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBlockData), &lightData, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Vincula el UBO al binding point 0
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboLights);

    }

    Scene::~Scene()
    {

    }

    void Scene::update()
    {
        angle += 0.01f;

        //camera
        glm::vec3 cameraDirection(
            cos(camera.getRotation_x()) * sin(camera.getRotation_y()),
            sin(camera.getRotation_x()),
            cos(camera.getRotation_x()) * cos(camera.getRotation_y())
        );

        glm::vec3 right = glm::normalize(glm::cross(cameraDirection, glm::vec3(0.f, 1.f, 0.f)));
        glm::vec3 up = glm::normalize(glm::cross(right, cameraDirection));

        glm::vec3 movement(0.f);

        if (keys[0]) movement += cameraDirection * 0.01f;
        if (keys[1]) movement -= cameraDirection * 0.01f;
        if (keys[2]) movement -= right * 0.01f;
        if (keys[3]) movement += right * 0.01f;

        camera.move(movement);

        //flor.update();
        //conejo.update();
    }

    void Scene::render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //renderiza el kybox
        skybox.render(camera);
        
        //se crea el view_matrix teniendo en cuenta la vista de la cámara
        glm::mat4 view_matrix = camera.get_transform_matrix_inverse();

        // se renderiza el conejo con transparencia
        // Se habilita la mezcla con el color de fondo usando el canal alpha y se deshabilita la escritura en el Z-Buffer:
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // SE RENDERIZA EL OBJ TRANSPARENTE

        // Se deshabilita la mezcla con el fondo y se restaura escritura en el Z-Buffer:
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);







        // se renderiza la flor  
        //                                             translate               angle                rotation               scalation 
        flor_0.render(view_matrix,         glm::vec3(-2.f, -1.5f, 1.f),        angle,        glm::vec3(0.f, 1.f,0.f),          1.f       );

        //                                             translate               angle                rotation               scalation 
        flor_1.render(view_matrix,         glm::vec3(-1.f, -1.7f, 4.5f),       angle,        glm::vec3(0.f, 1.f, 0.f),         1.f       );

        //                                             translate               angle                rotation               scalation 
        flor_2.render(view_matrix,         glm::vec3(2.f, -1.f, 1.f),          angle,        glm::vec3(0.f, 1.f, 0.f),         1.f       );

        //                                             translate               angle                rotation               scalation 
        flor_3.render(view_matrix,         glm::vec3(4.f, -1.5f, 6.f),         angle,        glm::vec3(0.f, 1.f, 0.f),         1.f       );

        //                                             translate              rotation              scalation 
        terrain.render(view_matrix,        glm::vec3(0.f, -4.f, 3.f),  glm::vec3(1.f, 0.f, 0.f),      1.f          );
    }

    void Scene::resize(int width_, int height_)
    {
        width = width_;
        height = height_;

        camera.set_ratio(float(width_) / height_);

        flor_0.resize   (width_, height_);
        flor_1.resize   (width_, height_);
        flor_2.resize   (width_, height_);
        flor_3.resize   (width_, height_);
        terrain.resize  (width_, height_);

        glViewport(0, 0, width_, height_);
    }

    void Scene::on_drag(int pointer_x, int pointer_y)
    {
        if (pointer_pressed)
        {
            float delta_x = (pointer_x - last_pointer_x) * 0.005f;
            float delta_y = (pointer_y - last_pointer_y) * 0.005f;

            camera.rotate(-delta_y, -delta_x);

            last_pointer_x = pointer_x;
            last_pointer_y = pointer_y;
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
            angle_delta_x = angle_delta_y = 0.f;
        }

    }

    void Scene::configure_material(GLuint program_id)
    {
        GLint material_color = glGetUniformLocation(program_id, "material_color");

        glUniform3f(material_color, 1.f, 1.f, 1.f);
    }

    void Scene::configure_light(GLuint program_id)
    {
        GLint light_position = glGetUniformLocation(program_id, "light.position");
        GLint light_color = glGetUniformLocation(program_id, "light.color");
        GLint ambient_intensity = glGetUniformLocation(program_id, "ambient_intensity");
        GLint diffuse_intensity = glGetUniformLocation(program_id, "diffuse_intensity");

        glUniform4f(light_position, 1.0f, 1.f, 1.f, 1.f);
        glUniform3f(light_color, 1.f, 1.f, 1.f);
        glUniform1f(ambient_intensity, 0.2f);
        glUniform1f(diffuse_intensity, 0.8f);
    }
}

