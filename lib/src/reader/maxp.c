/*!
 * \file
 * \brief file maxp.c
 *
 * maximum profile
 * https://docs.microsoft.com/en-us/typography/opentype/spec/maxp
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#include "maxp.h"
#include <string.h>
#include "parse_value.h"

/*!
 * \brief maxp_parse_maximum_profile
 *
 * Parse maximum profile
 * https://docs.microsoft.com/en-us/typography/opentype/spec/maxp
 *
 * \param data
 * \param data_size
 * \param offset
 * \param profile
 * \return
 */
int maxp_parse_maximum_profile(const uint8_t* data, size_t data_size, size_t offset, maximum_profile_t *profile)
{
    int ret;
    ret = parse_value_32u(data, data_size, &offset, &profile->version);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &profile->glyphs_count);
    if (ret) {
        return ret;
    }

    if (0x00005000 == profile->version) {
        return 0;
    }

    ret = parse_value_16u(data, data_size, &offset, &profile->max_points);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &profile->max_contours);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &profile->max_composite_points);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &profile->max_composite_contours);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &profile->max_zones);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &profile->max_twilight_points);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &profile->max_storage);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &profile->max_function_defs);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &profile->max_instruction_defs);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &profile->max_stack_elements);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &profile->max_size_of_instructions);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &profile->max_component_elements);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &profile->max_component_depth);
    if (ret) {
        return ret;
    }

    return 0;
}

