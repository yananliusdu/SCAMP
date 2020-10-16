/*
 * visual_recognition.cpp
 *
 *  Created on: 7 Jun 2020
 *      Author: Hector Castillo
 */
#include "..\inc\visual_recognition.hpp"

using namespace SCAMP5_PE;
const dreg_t dreg_map[4] = {R11, R6, R5, R4};

//******************************************************//
/*!
    @brief Constructor for the class visual recognition

    @param rows   	  Number of rows, must be power of 2
    @param columns    Number of columns, must be power of 2
    @param res		  Depth of every pixel
*/
visual_recognition::visual_recognition(uint8_t r, uint8_t c, uint8_t res)
{
	fails = 0;
	rows = r;
	columns = c;
	resolution = res;
	mask = 1;
	for (int i = 1; i < resolution; i++)
	{
		mask = (mask << 1) + 1;
	}

	state = init;

	img_comp.rows = r;
	img_comp.columns = c;
	img_comp.resolution = res;
	img_comp.mask = mask;

	img_prepoc.rows = r;
	img_prepoc.columns = c;
	img_prepoc.resolution = res;
	img_prepoc.mask = mask;
	img_prepoc.resolution_multiplier = 0x80 >> (resolution-1); // 128 shifted resolution-1 places;

	img_prepoc.collecting_db = true;
}


/*!
    @brief State machine for the particle filter

    @param 	No parameters
    @return Node in the topological map

*/
uint16_t visual_recognition::FSM_visual_recognition(void)
{
	uint16_t result = 0;
	state = img_prepoc.preprocess_image();

	vs_post_text("state: %d\n", state);

	switch(state)
	{
	case init:
		return result;
	case creating_database:
		//*** The database is already created ***//
		/***** First mechanism to stop database collection ****/
		if (img_prepoc.collecting_db == true)
		{
			break;
		}
		state = creating_M;
		img_comp.set_stored_images(img_prepoc.stored_images);
		seq_recognition.set_stored_images(img_prepoc.stored_images);

	case calculating_likehood:
		//**** Set some parameters ****//
		img_comp.set_stored_images(img_prepoc.stored_images);
		seq_recognition.set_stored_images(img_prepoc.stored_images);

		//**** First apply the motion model ****//
		if(img_comp.n_series > 0)
		{
			img_comp.move_next_image(img_prepoc.direction);
		}
		//**** Do the comparison *****//
		img_comp.absolute_differences();
		if(img_comp.n_series < FRAME_LENGTH)
		{
			break;
		}
		state = closing_loop;
	case closing_loop:
		result = seq_recognition.find_best_match(img_comp.n_series, false);
		state = update_and_resampling;
		if (result == 65535)
		{
			if(fails <= 4*FRAME_LENGTH)
			{
				fails++;
			}
		}
		else if(fails > 1)
		{
			fails -= 1;
		}
		if (fails >= 4*FRAME_LENGTH)
		{
			break;
		}
	case update_and_resampling:
		if((img_comp.n_series % FRAME_LENGTH) == 0)
		{
			seq_recognition.resampling();
		}
		break;
	default:
		break;
	}
	return result;
}
