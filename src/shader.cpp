#include <shader.h>
#include <cassert>
#include <glew.h>

void CompileShader(Shader *shader)
{
	glLinkProgram(shader->mProgram);
	GLint success;
	glGetProgramiv(shader->mProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLchar InfoLog[1024];
		glGetProgramInfoLog(shader->mProgram, sizeof(InfoLog), NULL, InfoLog);
		assert(false);
	}
	/*
	 // Only works against a bound VAO in core mode
	glValidateProgram(shader->mProgram);
	glGetProgramiv(shader->mProgram, GL_VALIDATE_STATUS, &success);
	if (!success)
	{
        GLchar InfoLog[1024];
        glGetProgramInfoLog(shader->mProgram, sizeof(InfoLog), NULL, InfoLog);
		assert(false);
	}*/
}

void AddProgram(Shader *s, const GLchar *text, GLuint type)
{
	GLint shader = glCreateShader(type);
	if (0 == shader)
	{
		assert(false);
	}
	glShaderSource(shader, 1, &text, nullptr);
	glCompileShader(shader);
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar InfoLog[1024];

		glGetShaderInfoLog(shader, 1024, NULL, InfoLog);
		assert(false);
	}
	glAttachShader(s->mProgram, shader);
	switch (type)
	{
	case GL_VERTEX_SHADER:
	{
		s->VertexShader = shader;
	}
	break;
	case GL_FRAGMENT_SHADER:
	{
		s->FragmentShader = shader;
	}
	break;
	}
	CompileShader(s);
}

void AddVertexShader(Shader *shader, const GLchar *text)
{
	AddProgram(shader, text, GL_VERTEX_SHADER);
}

void AddFragmentShader(Shader *shader, const GLchar *text)
{
	AddProgram(shader, text, GL_FRAGMENT_SHADER);
}

bool ShaderInit(Shader *shader)
{
	shader->mProgram = glCreateProgram();
	if (!shader->mProgram)
	{
		return false;
	}

	return true;
}

void ShaderLoadVS(Shader *shader, const char *shaderFilePath)
{
	std::ifstream fs(shaderFilePath);
	std::stringstream buffer;
	buffer << fs.rdbuf();
	AddVertexShader(shader, buffer.str().c_str());
}

void ShaderLoadFS(Shader *shader, const char *shaderFilePath)
{
	std::ifstream fs(shaderFilePath);
	std::stringstream buffer;
	buffer << fs.rdbuf();
	AddFragmentShader(shader, buffer.str().c_str());
}

void ShaderBind(Shader *shader)
{
	glUseProgram(shader->mProgram);
}