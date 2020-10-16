/*
 * image_preprocessing.cpp
 *
 *  Created on: 18 Jun 2020
 *      Author: Hector Castillo
 */


#include "..\inc\image_preprocessing.hpp"

using namespace SCAMP5_PE;
const dreg_t dreg_map[4] = {REG_DB_4, REG_DB_3, REG_DB_2, REG_DB_1};
const dreg_t dreg_temp_map[4] = {R11, R6, R5, R4};

uint8_t level_threshold = 60;
//******************************************************//
/*!
    @brief Constructor for the pre-processing class.

    @param No params.
*/
image_preprocessing::image_preprocessing(void)
{
	resolution_multiplier = 1;
	mask = 1;
	first_image = true;
	direction = No_movement;
	stored_images = 0;
}

/*!
    @brief Function to transform the values to the whole range.
	@param buff	Pointer to values to be modified.
    @param size	Number of values in the buff.
    @return 	No return.
*/
void image_preprocessing::linear_stretching(uint8_t * buff, uint8_t size)
{
	uint8_t max = 0;
	uint8_t min = 255;
	float a, b, temp;
	for(uint8_t i = 0; i < size; i++)
	{
		if (buff[i] > max)
		{
			max = buff[i];
		}
		if (buff[i] < min)
		{
			min = buff[i];
		}
	}
	a = (float)255/(float)(max - min);
	b = (0 - (float)min)*a;
	for(uint8_t i = 0; i < size; i++)
	{
		temp = (a*buff[i]) + b;
		buff[i] = (uint8_t)temp;
	}
}

/*!
    @brief Main pre-processing function.
    @param 	No parameters.
    @return	Current state of the system.
*/
state_type image_preprocessing::preprocess_image(void)
{
	uint8_t means[64];
	direction = forward;
	convert_Digital_input_image(E);
	convert_Analogue_input_image(E);

	if(collecting_db == true)
	{
	//	store_image_mem(&means[0], 64);
	//	save_whole_image(E, stored_images);
		return creating_database;
	}
	if (direction == No_movement)
	{
		return init;
	}
	// At this point we now it is possible to process the image
	scamp5_scan_areg_mean_8x8(E, &means[0]);
	//linear_stretching(&means[0], 64);

	load_analog_image(&means[0]);
	return calculating_likehood;
}

/*!
    @brief 	Load buffer values to an analogue register.
    @param 	buff	Values to stores in the register.
    @return	no return value.
*/
void image_preprocessing::load_analog_image(uint8_t * buff)
{
	uint8_t temp_value, indx = 0;

	scamp5_kernel_begin();
		res(A);
		SET(R5);
		all();
	scamp5_kernel_end();

	for(int j = 0; j < 8; j++)
	{
		scamp5_load_pattern(R5, 0b1000, 0b1000, 0b11111000, 0b11111000);
		scamp5_shift(R5, - j, 0);

		for (int i = 0; i < 8; i++)
		{
			temp_value = buff[indx++];
			scamp5_kernel_begin();
				WHERE(R5);
			scamp5_kernel_end();
			scamp5_load_in(A, temp_value - MAX_VALUE_PIX);

			scamp5_kernel_begin();
				ALL();
			scamp5_kernel_end();
			scamp5_shift(R5, 0, -1);
		}
	}

}


/*!
    @brief 	Store values in memory or file.
    @param 	ptr_data	Data to store in the memory.
    @param 	bytes		Size of data to store.
    @return	Result when storing the data.
*/
Response image_preprocessing::store_image_mem(uint8_t * ptr_data, uint8_t bytes)
{
/*	using namespace std;
	ofstream mem_file;
	mem_file.open(MEM_FILE_NAME);

	for (int i = 0; i< bytes; i++)
	{
		temp_buffer_store_data[indx_buff + i] = ptr_data[i];
		//mem_file << *ptr_data++;
	}
	indx_buff += bytes;
	stored_images++;
	mem_file << HEADER_MEM_FILE;
	for (int i = 0; i < indx_buff; i+=64)
	{
		mem_file << i/64;
		mem_file << STR_HEAD_LINE_MEM_FILE;
		mem_file.write((char *)&temp_buffer_store_data[i], bytes);
		mem_file << "\n";
	}
	mem_file.close();
	return save_mem_successful;*/
	using namespace std;
	fstream mem_file;
	string line;
	mem_file.open(MEM_FILE_NAME, fstream::out | fstream::app);

	if(stored_images == 0)
	{
		mem_file.seekg(0);
		mem_file << HEADER_MEM_FILE;
	}
	mem_file.seekg(mem_file.end);
	stored_images++;
	mem_file << stored_images;
	mem_file << STR_HEAD_LINE_MEM_FILE;
	mem_file.write((char *)&ptr_data[0], bytes);
	mem_file << "\n";
	mem_file << std::flush;
	mem_file.close();
	return save_mem_successful;
}


/*!
    @brief 	Find an string value in a input string.
    @param 	input_string	string to search.
    @return	index where the string is.
*/
uint8_t image_preprocessing::find_ID(std::string * input_string)
{
	int indx = input_string->find(STR_HEAD_LINE_MEM_FILE);
	if (indx < 0)
	{
		return 0;
	}
	return indx + HEADER_SIZE;
}


/*!
    @brief This function takes the whole image directly from the analogue register
     	   and save it to a text file. It is used just to debug and compare the results.
    @param reg  	Analogue register where the image is.
    @param n_img 	Number of image to store.
    @return no return.
*/
void image_preprocessing::save_whole_image(AREG reg, uint16_t n_img)
{
	using namespace std;
	char name[200] = "C:\\Users\\Hector\\Documents\\MSc_Robotics\\Dissertation\\results\\ground_truth_2\\img_";
	char number[10] = "0";
	sprintf(number, "%d", n_img);
	strcat(name, number);
	strcat(name, ".bmp");
	scamp5_kernel_begin();
		vs_sim::save_image(reg, name);
	scamp5_kernel_end();
}

bool image_preprocessing::capture_database(void)
{

	if (stored_images >= 1024)
	{
		return true;
	}
	uint8_t means[64];
	uint8_t row_init = ROWS*(stored_images/(COL_RESOLUTION/COLS)),  col_counter = 0;
	uint8_t row_counter = row_init, indx = 0;

	scamp5_scan_areg_mean_8x8(E, &means[0]);

	scamp5_kernel_begin();
		REFRESH(REG_DB_1);
		REFRESH(REG_DB_2);
		REFRESH(REG_DB_3);
		REFRESH(REG_DB_4);
		CLR(R5);
	scamp5_kernel_end();

	scamp5_load_in(ANALOG_REG_DATABASE,-128);

	//convert_Analogue();
	//*** This code loads the image in the correct slot ***//
	if ((stored_images/(COL_RESOLUTION/(DEPTH*ROWS))) % 2 == 0)
	{
		col_counter = COLS*(stored_images  % 32);
	}
	else
	{
		col_counter = COL_RESOLUTION - COLS*(stored_images  % 32) - COLS;
	}

	for (int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			scamp5_load_point(R5, row_counter, col_counter);
			scamp5_kernel_begin();
				WHERE(R5);
			scamp5_kernel_end();
			scamp5_in(ANALOG_REG_DATABASE, means[indx++] - MAX_VALUE_PIX);
			row_counter ++;
			if (row_counter % 8 == 0)
			{
				col_counter ++;
				row_counter -= 8;
			}

			scamp5_kernel_begin();
				REFRESH(REG_DB_1);
				REFRESH(REG_DB_2);
				REFRESH(REG_DB_3);
				REFRESH(REG_DB_4);
			scamp5_kernel_end();
		}
	}
	if(stored_images == 0)
	{
		scamp5_kernel_begin();
			CLR(REG_DB_1);
			CLR(REG_DB_2);
			CLR(REG_DB_3);
			CLR(REG_DB_4);
		scamp5_kernel_end();
	}
	stored_images++;
	//*********************************************************//
	scamp5_kernel_begin();
		REFRESH(REG_DB_1);
		REFRESH(REG_DB_2);
		REFRESH(REG_DB_3);
		REFRESH(REG_DB_4);
	scamp5_kernel_end();
	//scamp5_load_rect(R5, row_init, col_init, row_init + 8, col_init + 8);
	convert_Digital();
	return false;
}


void image_preprocessing::convert_Analogue(void)
{
	scamp5_in(ANALOG_REG_DATABASE,-128);
	int i = D2A_BITS;
	while(i--){
		int level = (1<<(8 - D2A_BITS))*(1<<i) - 1;
		//int level = 32*(1<<i) - 1;
		scamp5_kernel_all();
		scamp5_in(C,level);// note: range of 'scamp5_in' is [-128,127], thus +128 is out of range
		scamp5_dynamic_kernel_begin();
			WHERE(dreg_map[i]);
				add(ANALOG_REG_DATABASE,ANALOG_REG_DATABASE,C);
			ALL();
		scamp5_dynamic_kernel_end();
	}
}
void image_preprocessing::convert_Digital(void)
{
	int i = D2A_BITS;
	while(i--){
		int level = (1<<(8 - D2A_BITS))*(1<<i) - 1;

//		vs_post_text("level: %d\n", level);
//		int level = 32*(1<<i) - 1;

		scamp5_in(B, level-level_threshold);

		scamp5_kernel_begin();
		    all();
			sub(A, ANALOG_REG_DATABASE, B);
			where(A);
				MOV(R6,FLAG);
			all();
		scamp5_kernel_end();

		scamp5_dynamic_kernel_begin();
			CLR(R5);
			MOV(R5, R6);
			OR(dreg_map[i], R5);
		scamp5_dynamic_kernel_end();

		scamp5_in(B,level-level_threshold);
		scamp5_kernel_begin();
			WHERE(R6);
				sub(ANALOG_REG_DATABASE, ANALOG_REG_DATABASE, B);
			ALL();
		scamp5_kernel_end();

		scamp5_kernel_begin();
			REFRESH(REG_DB_1);
			REFRESH(REG_DB_2);
			REFRESH(REG_DB_3);
			REFRESH(REG_DB_4);
		scamp5_kernel_end();
	}
}

void image_preprocessing::convert_Analogue_input_image(AREG input_reg)
{
	scamp5_kernel_all();
	scamp5_in(input_reg,-128);
	int i = D2A_BITS;
	while(i--){
		int level = (1<<(8 - D2A_BITS))*(1<<i) - 1;
		//int level = 32*(1<<i) - 1;
		scamp5_kernel_all();
		scamp5_in(C,level);// note: range of 'scamp5_in' is [-128,127], thus +128 is out of range
		scamp5_dynamic_kernel_begin();
			WHERE(dreg_temp_map[i]);
				add(input_reg,input_reg,C);
			ALL();
		scamp5_dynamic_kernel_end();
	}
}
void image_preprocessing::convert_Digital_input_image(AREG input_reg)
{
	int i = D2A_BITS;
	while(i--){
//		int level = 32*(1<<i) - 1;
		int level = (1<<(8 - D2A_BITS))*(1<<i) - 1;
		scamp5_in(C, level - 128);
		scamp5_kernel_begin();
			all();
			sub(B, input_reg, C);
			SET(R0);
			where(B);
				MOV(R12,FLAG);
			all();
		scamp5_kernel_end();

		scamp5_dynamic_kernel_begin();
			MOV(dreg_temp_map[i], R12);
		scamp5_dynamic_kernel_end();

		scamp5_in(C,level);
		scamp5_kernel_begin();
			WHERE(R12);
			  sub(input_reg, input_reg, C);
			ALL();
		scamp5_kernel_end();

		scamp5_kernel_begin();
			REFRESH(dreg_temp_map[0]);
			REFRESH(dreg_temp_map[1]);
			REFRESH(dreg_temp_map[2]);
			REFRESH(dreg_temp_map[3]);
		scamp5_kernel_end();
	}
}

void image_preprocessing::load_mem_to_analog_register(AREG SCAMP_register,uint16_t n_image, uint8_t * string_save)
{
	uint8_t row_init = ROWS*(n_image/(COL_RESOLUTION/COLS)),  col_counter = 0;
	uint8_t row_counter = row_init, indx = 0;
	if ((n_image/(COL_RESOLUTION/(DEPTH*ROWS))) % 2 == 0)
	{
		col_counter = COLS*(n_image  % 32);
	}
	else
	{
		col_counter = COL_RESOLUTION - COLS*(n_image  % 32) - COLS;
	}
	for (int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			scamp5_kernel_begin();
				scamp5_load_point(R6, row_counter, col_counter);
				WHERE(R6);
				scamp5_in(SCAMP_register, string_save[indx++] - MAX_VALUE_PIX);
			scamp5_kernel_end();
			row_counter ++;
			if (row_counter % 8 == 0)
			{
				col_counter ++;
				row_counter -= 8;
			}
		}
	}

}


