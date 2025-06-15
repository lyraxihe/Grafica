#include "Terrain.hpp"
#include <glm.hpp>
#include <half.hpp>
#include <vector>
#include <gtc/type_ptr.hpp>
#include <SOIL2.h>

using glm::vec3;
using std::vector;
using half_float::half;

namespace PracticaKatya
{
    using namespace std;
    using namespace glm;

    const string Terrain::vertex_shader_code =

        "#version 330\n"
        ""
        "uniform mat4 model_view_matrix;"
        "uniform mat4 projection_matrix;"
        ""
        "layout (location = 0) in vec2 vertex_xz;"
        "layout (location = 1) in vec2 vertex_uv;"
        ""
        "uniform sampler2D sampler;"
        "uniform float     max_height;"
        ""
        "out float intensity;"
        ""
        "void main()"
        "{"
        "   float sample = texture (sampler, vertex_uv).r;"
        "   intensity    = sample * 0.75 + 0.25;"
        "   float height = sample * max_height;"
        "   vec4  xyzw   = vec4(vertex_xz.x, height, vertex_xz.y, 1.0);"
        "   gl_Position  = projection_matrix * model_view_matrix * xyzw;"
        "}";

    const string Terrain::fragment_shader_code =

        "#version 330\n"
        ""
        "in  float intensity;"
        "out vec4  fragment_color;"
        ""
        "void main()"
        "{"
        "vec3 Pink = vec3(0.8, 0.5, 0.6);"
        "vec3 Green  = vec3(0.1, 0.7, 0.07);"
        "vec3 finalColor = mix(Green, Pink, intensity);"
        "    fragment_color = vec4(finalColor, 1.0);"
        "}";

    const string Terrain::texture_path = "../../../shared/assets/heightmap.jpg";

    Terrain::Terrain(float width, float depth, unsigned x_slices, unsigned z_slices)
    :
    shader(vertex_shader_code, fragment_shader_code)
    {
        number_of_vertices = x_slices * z_slices;

        vector< half > coordinates(number_of_vertices * 2);     // Sólo es necesario guardar las coordenadas X y Z
        vector< half > texture_uvs(number_of_vertices * 2);

        float x = -width * .5f;
        float z = -depth * .5f;
        float u = 0.f;
        float v = 0.f;

        float x_step = width / float(x_slices);
        float z_step = depth / float(z_slices);
        float u_step = .6f / float(x_slices);
        float v_step = .6f / float(z_slices);

        int   coordinate_index = 0;

        for (unsigned j = 0; j < z_slices; ++j, z += z_step, v += v_step)
        {
            for (unsigned i = 0; i < x_slices; ++i, coordinate_index += 2, x += x_step, u += u_step)
            {
                coordinates[coordinate_index + 0] = half(x);
                coordinates[coordinate_index + 1] = half(z);
                texture_uvs[coordinate_index + 0] = half(u);
                texture_uvs[coordinate_index + 1] = half(v);
            }

            x += x_step = -x_step;    // Se invierte el sentido para hacer un zigzag
            u += u_step = -u_step;
        }

        // Se crean el VAO y los VBOs:
        glGenVertexArrays(1, &vao_id);
        glGenBuffers(VBO_COUNT, vbo_ids);

        // Se activa el VAO para configurarlo:
        glBindVertexArray(vao_id);

        // Se suben a un VBO los datos de coordenadas y se vinculan al VAO:
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[COORDINATES_VBO]);
        glBufferData(GL_ARRAY_BUFFER, coordinates.size() * sizeof(half), coordinates.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_HALF_FLOAT, GL_FALSE, 0, 0);

        // Se suben a un VBO los datos de coordenadas de textura y se vinculan al VAO:
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXTURE_UVS_VBO]);
        glBufferData(GL_ARRAY_BUFFER, texture_uvs.size() * sizeof(half), texture_uvs.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_HALF_FLOAT, GL_FALSE, 0, 0);
    
        // Configura el shader
        shader_program_id = shader.getID();
        shader.use();
        model_view_matrix_id = glGetUniformLocation(shader_program_id, "model_view_matrix");
        projection_matrix_id = glGetUniformLocation(shader_program_id, "projection_matrix");

        // Se establece la altura máxima del height map en el vertex shader:
        glUniform1f(glGetUniformLocation(shader_program_id, "max_height"), 5.f);

        // Se carga la textura y se envía a la GPU:
        texture_id = create_texture_2d_monochrome(texture_path);

        there_is_texture = texture_id > 0;

        // Se establece la configuración básica:
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    }

    Terrain::~Terrain()
    {
        glDeleteVertexArrays(1, &vao_id);
        glDeleteBuffers(VBO_COUNT, vbo_ids);
    }

    void Terrain::render(glm::mat4 view_matrix, glm::vec3 translation, glm::vec3 rotation, float scaleFactor)
    {
        shader.use();

        glm::mat4 model_matrix = glm::mat4(1);
        model_matrix = glm::translate(model_matrix, translation);
        model_matrix = glm::rotate(model_matrix, .1f, rotation);

        glm::mat4 model_view_matrix = view_matrix * model_matrix;
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(model_view_matrix));

        // Se selecciona la textura si está disponible:
        if (there_is_texture)
        {
            glBindTexture(GL_TEXTURE_2D, texture_id);
        }
        
        // Se selecciona el VAO que contiene los datos del objeto y se dibujan sus vértices
        // conectándolos con líneas:
        glBindVertexArray(vao_id);
        glDrawArrays(GL_LINE_STRIP, 0, number_of_vertices);
    }

    void Terrain::resize(int width_, int height_)
    {
        // Activa el shader de este objeto
        glUseProgram(shader_program_id);

        // Calcula la proyección y actualiza el uniforme
        glm::mat4 projection_matrix = glm::perspective(20.f, GLfloat(width_) / height_, 1.f, 500.f);

        glUniformMatrix4fv(projection_matrix_id, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        // Establece el viewport (estado global, no depende del shader)
        glViewport(0, 0, width_, height_);
    }

    GLuint  Terrain::create_texture_2d_monochrome(const std::string& texture_path)
    {
        int image_width = 0;
        int image_height = 0;
        int image_channels = 0;

        // Forzamos una imagen de un canal (L = luminancia, escala de grises)
        unsigned char* loaded_pixels = SOIL_load_image(texture_path.c_str(),
            &image_width,
            &image_height,
            &image_channels,
            SOIL_LOAD_L);
        if (loaded_pixels)
        {
            GLuint texture_id;
            glGenTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_2D, texture_id);

            // Configura los parámetros de la textura:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D,
                0,
                GL_R8,
                image_width,
                image_height,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                loaded_pixels);

            glGenerateMipmap(GL_TEXTURE_2D);

            SOIL_free_image_data(loaded_pixels);

            return texture_id;
        }
        return static_cast<GLuint>(-1);
    }

}
