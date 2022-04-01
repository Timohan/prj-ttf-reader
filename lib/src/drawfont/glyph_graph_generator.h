/*!
 * \file
 * \brief file glyph_graph_generator.h
 *
 * Generates the glyph into image
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#ifndef GLYPH_GRAPH_GENERATOR_H
#define GLYPH_GRAPH_GENERATOR_H

#include <stdint.h>
#include <stddef.h>
#include "../prj-ttf-reader.h"
#include "../font_tables.h"

int glyph_graph_generator_generate_graph(const uint32_t *list_characters, uint32_t list_characters_size,
                                         const uint8_t *data, size_t data_size, float font_size_px,
                                         font_tables_t *tables, int quality,
                                         prj_ttf_reader_data_t *image_data,
                                         const horizontal_metrics_table_t *hor_metrics_table,
                                         const horizontal_header_table_t *hor_header_table);

#endif // GLYPH_GRAPH_GENERATOR_H
