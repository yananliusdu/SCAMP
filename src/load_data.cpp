/*
 * load_data.cpp
 *
 *  Created on: 10 Jun 2020
 *      Author: DELL
 */
#include <stdio.h>
#include "..\inc\load_data.hpp"
#include "..\inc\global_declarations.hpp"

using namespace SCAMP5_PE;
const dreg_t dreg_map[4] = {R11, R6, R5, R4};
const dreg_t database_map[4] = {R10, R9, R8, R7};

char *fc = "fc";
char *conv = "conv";
char *bmp = ".bmp";
char route[120] = "E:\\DeepCNN\\Hector\\fall_images_bmp\\fall_";
char routeDB[20] = "database.bmp";
uint16_t iteration = 0;
uint8_t load_image_bottom_value = -50;

bool load_database(void)
{
	vs_post_text("first register!\n");
	if(!load_data(0, database_map[3], 0))
	{
		vs_post_text("Database not loaded!\n");
		return false;
	}
	vs_post_text("second register!\n");
	if(!load_data(0, database_map[2], 1))
	{
		vs_post_text("Database not loaded!\n");
		return false;
	}
	vs_post_text("third register!\n");
	if(!load_data(0, database_map[1], 2))
	{
		vs_post_text("Database not loaded!\n");
		return false;
	}
	vs_post_text("fourth register!\n");
	if(!load_data(0, database_map[0], 3))
	{
		vs_post_text("Database not loaded!\n");
		return false;
	}

	if(vs_gui_request_done() && vs_gui_is_on()){
		scamp5_in(ANALOG_REG_DATABASE,-128);
		int i = D2A_BITS;
		while(i--){
			int level = (1<<(8 - D2A_BITS))*(1<<i) - 1;
			scamp5_kernel_all();
			scamp5_in(C,level);// note: range of 'scamp5_in' is [-128,127], thus +128 is out of range
			scamp5_dynamic_kernel_begin();
				WHERE(dreg_map[i]);
				add(ANALOG_REG_DATABASE,ANALOG_REG_DATABASE,C);
				ALL();
			scamp5_dynamic_kernel_end();
		}
	}
	vs_post_text("Database loaded!\n");
	return true;
}

void load_image_input_register(AREG reg)
{
	if(vs_gui_request_done() && vs_gui_is_on()){
		char buf[120];

		sprintf(buf, "%s%04d%s", route, iteration, bmp);
		vs_gui_request_image(buf, D2A_BITS,[&](vs_dotmat const& dotmat,int s){
			if(s<0){
				vs_post_text("image not received in time!\n");
				return;
			}else
			if(s<D2A_BITS){
				load_dreg_image(dreg_map[s],(const uint8_t*)dotmat.get_buffer(),dotmat.get_height(),dotmat.get_width());
			}
		});
		do{
			refresh_dreg_storage();
			scamp5_kernel_begin();
				REFRESH(REG_DB_1);
				REFRESH(REG_DB_2);
				REFRESH(REG_DB_3);
				REFRESH(REG_DB_4);
			scamp5_kernel_end();
			vs_process_message();
		}while(!vs_gui_request_done());

		scamp5_in(reg,load_image_bottom_value);
		int i = D2A_BITS;
		while(i--){
			int level = (1<<(8 - D2A_BITS))*(1<<i) - 1;
			scamp5_in(B,level);// note: range of 'scamp5_in' is [-128,127], thus +128 is out of range
			scamp5_dynamic_kernel_begin();
				WHERE(dreg_map[i]);
				  add(reg, reg, B);
				ALL();
			scamp5_dynamic_kernel_end();
			scamp5_kernel_begin();
				REFRESH(REG_DB_1);
				REFRESH(REG_DB_2);
				REFRESH(REG_DB_3);
				REFRESH(REG_DB_4);
			scamp5_kernel_end();
		}
	}
	iteration++;
}

bool load_data(int data_type, DREG Dreg, int data_num)
{
	bool database_loaded = true;
	if(vs_gui_request_done() && vs_gui_is_on()){
		char *buf = new char[sizeof(data_num) + strlen(bmp) +1];
		sprintf(buf, "%d%s", data_num, bmp);
		vs_post_text(buf);
		vs_post_text("\n");
		vs_gui_request_image(buf,1,[&](vs_dotmat const& dotmat,int s){
			if(s<0){
				vs_post_text("image not received in time!\n");
				database_loaded = false;
				return;
			}
			load_dreg_image(Dreg,(const uint8_t*)dotmat.get_buffer(),dotmat.get_height(),dotmat.get_width());
			vs_post_text("dreg image loaded: %hd x %hd \n",dotmat.get_width(),dotmat.get_height());
		});
		do{
		    scamp5_kernel_begin();
		    	REFRESH(Dreg);
			scamp5_kernel_end();
			vs_process_message();
		}while(!vs_gui_request_done());
	}
	return database_loaded;
}

void load_dreg_image(DREG target_dreg,const uint8_t*image_buffer,uint16_t n_rows,uint16_t n_cols)
{
	const size_t row_bytes = n_cols/8;
    scamp5_dynamic_kernel_begin();
    	CLR(target_dreg);
	scamp5_dynamic_kernel_end();
    scamp5_draw_begin(target_dreg);
    for(int r=0;r<n_rows;r++){
		const uint8_t*row_array = &image_buffer[r*row_bytes];
		int u = 0;
		while(u<(n_cols/8)){
			if(row_array[u]==0x00){
				u += 1;
			}else
			if(row_array[u]==0xFF){
				int u0 = u;
				int u1 = u;
				u += 1;
				while(u<(n_cols/8)){
					if(row_array[u]==0xFF){
						u1 = u;
						u += 1;
					}else{
						break;
					}
				};
				scamp5_draw_rect(r,u0*8,r,u1*8 + 7);
			}else{
				uint8_t w = row_array[u];
				uint8_t m = 1;
				for(int c=u*8;c<(u*8 + 8);c++){
					if(w&m){
						scamp5_draw_pixel(r,c);
					}
					m <<= 1;
				}
				u += 1;
			}
		}
		if(r%16==0){
			refresh_dreg_storage();
		}
	}
	scamp5_draw_end();
}

void refresh_dreg_storage(){
	scamp5_kernel_begin();
		REFRESH(dreg_map[0]);
		REFRESH(dreg_map[1]);
		REFRESH(dreg_map[2]);
		REFRESH(dreg_map[3]);
	scamp5_kernel_end();
}

// this function store trained parameters from BN to an analogue register
void setBN_offset(int param[], AREG Areg, int BN)
{
	scamp5_kernel_begin();
		CLR(R0);
		res(Areg);
	scamp5_kernel_end();

	int width = 0;
	int start_x = 0;
	int start_y = 0;
	int count = 0;

	switch(BN)
	{
		case 1:
			width = 64;
			start_x = 0;
			start_y = 0;
			for(int i = 0; i < 4; i++)
			{
				for(int j = 0; j < 4; j++)
				{
					start_x = width*i;
					start_y = 192 - width*j;
					scamp5_load_in(param[count]);
					scamp5_draw_begin(R0);
						scamp5_draw_rect(start_x, start_y, start_x+width-1,start_y+width-1);
					scamp5_draw_end();
					scamp5_kernel_begin();
						WHERE(R0);
						  mov(Areg,IN);
						all();
						CLR(R0);
					scamp5_kernel_end();
					count++;
				}
			}
			break;

		case 2:
			width = 32;
			for(int i = 0; i < test_num; i++)
			{
				int load_in = param[count];
				if(load_in > 127)
				{
					load_in = 127;
				}
				else if (load_in < -128)
				{
					load_in = -128;
				}
				scamp5_load_in(load_in);
				scamp5_draw_begin(R0);
					scamp5_draw_rect(start_point[i][0], start_point[i][1], start_point[i][0]+width-1,start_point[i][1]+width-1);
				scamp5_draw_end();

				scamp5_kernel_begin();
					WHERE(R0);
					  mov(Areg,IN);
					all();
					CLR(R0);
				scamp5_kernel_end();
				count++;
			}

			break;

	}

}


void acc_test_fn(int load_num, int predicted_digit, unsigned int &correct_num, unsigned int con_mat[10][10])
{
	if(load_num <=979)
	{
       if(predicted_digit == 0)
       {
    	   correct_num++;
       }
       con_mat[0][predicted_digit]++;
	}
	else if( load_num > 979 && load_num <= 2112)
	{
           if(predicted_digit == 1)
           {
        	   correct_num++;
           }
	   con_mat[1][predicted_digit]++;
	}
	else if( load_num > 2112 && load_num <= 3142)
	{
           if(predicted_digit == 2)
           {
        	   correct_num++;
           }
       con_mat[2][predicted_digit]++;
	}
	else if( load_num > 3142 && load_num <= 4152)
	{
           if(predicted_digit == 3)
           {
        	   correct_num++;
           }
           con_mat[3][predicted_digit]++;
	}
	else if( load_num > 4152 && load_num <= 5134)
	{
           if(predicted_digit == 4)
           {
        	   correct_num++;
           }
           con_mat[4][predicted_digit]++;
	}
	else if( load_num > 5134 && load_num <= 6024)
	{
           if(predicted_digit == 5)
           {
        	   correct_num++;
           }
           con_mat[5][predicted_digit]++;
	}
	else if( load_num > 6024 && load_num <= 6978)
	{
           if(predicted_digit == 6)
           {
        	   correct_num++;
           }
           con_mat[6][predicted_digit]++;
	}
	else if( load_num > 6978 && load_num <= 8006)
	{
           if(predicted_digit == 7)
           {
        	   correct_num++;
           }
           con_mat[7][predicted_digit]++;
	}
	else if( load_num > 8006 && load_num <= 8978)
	{
           if(predicted_digit == 8)
           {
        	   correct_num++;
           }
           con_mat[8][predicted_digit]++;
	}
	else if( load_num > 8978 && load_num <= 9985)
	{
           if(predicted_digit == 9)
           {
        	   correct_num++;
           }
           con_mat[9][predicted_digit]++;
	}

}
