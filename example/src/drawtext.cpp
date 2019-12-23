/*!
 * \file
 * \brief file drawtext.cpp
 *
 * Inits and draws the text
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#include "drawtext.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/*!
 * \brief The Vertice struct
 *
 * 2D texture vertice point
 */
struct Vertice
{
    float x, y;
    float s, t;
};

/*!
 * \brief DrawText::DrawText
 *
 * Constructor
 */
DrawText::DrawText()
    : m_charactersCount(0)
{

}

/*!
 * \brief DrawText::init
 *
 * inits the text
 * Generates the texture with function addImage()
 * Generates the vertices
 *
 * \param data contains all image and text data from prj-ttf-reader
 * \param text contains the text to render
 * \return true on success
 */
bool DrawText::init(prj_ttf_reader_data_t *data, const char *text)
{
    if (!addImage(data)) {
        return false;
    }

    int verticesIndex = 0;
    uint32_t i;
    float leftX, bottomY;
    float kerning = 0;
    Vertice *vertices;
    const prj_ttf_reader_glyph_data_t *glyph;
    uint32_t *list_characters, list_characters_size;

    glGenVertexArrays(1, &m_vertexArrayObject);
    glGenBuffers(1, &m_vertexBufferObject);
    glBindVertexArray(m_vertexArrayObject);

    if (prj_ttf_reader_get_characters(text, &list_characters, &list_characters_size)
            || !list_characters_size) {
        return false;
    }

    vertices = new Vertice[list_characters_size*4];
    leftX = 20;
    for (i=0;i<list_characters_size;i++) {
        glyph = prj_ttf_reader_get_character_glyph_data(list_characters[i], data);
        if (!glyph) {
            continue;
        }

        if (i==0) {
            // set the line y position
            bottomY = static_cast<float>(glyph->image_pixel_bottom_y-glyph->image_pixel_top_y+glyph->image_pixel_offset_line_y) + 20;
        }

        if (i) {
            kerning = prj_ttf_reader_get_kerning(list_characters[i-1], list_characters[i], data);
        }

        if (glyph->image_pixel_bearing <= 0) {
            leftX += glyph->image_pixel_bearing;
        }
        leftX += kerning;
        leftX = roundf(leftX);

        vertices[verticesIndex].x = leftX;
        vertices[verticesIndex].y = bottomY - static_cast<float>(glyph->image_pixel_bottom_y-glyph->image_pixel_top_y) - static_cast<float>(glyph->image_pixel_offset_line_y);
        vertices[verticesIndex].s = static_cast<float>(glyph->image_pixel_left_x)/static_cast<float>(data->image.width);
        vertices[verticesIndex].t = static_cast<float>(glyph->image_pixel_top_y)/static_cast<float>(data->image.height);
        verticesIndex++;

        vertices[verticesIndex].x = leftX + static_cast<float>(glyph->image_pixel_right_x - glyph->image_pixel_left_x);
        vertices[verticesIndex].y = bottomY - static_cast<float>(glyph->image_pixel_bottom_y-glyph->image_pixel_top_y) - static_cast<float>(glyph->image_pixel_offset_line_y);
        vertices[verticesIndex].s = static_cast<float>(glyph->image_pixel_right_x)/static_cast<float>(data->image.width);
        vertices[verticesIndex].t = static_cast<float>(glyph->image_pixel_top_y)/static_cast<float>(data->image.height);
        verticesIndex++;

        vertices[verticesIndex].x = leftX;
        vertices[verticesIndex].y = bottomY - static_cast<float>(glyph->image_pixel_offset_line_y);
        vertices[verticesIndex].s = static_cast<float>(glyph->image_pixel_left_x)/static_cast<float>(data->image.width);
        vertices[verticesIndex].t = static_cast<float>(glyph->image_pixel_bottom_y)/static_cast<float>(data->image.height);
        verticesIndex++;

        vertices[verticesIndex].x = leftX + static_cast<float>(glyph->image_pixel_right_x - glyph->image_pixel_left_x);
        vertices[verticesIndex].y = bottomY - static_cast<float>(glyph->image_pixel_offset_line_y);
        vertices[verticesIndex].s = static_cast<float>(glyph->image_pixel_right_x)/static_cast<float>(data->image.width);
        vertices[verticesIndex].t = static_cast<float>(glyph->image_pixel_bottom_y)/static_cast<float>(data->image.height);
        verticesIndex++;

        m_charactersCount++;
        if (kerning + glyph->image_pixel_bearing < 0) {
            leftX -= glyph->image_pixel_bearing;
        }

        leftX += glyph->image_pixel_advance_x;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertice)*list_characters_size*4, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    free(list_characters);
    delete[]vertices;
    return true;
}

/*!
 * \brief DrawText::render
 *
 * Renders the text
 */
void DrawText::render()
{
    int i;
    glBindVertexArray(m_vertexArrayObject);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureImage);
    for (i=0;i<m_charactersCount;i++) {
        glDrawArrays(GL_TRIANGLE_STRIP, i*4, 4);
    }
    glBindVertexArray(0);
}

/*!
 * \brief DrawText::addImage
 *
 * Create image from data to 2D texture image
 *
 * \param data image is taken from this
 * \return true on success
 */
bool DrawText::addImage(prj_ttf_reader_data_t *data)
{
    int x, y, i;
    unsigned char* imageData = nullptr;

    glGenTextures(1, &m_textureImage);

    if (!m_textureImage) {
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, m_textureImage);

    imageData = new unsigned char[data->image.width*data->image.height*4];

    for (x=0;x<data->image.width;x++) {
        for (y=0;y<data->image.height;y++) {
            for (i=0;i<4;i++) {
                imageData[(y*data->image.width+x)*4+i] = data->image.data[(y*data->image.width+x)];
            }
        }
    }

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data->image.width, data->image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

    delete[]imageData;

    if (m_textureImage == 0) {
        return false;
    }

    return true;
}
