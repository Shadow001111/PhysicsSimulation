#include "VAO.h"

VAO::VAO()
{
    glGenVertexArrays(1, &id);
}

VAO::~VAO()
{
    glDeleteVertexArrays(1, &id);
}

VAO::VAO(VAO&& other) noexcept : id(other.id)
{
	other.id = 0;
}

VAO& VAO::operator=(VAO&& other) noexcept
{
    if (this != &other)
    {
        glDeleteVertexArrays(1, &id);
        id = other.id;
        other.id = 0;
    }
	return *this;
}

void VAO::bind() const
{
    glBindVertexArray(id);
}

void VAO::unbind() const
{
    glBindVertexArray(0);
}

GLuint VAO::getID() const
{
    return id;
}