
#include "Object.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <cassert>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SOIL2.h>
#include <glad/glad.h>
#include <gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

namespace PracticaKatya
{

    const string Object::vertex_shader_code =

        "#version 330\n"
        ""
        "uniform mat4 model_view_matrix;"
        "uniform mat4 projection_matrix;"
        ""
        "layout (location = 0) in vec3 vertex_coordinates;"
        "layout (location = 1) in vec2 vertex_texture_uv;"
        ""
        "out vec2 texture_uv;"
        ""
        "void main()"
        "{"
        "   gl_Position = projection_matrix * model_view_matrix * vec4(vertex_coordinates, 1.0);"
        "   texture_uv  = vertex_texture_uv;"
        "}";

    const string Object::fragment_shader_code =

        "#version 330\n"
        ""
        "uniform sampler2D sampler;"
        ""
        "in  vec2 texture_uv;"
        "out vec4 fragment_color;"
        ""
        "void main()"
        "{"
        "   fragment_color = vec4(texture (sampler, texture_uv).rgb, 0.5);"
        "}";

    Object::Object(const std::string mesh_file_path, const std::string texture_path)
    :
        shader(vertex_shader_code, fragment_shader_code),
        angle(0)
    {

        // Crea la textura y la malla
        texture_id = create_texture_2d(texture_path);
        there_is_texture = texture_id > 0;
        load_mesh(mesh_file_path);

        // Configura el shader
        shader_program_id = shader.getID();
        glUseProgram(shader_program_id);

        model_view_matrix_id = glGetUniformLocation(shader_program_id, "model_view_matrix");
        projection_matrix_id = glGetUniformLocation(shader_program_id, "projection_matrix");


    }

    Object::~Object()
    {
        glDeleteVertexArrays(1, &vao_id);
        glDeleteBuffers(VBO_COUNT, vbo_ids);
        if (there_is_texture)
            glDeleteTextures(1, &texture_id);
    }

    void Object::update()
    {
        angle += 0.01f;
    }

    void Object::resize(int width_, int height_)
    {
        // Activa el shader de este objeto
        glUseProgram(shader_program_id);

        // Calcula la proyección y actualiza el uniforme
        glm::mat4 projection_matrix = glm::perspective(20.f, GLfloat(width_) / height_, 1.f, 5000.f);
        glUniformMatrix4fv(projection_matrix_id, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        // Establece el viewport (estado global, no depende del shader)
        glViewport(0, 0, width_, height_);
    }

    void Object::render(glm::mat4 view_matrix, glm::vec3 translation, glm::vec3 rotation, float scaleFactor)
    {
         shader.use();

         // Construir la matriz modelo (modifícala según lo que necesites)
         glm::mat4 model_matrix = glm::mat4(1);
         model_matrix = glm::translate(model_matrix, translation);
         model_matrix = glm::rotate(model_matrix, angle, rotation);
         model_matrix = glm::scale(model_matrix, glm::vec3(scaleFactor));

         glm::mat4 model_view_matrix = view_matrix * model_matrix;
         glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(model_view_matrix));

         // Vincula la textura a la unidad 0
         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, texture_id);

         // Enlaza el VAO y dibuja
         glBindVertexArray(vao_id);
         glDrawElements(GL_TRIANGLES, number_of_indices, GL_UNSIGNED_SHORT, 0);
         glBindVertexArray(0);
    }

    void Object::load_mesh(const std::string& mesh_file_path)
    {
        Assimp::Importer importer;

        auto scene = importer.ReadFile
        (
            mesh_file_path,
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType
        );

        // Si scene es un puntero nulo significa que el archivo no se pudo cargar con éxito:

        if (scene && scene->mNumMeshes > 0)
        {
            // se selecciona la primera malla
            auto mesh = scene->mMeshes[0];
            size_t number_of_vertices = mesh->mNumVertices;

            std::vector<glm::vec2> texCoords;
            texCoords.reserve(number_of_vertices);

            for (unsigned int i = 0; i < number_of_vertices; ++i) {
                // Si existen coordenadas UV, se usan. Sino, se asigna (0,0)
                if (mesh->mTextureCoords[0]) 
                {
                    texCoords.push_back(glm::vec2(mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y));
                }
                else 
                {
                    texCoords.push_back(glm::vec2(0.0f, 0.0f));
                }
            }

            // Se generan índices para los VBOs del objeto:

            glGenBuffers(VBO_COUNT, vbo_ids);
            glGenVertexArrays(1, &vao_id);

            // Se activa el VAO del objeto para configurarlo:

            glBindVertexArray(vao_id);

            // Se suben a un VBO los datos de coordenadas y se vinculan al VAO:

            static_assert(sizeof(aiVector3D) == sizeof(fvec3), "aiVector3D should composed of three floats");

            glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COORDINATES_VBO]);
            glBufferData(GL_ARRAY_BUFFER, number_of_vertices * sizeof(aiVector3D), mesh->mVertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

            // Se suben las coordenadas de textura (UV)
            glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXCOORDS_VBO]);
            glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);  // ubicación en el shader: layout(location = 1)
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

            // El número de indices se sabe multiplicando por tres cada cara (ya que se conforma por 3 vertices)
            number_of_indices = mesh->mNumFaces * 3;

            vector< GLshort > indices(number_of_indices);

            auto vertex_index = indices.begin();

            // Recorre el número de caras que tenga el mesh para guardar la información de los indices
            for (unsigned i = 0; i < mesh->mNumFaces; ++i)
            {
                auto& face = mesh->mFaces[i];

                assert(face.mNumIndices == 3);

                *vertex_index++ = face.mIndices[0];
                *vertex_index++ = face.mIndices[1];
                *vertex_index++ = face.mIndices[2];
            }

            // Se suben a un EBO los datos de índices:
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids[INDICES_EBO]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLshort), indices.data(), GL_STATIC_DRAW);
        }
    }

    GLuint Object::create_texture_2d(const std::string& texture_path)
    {
        // Cargar la imagen con SOIL_load_image
        int image_width = 0;
        int image_height = 0;
        int image_channels = 0;

        uint8_t* loaded_pixels = SOIL_load_image
        (
            texture_path.c_str(),
            &image_width,
            &image_height,
            &image_channels,
            SOIL_LOAD_RGBA
        );

        if (loaded_pixels)
        {
            // Habilita y genera la textura en OpenGL
            GLuint texture_id;
            glEnable(GL_TEXTURE_2D);
            glGenTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_2D, texture_id);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Se envían los datos de la imagen a la GPU
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA,
                image_width,
                image_height,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                loaded_pixels
            );

            glGenerateMipmap(GL_TEXTURE_2D);

            // Liberamos la imagen cargada en memoria
            SOIL_free_image_data(loaded_pixels);

            return texture_id;
        }

        return static_cast<GLuint>(-1);
    }
}