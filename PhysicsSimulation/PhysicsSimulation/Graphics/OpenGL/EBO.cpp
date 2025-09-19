#include "EBO.h"

EBO::EBO()
{
    glGenBuffers(1, &id);
}

EBO::~EBO()
{
    glDeleteBuffers(1, &id);
}

EBO::EBO(EBO&& other) noexcept : id(other.id)
{
    id = other.id;
    other.id = 0;
}

EBO& EBO::operator=(EBO&& other) noexcept
{
    if (this != &other)
    {
        glDeleteBuffers(1, &id);
        id = other.id;
        other.id = 0;
    }
    return *this;
}

void EBO::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void EBO::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::setData(const void* data, GLsizeiptr size, GLenum usage)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
}

GLuint EBO::getID() const
{
    return id;
}