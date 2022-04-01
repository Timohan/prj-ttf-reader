/*!
 * \file
 * \brief file parse_value.h
 *
 * Parsing values from the data
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#ifndef PARSEVALUE_H
#define PARSEVALUE_H

#include <stdint.h>
#include <stddef.h>

int parse_value_8u(const uint8_t* data, size_t data_size, size_t *offset, uint8_t *parsed_value);
int parse_value_8i(const uint8_t* data, size_t data_size, size_t *offset, int8_t *parsed_value);
int parse_value_16u(const uint8_t* data, size_t data_size, size_t *offset, uint16_t *parsed_value);
int parse_value_16i(const uint8_t* data, size_t data_size, size_t *offset, int16_t *parsed_value);
int parse_value_32u(const uint8_t* data, size_t data_size, size_t *offset, uint32_t *parsed_value);
int parse_value_32f(const uint8_t* data, size_t data_size, size_t *offset, float *parsed_value);
int parse_value_64i(const uint8_t* data, size_t data_size, size_t *offset, int64_t *parsed_value);

int parse_value__string(const uint8_t* data, size_t data_size, size_t *offset, uint8_t *string, uint32_t size);

#endif // TTFPARSEVALUE_H
