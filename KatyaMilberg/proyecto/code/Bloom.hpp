/// @author Katya
/// @copyright () Katya

#ifndef BLOOM_HEADER
#define BLOOM_HEADER

#include <string>
#include "Shader.hpp"


namespace PracticaKatya
{
    /// @brief Esta clase gestiona el código para aplicar BLOOM.
    /// Aplica HDR y toma los valores de la escena para identificar aquellos colores claros.
    /// Luego crea una imágen borrosa con el proceso ping-pong.
    /// Mezcla ambas imágenes generando un postprocesado de bloom.
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

        float threshold = 0.9f;      // Valor para aplicar bloom
        int blurIterations = 15;     // Número de pasadas para el desenfoque
        float exposure = 1.2f;       // Cantidad

        // Código GLSL estático para inicializar los shaders
        static const std::string quad_vertex_shader_code;
        static const std::string threshold_fragment_shader_code;
        static const std::string blur_fragment_shader_code;
        static const std::string final_fragment_shader_code;

        // Identificadores para el quad de pantalla
        GLuint quadVAO, quadVBO;

        /// Crea el FBO HDR y los FBOs para blur
        void initBuffers();

        /// Guarda la posición de lo vertices quad y hace bind al quadVAO
        void renderQuad();

        // Guarda los valores actuales de ancho y alto
        int width, height;

    public:

        /// @brief Asigna los valores necesarios y llama a initBuffers();
        /// @param width El código recibe el ancho de la pantalla
        /// @param height El código recibe el alto de la pantalla
        Bloom(int width, int height);

        /// Elimina
        ~Bloom();

        /// Aplica todo el proceso: threshold -> blur -> combinación final
        void render();

        /// Vincula el HDR FBO para que todo lo que se renderice se guarde en la textura de color
        void beginRender();

    };

}

#endif