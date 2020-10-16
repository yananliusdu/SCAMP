/*
 * visual_recognition.hpp
 *
 *  Created on: 7 Jun 2020
 *      Author: Hector Castillo
 */

#ifndef VISUAL_RECOGNITION_HPP_
#define VISUAL_RECOGNITION_HPP_

#include "image_preprocessing.hpp"
#include "global_declarations.hpp"
#include "image_comparison.hpp"
#include "sequence_recognition.hpp"
#include "load_data.hpp"
#include <scamp5.hpp>
#include <cmath>




class visual_recognition {

public:

	uint8_t resolution;
	uint8_t rows;
	uint8_t columns;
	uint8_t mask;
	uint16_t fails;
	state_type  state;
	image_preprocessing 	img_prepoc;
	image_comparison		img_comp;
	sequence_recognition 	seq_recognition;


	visual_recognition(uint8_t rows, uint8_t columns, uint8_t res);
	uint16_t FSM_visual_recognition(void);


#if 0
	void get_image_resolution(void);
	void preprocess_image(void);
	void get_analog_image(void);
	void low_image_resolution(void);
	void low_image_resolution_second_option(void);
	void low_image_res_low(void);

	void get_analog_image_one_register(void);
#endif

};



#endif /* VISUAL_RECOGNITION_HPP_ */
