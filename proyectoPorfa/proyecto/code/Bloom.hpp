#ifndef BLOOM_HEADER
#define BLOOM_HEADER

#include <string>
#include "Shader.hpp"


namespace PracticaKatya
{

    class Bloom
    {
    

    private:
        // FBO para la escena HDR y sus texturas asociadas
        GLuint hdrFBO;
        GLuint colorBuffer;
        GLuint rboDepth;

        // FBOs para el proceso de desenfoque (ping-pong)
        GLuint pingpongFBO[2];
        GLuint pingpongColorbuffers[2];

        // Shaders utilizados en el postprocesado
        Shader thresholdShader;
        Shader blurShader;
        Shader finalShader;


        float threshold = .7f;       // Valor inicial, ajustable
        int blurIterations = 12;      // Número de pasadas para el desenfoque
        float exposure = 1.5f;        // Valor inicial, ajustable

        // Código GLSL estático para inicializar nuestros shaders
        static const std::string quad_vertex_shader_code;
        static const std::string threshold_fragment_shader_code;
        static const std::string blur_fragment_shader_code;
        static const std::string final_fragment_shader_code;

        // Identificadores para el quad de pantalla
        GLuint quadVAO, quadVBO;
        // Funciones auxiliares
        void initBuffers();
        void renderQuad();

        // Guarda los valores actuales de ancho y alto
        int width, height;

    public:
        Bloom(int width, int height);
        ~Bloom();
        // Aplica todo el pipeline: threshold -> blur -> combinación final
        void render();
        void beginRender();
        // Llama a esta función para redimensionar los buffers

    };

}

#endif