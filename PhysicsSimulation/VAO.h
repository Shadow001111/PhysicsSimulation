#pragma once
#include <glad/glad.h>

// VAO class for managing OpenGL Vertex Array Objects
class VAO
{
public:
    VAO();

    ~VAO();

    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;
    VAO(VAO&& other) noexcept;
    VAO& operator=(VAO&& other) noexcept;

    void bind() const;

    void unbind() const;

    GLuint getID() const;

private:
    GLuint id;
};