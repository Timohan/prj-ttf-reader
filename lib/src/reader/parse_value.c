/*!
 * \file
 * \brief file parse_value.c
 *
 * Parsing values from the data
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#include "parse_value.h"
#include <string.h>
#include <errno.h>

/*!
 * \brief parse_value_8u
 *
 * Get's uint8_t on offset position from data
 * and increases the offset by 1 (sizeof(8bit))
 *
 * \param data
 * \param data_size
 * \param offset
 * \param parsed_value
 * \return 0 on success
 */
int parse_value_8u(const uint8_t* data, size_t data_size, size_t *offset, uint8_t *parsed_value)
{
    if (data_size <= *offset) {
        return EIO;
    }
    *parsed_value = data[*offset];
    *offset += 1;
    return 0;
}

/*!
 * \brief parse_value_8i
 *
 * Get's int8_t on offset position from data
 * and increases the offset by 1 (sizeof(8bit))
 *
 * \param data
 * \param data_size
 * \param offset
 * \param parsed_value
 * \return
 */
int parse_value_8i(const uint8_t* data, size_t data_size, size_t *offset, int8_t *parsed_value)
{
    if (data_size <= *offset) {
        return EIO;
    }
    *parsed_value = (int8_t)(data[*offset]);
    *offset += 1;
    return 0;
}

/*!
 * \brief parse_value_16u
 *
 * Get's uint16_t on offset position from data
 * and increases the offset by 2 (sizeof(16bit))
 *
 * \param data
 * \param data_size
 * \param offset
 * \param parsed_value
 * \return 0 on success
 */
int parse_value_16u(const uint8_t* data, size_t data_size, size_t *offset, uint16_t *parsed_value)
{
    if (data_size <= *offset + 1) {
        return EIO;
    }
    *parsed_value = (uint16_t)((data[*offset] << 8) | data[*offset+1]);
    *offset += 2;
    return 0;
}

/*!
 * \brief parse_value_16i
 *
 * Get's int16_t on offset position from data
 * and increases the offset by 2 (sizeof(16bit))
 *
 * \param data
 * \param data_size
 * \param offset
 * \param parsed_value
 * \return 0 on success
 */
int parse_value_16i(const uint8_t* data, size_t data_size, size_t *offset, int16_t *parsed_value)
{
    if (data_size <= *offset + 1) {
        return EIO;
    }
    *parsed_value = (int16_t)((data[*offset] << 8) | data[*offset+1]);
    *offset += 2;
    return 0;
}

/*!
 * \brief parse_value_32u
 *
 * Get's uint32_t on offset position from data
 * and increases the offset by 4 (sizeof(32bit))
 *
 * \param data
 * \param data_size
 * \param offset
 * \param parsed_value
 * \return 0 on success
 */
int parse_value_32u(const uint8_t* data, size_t data_size, size_t *offset, uint32_t *parsed_value)
{
    if (data_size <= *offset + 3) {
        return EIO;
    }
    *parsed_value = (uint32_t)(
                (data[*offset] << 24) | (data[*offset+1] << 16) |
                (data[*offset+2] << 8) | data[*offset+3]);
    *offset += 4;
    return 0;
}

/*!
 * \brief parse_value_32f
 *
 * Get's float on offset position from data
 * and increases the offset by 4 (sizeof(float))
 *
 * \param data
 * \param data_size
 * \param offset
 * \param parsed_value
 * \return 0 on success
 */
int parse_value_32f(const uint8_t* data, size_t data_size, size_t *offset, float *parsed_value)
{
    if (data_size <= *offset + 3) {
        return EIO;
    }
    memcpy(parsed_value, data+(*offset), 4);
    *offset += 4;
    return 0;
}

/*!
 * \brief parse_value_64i
 * Get's int64_t on offset position from data
 * and increases the offset by 8 (sizeof(16bit))
 * \param data
 * \param data_size
 * \param offset
 * \param parsed_value
 * \return 0 on success
 */
int parse_value_64i(const uint8_t* data, size_t data_size, size_t *offset, int64_t *parsed_value)
{
    if (data_size <= *offset + 7) {
        return EIO;
    }
    *parsed_value = (int64_t)(
                ((int64_t)(data[*offset]) << 56) | ((int64_t)(data[*offset+1]) << 48)  |
                ((int64_t)(data[*offset+2]) << 40) | ((int64_t)(data[*offset+3]) << 32)  |
                ((int64_t)(data[*offset+4]) << 24) | ((int64_t)(data[*offset+5]) << 16) |
                ((int64_t)(data[*offset+6]) << 8) | (int64_t)(data[*offset+7]));
    *offset += 8;
    return 0;
}

/*!
 * \brief parse_value__string
 *
 * get string from the data
 *
 * \param data
 * \param data_size
 * \param offset
 * \param string
 * \param size
 * \return 0 on success
 */
int parse_value__string(const uint8_t* data, size_t data_size, size_t *offset, uint8_t *string, uint32_t size)
{
    if (data_size <= *offset + size-1) {
        return EIO;
    }

    memcpy(string, data+(*offset), size);
    string[size] = '\0';
    *offset += size;
    return 0;
}
