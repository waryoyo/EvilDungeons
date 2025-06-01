#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 screenSize;  // Pass screen width and height here

void main() {
    vec2 pos = aPos;

    // Correct X coordinate by aspect ratio (height/width)
    float aspect = screenSize.y / screenSize.x;
    pos.x *= aspect;

    gl_Position = vec4(pos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
