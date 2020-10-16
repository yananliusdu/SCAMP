/*
 * sequence_recognition.hpp
 *
 *  Created on: 18 Jun 2020
 *      Author: Hector Castillo
 */

#ifndef SEQUENCE_RECOGNITION_HPP_
#define SEQUENCE_RECOGNITION_HPP_

#include <scamp5.hpp>
#include <cmath>
#include "global_declarations.hpp"
#include <iostream>
#include <fstream>


typedef struct particle_type{
	uint16_t row;
	uint16_t column;
	uint8_t value;
}particle_type;
class  sequence_recognition{

public:
	uint8_t slopes = SLOPES;
	uint8_t N_vect_diff = 0;
	image_dif * ptr_Vector_diff;
	uint32_t stored_images = 0;
	particle_type best_particle;
	particle_type last_particle;

	state_type state;
	sequence_recognition(void);
	void set_stored_images(uint32_t  n);
	uint16_t find_best_match(uint16_t nseries, bool mapping);
	void add_weights(void);
	void save_image_fail(AREG reg, uint16_t nseries);
	void save_image_future_comparison(AREG reg, uint16_t n_img, uint16_t nseries);
	void resampling(void);
	particle_type find_highest_value(AREG reg, uint8_t * buff);
	particle_type get_highest_values(void);
};


#endif /* SEQUENCE_RECOGNITION_HPP_ */
