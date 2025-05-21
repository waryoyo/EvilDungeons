#include <engine_core/graphics/shader.hpp>

std::string Shader::s_basePath = "shaders/";

void Shader::SetBasePath(const std::string& basePath)
{
	s_basePath = basePath;
}

const std::string& Shader::GetBasePath()
{
	return s_basePath;
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vertexSrc = loadFile(s_basePath + vertexPath);
	std::string fragmentSrc = loadFile(s_basePath + fragmentPath);

	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

void Shader::use() const
{
	glUseProgram(ID);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	//testsss
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}


void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setInt(const std::string& name, GLuint value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setBool(const std::string& name, GLboolean value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, GLfloat value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}


std::string Shader::loadFile(const std::string& path)
{
	std::ifstream in(path);
	if (!in) {
		std::cerr << "dead";
		exit(0);
	}

	std::stringstream buf;
	buf << in.rdbuf();
	return buf.str();
}

GLuint Shader::compileShader(GLenum type, const std::string& src)
{
	GLuint shader = glCreateShader(type);
	const GLchar* srcChar = src.c_str();

	glShaderSource(shader, 1, &srcChar, nullptr);
	glCompileShader(shader);

	// TODO: add error checks here

	return shader;
}

