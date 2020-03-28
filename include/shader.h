#pragma once
#include <glew.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
struct Shader
{
	GLint VertexShader;
	GLint FragmentShader;
	GLuint mProgram;
};

bool ShaderInit(Shader *shader);
void ShaderLoadVS(Shader *shader, const char *shaderFilePath);
void ShaderLoadFS(Shader *shader, const char *shaderFilePath);
void ShaderBind(Shader *shader);
