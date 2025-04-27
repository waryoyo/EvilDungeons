#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

class Shader {
public:
	static void SetBasePath(const std::string& basePath);
	static const std::string& GetBasePath();

	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();

	void use() const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setMat4(const std::string& name, const glm::mat4& matrix) const;
	void setInt(const std::string& name, GLuint value) const;
	void setBool(const std::string& name, GLboolean value) const;
	void setFloat(const std::string& name, GLfloat value) const;

private:
	static std::string s_basePath;
	GLuint ID;

	std::string loadFile(const std::string& path);
	GLuint compileShader(GLenum type, const std::string& src);
};