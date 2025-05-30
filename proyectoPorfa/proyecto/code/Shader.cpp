
#include "Shader.hpp"
#include <SDL.h>

namespace PracticaKatya
{
	Shader::Shader(const std::string vertexShaderCode, const std::string fragmentShaderCode)
	{
		shader_id = compile_shaders(vertexShaderCode, fragmentShaderCode);
	}

	Shader::~Shader()
	{
		glDeleteProgram(shader_id);
	}


	void Shader::use()
	{
		glUseProgram(shader_id);
	}

	GLuint Shader::getID()
	{
		return shader_id;
	}

	GLuint Shader::compile_shaders(const std::string& vertexShaderCode, const std::string& fragmentShaderCode)
	{
		GLint succeeded = GL_FALSE;

		// Create the vertex and fragment shader objects
		GLuint   vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

		// Set the shader source code
		const char* vertexShadersCode[] = { vertexShaderCode.c_str() };
		const char* fragmentShadersCode[] = { fragmentShaderCode.c_str() };
		const GLint    vertexShadersSize[] = { (GLint)vertexShaderCode.size() };
		const GLint  fragmentShadersSize[] = { (GLint)fragmentShaderCode.size() };

		// Compile the shaders
		glShaderSource(vertexShaderId, 1, vertexShadersCode, vertexShadersSize);
		glShaderSource(fragmentShaderId, 1, fragmentShadersCode, fragmentShadersSize);

		glCompileShader(vertexShaderId);
		glCompileShader(fragmentShaderId);

		// Check for compilation errors
		glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &succeeded);
		if (!succeeded)
			show_compilation_error(vertexShaderId);

		glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &succeeded);
		if (!succeeded)
			show_compilation_error(fragmentShaderId);

		// Create the shader program and attach the shaders
		GLuint programID = glCreateProgram();

		glAttachShader(programID, vertexShaderId);
		glAttachShader(programID, fragmentShaderId);

		// Link the program
		glLinkProgram(programID);

		// Check for linkage errors
		glGetShaderiv(programID, GL_LINK_STATUS, &succeeded);
		if (not succeeded)
			show_linkage_error(programID);

		// Clean up the shader objects as they are no longer needed
		glDeleteShader(vertexShaderId);
		glDeleteShader(fragmentShaderId);

		return programID;
	}


	void Shader::show_compilation_error(GLuint shader_id)
	{
		static auto message = "Error while compiling a shader.";

		std::string       infoLog;
		GLint       infoLogLenght;

		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &infoLogLenght);

		infoLog.resize(infoLogLenght);

		glGetShaderInfoLog(shader_id, infoLogLenght, NULL, &infoLog.front());

		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, message, infoLog.c_str(), nullptr);

		throw message;
	}

	void Shader::show_linkage_error(GLuint program_id)
	{
		static auto message = "Error while linking shaders.";

		std::string       infoLog;
		GLint       infoLogLenght;

		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLogLenght);

		infoLog.resize(infoLogLenght);

		glGetProgramInfoLog(program_id, infoLogLenght, NULL, &infoLog.front());

		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, message, infoLog.c_str(), nullptr);

		throw message;
	}

}