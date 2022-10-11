#include "Shader.h"
#include "Renderer.h"

#include <iostream>
#include <fstream>
#include <sstream>

/*
  Shader loading inspired by tutorial on:
  https://www.youtube.com/c/TheChernoProject
*/

/**
 * @brief Construct a new Shader:: Shader object
 * 
 * @param filepath Path to the shader.
 */
Shader::Shader(const string& filepath)
  : m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource src = ParseShader(filepath);
	m_RendererID = CreateShader(src.VertexSource, src.FragmentSource);

}


Shader::~Shader(){
	glDeleteProgram(m_RendererID);
}


/**
 * @brief Bind the shader.
 * 
 */
void Shader::Bind() const{
	glUseProgram(m_RendererID);
}


/**
 * @brief Unbind the shader.
 * 
 */
void Shader::Unbind() const{
	glUseProgram(0);
}

/**
 * @brief Set the shader uniform - 1 float.
 * 
 * @param name Uniform name.
 * @param value 
 */
void Shader::SetUniform1f(const std::string& name, float value) 
{
  	glUniform1f(GetUniformLocation(name), value);
}


/** 
 * @brief Set the shader uniform - 1 integer.
 * 
 * @param name Uniform name.
 * @param value 
 */
void Shader::SetUniform1i(const std::string& name, int value) 
{
  	glUniform1i(GetUniformLocation(name), value);
}


/**
 * @brief Set the shader uniform - 2 float vector.
 * 
 * @param name 
 * @param v0 
 * @param v1 
 * @param v2 
 */
void Shader::SetUniform2f(const std::string& name, float v0, float v1) 
{
  	glUniform2f(GetUniformLocation(name), v0, v1);
}


/**
 * @brief Set the shader uniform - 3 float vector.
 * 
 * @param name 
 * @param v0 
 * @param v1 
 * @param v2 
 */
void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2) 
{
  	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}


/**
 * @brief Set the shader uniform - 4 float vector.
 * 
 * @param name 
 * @param v0 
 * @param v1 
 * @param v2 
 * @param v3 
 */
void Shader::SetUniform4f(const string& name, float v0, float v1, float v2, float v3){
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}


/**
 * @brief Set the shader uniform - mat4 of integers.
 * 
 * @param name 
 * @param value 
 */
void Shader::SetUniformMat1i(const std::string& name, const int* value) 
{
  	glUniform1iv(GetUniformLocation(name), 2, value);
}


/**
 * @brief Set the shader uniform - mat4 of floats.
 * 
 * @param name 
 * @param matrix 
 */
void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix) 
{
  	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}


/**
 * @brief Get location of uniform.
 * 
 * @param name 
 * @return int 
 */
int Shader::GetUniformLocation(const string& name){
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
		return m_UniformLocationCache[name];
	}

	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1) {
		cout << "Warning: uniform " << name << " - doesn't exist!" << endl;
	}
	m_UniformLocationCache[name] = location;

	return location;
}


/**
 * @brief Create the shader from strings.
 * 
 * @param vertexShader Actual text of the vertex shader.
 * @param fragmentShader Actual text of the fragment shader.
 * @return unsigned int 
 */
unsigned int Shader::CreateShader(const string& vertexShader, const string& fragmentShader){
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}


/**
 * @brief Compile the shader
 * 
 * @param type Type of the shader - vertex/fragment
 * @param source Source code of the shader.
 * @return unsigned int 
 */
unsigned int Shader::CompileShader(unsigned int type, const string& source){
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, NULL);
	glCompileShader(id);

	int res;
	glGetShaderiv(id, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		cout << "Failed to compile shader!" << endl;
		cout << message << endl;
		glDeleteShader(id);
		return 0;
	}

	// TODO: error handling

	return id;
}


/**
 * @brief Parse the shader from a file.
 * 
 * @param filepath Path to the shader
 * @return ShaderProgramSource 
 */
ShaderProgramSource Shader::ParseShader(const string& filepath){

	enum class ShaderType{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	ifstream stream(filepath);
	string line;
	stringstream ss[2];
	ShaderType type = ShaderType::NONE;



	while (getline(stream, line)) {
		if (line.find("#shader") != string::npos ) {
		if (line.find("vertex") != string::npos) {
			type = ShaderType::VERTEX;
		}
		else if (line.find("fragment") != string::npos) {
			type = ShaderType::FRAGMENT;
		}
		}
		else{
		ss[(int)type] << line << endl;
		}
	}

	return { ss[0].str(), ss[1].str() };
}
