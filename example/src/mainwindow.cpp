/*!
 * \file
 * \brief file mainwindow.cpp
 *
 * Mainwindow
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#include "mainwindow.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

/*!
 * \brief MainWindow::MainWindow
 *
 * Constructor
 */
MainWindow::MainWindow() :
    m_resolutionWidth(800),
    m_resolutionHeight(600)
{

}

/*!
 * \brief MainWindow::reshapeWindow
 *
 * sets glViewport and projection matrix
 *
 * \param window
 * \param width
 * \param height
 */
void MainWindow::reShapeWindow(GLFWwindow * /* window */, int width, int height)
{
    glViewport(0, 0, width, height);
    m_projection.setOrth(0, static_cast<float>(width), 0, static_cast<float>(height), -1.0f, 1.0f);
}

/*!
 * \brief MainWindow::keyboard_callback
 *
 * keyboard callback from glfw
 *
 * \param window
 * \param key
 * \param scancode
 * \param action
 * \param mods
 */
void MainWindow::keyboardCallback(GLFWwindow *window, int key, int /* scancode */, int action, int /* mods */)
{
    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS ) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}
/*!
 * \brief MainWindow::initWindow
 *
 * init the main window & opengl + Drawing the text
 *
 * \param data
 * \param text
 * \return 0 on sucecess
 */
int MainWindow::initWindow(prj_ttf_reader_data_t *data, const char *text)
{
    if (!glfwInit()) {
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    m_window = glfwCreateWindow(m_resolutionWidth, m_resolutionHeight, "MainWindow", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    glfwSetKeyCallback(m_window, keyboardCallback);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        return EXIT_FAILURE;
    }

    glfwShowWindow(m_window);
    if (m_shaders.init() != 0) {
        return EXIT_FAILURE;
    }

    if (!m_drawText.init(data, text)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/*!
 * \brief MainWindow::render
 *
 * renders the text
 */
void MainWindow::render()
{
    while (!glfwWindowShouldClose(m_window)) {
        reShapeWindow(m_window, m_resolutionWidth, m_resolutionHeight);
        glClearColor(0, 0, 0, 1);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(m_shaders.generalShader() );
        glUniform1i(glGetUniformLocation(m_shaders.generalShader(), "texture" ), 0);
        glUniformMatrix4fv( glGetUniformLocation(m_shaders.generalShader(), "model" ), 1, 0, m_model.getMatrixTable()->data);
        glUniformMatrix4fv( glGetUniformLocation(m_shaders.generalShader(), "view" ), 1, 0, m_view.getMatrixTable()->data);
        glUniformMatrix4fv( glGetUniformLocation(m_shaders.generalShader(), "projection" ), 1, 0, m_projection.getMatrixTable()->data);
        glUniformMatrix4fv( glGetUniformLocation(m_shaders.generalShader(), "vertex_transform" ), 1, 0, m_view.getMatrixTable()->data);

        m_drawText.render();

        glfwSwapBuffers( m_window );
        glfwPollEvents();
    }
}
