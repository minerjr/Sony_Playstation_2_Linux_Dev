/****************************************************************************\
|* Test program for the framework
|* ==============================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: SjG, various others
|* First created			: Thu Jul 18 13:25:57 GMT 2002
|* Modification history		: See class header files
|*
\****************************************************************************/

extern "C" {
#include "joypadlib.h"
}
#include "framework.h"
#include "image.h"
#include "sprite.h"
#include "model.h"

	static ps2_vu0_fvector camera_p = { 0.0f, 0.0f, -1000.0f, 0.0f };
	static ps2_vu0_fvector camera_zd = { 0.0f, 0.0f, 1.0f, 1.0f };
	static ps2_vu0_fvector camera_yd = { 0.0f, 1.0f, 0.0f, 1.0f };
	static ps2_vu0_fvector camera_rot = { 0, 0, 0, 0 };
	static ps2_vu0_fvector light0 = { 1.0f, 0.0f, 0.3f, 0.0f };
	static ps2_vu0_fvector light1 = { 0.0f, 1.0f, 0.3f, 0.0f };
	static ps2_vu0_fvector light2 = { 0.0f, 0.0f, 1.0f, 0.0f };
	static ps2_vu0_fvector color0 = { 0.4f, 0.4f, 0.4f, 1.0f };
	static ps2_vu0_fvector color1 = { 0.4f, 0.4f, 0.4f, 1.0f };
	static ps2_vu0_fvector color2 = { 0.4f, 0.4f, 0.4f, 1.0f };
	static ps2_vu0_fvector ambient = { 0.2f, 0.2f, 0.2f, 0.0f };
	static ps2_vu0_fmatrix local_world;
	static ps2_vu0_fmatrix world_view;
	static ps2_vu0_fmatrix view_screen;
	static ps2_vu0_fmatrix local_screen;
	static ps2_vu0_fmatrix normal_light;
	static ps2_vu0_fmatrix light_color;
	static ps2_vu0_fmatrix local_light;
	static ps2_vu0_fmatrix work;
	
int main(int argc, char **argv)
	{
	FILE *imglist;

	imglist = fopen("images.txt", "r");
	char imgname1[128];
	char imgname2[128];
	
	fscanf(imglist, "%s", imgname1);
	fscanf(imglist, "%s", imgname2);
	fclose(imglist);
	
	int active_joypad = 0;
	int buttons;
	float button_pressure[PS2PAD_NUM_PRESSURE_BUTTONS];
	float joysticks1[ 2 ][ 2 ];
	float joysticks2[ 2 ][ 2 ];
	float ballvel[2];

	Framework *ps2 = new Framework(argc, argv);
	ps2->start();
	ps2_gs_gparam *params = ps2->getParams();

	// initialize joypadlib
	joypadlib_initialize();
	
	/************************************************************************\
	|* Initialise the background image
	\************************************************************************/
	Image im(imgname1);
	Image im2(imgname2);


	im.setAddress(ps2);
	im2.setAddress(ps2);
	Sprite tux(ps2, &im, 0, 0, (params->width)<<4, (params->height)<<4);
	Sprite tux2(ps2, &im2, 0, 0, (params->width)<<4, (params->height)<<4);
	
	/************************************************************************\
	|* Load in a model
	\************************************************************************/
	Model model("lib/models/tris.md2");
	Model model2("lib/models/tris.md2");
	Model model3("lib/models/tris.md2");

	_log << "Build model packets\n";	
	//model.frameIndex = 192;
	model.buildPacket();
	model2.buildPacket();
	model3.buildPacket();


	//model3.dump();

	model.objTrans[0] = -500.0f;
	model2.objTrans[0] = 500.0f;

	model.objRot[1] = -M_PI / 2.0f;
	model2.objRot[1] = M_PI / 2.0f;

	ballvel[0] = 10.0f;
	ballvel[1] = 10.0f;
	
	
	/************************************************************************\
	|* Run the main loop
	\************************************************************************/
	while (1)
		{

		model.runAnimation("ru");
		model2.runAnimation("death1");
		model3.runAnimation("ru");
		// joypad stuff
		joypadlib_update();
		buttons = joypadlib_get_joypad_buttons( active_joypad, button_pressure, PRESSURE_TYPE_FLOAT );
//		if( buttons & PS2PAD_BUTTON_CROSS )
//			break;
		if( button_pressure[ PS2PAD_PRESSURE_CROSS ] > 0.0f )
			break;
		else if( button_pressure[ PS2PAD_PRESSURE_DIGITAL_UP ] > 0.0f )
			model.objTrans[1] -= 10.0f * button_pressure[ PS2PAD_PRESSURE_DIGITAL_UP ];
		else if( button_pressure[ PS2PAD_PRESSURE_DIGITAL_DOWN ] > 0.0f )
			model.objTrans[1] += 10.0f * button_pressure[ PS2PAD_PRESSURE_DIGITAL_DOWN ];

		if( model.objTrans[1] < -300.0f )
			model.objTrans[1] = -300.0f;
		if( model.objTrans[1] > 300.0f )
			model.objTrans[1] = 300.0f;

		joypadlib_get_joypad_sticks( active_joypad, joysticks1[0], joysticks1[1] );

//		model.objTrans[0] += joysticks[1][0] * 5.0f;
//		model.objTrans[1] += joysticks[1][1] * 5.0f;

		buttons = joypadlib_get_joypad_buttons( 1 - active_joypad, button_pressure, PRESSURE_TYPE_FLOAT );
		if( button_pressure[ PS2PAD_PRESSURE_CROSS ] > 0.0f )
			break;
		else if( button_pressure[ PS2PAD_PRESSURE_DIGITAL_UP ] > 0.0f )
			model2.objTrans[1] -= 10.0f * button_pressure[ PS2PAD_PRESSURE_DIGITAL_UP ];
		else if( button_pressure[ PS2PAD_PRESSURE_DIGITAL_DOWN ] > 0.0f )
			model2.objTrans[1] += 10.0f * button_pressure[ PS2PAD_PRESSURE_DIGITAL_DOWN ];

		if( model2.objTrans[1] < -300.0f )
			model2.objTrans[1] = -300.0f;
		if( model2.objTrans[1] > 300.0f )
			model2.objTrans[1] = 300.0f;

		joypadlib_get_joypad_sticks( 1 - active_joypad, joysticks2[0], joysticks2[1] );

//		model2.objTrans[0] += joysticks[1][0] * 5.0f;
//		model2.objTrans[1] += joysticks[1][1] * 5.0f;

		model3.objRot[0] += rand()%1000 / 1000.0f * M_PI / 10.0f;
		model3.objRot[1] += rand()%1000 / 1000.0f * M_PI / 10.0f;
		model3.objRot[2] += rand()%1000 / 1000.0f * M_PI / 10.0f;

		while( model3.objRot[0] > M_PI * 2.0f )
			model3.objRot[0] -= M_PI * 2.0f;
		while( model3.objRot[1] > M_PI * 2.0f )
			model3.objRot[1] -= M_PI * 2.0f;
		while( model3.objRot[2] > M_PI * 2.0f )
			model3.objRot[2] -= M_PI * 2.0f;

		model3.objTrans[0] += ballvel[0];
		model3.objTrans[1] += ballvel[1];

		if( model3.objTrans[0] < -400.0f ) {
			if( model3.objTrans[1] - model.objTrans[1] > -175.0f &&
				model3.objTrans[1] - model.objTrans[1] < 175.0f ) {
				model3.objTrans[0] = -400.0f;
				ballvel[0] = ( 1.5f + joysticks1[1][0] ) * 10.0f;
				ballvel[1] = joysticks1[1][1] * 20.0f;
			}
			else {
				model3.objTrans[0] = 200.0f;
				ballvel[0] = -10.0f;
				ballvel[1] = 10.0f;
			}
		}
		if( model3.objTrans[0] > 400.0f ) {
			if( model3.objTrans[1] - model2.objTrans[1] > -175.0f &&
				model3.objTrans[1] - model2.objTrans[1] < 175.0f ) {
				model3.objTrans[0] = 400.0f;
				ballvel[0] = ( 1.5f - joysticks2[1][0] ) * -10.0f;
				ballvel[1] = joysticks2[1][1] * 20.0f;
			}
			else {
				model3.objTrans[0] = -200.0f;
				ballvel[0] = 10.0f;
				ballvel[1] = 10.0f;
			}
		}

		if( model3.objTrans[1] < -330.0f ) {
			model3.objTrans[1] = -330.0f;
			ballvel[1] *= -1.0f;
		}
		if( model3.objTrans[1] > 330.0f ) {
			model3.objTrans[1] = 330.0f;
			ballvel[1] *= -1.0f;
		}
		//tux.setXY((int)model3.objTrans[0] * 10,(int)model3.objTrans[1] * 10);
		tux2.draw();
		tux.draw();		
		// view -> screen matrix
		ps2_vu0_view_screen_matrix(view_screen, 512.0f, 1.0f,
			params->pixel_ratio, params->center_x, params->center_y, 
			1.0f, ((1 << 24) - 1), 1.0f, 65536.0f);

		// color&normal matrix setting
		ps2_vu0_normal_light_matrix(normal_light, light0, light1, light2);

		// world -> view matrix
		vu0_rot_camera_matrix(world_view, camera_p, camera_zd, camera_yd,
								camera_rot);

		// light color
		ps2_vu0_light_color_matrix(light_color,color0, color1, color2, ambient);
		
		
		// Rotate the model
/*		if( button & PS2PAD_PRESSURE_DIGITAL_LEFT )
			model.objRot[1] += M_PI/150.0f;
		else if( buttons & PS2PAD_PRESSURE_DIGITAL_RIGHT )
			model.objRot[1] -= M_PI/150.0f;*/
		
		// Rotation must be between -PI and PI, otherwise it does weird things
		while (model.objRot[1] > M_PI)
		 	model.objRot[1] -= 2.0f*M_PI;
		while( model.objRot[1] < -M_PI )
			model.objRot[1] += 2.0f * M_PI;

		while (model2.objRot[1] > M_PI)
		 	model2.objRot[1] -= 2.0f*M_PI;
		while( model2.objRot[1] < -M_PI )
			model2.objRot[1] += 2.0f * M_PI;

		// local -> world (rotate)matrix
		ps2_vu0_unit_matrix(work);
		ps2_vu0_rot_matrix(work, work, model.objRot0);
		ps2_vu0_rot_matrix(local_world, work, model.objRot);

		// light(normal) -> local_light matrix
		ps2_vu0_mul_matrix(local_light, normal_light, local_world);

		// local -> world (rotate&translate)matrix
		ps2_vu0_trans_matrix(local_world, local_world, model.objTrans);

		// local -> screen matrix
		ps2_vu0_mul_matrix(work, world_view, local_world);
		ps2_vu0_mul_matrix(local_screen, view_screen, work);

		// if we got the pointers for the matrices in the dma list
		if (model.localScreen != NULL)
			{
			// copy the tranform matrices into the dma list
			memcpy(model.localScreen, local_screen, sizeof(ps2_vu0_fmatrix));
			memcpy(model.localLight,  local_light,  sizeof(ps2_vu0_fmatrix));
			memcpy(model.lightColour, light_color,  sizeof(ps2_vu0_fmatrix));
			}

		// send it off
		model.draw(ps2);
				
		// local -> world (rotate)matrix
		ps2_vu0_unit_matrix(work);
		ps2_vu0_rot_matrix(work, work, model2.objRot0);
		ps2_vu0_rot_matrix(local_world, work, model2.objRot);

		// light(normal) -> local_light matrix
		ps2_vu0_mul_matrix(local_light, normal_light, local_world);

		// local -> world (rotate&translate)matrix
		ps2_vu0_trans_matrix(local_world, local_world, model2.objTrans);

		// local -> screen matrix
		ps2_vu0_mul_matrix(work, world_view, local_world);
		ps2_vu0_mul_matrix(local_screen, view_screen, work);

		// if we got the pointers for the matrices in the dma list
		if (model2.localScreen != NULL)
			{
			// copy the tranform matrices into the dma list
			memcpy(model2.localScreen, local_screen, sizeof(ps2_vu0_fmatrix));
			memcpy(model2.localLight,  local_light,  sizeof(ps2_vu0_fmatrix));
			memcpy(model2.lightColour, light_color,  sizeof(ps2_vu0_fmatrix));
			}

		// send it off
		model2.draw(ps2);
				
		// local -> world (rotate)matrix
		ps2_vu0_unit_matrix(work);
		ps2_vu0_rot_matrix(work, work, model3.objRot0);
		ps2_vu0_rot_matrix(local_world, work, model3.objRot);

		// light(normal) -> local_light matrix
		ps2_vu0_mul_matrix(local_light, normal_light, local_world);

		// local -> world (rotate&translate)matrix
		ps2_vu0_trans_matrix(local_world, local_world, model3.objTrans);

		// local -> screen matrix
		ps2_vu0_mul_matrix(work, world_view, local_world);
		ps2_vu0_mul_matrix(local_screen, view_screen, work);

		// if we got the pointers for the matrices in the dma list
		if (model3.localScreen != NULL)
			{
			// copy the tranform matrices into the dma list
			memcpy(model3.localScreen, local_screen, sizeof(ps2_vu0_fmatrix));
			memcpy(model3.localLight,  local_light,  sizeof(ps2_vu0_fmatrix));
			memcpy(model3.lightColour, light_color,  sizeof(ps2_vu0_fmatrix));
			}

		// send it off
		model3.draw(ps2);
						
		im.upload(ps2);
		im2.upload(ps2);
		//model.runAnimation("stand");
		ps2->flip();

		//joypadlib_shutdown();
		
		//return 1;
		}
	joypadlib_shutdown();

	}
	
