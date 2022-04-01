/*!
 * \file
 * \brief file drawtext.h
 *
 * Inits and draws the text
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#ifndef DRAWTEXT_H
#define DRAWTEXT_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <prj-ttf-reader.h>

class DrawText
{
public:
    DrawText();
    bool init(prj_ttf_reader_data_t *data, const char *text);
    void render();

private:
    GLuint m_vertexBufferObject;
    GLuint m_vertexArrayObject;
    GLuint m_textureImage;

    int m_charactersCount;

    bool addImage(prj_ttf_reader_data_t *data);
};

#endif // DRAWTEXT_H
