#pragma once
#include <glad/glad.h>

// EBO class for managing OpenGL Element Buffer Objects
class EBO
{
public:
    EBO();
    ~EBO();
    EBO(const EBO& other) = delete;
    EBO& operator=(const EBO& other) = delete;
    EBO(EBO&& other) noexcept;
    EBO& operator=(EBO&& other) noexcept;

    void bind() const;
    void unbind() const;
    void setData(const void* data, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);
    GLuint getID() const;
private:
    GLuint id;
};