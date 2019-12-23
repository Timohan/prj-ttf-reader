/*!
 * \file
 * \brief file maxp.h
 *
 * maximum profile
 * https://docs.microsoft.com/en-us/typography/opentype/spec/maxp
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#ifndef MAXP_H
#define MAXP_H

#include <stdint.h>
#include <stddef.h>

/*!
 * \brief The maximum_profile_t struct
 * https://docs.microsoft.com/en-us/typography/opentype/spec/maxp
 */
typedef struct
{
    uint32_t version;
    uint16_t glyphs_count;
    uint16_t max_points;
    uint16_t max_contours;
    uint16_t max_composite_points;
    uint16_t max_composite_contours;
    uint16_t max_zones;
    uint16_t max_twilight_points;
    uint16_t max_storage;
    uint16_t max_function_defs;
    uint16_t max_instruction_defs;
    uint16_t max_stack_elements;
    uint16_t max_size_of_instructions;
    uint16_t max_component_elements;
    uint16_t max_component_depth;
} maximum_profile_t;

int maxp_parse_maximum_profile(const uint8_t* data, size_t data_size, size_t offset, maximum_profile_t *profile);

#endif // MAXP_H
