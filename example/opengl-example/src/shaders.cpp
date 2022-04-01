/*!
 * \file
 * \brief file shaders.cpp
 *
 * Compile shader
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#include "shaders.h"
#include <stdio.h>

/*!
 * \brief Shaders::m_vertShader
 *
 * simple 2D vertex shader
 */
const char *Shaders::m_vertShader = "attribute vec2 vertex;" \
        "attribute vec2 tex_coord;" \
        "uniform mat4 model;" \
        "uniform mat4 view;" \
        "uniform mat4 projection;" \
        "uniform mat4 vertex_transform;" \
        "void main()" \
        "{" \
        "    gl_TexCoord[0].xy       = tex_coord.xy;" \
        "    gl_Position             = projection*(view*(model*vertex_transform*vec4(vertex, 1.0f, 1.0f)));" \
        "}";
/*!
 * \brief Shaders::m_fragShader
 *
 * fragment shader
 */
const char *Shaders::m_fragShader = "uniform sampler2D texture;" \
        "void main()" \
        "{" \
        "    if (texture2D(texture, gl_TexCoord[0].xy).r <= 0.0f) {" \
        "        discard;" \
        "    }" \
        "    gl_FragColor = texture2D(texture, gl_TexCoord[0].xy);" \
        "}";

/*!
 * \brief Shaders::Shaders
 *
 * constructor
 */
Shaders::Shaders() :
    m_generalShader(0)
{
}

/*!
 * \brief Shaders::~Shaders
 *
 * destructor
 */
Shaders::~Shaders()
{
    if (m_generalShader) {
        glDeleteProgram(m_generalShader);
    }
}

/*!
 * \brief Shaders::generalShader
 * \return general shader
 */
GLuint Shaders::generalShader()
{
    return m_generalShader;
}

/*!
 * \brief Shaders::init
 *
 * loads the shaders
 *
 * \return 0 on success
 */
int Shaders::init()
{
    GLuint shaderHandle = glCreateProgram();
    GLint status;
    GLuint fragShader;
    GLuint vertShader;


    if (compileShader(m_vertShader, GL_VERTEX_SHADER, &vertShader) != 0) {
        fprintf( stderr, "Compile shader failed vert\n");
        return -1;
    }
    glAttachShader(shaderHandle, vertShader);
    glDeleteShader(vertShader);

    if (compileShader(m_fragShader, GL_FRAGMENT_SHADER, &fragShader) != 0) {
        fprintf( stderr, "Compile shader failed frag\n");
        return -1;
    }
    glAttachShader(shaderHandle, fragShader);
    glDeleteShader(fragShader);

    glLinkProgram(shaderHandle);

    glGetProgramiv(shaderHandle, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        fprintf(stderr, "The link operation is failed\n");
        return -1;
    }
    m_generalShader = shaderHandle;
    return 0;
}

/*!
 * \brief Shaders::compileShader
 *
 * compile the shader
 *
 * \param source, source text of the shader
 * \param type, type of the shader (vertex or frag)
 * \param shader, returns shader here, if return is 0
 * \return 0 on success
 */
int Shaders::compileShader(const char *source, const GLenum type, GLuint *shader)
{
    GLint status;
    GLuint shaderHandle = glCreateShader(type);

    if (shaderHandle == 0) {
        fprintf(stderr, "Creating the shader failed\n");
        return -1;
    }

    glShaderSource(shaderHandle, 1, &source, nullptr);
    glCompileShader(shaderHandle);

    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);

    if(status == GL_FALSE) {
        glDeleteShader(shaderHandle);
        fprintf(stderr, "Compiling shader failed - compile status is false\n");
        return -1;
    }

    *shader = shaderHandle;
    return 0;
}
