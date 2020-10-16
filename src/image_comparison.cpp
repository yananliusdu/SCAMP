/*
 * image_comparison.cpp
 *
 *  Created on: 18 Jun 2020
 *      Author: Hector Castillo
 */
#include "..\inc\image_comparison.hpp"

using namespace SCAMP5_PE;

/*!
    @brief 	Constructor for class image_comparison.
    @param 	No parameters.
    @return No return.
*/
image_comparison::image_comparison(void)
{
	stored_images = 0;
	mask = 1;
	n_series = 0;
}


/*!
    @brief 	Function to add values in a parallel way .
    @param 	reg	Register to add values.
    @param 	aux	Register to do some additional operation.
    @return No return.
*/
void image_comparison::sum_analog_reg(AREG reg, AREG aux)
{
	uint8_t limit = 3;
	scamp5_kernel_begin();
		ALL();
	scamp5_kernel_end();
		scamp5_load_in(TH_DIFF);

	scamp5_kernel_begin();
		sub(aux, reg, IN);
	scamp5_kernel_end();
	scamp5_in(reg, 0);

	scamp5_kernel_begin();
		where(aux);
		MOV(R5, FLAG);
		all();
	scamp5_kernel_end();
	scamp5_load_in(2);

	scamp5_kernel_begin();
		WHERE(R5);
		mov(reg, IN);
	scamp5_kernel_end();

	for (uint8_t i = 0; i < 3; i++)
	{
		scamp5_kernel_begin();
			ALL();
		scamp5_kernel_end();
		scamp5_load_pattern(R6, 0b11111111, 0b11111000 | (3 >>i), 0b11111111, 0b11111000 | (3 >>i));

		scamp5_kernel_begin();
			mov(aux,reg);
		scamp5_kernel_end();
		scamp5_shift(aux, (4 >> i), 0);

		scamp5_kernel_begin();
			WHERE(R6);
			add(reg, reg, aux);
		scamp5_kernel_end();
	}
	for (uint8_t i = 0; i < 3; i++)
	{
		scamp5_kernel_begin();
			ALL();
		scamp5_kernel_end();
		scamp5_load_pattern(R6,  0b11111000 | (3 >>i), 0b11111111,  0b11111000 | (3 >>i), 0b11111111);

		scamp5_kernel_begin();
			mov(aux,reg);
		scamp5_kernel_end();
		scamp5_shift(aux, 0, (4 >> i));

		scamp5_kernel_begin();
			WHERE(R6);
			add(reg,reg, aux);
		scamp5_kernel_end();
	}
	for (uint8_t i = 0; i < limit; i++)
	{
		scamp5_load_pattern(R6, 0b11111000, 0b11110111 + (1 << i), 0b11111000, 0b11110111 + (1 << i));
		scamp5_kernel_begin();
			ALL();
			mov(aux, reg);
		scamp5_kernel_end();
		scamp5_shift(aux, -1 << i, 0);
		scamp5_shift(R6, -1 << i, 0);

		scamp5_kernel_begin();
			WHERE(R6);
			mov(reg, aux);
		scamp5_kernel_end();
	}
	for (uint8_t i = 0; i < limit; i++)
	{
		scamp5_load_pattern(R6, 0b11110111 + (1 << i), 0b11111111, 0b11110111 + (1 << i), 0b11111111);
		scamp5_kernel_begin();
			ALL();
			mov(aux, reg);
		scamp5_kernel_end();
		scamp5_shift(aux, 0, -1 << i);
		scamp5_shift(R6, 0, -1 << i);

		scamp5_kernel_begin();
			WHERE(R6);
			mov(reg, aux);
		scamp5_kernel_end();
	}
	/*for(uint8_t i = 0; i < (factor - 1); i++)
	{
		scamp5_kernel_begin();
			ALL();
			divq(aux, reg);
			mov(reg, aux);
		scamp5_kernel_end();
	}*/
}


/*!
    @brief 	Function to calculate absolute image differences.
    @param 	No parameters.
    @return No return.
*/
void image_comparison::absolute_differences(void)
{
	uint16_t rows_limit = 0;

	rows_limit = ROWS*stored_images/(COL_RESOLUTION/(DEPTH*COLS));
	if(rows_limit >= 255)
	{
		rows_limit = 255;
	}
	scamp5_kernel_begin();
		ALL();
		sub(B,D,E);
		abs(C, B);
		CLR(R5);
	scamp5_kernel_end();

	sum_analog_reg(C, B);

	scamp5_kernel_begin();
		ALL();
	scamp5_kernel_end();
	scamp5_load_rect(R6, 0, 0, rows_limit, 255);
	scamp5_load_in(MAX_SUM_DIFF);

	scamp5_kernel_begin();
		WHERE(R6);
		sub(B, IN, C);
		add(ANALOG_REG_PARTICLES, ANALOG_REG_PARTICLES, B);
	scamp5_kernel_end();

	n_series++;
}


/*!
    @brief 	Function to set the store images.
    @param 	st	number of stored images.
    @return No return.
*/
void image_comparison::set_stored_images(uint32_t st)
{
	stored_images = st;
}


/*!
    @brief 	Function to calculate the Hamming distance, not used in this version.
    @param 	No parameters.
    @return No return.
*/
/*void image_comparison::Hammming_distance(void)
{
	int k, l;
	int sum_array = 0;
	uint8_t c0, r0, rows_limit = 0;
	rows_limit = ROWS*stored_images/(COL_RESOLUTION/(DEPTH*COLS));

	scamp5_kernel_begin();
		ALL();
		scamp5_in(C, -128);
		scamp5_in(D, -128);
		MOV(R5, REGISTER_DIGITAL_CURRENT_IMAGE);
		XOR(R4, R5, REGISTER_IMAGES_MEM);
		scamp5_scan_events(R4, &buff[0], 256, 32,4);
		WHERE(R4);
		scamp5_in(C, 1);
	scamp5_kernel_end();
	//add(ANALOG_REG_DIFF, ANALOG_REG_DIFF, B);
	for (k = 0; k < rows_limit; k+=rows)
	{
		for (l = 0; l < COL_RESOLUTION; l += resolution*columns)
		{
			c0 = l;
			r0 = k;
			scamp5_kernel_begin();
				scamp5_scan_areg(C, &buff[0], r0, c0, r0 + rows - 1, c0 + (resolution*columns) - 1, 1, 1);
				sum_array = (32 - sum_array_values(&buff[0], 64));
				scamp5_load_rect(R6, k, l, k + 7, l + 7);// + n_series - (n_series /FRAME_LENGTH)*FRAME_LENGTH);
				//scamp5_load_point(R6, k, l);//, k + 7, l + 7);
				WHERE(R6);
				scamp5_in(D, sum_array);
				//mov(ANALOG_REG_PARTICLES, D);
				add(ANALOG_REG_PARTICLES, ANALOG_REG_PARTICLES, D);
			scamp5_kernel_end();
		}
	}
	n_series++;
}
*/
/*!
    @brief 	Function to shift the values in the forward direction.
    @param 	No parameters.
    @return No return.
*/
void image_comparison::move_forward(void)
{
	scamp5_kernel_begin();
		ALL();
		CLR(R4);
		CLR(R5);
		CLR(R6);
	scamp5_kernel_end();
	scamp5_in(B, -128);
	scamp5_in(C, -128);

	scamp5_kernel_begin();
		/* Copy image to use it later */
		mov(B, ANALOG_REG_PARTICLES); // Save the original Image
	scamp5_kernel_end();
	/*** Load pattern ***/
	scamp5_load_pattern(R6, 0, 0, 0b11110111, 0b11111111);

	scamp5_kernel_begin();
		/*** Shift to the left ***/
		WHERE(R6);
		mov(C, ANALOG_REG_PARTICLES);  			// Noise is added if this is not present
	scamp5_kernel_end();
	scamp5_shift(ANALOG_REG_PARTICLES, - resolution*columns, 0);
	/*** Shift to the right ***/
	scamp5_shift(R6, 0, -8);

	scamp5_kernel_begin();
		WHERE(R6);
		mov(C, ANALOG_REG_PARTICLES);  			// Noise is added if this is not present
	scamp5_kernel_end();
	scamp5_shift(ANALOG_REG_PARTICLES, resolution*columns, 0);


	/*** Load pattern first Columns***/
	scamp5_load_pattern(R6, 0, 0, 0b11110111, 0b111);
	scamp5_shift(R6, 0, 8);

	scamp5_kernel_begin();
		WHERE(R6);
		mov(C, B);  			// Noise is added if this is not present
		ALL();
	scamp5_kernel_end();
	scamp5_shift(C, 0, -ROWS);
	scamp5_shift(R6, 0, -ROWS);

	scamp5_kernel_begin();
		WHERE(R6);
		mov(ANALOG_REG_PARTICLES, C);
	scamp5_kernel_end();

	/*** Load pattern last columns***/
	scamp5_load_pattern(R6, 0, 0, 0b11110111, 0b111);
	scamp5_shift(R6, -248, 0);

	scamp5_kernel_begin();
		WHERE(R6);
		mov(C, B);  			// Noise is added if this is not present
		ALL();
	scamp5_kernel_end();
	scamp5_shift(C, 0, -ROWS);
	scamp5_shift(R6, 0, -ROWS);

	scamp5_kernel_begin();
		WHERE(R6);
		mov(ANALOG_REG_PARTICLES, C);
		//**** To add a black rectangle at the beginning **//
		ALL();
	scamp5_kernel_end();
	scamp5_in(B, -128);
	scamp5_load_rect(R6, 0, 0, rows - 1, columns*resolution - 1);

	scamp5_kernel_begin();
		WHERE(R6);
		mov(ANALOG_REG_PARTICLES, B);
	scamp5_kernel_end();
}

/*!
    @brief 	Function to apply motion model.
    @param 	mov	direction of movement.
    @return No return.
*/
void image_comparison::move_next_image(movement_type mov)
{
	switch(mov)
	{
	case forward:
		move_forward();
		break;
	case No_movement:
		return;
	default:
		move_forward();
	}
}

/*!
    @brief 	Function to add all the values within an array.
    @param 	ptr_array	values to add.
    @param 	length		total number of values.
    @return Addition value.
*/
int image_comparison::sum_array_values(uint8_t * ptr_array, uint8_t length)
{
	int sum = 0;
	for(uint8_t i = 0; i < length; i++)
	{
		sum += ptr_array[i] - MAX_VALUE_PIX;
	}
	sum = sum / length;
	if(sum > 100)
	{
		sum = 100;
	}
	return sum;
}
