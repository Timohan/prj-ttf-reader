/*!
 * \file
 * \brief file main.cpp
 *
 * Main of opengl example via glfw
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#include "mainwindow.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <prj-ttf-reader.h>

static MainWindow m_mainWindow;

static void help()
{
    printf("Usage: ./opengl-example -s <\"text string\"> -f <path to ttf> -z <font size> -r <rotate 0 to PI*2>\n");
    printf("-s \"text string\"\n");
    printf("-f \"full filepath to font.tff\"\n");
    printf("-z font size (float)\n");
    printf("-r rotate (float), must be between 0 to PI*2\n");
    printf("-a accurary of drawing (int), must be at least 2, prefer 5 or 10, 5 means each pixel is drawed by 5x5 subpixels\n");
    printf("-x subpixel move x (float), must be > 0 < accurary of drawing\n");
    printf("-y subpixel move y (float), must be > 0 < accurary of drawing\n");
    printf("For example:\n./opengl-example -s \"text goes here\" -f \"/tmp/font.ttf\" -z 25.0 -r 4.5 -x 1 -y 1\n");
}

/*!
 * \brief main
 * \param argc
 * \param argv
 * \return
 */
int main(int argc , char *argv[])
{
    int ret = EXIT_SUCCESS;
    int opt;
    char *textString = nullptr;
    char *fileFont = nullptr;
    float fontSize = 60;
    float rotate = 0;
    float move_x = 0, move_y = 0;
    int accuracy_quality = 5;
    prj_ttf_reader_data_t *data;

    while((opt = getopt(argc, argv, "hs:f:z:r:x:y:a:")) != -1)  {
        switch(opt)
        {
            case 'h':
                help();
                free(textString);
                free(fileFont);
                exit(0);
                break;
            case 's':
                textString = strdup(optarg);
                break;
            case 'f':
                fileFont = strdup(optarg);
                break;
            case 'a':
                accuracy_quality = atoi(optarg);
                break;
            case 'z':
                fontSize = static_cast<float>(atof(optarg));
                break;
            case 'r':
                rotate = static_cast<float>(atof(optarg));
                break;
            case 'x':
                move_x = static_cast<float>(atof(optarg));
                break;
            case 'y':
                move_y = static_cast<float>(atof(optarg));
                break;
            default:
                break;
        }
    }

    if (!textString) {
        printf("Error: Text string is missing\n");
        help();
        ret = EXIT_FAILURE;
    } else if (!fileFont) {
        printf("Error: Full filepath to font is missing\n");
        help();
        ret = EXIT_FAILURE;
    } else if (fontSize <= 0) {
        printf("Error: Font size must be larger than 0\n");
        help();
        ret = EXIT_FAILURE;
    }

    if (ret != EXIT_FAILURE) {
        data = prj_ttf_reader_init_data();
        if (!prj_ttf_reader_generate_glyphs_utf8_rotate(textString, fileFont, fontSize, accuracy_quality, data,
            rotate, move_x, move_y)) {
            m_mainWindow.initWindow(data, textString);
            prj_ttf_reader_clear_data(&data);
            m_mainWindow.render();
        } else {
            printf("Error: .tff file parsing is failed\n");
            ret = EXIT_FAILURE;
        }
    }

    free(textString);
    free(fileFont);
    return ret;
}
