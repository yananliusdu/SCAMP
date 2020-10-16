/*
 * requence_recognition.cpp
 *
 *  Created on: 18 Jun 2020
 *      Author: Hector Castillo
 */
#include "..\inc\sequence_recognition.hpp"

/*!
    @brief Constructor for class sequence recognition.

    @param No params.
    @return No parameters.
*/
sequence_recognition::sequence_recognition(void){
	stored_images = 0;
}


/*!
    @brief Function to set the number of stored images.
    @param n 	number of images.
    @return no return.
*/
void sequence_recognition::set_stored_images(uint32_t  n)
{
	stored_images = n;
}


/*!
    @brief Function used in the particle filter to add the weights of the last 8 iterations.
    @param No params.
    @return	No return.
*/
void sequence_recognition::add_weights(void)
{
	//** Add the first 4 and last 4 bits **//
	scamp5_kernel_begin();
		ALL();
		scamp5_in(C, -128);
		mov(B, ANALOG_REG_PARTICLES);
		scamp5_load_pattern(R6,  0,0, 0b11111100, 0b11100011);
		WHERE(R6);
		mov(C, B);
		ALL();
		scamp5_shift(C, -4, 0);
		scamp5_shift(R6,-4, 0);
		WHERE(R6);
		add(B, B, C);
		ALL();
		//neg(B, ANALOG_REG_PARTICLES);
	scamp5_kernel_end();

	//** Add the 2 and 2 bits **//
	scamp5_kernel_begin();
		scamp5_in(C, -128);
		scamp5_load_pattern(R6,  0,0, 0b11111100, 0b11100001);
		scamp5_shift(R6,-4, 0);
		WHERE(R6);
		mov(C, B);
		ALL();
		scamp5_shift(C, -2, 0);
		scamp5_shift(R6,-2, 0);
		WHERE(R6);
		add(B, B, C);
	scamp5_kernel_end();

	//** Add the 1 and 1 bits **//
	scamp5_kernel_begin();
		scamp5_in(C, -128);
		scamp5_load_pattern(R6,  0,0, 0b11111100, 0b11100000);
		scamp5_shift(R6,-6, 0);
		WHERE(R6);
		mov(C, B);
		ALL();
		scamp5_shift(C, -1, 0);
		scamp5_shift(R6,-1, 0);
		WHERE(R6);
		add(B, B, C);

	scamp5_kernel_end();

	scamp5_kernel_begin();
		ALL();
		scamp5_in(C, -128);
		WHERE(R6);
		mov(C,B);
	scamp5_kernel_end();

}

/*!
    @brief Function used to find the highest value in the analogue register of the SCAMP.
    @param reg		SCAMP analogue register to search.
    @param buff 	pointer to the array with the particles higher than zero.
    @return	Particle with the highest value.
*/
particle_type sequence_recognition::find_highest_value(AREG reg, uint8_t * buff)
{
	uint8_t temp_value = 0;
	uint8_t row_1, col_1;
	particle_type particle;
	particle.value = 120;
	particle.row = 255;
	uint16_t i = 0;

	row_1 = buff[i+1];
	col_1 = buff[i];
	while ((row_1 != 0) || (col_1 != 0))
	{
		row_1 = buff[i+1];
		col_1 = buff[i];
		scamp5_kernel_begin();
			scamp5_scan_areg(reg, &temp_value, row_1, col_1, row_1, col_1, 1,1);
			//temp_value = scamp5_read_areg(ANALOG_REG_PARTICLES, col_1, row_1);
		scamp5_kernel_end();
		if (temp_value >= particle.value)
		{
			if (row_1 < particle.row)
			{
				particle.value = temp_value;
				particle.column = buff[i];
				particle.row = buff[i+1];
			}
		}
		i+=2;
	}
	return particle;
}

/*!
    @brief Function to get the highest value of a register, different method that the previous.
    @param no parameters.
    @return	Particle with the highest value.
*/
particle_type sequence_recognition::get_highest_values(void)
{

	uint8_t temp_value = 0;
	particle_type particle;
	particle.value = 120;
	particle.row = 30000;
	particle.column = 0;
	uint8_t buff[NUMBER_OF_CORRECT_PARTICLES*2] = {0};
	uint8_t Th = 63;
	int distance = 1000000, temp_dist;

	bool maximum = false;
	scamp5_kernel_begin();
		ALL();
		mov(C, ANALOG_REG_PARTICLES);
		scamp5_load_pattern(R6, 0b100, 0b1000, 0b11111100, 0b11111000);
		//scamp5_load_pattern(R6, 0b1000, 0b1000, 0b11111000, 0b11111000);
		//scamp5_load_pattern(R6, 0b11, 0b111, 0b11111100, 0b11111000);
		scamp5_in(B, -127);
	do{
			all();
			scamp5_load_in(Th);
			WHERE(R6);
			sub(B, C,IN);
			where(B);
			MOV(R5,FLAG);
			scamp5_scan_events(R5, &buff[0], NUMBER_OF_CORRECT_PARTICLES, 0, 0);

			for (uint16_t j = 0; j < NUMBER_OF_CORRECT_PARTICLES*2 ; j+=2)
			{
				if ((buff[j] == 0) && (buff[j+1] == 0))
				{
					break;
				}
				all();
				//temp_value = scamp5_read_areg(ANALOG_REG_PARTICLES, buff[j+1] + 2,  buff[j] + 2);
				scamp5_scan_areg(C, &temp_value, buff[j+1], buff[j], buff[j+1], buff[j], 1,1);
				maximum = true;
				if (temp_value >=  particle.value)
				{
					if (temp_value == particle.value)
					{
						temp_dist = (last_particle.column - buff[j])*(last_particle.column - buff[j]) + (last_particle.row - buff[j+1])*(last_particle.row - buff[j+1]);
						if (temp_dist <= distance)
						{
							particle.value = temp_value;
							particle.column = buff[j];
							particle.row = buff[j+1];
							distance = temp_dist;
						}
					}
					else
					{
						particle.value = temp_value;
						particle.column = buff[j];
						particle.row = buff[j+1];
					}
				}
			}
		Th = Th >> 1;
	}
	while(maximum == false && (Th > THRESHOLD_PARTICLE_FILTER));
	scamp5_kernel_end();
	return particle;
}

/*!
    @brief Function to find the best particle in the filter.
    @param nseries	Number of image processed; useful in simulation.
    @param mapping  bool value to show if the map is been collected. (This part of the code is not working now)
    @return			Number of node in the particle filter.
*/
uint16_t sequence_recognition::find_best_match(uint16_t nseries, bool mapping)
{
	//uint8_t buff[stored_images] = {0};
	uint16_t row, column;
	uint16_t result = 0;
	best_particle = get_highest_values();

	//add_weights();
	//Now look for the result

		//ALL();
		//scamp5_in(B, THRESHOLD_PARTICLE_FILTER);
		//sub(D,A,B);
		//where(D);
		//MOV(R5,FLAG);

		//scamp5_load_pattern(R6, 0b1000, 0b1000, 0b11111000, 0b11111000);
		//CLR(R7);
		//AND(R7, R5,R6);
		//scamp5_scan_events(R7, &buff[0], stored_images/2, 0,0);

		//best_particle = find_highest_value(ANALOG_REG_PARTICLES, &buff[0]);
	row = best_particle.row;
	column =  best_particle.column;

	if((row/8) % 2 == 0)
	{
		column = column +1;
		column = column / (8);
		column = column + 1;
	}
	else
	{
		column = column;// - 7;
		column = (256 - column)/8;
	}
	row = (row/8)*32;
	result = row + column;
	if (result > stored_images)
	{
		return 65535;
	}
	return result;
}


/*!
    @brief Function to save a wrong predicted image. Used just in simulation.
    @param reg		Register to save.
    @param nseries  number of processed image.
    @return			No return.
*/
void sequence_recognition::save_image_fail(AREG reg, uint16_t nseries)
{
	using namespace std;
	char name[100] = "C:\\Users\\Hector\\Documents\\MSc_Robotics\\Dissertation\\results\\results_2\\img_";
	char series[6] = "0";
	sprintf(series, "%04d", nseries);
	strcat(name, series);
	strcat(name, ".bmp");
	scamp5_kernel_begin();
		vs_sim::save_image(reg, name);
	scamp5_kernel_end();
}


/*!
    @brief Function to save the input image for future comparison.
    @param reg		Register to save.
    @param n_img 	Number of image to save.
    @param nseries	Number of processed image.
    @return	No return.
*/
void sequence_recognition::save_image_future_comparison(AREG reg, uint16_t n_img, uint16_t nseries)
{
	using namespace std;
	char name[100] = "C:\\Users\\Hector\\Documents\\MSc_Robotics\\Dissertation\\results\\results_2\\img_";
	char number[6] = "0";
	char series[6] = "0";
	sprintf(number, "%04d", n_img);
	sprintf(series, "%04d", nseries);
	strcat(name, series);
	strcat(name, "_");
	strcat(name, number);
	strcat(name, ".bmp");
	scamp5_kernel_begin();
		vs_sim::save_image(reg, name);
	scamp5_kernel_end();
}

/*!
    @brief 	Resampling used in the particle filter.
    @param 	No params.
    @return	No return.
*/
void sequence_recognition::resampling(void)
{
	int8_t temp_value = (best_particle.value - 128);
	if (best_particle.value < 128 )
	{
		return;
	}
	scamp5_kernel_begin();
		ALL();
	scamp5_kernel_end();

	scamp5_load_in(temp_value*GAMMA);
	scamp5_kernel_begin();
		sub(B, ANALOG_REG_PARTICLES, IN);
		mov(ANALOG_REG_PARTICLES, B);
		ALL();
	scamp5_kernel_end();
}
