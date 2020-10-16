/*
 * load_data.hpp
 *
 *  Created on: 10 Jun 2020
 *      Author: DELL
 */
#include <scamp5.hpp>
#include <math.h>

//#include "REGISTER_ENUMS.hpp"
//#include "MISC_FUNCTIONS.hpp"

#ifndef LOAD_DATA_HPP_
#define LOAD_DATA_HPP_

#define SOURCE_IMG 0
#define KERNEL_FILTER 1
#define FC_WEIGHT 2
#define FEATURE_NUM 16


void load_image_input_register(AREG reg);
bool load_database(void);
bool load_data(int data_type, DREG Dreg, int data_num);
void load_dreg_image(DREG target_dreg,const uint8_t*image_buffer,uint16_t n_rows,uint16_t n_cols);
void refresh_dreg_storage();
void setBN_offset(int param[], AREG Areg, int BN);
void acc_test_fn(int load_num, int predicted_digit, unsigned int &correct_num, unsigned int con_mat[10][10]);

//offset for batch normalisation
const int BN_fc[10] = {106,   28,    7,  -68,  270,  -13, -219, -171,  -70,   77};
const int BN_fc_deep[10] = {124,   42,  -47, -175,  179,  -93, 80, -114,   75,  228};
const int test_num = 64;
const int width = 32;

const int start_point[64][2] = {{0,224}, {0,96}, {128,224},{128,96},
							    {0,192}, {0,64}, {128,192},{128,64},
								{0,160}, {0,32}, {128,160},{128,32},
								{0,128}, {0,0},  {128,128},{128,0},
								{32,224},{32,96},{160,224},{160,96},
							    {32,192},{32,64},{160,192},{160,64},
								{32,160},{32,32},{160,160},{160,32},
								{32,128},{32,0}, {160,128},{160,0},
   	   	   	   	   	   	   	   	{64,224},{64,96},{192,224},{192,96},
								{64,192},{64,64},{192,192},{192,64},
								{64,160},{64,32},{192,160},{192,32},
								{64,128},{64,0}, {192,128},{192,0},
								{96,224},{96,96},{224,224},{224,96},
								{96,192},{96,64},{224,192},{224,64},
								{96,160},{96,32},{224,160},{224,32},
								{96,128},{96,0}, {224,128},{224,0}};

const int BN_offset2[64] = {146,  299,    6,  195,   97,  162,
							  35,  -47,   13,  176,    5,  287,
							  40,  210,  133,   -2,  194,  244,
							 149,   67,   77,  260,  194,  141,
							 216,  251,   12,   44,  222,  226,
							 203,  167,  167,  135,  138,  -47,
							 145,  -28,   59,  217,  240,  -14,
							   1,  -25,  224,  203,   -3,  192,
							  11,   -0, -113,  190,  237,   48,
							 137,  241,   69,   59,  -50,  -34,
							  -1,  138,  179,   23};

//const int BN_conv_adj2[test_num] = {100, 150, 30, 130, 0, -20, -10, 80,
//									  0, 30, 40, 0, -22, 110, 25, 200,
//									  10, 110, 20, -10, 50, 90, 75, 90,
//									  55, 30, -10, 40, 30, 200, 200, 100,
//									  75, 70, 80, -30, 40, -40, 35, -40,
//									  -20, 5, 10, 60, 60, 200, -90, -200,
//									  0, 200, 10, 25, 25, -20, 47, 100,
//									  200, 200, 58, 33,200, 40, 200, 15};

#endif /* LOAD_DATA_HPP_ */
