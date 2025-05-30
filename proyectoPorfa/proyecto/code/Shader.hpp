
#pragma once



#ifndef SHADER_HEADER
#define SHADER_HEADER

#include <glad/glad.h>
#include <string>


namespace PracticaKatya
{

	class Shader
	{
	private:

		GLuint shader_id;

	public:

		Shader(const std::string vertexShaderCode, const std::string fragmentShaderCode);
		~Shader();

		void   use();
		GLuint getID();

	private:

		GLuint compile_shaders(const std::string& vertexShaderCode, const std::string& fragmentShaderCode);
		void   show_linkage_error(GLuint program_id);
		void   show_compilation_error(GLuint  shader_id);

	};


}

#endif