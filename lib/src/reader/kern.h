/*!
 * \file
 * \brief file kern.h
 *
 * kerning table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/kern
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#ifndef KERN_H
#define KERN_H

#include <stddef.h>
#include <stdint.h>
#include "../prj-ttf-reader.h"
#include "cmap.h"

int kern_parse(const uint8_t *data, size_t data_size, size_t offset,
               prj_ttf_reader_data_t *image_data, float rate,
               character_table_t *corr_character_table);

#endif // KERN_H
