#include <iostream>
#include <gtc/type_ptr.hpp>                 // value_ptr
#include "Bloom.hpp"

namespace PracticaKatya
{

    using namespace std;

    // Definiciones de código GLSL (puedes definirlos usando raw strings)
    const std::string Bloom::quad_vertex_shader_code = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoords;
        out vec2 TexCoords;
        void main(){
            TexCoords = aTexCoords;
            gl_Position = vec4(aPos, 0.0, 1.0);
        }
    )";

    const std::string Bloom::threshold_fragment_shader_code = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoords;
        uniform sampler2D scene;
        uniform float threshold;
        void main(){
            vec3 color = texture(scene, TexCoords).rgb;
            float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
            if(brightness > threshold)
                FragColor = vec4(color, 1.0);
            else
                FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        }
    )";

    const std::string Bloom::blur_fragment_shader_code = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoords;
        uniform sampler2D image;
        uniform bool horizontal;
        const float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
        void main(){
            vec2 tex_offset = 1.0 / textureSize(image, 0);
            vec3 result = texture(image, TexCoords).rgb * weight[0];
            if(horizontal){
                for(int i = 1; i < 5; ++i){
                    result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
                    result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
                }
            } else {
                for(int i = 1; i < 5; ++i){
                    result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
                    result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
                }
            }
            FragColor = vec4(result, 1.0);
        }
    )";

    const std::string Bloom::final_fragment_shader_code = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoords;
        uniform sampler2D scene;
        uniform sampler2D bloomBlur;
        uniform float exposure;
        void main(){
            vec3 hdrColor = texture(scene, TexCoords).rgb;
            vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
            vec3 finalColor = hdrColor + bloomColor;
            finalColor = vec3(1.0) - exp(-finalColor * exposure);
            FragColor = vec4(finalColor, 1.0);
        }
    )";

    Bloom::Bloom(int width, int height)
    :
        width(width), height(height),
        thresholdShader(quad_vertex_shader_code, threshold_fragment_shader_code),
        blurShader(quad_vertex_shader_code, blur_fragment_shader_code),
        finalShader(quad_vertex_shader_code, final_fragment_shader_code),
        quadVAO(0)
    {
        initBuffers();
    }


    Bloom:: ~Bloom()
    {
        glDeleteFramebuffers(1, &hdrFBO);
        glDeleteTextures(1, &colorBuffer);
        glDeleteRenderbuffers(1, &rboDepth);
        glDeleteFramebuffers(2, pingpongFBO);
        glDeleteTextures(2, pingpongColorbuffers);
        if (quadVAO)
            glDeleteVertexArrays(1, &quadVAO);
        if (quadVBO)
            glDeleteBuffers(1, &quadVBO);
    }
    void Bloom::initBuffers() {
        // 1. Crear FBO HDR
        glGenFramebuffers(1, &hdrFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

        glGenTextures(1, &colorBuffer);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "HDR Framebuffer no está completo!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. Crear FBOs para blur (ping-pong)
        glGenFramebuffers(2, pingpongFBO);
        glGenTextures(2, pingpongColorbuffers);
        for (unsigned int i = 0; i < 2; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "Pingpong Framebuffer #" << i << " no está completo!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Bloom::renderQuad() {
        if (quadVAO == 0) {
            float quadVertices[] = {
                // positions   // texCoords
                -1.0f,  1.0f,  0.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f,
                 1.0f, -1.0f,  1.0f, 0.0f,

                -1.0f,  1.0f,  0.0f, 1.0f,
                 1.0f, -1.0f,  1.0f, 0.0f,
                 1.0f,  1.0f,  1.0f, 1.0f
            };
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    // Este método aplica el pipeline de post-procesado Bloom.
    void Bloom::render() {
        // 1. Aplicar el shader de threshold para extraer zonas brillantes
        thresholdShader.use();
        GLint locThreshold = glGetUniformLocation(thresholdShader.getID(), "threshold");
        glUniform1f(locThreshold, threshold);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        GLint locScene = glGetUniformLocation(thresholdShader.getID(), "scene");
        glUniform1i(locScene, 0);

        // Renderizamos la textura resultante del threshold a pingpongFBO[0]
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
        glClear(GL_COLOR_BUFFER_BIT);
        renderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. Aplicar blur en múltiples iteraciones (ping-pong)
        bool horizontal = true, first_iteration = true;
        blurShader.use();
        for (int i = 0; i < blurIterations; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            GLint locHorizontal = glGetUniformLocation(blurShader.getID(), "horizontal");
            glUniform1i(locHorizontal, horizontal ? 1 : 0);
            glActiveTexture(GL_TEXTURE0);
            if (first_iteration)
                glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[0]);
            else
                glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
            GLint locImage = glGetUniformLocation(blurShader.getID(), "image");
            glUniform1i(locImage, 0);
            renderQuad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 3. Combinar la escena original (colorBuffer) con el bloom (última textura blur)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        finalShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        GLint locScene_ = glGetUniformLocation(finalShader.getID(), "scene");
        glUniform1i(locScene_, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        GLint locBloomBlur = glGetUniformLocation(finalShader.getID(), "bloomBlur");
        glUniform1i(locBloomBlur, 1);
        GLint locExposure = glGetUniformLocation(finalShader.getID(), "exposure");
        glUniform1f(locExposure, exposure);
        renderQuad();
    }

    void Bloom::beginRender() {
        // Vincula el HDR FBO para que todo lo que se renderice se guarde en la textura de color
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    }

}
