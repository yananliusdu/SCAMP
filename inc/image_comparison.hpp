/*
 * image_comparison.hpp
 *
 *  Created on: 18 Jun 2020
 *      Author: Hector Castillo
 */

#ifndef IMAGE_COMPARISON_HPP_
#define IMAGE_COMPARISON_HPP_

#include <scamp5.hpp>
#include <cmath>
#include "global_declarations.hpp"
#include <string>
#include <iostream>
#include <fstream>

class image_comparison{

public:
	uint8_t rows;
	uint16_t n_series;
	uint8_t columns;
	uint8_t resolution;
	uint8_t mask;
	uint8_t buff[64];
	uint32_t stored_images = 0;


	image_comparison(void);


	void load_image(uint16_t index);
	Response load_image_mem(uint8_t * ptr_data, uint8_t bytes, uint16_t index);
	void load_image_4_pixels_level(uint8_t * image, AREG Register);
	void set_stored_images(uint32_t st);
	int sum_array_values(uint8_t * ptr_array, uint8_t length);
	void move_next_image(movement_type mov);
	void move_forward(void);
	void absolute_differences(void);
	void sum_analog_reg(AREG reg, AREG aux);
};

#endif
