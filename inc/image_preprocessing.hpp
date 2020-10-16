/*
 * image_preprocessing.hpp
 *
 *  Created on: 18 Jun 2020
 *      Author: Hector Castillo
 */

#ifndef IMAGE_PREPROCESSING_HPP_
#define IMAGE_PREPROCESSING_HPP_

#include <scamp5.hpp>
#include <cmath>
#include "global_declarations.hpp"
#include <iostream>
#include <fstream>


class image_preprocessing{

public:
		bool collecting_db = false;
		bool first_image = false;
		bool process_image = false;
		uint8_t rows;
		uint8_t columns;
		uint8_t resolution;
		uint8_t mask;
		uint8_t resolution_multiplier;
		//uint8_t temp_buffer_store_data[64*DATABASE_SIZE];
		movement_type direction;
		uint16_t indx_buff = 0;
		uint32_t stored_images = 0;


		image_preprocessing(void);
		Response store_image_mem(uint8_t * ptr_data, uint8_t bytes);
		state_type preprocess_image(void);
		uint8_t find_ID(std::string * input_string);
		void save_whole_image(AREG reg, uint16_t n_img);
		movement_type measure_similarity_brightness(void);
		void load_analog_image(uint8_t * buff);
//		void load_mem_to_analog_register(AREG SCAMP_register,uint16_t n_image, uint8_t * string_save);
		void linear_stretching(uint8_t * buff, uint8_t size);
		movement_type measure_similarity_edges(void);
		void load_mem_to_analog_register(AREG SCAMP_register,uint16_t n_image, uint8_t * string_save);
		bool capture_database(void);
		void convert_Digital(void);
		void convert_Analogue(void);
		void convert_Analogue_input_image(AREG input_reg);
		void convert_Digital_input_image(AREG input_reg);
};

#endif /* IMAGE_PREPROCESSING_HPP_ */
