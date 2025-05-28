
#include "Object.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <cassert>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SOIL2.h>

using namespace std;
using namespace glm;

namespace udit
{

    Object::Object(const std::string mesh_file_path, const std::string texture_path)
    {
        texture_id = create_texture_2d(texture_path);
        there_is_texture = texture_id > 0;
        load_mesh(mesh_file_path);
    }

    Object::~Object()
    {
        glDeleteVertexArrays(1, &vao_id);
        glDeleteBuffers(VBO_COUNT, vbo_ids);
        if (there_is_texture)
            glDeleteTextures(1, &texture_id);
    }

    void Object::render()
    {

         // Vincula la textura a la unidad 0
         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, texture_id);

         // Se dibuja la malla:
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