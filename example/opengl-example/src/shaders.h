/*!
 * \file
 * \brief file shaders.h
 *
 * Compile shader
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */


#ifndef SHADERS_H
#define SHADERS_H

#include <GL/glew.h>
#include <GL/gl.h>

class Shaders
{
public:
    Shaders();
    ~Shaders();

    int init();

    GLuint generalShader();
private:
    static const char *m_vertShader;
    static const char *m_fragShader;
    GLuint m_generalShader;

    static int compileShader(const char *source, const GLenum type, GLuint *shader);
};

#endif // SHADERS_H
