#include "VBO.h"

VBO::VBO()
{
    glGenBuffers(1, &id);
}

VBO::~VBO()
{
    glDeleteBuffers(1, &id);
}

VBO::VBO(VBO&& other) noexcept : id(other.id)
{
    other.id = 0;
}

VBO& VBO::operator=(VBO&& other) noexcept
{
    if (this != &other)
    {
        glDeleteBuffers(1, &id);
        id = other.id;
        other.id = 0;
    }
    return *this;
}

void VBO::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VBO::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::setData(const void* data, GLsizeiptr size, GLenum usage)
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

GLuint VBO::getID() const
{
    return id;
}