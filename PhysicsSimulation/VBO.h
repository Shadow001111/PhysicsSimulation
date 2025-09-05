#pragma once
#include <glad/glad.h>

// VBO class for managing OpenGL Vertex Buffer Objects
class VBO
{
public:
    VBO();
    ~VBO();
    VBO(const VBO& other) = delete;
    VBO& operator=(const VBO& other) = delete;
    VBO(VBO&& other) noexcept;
    VBO& operator=(VBO&& other) noexcept;

    void bind() const;
    void unbind() const;
    void setData(const void* data, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);
    GLuint getID() const;
private:
    GLuint id;
};