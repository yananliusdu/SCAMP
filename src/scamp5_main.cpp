/*
 * Scamp5d M0 Example 1 - Image Capturing and Host Display
 *
 */
#include <scamp5.hpp>
#include "..\inc\visual_recognition.hpp"
using namespace SCAMP5_PE;

#define SIZE 3

int main(){
	int it = 0;
	// To setup simulator

	// simulation client configuration
	//vs_sim::config("server_ip","127.0.0.1");
	//vs_sim::config("server_port","27715");
	//vs_sim::enable_keyboard_control();

	// Initialization
	vs_init();

	// Setup Host GUI
	uint16_t result;
    auto display_1 = vs_gui_add_display("Analogue database",0,0);
    auto display_input_image = vs_gui_add_display("Input_image",0,1);

    auto display_input_R7 = vs_gui_add_display("R7 images",1,0);
    auto display_input_R8 = vs_gui_add_display("R8 images",1,1);
    auto display_input_R9 = vs_gui_add_display("R9 images",2,0);
    auto display_input_R10 = vs_gui_add_display("R10 images",2,1);

    auto display30 = vs_gui_add_display("E",3,0);
    auto display31 = vs_gui_add_display("A",3,1);

//    vs_gui_set_info(VS_M0_PROJECT_INFO_STRING);

    //*** The following code loads the database ***//
	visual_recognition Handful_bits(ROWS, COLS, DEPTH);
	Handful_bits.state = creating_database;
	Handful_bits.img_prepoc.collecting_db = false;
	scamp5_kernel_begin();
		ALL();
		res(A);
		CLR(REG_DB_1);
		CLR(REG_DB_2);
		CLR(REG_DB_3);
		CLR(REG_DB_4);
	scamp5_kernel_end();
	scamp5_in(ANALOG_REG_DATABASE, -128);
	Handful_bits.img_prepoc.collecting_db = !load_database();// Database is loaded in register D;

	vs_post_text("Handful_bits.img_prepoc.collecting_db %d\n", Handful_bits.img_prepoc.collecting_db);


	while(1){
		vs_frame_loop_control();

		//*** Refresh the digital registers for the Database ***//
		scamp5_kernel_begin();
			REFRESH(REG_DB_1);
			REFRESH(REG_DB_2);
			REFRESH(REG_DB_3);
			REFRESH(REG_DB_4);
		scamp5_kernel_end();

		//*** Get input image ***//
        int gain = vs_gui_read_slider(VS_GUI_FRAME_GAIN);

//        scamp5_kernel_begin();
//             get_image(E,C);
// 		scamp5_kernel_end();

        //upload fall images to E

        load_image_input_register(E);

        //load_image_input_register(E);
        scamp5_output_image(E, display_input_image);

//        result = Handful_bits.FSM_visual_recognition();

        //*** Convert database to Analogue, if captured **//

//        vs_post_text("collecting_db %d\n", Handful_bits.img_prepoc.collecting_db);

        Handful_bits.img_prepoc.collecting_db = true;

        if(Handful_bits.img_prepoc.collecting_db == false)
        {
        	Handful_bits.img_prepoc.convert_Analogue();
        }
        //*** Otherwise, capture it **//
        else if(Handful_bits.img_prepoc.capture_database())
        {
        	vs_gui_save_image(display_input_R7, "0.bmp");
        	vs_gui_save_image(display_input_R8, "1.bmp");
        	vs_gui_save_image(display_input_R9, "2.bmp");
        	vs_gui_save_image(display_input_R10, "3.bmp");
        	Handful_bits.img_prepoc.collecting_db = false;
        }

        if(vs_gui_is_on())
         {
         	scamp5_output_image(E, display30);
         	scamp5_output_image(R5, display31);
     		scamp5_output_image(D, display_1);
     		scamp5_output_image(R7, display_input_R7);
     		scamp5_output_image(R8, display_input_R8);
     		scamp5_output_image(R9, display_input_R9);
     		scamp5_output_image(R10, display_input_R10);
         }

        //vs_post_text("Result: %d\n", it);
        it ++;

    }

	return 0;
}
