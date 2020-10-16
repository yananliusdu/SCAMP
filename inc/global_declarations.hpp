/*
 * image_preprocessing.hpp
 *
 *  Created on: 18 Jun 2020
 *      Author: Hector Castillo
 */

#ifndef GLOBAL_DECLARATIONS_HPP_
#define GLOBAL_DECLARATIONS_HPP_

#include <scamp5.hpp>
#include <cmath>

#define SIZE_8x8_MASK 64
#define ROW_RESOLUTION 256
#define COL_RESOLUTION 256


#define REG_DB_1	SCAMP5_PE::R7
#define REG_DB_2	SCAMP5_PE::R8
#define REG_DB_3	SCAMP5_PE::R9
#define REG_DB_4	SCAMP5_PE::R10

#define D2A_BITS	4
#define ADC_MAX_NUMBER 4095u
#define MAX_VALUE_PIX 128u


#define FOUR_PIXELS_ONE_REG	true

#define DATABASE_SIZE 915
#define Rw 10
#define NUMBER_FRAMES_N 50
#define MAX_IMAGES 500

#define HAMMING_DST
#define FIX_STD_DEV 1/256
#define REGISTER_STORE_LAST_IMAGE SCAMP5_PE::F
#define SLOPES 3
#define REGISTER_DIGITAL_CURRENT_IMAGE  SCAMP5_PE::R10
#define REGISTER_IMAGES_MEM 			SCAMP5_PE::R9
#define STR_HEAD_LINE_MEM_FILE 			"Hector: "
#define HEADER_SIZE						8
#define HEADER_MEM_FILE					"File Correct \n"
#define MEM_FILE_NAME					"images_file.dat"
#define ANALOG_REG_PARTICLES	SCAMP5_PE::A
#define ANALOG_REG_DATABASE		SCAMP5_PE::D
#define THRESHOLD_PARTICLE_FILTER			0
#define THRESHOLD_RESAMPLING_PARTICLE_FILTER  0
#define FRAME_LENGTH	14
#define TH_SIMILARITY	25
#define ROWS	8
#define COLS	8
#define DEPTH	1
#define NUMBER_OF_CORRECT_PARTICLES 300
#define MAX_SUM_DIFF 20
#define GAMMA 1
#define TH_DIFF 10
typedef enum state_type{
	init,
	creating_database,
	creating_M,
	DTW,
	update_M,
	calculating_subroute,
	closing_loop,
	calculating_likehood,
	update_and_resampling
}state_type;


typedef enum Response{
	save_mem_successful,
	err_save_mem,
	load_mem_successful,
	err_load_mem
}Response;

typedef enum movement_type{
	forward,
	backward,
	No_movement
}movement_type;

class image_dif{
public:
	float diff;
	//uint16_t ID;
};

class vector_dif{
public:
	float V[DATABASE_SIZE];
	//image_dif V[IMAGES_STORED_BEFORE_COMPARE];
};

#endif
