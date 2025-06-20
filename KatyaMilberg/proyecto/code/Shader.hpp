/// @author Katya
/// @copyright () Katya

#pragma once

#ifndef SHADER_HEADER
#define SHADER_HEADER

#include <glad/glad.h>
#include <string>


namespace PracticaKatya
{
	/// @brief Esta clase gestiona el código de los shaders.
	/// Crea el shader_id con vertexShaderCode y fragmentShaderCode
	class Shader
	{
	private:

		GLuint shader_id;

	public:
		/// inicializa el shader
		/// @param vertexShaderCode   El código del vertexShader
		/// @param fragmentShaderCode El código del fragmentShader
		Shader(const std::string vertexShaderCode, const std::string fragmentShaderCode);
		~Shader();

		/// ejecuta glUseProgram(shader_id);
		void   use();

		/// Devuelve el id del shader
		/// @return El id del shader
		GLuint getID();

	private:
		/// compila los shaders y devuelve el shader id
		/// @return El id del programa
		/// @param vertexShaderCode   El código del vertexShader
		/// @param fragmentShaderCode El código del fragmentShader
		GLuint compile_shaders(const std::string& vertexShaderCode, const std::string& fragmentShaderCode);
		
		/// muestra un error de link en caso que lo haya
		/// @param program_id El id del programa
		void   show_linkage_error(GLuint program_id);

		/// muestra un error de compilación en caso que lo haya
		/// @param program_id El id del shader
		void   show_compilation_error(GLuint  shader_id);

	};


}

#endif