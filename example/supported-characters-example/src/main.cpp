/*!
 * \file
 * \brief file main.cpp
 *
 * Main of supported characters example
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <prj-ttf-reader.h>

static void help()
{
    printf("Usage: ./supported-characters-example -f <path to ttf>\n");
    printf("For example:\n./supported-characters-example -f \"/tmp/font.ttf\"\n");
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
    char *fileFont = nullptr;
    prj_ttf_reader_supported_characters_t *supported_characters;

    while((opt = getopt(argc, argv, "hf:")) != -1)  {
        switch(opt)
        {
            case 'h':
                help();
                free(fileFont);
                exit(0);
                break;
            case 'f':
                fileFont = strdup(optarg);
                break;
            default:
                break;
        }
    }

    if (!fileFont) {
        printf("Error: Full filepath to font is missing\n");
        help();
        ret = EXIT_FAILURE;
    }

    if (ret != EXIT_FAILURE) {
        supported_characters = prj_ttf_reader_init_supported_character();
        if (prj_ttf_reader_get_supported_characters(fileFont, supported_characters)) {
            printf("Error: Cannot get supported chracters from the file %s\n", fileFont);
            ret = EXIT_FAILURE;
        } else {
            printf("Supported characters count in ttf: %u\nSupported characters are:", supported_characters->character_list_count);
            for (uint32_t i=0;i<supported_characters->character_list_count;i++) {
                if (i == 0) {
                    printf("%u", supported_characters->list_character[i]);
                } else {
                    printf(", %u", supported_characters->list_character[i]);
                }
                if (i+1 == supported_characters->character_list_count) {
                    printf("\n");
                }
            }
        }
        prj_ttf_reader_clear_supported_character(&supported_characters);
    }

    free(fileFont);
    return ret;
}
