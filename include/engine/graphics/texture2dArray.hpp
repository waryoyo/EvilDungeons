#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

class Texture2DArray {
public:
    static void SetBasePath(const std::string& basePath);
    static const std::string& GetBasePath();

    Texture2DArray(int w, int h, int layers);
    ~Texture2DArray();

    void setLayer(int layerIndex, unsigned char* pixelData);
    void generateMipmap();
    void bind(GLuint unit) const;
    static void unbind(GLuint unit = 0);

private:
    GLuint id;
    int   width, height, layers;

};

