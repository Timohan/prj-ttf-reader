/*!
 * \file
 * \brief file mainwindow.h
 *
 * Mainwindow
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

struct GLFWwindow;
#include "matrix4x4.h"
#include "shaders.h"
#include "drawtext.h"

class MainWindow
{
public:
    MainWindow();
    int initWindow(prj_ttf_reader_data_t *data, const char *text);
    void render();

private:
    int m_resolutionWidth, m_resolutionHeight;

    Matrix4x4 m_model;
    Matrix4x4 m_view;
    Matrix4x4 m_projection;

    GLFWwindow *m_window;
    Shaders m_shaders;
    DrawText m_drawText;

    static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void reShapeWindow(GLFWwindow* window, int width, int height);
    static void errorCallback(int error, const char* description);


};

#endif // MAINWINDOW_H
