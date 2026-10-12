#include <stdio.h>
#include "dmascheduler.h"
#include "dmapacket.h"
#include "dmamemory.h"
#include "addregisters.h"
#include "modelloader.h"
#include "vumgr.h"
#include "matrix.h"
#include "custommodel.h"
#include "keyframe.h"
#include "texture.h"
#include "ps2defines.h"
#include "gameclock.h"
#include "level.h"
#include "input.h"
#include "collision.h"
#include <math.h>
#include <time.h>

#define STARTSCREEN1 0
#define STARTSCREEN2 1
#define MAINMENU 2
#define GAME 3
#define INGAMEMENU 4
#define EXIT 5
#define CHANGETOGAME 6
#define CHANGETOINGAMEMENU 7
#define CHANGETOMAINMENU 8
#define CHANGETOSTARTSCREEN1 9
#define CHANGETOSTARTSCREEN2 10

#define MAINMENUMAX 2
#define INGAMEMENUMAX 5

Dma_Memory_Engine *DmaMemoryEngine;
Dma_Memory_Scheduler *DmaMemoryScheduler;
VU_Manager *VUManager;
Matrix_Manager *MatrixManager;
Texture_Engine *TextureEngine;
Game_Clock_Engine *GameClockEngine;
Level_Engine *LevelEngine;
Keyframe_Model_Engine *KeyframeEngine;
Input_Engine *InputEngine;
PS2Pad *joypad;

Collision_Detection *playerCollision;
Collision_Detection *npcCollision;


int currentGameState = CHANGETOSTARTSCREEN1;
int prevGameState = CHANGETOSTARTSCREEN1;
int cursorIndex = 0;

bool done = false;
time_t currentTime;
time_t prevTime;

int runAnimation = 0;

vectorf mainCharacterPosition;
vectorf mainCharacterTestPosition;
vectorf npcPosition;

float mainCharacterAngle = 0.0f;
float worldAngle = 0.0f;
float playerSpeed = 2.0f;
float cameraDistance = -100.0f;


Custom_Model *backGroundImages;
Custom_Model *unhighlightedOptions;
Custom_Model *highlightedOptions;


int setupGame();
int shutDownGame();

int doInput();
int doDraw();
int doGameLogic();

int main()
{

	setupGame();
	printf("Game Started\n");
		
	while(!done)
	{
		runAnimation = 0;
		doInput();
		
		doGameLogic();

		doDraw();

	}


	shutDownGame();

	return 0;

}

int doGameLogic()
{
	//Do Game Logic
	switch(currentGameState)
	{
	case STARTSCREEN1:
		{
			time(&currentTime);
			if (currentTime - prevTime > 6)
			{
				prevTime = currentTime;
				prevGameState = currentGameState;
				currentGameState = CHANGETOSTARTSCREEN2;
			}
		}
		break;
	case STARTSCREEN2:
		{
			time(&currentTime);
			if (currentTime - prevTime > 6)
			{
				prevTime = currentTime;
				prevGameState = currentGameState;
				currentGameState = CHANGETOMAINMENU;
			}
		}
		break;
	case CHANGETOGAME:
		{
			//Add the texture to the dma scheduler

			DmaMemoryScheduler->clearList(CHANNEL1);
			DmaMemoryScheduler->add(TextureEngine->getPacket(5));
			DmaMemoryScheduler->add(LevelEngine->getPacket(0));

			DmaMemoryScheduler->add(TextureEngine->getPacket(4));
			//Add main character model
			DmaMemoryScheduler->add(KeyframeEngine->getFirstPacket(0));
			DmaMemoryScheduler->add(KeyframeEngine->getSecondPacket(0));	
			//Add NPC model
			DmaMemoryScheduler->add(KeyframeEngine->getFirstPacket(1));
			DmaMemoryScheduler->add(KeyframeEngine->getSecondPacket(1));	
			currentGameState = GAME;

		}
		break;
	case CHANGETOMAINMENU:
		{
			//Add the different backgrounds
			DmaMemoryScheduler->clearList(CHANNEL1);
			DmaMemoryScheduler->clearList(CHANNEL2);
			DmaMemoryScheduler->add(TextureEngine->getPacket(1));
			DmaMemoryScheduler->add(TextureEngine->getPacket(2));
			DmaMemoryScheduler->add(TextureEngine->getPacket(3));
			
			DmaMemoryScheduler->add(backGroundImages->getModelPacket());		
			//Add the unhighlighted options
			DmaMemoryScheduler->add(unhighlightedOptions->getModelPacket());
			//Add the highlighted options
			DmaMemoryScheduler->add(highlightedOptions->getModelPacket());


			
			currentGameState = MAINMENU;
		}
		break;
	case CHANGETOINGAMEMENU:
		{
			//Add the different backgrounds
			DmaMemoryScheduler->clearList(CHANNEL1);
			DmaMemoryScheduler->clearList(CHANNEL2);
			DmaMemoryScheduler->add(TextureEngine->getPacket(1));
			DmaMemoryScheduler->add(TextureEngine->getPacket(2));
			DmaMemoryScheduler->add(TextureEngine->getPacket(3));

			DmaMemoryScheduler->add(backGroundImages->getModelPacket());					
			//Add the unhighlighted options
			DmaMemoryScheduler->add(unhighlightedOptions->getModelPacket());
			//Add the highlighted options
			DmaMemoryScheduler->add(highlightedOptions->getModelPacket());

			

			
			currentGameState = INGAMEMENU;
		}
		break;
	case CHANGETOSTARTSCREEN1:
		{
			//Add the different backgrounds
			DmaMemoryScheduler->clearList(CHANNEL1);
			DmaMemoryScheduler->clearList(CHANNEL2);
			DmaMemoryScheduler->add(TextureEngine->getPacket(0));
			DmaMemoryScheduler->add(backGroundImages->getModelPacket());
			currentGameState = STARTSCREEN1;
		}
		break;
	case CHANGETOSTARTSCREEN2:
		{
			//Add the different backgrounds
			DmaMemoryScheduler->clearList(CHANNEL1);
			DmaMemoryScheduler->clearList(CHANNEL2);
			DmaMemoryScheduler->add(TextureEngine->getPacket(1));
			DmaMemoryScheduler->add(backGroundImages->getModelPacket());
			backGroundImages->setTextureID(1);
			currentGameState = STARTSCREEN2;
		}
		break;
	case EXIT:
		{
			done = true;
		}
		break;
	}

	return 0;
}

int doDraw()
{

	//FILE *testFile = fopen("/root/test3.txt", "a+");
	switch(currentGameState)
	{
	case STARTSCREEN1:
	case STARTSCREEN2:
		{
			MatrixManager->setMode(MODELVIEW);
			MatrixManager->push();
			MatrixManager->loadIdentity();
			MatrixManager->translate(0.0f, 0.0f, -40.0f);
			backGroundImages->begin(SPRITE, VERTEX_T | COLOR_T | NORMAL_T | ST_T);
			{
				
				//Set the models normal
				backGroundImages->addNormal(0.0f, 0.0f, -1.0f);
				//Must add st first, color second then the vertex data
				//Set the texture coordinate.
				//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
				backGroundImages->addST(0.0f, 0.0f);
				//Set the color of the vertex. can set the alpha value as alpha is enabled
				backGroundImages->addColor(0.5f, 0.5f, 0.5f, 1.0f);
				//Finally add the vertex
				backGroundImages->addVertex(-16.0f, 16.0f, 0.0f);

				//Must add st first, color second then the vertex data
				//Set the texture coordinate.
				//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
				backGroundImages->addST(1.0f, 1.0f);
				//Set the color of the vertex. can set the alpha value as alpha is enabled
				backGroundImages->addColor(0.5f, 0.5f, 0.5f, 1.0f);
				//Finally add the vertex
				backGroundImages->addVertex(16.0f, -16.0f, 0.0f);

			}
			backGroundImages->end();
			MatrixManager->pop();

			//Clear the screen with the default color
			sps2UScreenClear(0x00, 0x00, 0x00);
			//Build the list of things to transfer along
			//Dma channel 1 and 2
			DmaMemoryScheduler->buildList(CHANNEL1);
			//Start the transfer along channel 1
			DmaMemoryScheduler->transfer(CHANNEL1);
			//Wait for the transfer to finish
			DmaMemoryScheduler->waitForTransfer(CHANNEL1);

			DmaMemoryScheduler->buildList(CHANNEL2);
			//Start the transfer along channel 1
			DmaMemoryScheduler->transfer(CHANNEL2);
			//Wait for the transfer to finish
			DmaMemoryScheduler->waitForTransfer(CHANNEL2);

			sps2UScreenSwap();
			
		}
		break;

	case MAINMENU:
		{
			MatrixManager->setMode(MODELVIEW);
			
			MatrixManager->push();
			MatrixManager->loadIdentity();
			MatrixManager->translate(0.0f, 0.0f, -40.0f);
			backGroundImages->begin(SPRITE, VERTEX_T | COLOR_T | NORMAL_T | ST_T);
			{
				
				//Set the models normal
				backGroundImages->addNormal(0.0f, 0.0f, -1.0f);
				//Must add st first, color second then the vertex data
				//Set the texture coordinate.
				//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
				backGroundImages->addST(0.0f, 0.0f);
				//Set the color of the vertex. can set the alpha value as alpha is enabled
				backGroundImages->addColor(0.125f, 0.125f, 0.125f, 1.0f);
				//Finally add the vertex
				backGroundImages->addVertex(-16.0f, 16.0f, 0.0f);

				//Must add st first, color second then the vertex data
				//Set the texture coordinate.
				//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
				backGroundImages->addST(1.0f, 1.0f);
				//Set the color of the vertex. can set the alpha value as alpha is enabled
				backGroundImages->addColor(0.125f, 0.125f, 0.125f, 1.0f);
				//Finally add the vertex
				backGroundImages->addVertex(16.0f, -16.0f, 0.0f);

			}
			backGroundImages->end();
			MatrixManager->pop();
			
			//backGroundImages->getModelPacket()->setSend(0);
			//highlightedOptions->getModelPacket()->setSend(0);
//Custom_Model *unhighlightedOptions;
//Custom_Model *highlightedOptions;

			MatrixManager->push();
			MatrixManager->loadIdentity();
			MatrixManager->translate(0.0f, 0.0f, -10.0f);
			unhighlightedOptions->begin(SPRITE, VERTEX_T | COLOR_T | NORMAL_T | ST_T | ALPHA_T);
			{

				//Start New
				if (cursorIndex != 0)
				{
					//Set the models normal
					unhighlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(0.0f, 0.0f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(-4.0f, 4.0f, 0.1f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(1.0f, 0.125f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(4.0f, 3.0f, 0.1f);
				}
				if (cursorIndex != 1)
				{
					//Set the models normal
					unhighlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(0.0f, 0.25f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(-4.0f, 3.0f, 0.1f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(1.0f, 0.375f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(4.0f, 2.0f, 0.1f);
				}
				if (cursorIndex != 2)
				{
					//Set the models normal
					unhighlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(0.0f, 0.375f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(-4.0f, 2.0f, 0.1f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(1.0f, 0.5f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(4.0f, 1.0f, 0.1f);
				}



			}
			unhighlightedOptions->end();
			MatrixManager->pop();
			
			MatrixManager->push();
			MatrixManager->loadIdentity();
			MatrixManager->translate(0.0f, 0.0f, -10.0f);
			highlightedOptions->begin(SPRITE, VERTEX_T | COLOR_T | NORMAL_T | ST_T | ALPHA_T);
			{
				
				//Start New
				if (cursorIndex == 0)
				{
					//Set the models normal
					highlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(0.0f, 0.0f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(-4.0f, 4.0f, 0.0f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(1.0f, 0.125f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(4.0f, 3.0f, 0.0f);
				}
				else if (cursorIndex == 1)
				{
					//Set the models normal
					highlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(0.0f, 0.25f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(-4.0f, 3.0f, 0.0f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(1.0f, 0.375f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(4.0f, 2.0f, 0.0f);
				}
				else if (cursorIndex == 2)
				{
					//Set the models normal
					highlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(0.0f, 0.375f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(-4.0f, 2.0f, 0.0f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(1.0f, 0.5f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(4.0f, 1.0f, 0.0f);
				}

			}
			highlightedOptions->end();

			MatrixManager->pop();

			//Clear the screen with the default color
			sps2UScreenClear(0x00, 0x00, 0x00);
			//Build the list of things to transfer along
			//Dma channel 1 and 2
			DmaMemoryScheduler->buildList(CHANNEL1);
			//Start the transfer along channel 1
			DmaMemoryScheduler->transfer(CHANNEL1);
			//Wait for the transfer to finish
			DmaMemoryScheduler->waitForTransfer(CHANNEL1);

			DmaMemoryScheduler->buildList(CHANNEL2);
			//Start the transfer along channel 1
			DmaMemoryScheduler->transfer(CHANNEL2);
			//Wait for the transfer to finish
			DmaMemoryScheduler->waitForTransfer(CHANNEL2);

			sps2UScreenSwap();

		}
		break;

	case INGAMEMENU:
		{
			MatrixManager->setMode(MODELVIEW);
			
			MatrixManager->push();
			MatrixManager->loadIdentity();
			MatrixManager->translate(0.0f, 0.0f, -40.0f);
			backGroundImages->begin(SPRITE, VERTEX_T | COLOR_T | NORMAL_T | ST_T);
			{
				
				//Set the models normal
				backGroundImages->addNormal(0.0f, 0.0f, -1.0f);
				//Must add st first, color second then the vertex data
				//Set the texture coordinate.
				//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
				backGroundImages->addST(0.0f, 0.0f);
				//Set the color of the vertex. can set the alpha value as alpha is enabled
				backGroundImages->addColor(0.125f, 0.125f, 0.125f, 1.0f);
				//Finally add the vertex
				backGroundImages->addVertex(-16.0f, 16.0f, 0.0f);

				//Must add st first, color second then the vertex data
				//Set the texture coordinate.
				//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
				backGroundImages->addST(1.0f, 1.0f);
				//Set the color of the vertex. can set the alpha value as alpha is enabled
				backGroundImages->addColor(0.125f, 0.125f, 0.125f, 1.0f);
				//Finally add the vertex
				backGroundImages->addVertex(16.0f, -16.0f, 0.0f);

			}
			backGroundImages->end();
			MatrixManager->pop();
			
			//backGroundImages->getModelPacket()->setSend(0);
			//highlightedOptions->getModelPacket()->setSend(0);
//Custom_Model *unhighlightedOptions;
//Custom_Model *highlightedOptions;
			MatrixManager->push();
			MatrixManager->loadIdentity();
			MatrixManager->translate(0.0f, 0.0f, -10.0f);
			unhighlightedOptions->begin(SPRITE, VERTEX_T | COLOR_T | NORMAL_T | ST_T | ALPHA_T);
			{

				//Resume
				if (cursorIndex != 0)
				{
					//Set the models normal
					unhighlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(0.0f, 0.875f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(-4.0f, 4.0f, 0.1f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(1.0f, 1.0f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(4.0f, 3.0f, 0.1f);
				}
				//Stats
				if (cursorIndex != 1)
				{
					//Set the models normal
					unhighlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(0.0f, 0.5f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(-4.0f, 3.0f, 0.1f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(1.0f, 0.625f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(4.0f, 2.0f, 0.1f);
				}
				//Inventory
				if (cursorIndex != 2)
				{
					//Set the models normal
					unhighlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(0.0f, 0.625f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(-4.0f, 2.0f, 0.1f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(1.0f, 0.75f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(4.0f, 1.0f, 0.1f);
				}
				//Quest
				if (cursorIndex != 3)
				{
					//Set the models normal
					unhighlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(0.0f, 0.75f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(-4.0f, 1.0f, 0.1f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(1.0f, 0.875f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(4.0f, 0.0f, 0.1f);
				}
				//Save
				if (cursorIndex != 4)
				{
					//Set the models normal
					unhighlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(0.0f, 0.125f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(-4.0f, 0.0f, 0.1f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(1.0f, 0.25f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(4.0f, -1.0f, 0.1f);
				}
				//Exit
				if (cursorIndex != 5)
				{
					//Set the models normal
					unhighlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(0.0f, 0.375f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(-4.0f, -1.0f, 0.1f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					unhighlightedOptions->addST(1.0f, 0.5f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					unhighlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					unhighlightedOptions->addVertex(4.0f, -2.0f, 0.1f);
				}

			}
			unhighlightedOptions->end();
			MatrixManager->pop();
			
			MatrixManager->push();
			MatrixManager->loadIdentity();
			MatrixManager->translate(0.0f, 0.0f, -10.0f);
			highlightedOptions->begin(SPRITE, VERTEX_T | COLOR_T | NORMAL_T | ST_T | ALPHA_T);
			{
				
				//Resume
				if (cursorIndex == 0)
				{
					//Set the models normal
					highlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(0.0f, 0.875f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(-4.0f, 4.0f, 0.0f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(1.0f, 1.0f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(4.0f, 3.0f, 0.0f);
				}
				//Stats
				else if (cursorIndex == 1)
				{
					//Set the models normal
					highlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(0.0f, 0.5f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(-4.0f, 3.0f, 0.0f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(1.0f, 0.625f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(4.0f, 2.0f, 0.0f);
				}
				//Inventory
				else if (cursorIndex == 2)
				{
					//Set the models normal
					highlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(0.0f, 0.625f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(-4.0f, 2.0f, 0.0f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(1.0f, 0.75f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(4.0f, 1.0f, 0.0f);
				}
				//Quest
				else if (cursorIndex == 3)
				{
					//Set the models normal
					highlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(0.0f, 0.75f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(-4.0f, 1.0f, 0.0f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(1.0f, 0.875f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(4.0f, 0.0f, 0.0f);
				}
				//Save
				else if (cursorIndex == 4)
				{
					//Set the models normal
					highlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(0.0f, 0.125f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(-4.0f, 0.0f, 0.0f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(1.0f, 0.25f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(4.0f, -1.0f, 0.0f);
				}
				//Exit
				else if (cursorIndex == 5)
				{
					//Set the models normal
					highlightedOptions->addNormal(0.0f, 0.0f, -1.0f);
					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(0.0f, 0.375f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(-4.0f, -1.0f, 0.0f);

					//Must add st first, color second then the vertex data
					//Set the texture coordinate.
					//NOTE: (0.0f, 0.0f) is the top left and (1.0f, 1.0f) is the bottom right
					highlightedOptions->addST(1.0f, 0.5f);
					//Set the color of the vertex. can set the alpha value as alpha is enabled
					highlightedOptions->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					//Finally add the vertex
					highlightedOptions->addVertex(4.0f, -2.0f, 0.0f);
				}

			}
			highlightedOptions->end();

			MatrixManager->pop();

			//Clear the screen with the default color
			sps2UScreenClear(0x00, 0x00, 0x00);
			//Build the list of things to transfer along
			//Dma channel 1 and 2
			DmaMemoryScheduler->buildList(CHANNEL1);
			//Start the transfer along channel 1
			DmaMemoryScheduler->transfer(CHANNEL1);
			//Wait for the transfer to finish
			DmaMemoryScheduler->waitForTransfer(CHANNEL1);

			DmaMemoryScheduler->buildList(CHANNEL2);
			//Start the transfer along channel 1
			DmaMemoryScheduler->transfer(CHANNEL2);
			//Wait for the transfer to finish
			DmaMemoryScheduler->waitForTransfer(CHANNEL2);

			sps2UScreenSwap();

		}
		break;

	case GAME:
		{
			MatrixManager->setMode(MODELVIEW);
			//Setup camera
			MatrixManager->loadIdentity();
			MatrixManager->translate(0.0f, 0.0f, cameraDistance);
			MatrixManager->rotate(-worldAngle, 0.0f, 1.0f, 0.0f);
			MatrixManager->translate(-mainCharacterPosition[0], -mainCharacterPosition[1], -mainCharacterPosition[2]);


			MatrixManager->push();
			MatrixManager->translate(0.0f, -30.0f, 0.0f);
			if (LevelEngine->updateLevel(0) == 0)
			{
				//done = true;
				//return 0;
			}
			MatrixManager->pop();		

			//fprintf(testFile, "Start Drawing\n");
			//Clear the screen with the default color
			sps2UScreenClear(0x00, 0x00, 0x00);
			//Build the list of things to transfer along
			//Dma channel 1 and 2
			DmaMemoryScheduler->buildList(CHANNEL1);
			//Start the transfer along channel 1
			DmaMemoryScheduler->transfer(CHANNEL1);

			///Draw the NPC
			MatrixManager->push();
			MatrixManager->translate(npcPosition[0], npcPosition[1], npcPosition[2]);
			KeyframeEngine->runAnimation(1, 2);
			KeyframeEngine->flipSendPacket(1);
			MatrixManager->pop();


			//Draw the main character
			MatrixManager->push();
			MatrixManager->loadIdentity();
			MatrixManager->translate(0.0f, 0.0f, cameraDistance);
			MatrixManager->rotate(-PI / 2,0.0f, 1.0f, 0.0f);
			MatrixManager->rotate(mainCharacterAngle, 0.0f, 1.0f, 0.0f);
			KeyframeEngine->runAnimation(0, runAnimation);
			KeyframeEngine->flipSendPacket(0);
			MatrixManager->pop();

			//Wait for the transfer to finish
			DmaMemoryScheduler->waitForTransfer(CHANNEL1);
			//fprintf(testFile, "drawing finished\n");										
			//Flip the video buffers
			sps2UScreenSwap();

		}
		break;
	}
	//fclose(testFile);
	return 1;
}


int doInput()
{
	vectorf upVector = {0.0f, 0.0f, -1.0f, 1.0f};
	vectorf mainCharacterFaceAngle = {1.0f, 0.0f, 0.0f, 1.0f};
	
	if (InputEngine->readInput())
	{
		switch(currentGameState)
		{

		case GAME:
			{
				if(joypad->IsButtonPressed(PS2Pad::BUTTON_TRIANGLE))
				{
					prevGameState = currentGameState;
					currentGameState = CHANGETOINGAMEMENU;
				}

				
				if ((joypad->GetLeftX() != 0.0f) || (joypad->GetLeftY() != 0.0f))
				{
					runAnimation = 1;
					mainCharacterFaceAngle[0] = joypad->GetLeftX();
					mainCharacterFaceAngle[2] = -joypad->GetLeftY();
					

					if ((-joypad->GetLeftY() >= 0.0f) && (joypad->GetLeftX() >= 0.0f))
					{
						upVector[0] = 1.0f;
						upVector[1] = 0.0f;
						upVector[2] = 0.0f;
						upVector[3] = 1.0f;
						mainCharacterAngle = ps2_vu0_inner_product(upVector, mainCharacterFaceAngle);		
						mainCharacterAngle = PI - (mainCharacterAngle * PI * 0.5f);
					}
					else if (-joypad->GetLeftY() >= 0.0f && joypad->GetLeftX() <= 0.0f)
					{
						upVector[0] = 0.0f;
						upVector[1] = 0.0f;
						upVector[2] = 1.0f;
						upVector[3] = 1.0f;
						mainCharacterAngle = ps2_vu0_inner_product(upVector, mainCharacterFaceAngle);
						mainCharacterAngle = -PI + ((1.0f - mainCharacterAngle) * PI * 0.5f);
					}
					else if (-joypad->GetLeftY() <= 0.0f && joypad->GetLeftX() <= 0.0f)
					{
						upVector[0] = -1.0f;
						upVector[1] = 0.0f;
						upVector[2] = 0.0f;
						upVector[3] = 1.0f;
						mainCharacterAngle = ps2_vu0_inner_product(upVector, mainCharacterFaceAngle);
						mainCharacterAngle = -PI * 0.5f + ((1.0f - mainCharacterAngle) * PI * 0.5f);
					}
					else if (-joypad->GetLeftY() <= 0.0f && joypad->GetLeftX() >= 0.0f)
					{
						upVector[0] = 0.0f;
						upVector[1] = 0.0f;
						upVector[2] = -1.0f;
						upVector[3] = 1.0f;
						
						mainCharacterAngle = ps2_vu0_inner_product(upVector, mainCharacterFaceAngle);
						mainCharacterAngle = PI * 0.5f - (mainCharacterAngle * PI * 0.5f);
					}


					//printf("Main Character Angle = %f\n", mainCharacterAngle);

					float workAngle = worldAngle + mainCharacterAngle;
					if (workAngle > PI)workAngle -= 2.0f * PI;
					else if (workAngle < -PI)workAngle += 2.0f * PI;

					



					
					mainCharacterTestPosition[0] += sinf(workAngle) * playerSpeed;
					mainCharacterTestPosition[2] += cosf(workAngle) * playerSpeed;

					playerCollision->setPosition(mainCharacterTestPosition);

					if (!playerCollision->testRadiusSquared(npcCollision))
					{
						mainCharacterPosition[0] = mainCharacterTestPosition[0];
						mainCharacterPosition[2] = mainCharacterTestPosition[2];
					}
					else
					{
						playerCollision->setPosition(mainCharacterPosition);
						mainCharacterTestPosition[0] = mainCharacterPosition[0];
						mainCharacterTestPosition[2] = mainCharacterPosition[2];

					}
									
					

				}

				if ((joypad->GetRightX() != 0.0f) || (joypad->GetRightY() != 0.0f))
				{
					
					cameraDistance -= joypad->GetRightY();
					if (cameraDistance > -32.0f) cameraDistance = -32.0f;
					if (cameraDistance < -150.0f) cameraDistance = -150.0f;
					
					worldAngle -= joypad->GetRightX() * 0.1f;
					if (worldAngle > PI)worldAngle = -PI;
					else if (worldAngle < -PI)worldAngle = PI;
				}
			}
			break;
		case INGAMEMENU:
			{
				if (joypad->IsButtonPressed(PS2Pad::BUTTON_CROSS) && !joypad->IsButtonHeld(PS2Pad::BUTTON_CROSS))
				{
					if (cursorIndex == 0)
					{
						currentGameState = CHANGETOGAME;
						prevGameState = INGAMEMENU;
						cursorIndex = 0;
						
					}
					else if (cursorIndex == 5)
					{
						currentGameState = EXIT;
						prevGameState = INGAMEMENU;
						cursorIndex = 0;
					}
				}

				if (joypad->IsButtonPressed(PS2Pad::BUTTON_UP) && !joypad->IsButtonHeld(PS2Pad::BUTTON_UP))
				{
					cursorIndex--;
					if (cursorIndex < 0)cursorIndex = 0;
				}
				else if (joypad->IsButtonPressed(PS2Pad::BUTTON_DOWN) && !joypad->IsButtonHeld(PS2Pad::BUTTON_DOWN))
				{
					cursorIndex++;
					if (cursorIndex > INGAMEMENUMAX)cursorIndex = INGAMEMENUMAX;
				}
				
			}
			break;
		case MAINMENU:
			{
				if (joypad->IsButtonPressed(PS2Pad::BUTTON_CROSS) && !joypad->IsButtonHeld(PS2Pad::BUTTON_CROSS))
				{
					if (cursorIndex == 0)
					{
						currentGameState = CHANGETOGAME;
						prevGameState = MAINMENU;
						cursorIndex = 0;
						
					}
					else if (cursorIndex == 2)
					{
						currentGameState = EXIT;
						prevGameState = MAINMENU;
						cursorIndex = 0;
					}
				}

				if (joypad->IsButtonPressed(PS2Pad::BUTTON_UP) && !joypad->IsButtonHeld(PS2Pad::BUTTON_UP))
				{
					cursorIndex--;
					if (cursorIndex < 0)cursorIndex = 0;
				}
				else if (joypad->IsButtonPressed(PS2Pad::BUTTON_DOWN) && !joypad->IsButtonHeld(PS2Pad::BUTTON_DOWN))
				{
					cursorIndex++;
					if (cursorIndex > MAINMENUMAX)cursorIndex = MAINMENUMAX;
				}
				
			}
			break;

		}
		
	}

	return 0;

}


int setupGame()
{

	printf("\nPS2 engine - test program\n");

	time(&prevTime);
	time(&currentTime);

	mainCharacterPosition[0] = 0.0f;
	mainCharacterPosition[1] = 0.0f;
	mainCharacterPosition[2] = 0.0f;
	mainCharacterPosition[3] = 1.0f;

	npcPosition[0] = 64.0f;
	npcPosition[1] = 0.0f;
	npcPosition[2] = -100.0f;
	npcPosition[3] = 1.0f;

	mainCharacterTestPosition[0] = 0.0f;
	mainCharacterTestPosition[1] = 0.0f;
	mainCharacterTestPosition[2] = 0.0f;
	mainCharacterTestPosition[3] = 1.0f;

	
	printf("Create Input Engine: ");
	InputEngine = new Input_Engine();
	joypad = InputEngine->getInputDevice();
	printf("done\n");

	printf("Create Game Clock Engine: ");
	GameClockEngine = new Game_Clock_Engine();
	printf("done\n");

	printf("Create Memory Engine: ");
	//Allocate 8 megs for dma memory, IE textures and models ( 2072 * 4K)
	DmaMemoryEngine = new Dma_Memory_Engine(2072);
	//Setup the dma scheduler, takes care of the complicated transfers
	DmaMemoryScheduler = new Dma_Memory_Scheduler(DmaMemoryEngine);
	printf("done\n");


	//Setup the video
	sps2UScreenInit(0);	

	printf("Create VU Manager: ");
	//Setup the vu manager usd to upload programs and gain accesss the
	//uu's.
	VUManager = new VU_Manager(DmaMemoryEngine, VU0_ENABLED | VU1_ENABLED);
	printf("done\n");


	printf("Create Texture Engine: ");
	TextureEngine = new Texture_Engine(DmaMemoryEngine);
	printf("done\n");


	printf("Create Matrix Manager: ");
	MatrixManager = new Matrix_Manager();
	printf("done\n");

	printf("Create Keyframe Engine: ");
	KeyframeEngine = new Keyframe_Model_Engine(MatrixManager, DmaMemoryEngine, TextureEngine);
	KeyframeEngine->addModel("data/models/tris.md2");
	KeyframeEngine->addModel("data/models/Lamp_post.md2");
	//KeyframeEngine->addModel("data/models/test.md2");
	printf("done\n");

	printf("Create Level Engine: ");
	LevelEngine = new Level_Engine(MatrixManager, DmaMemoryEngine, TextureEngine);
	printf("done\n");

	printf("Upload VU1 micro program: ");
	VUManager->vuUpload(VU1,"libexec/vu1/trans.vu");
	printf("done\n");

	printf("Load Texture: ");
	TextureEngine->loadTexture("data/textures/logo.bmp", 0);
	TextureEngine->loadTexture("data/textures/screen02.cif", 1);
	TextureEngine->loadTexture("data/textures/menuunselected.cif", 3);
	TextureEngine->loadTexture("data/textures/menuselected.cif", 4);
	TextureEngine->loadTexture("data/textures/tris.bmp", 0);
	TextureEngine->loadTexture("data/textures/texture03.bmp", 1);
	printf("done\n");


	printf("Create Menu Models: ");
	backGroundImages = new Custom_Model(MatrixManager, CHANNEL2, DmaMemoryEngine, TextureEngine);
	unhighlightedOptions = new Custom_Model(MatrixManager, CHANNEL2, DmaMemoryEngine, TextureEngine);
	highlightedOptions = new Custom_Model(MatrixManager, CHANNEL2, DmaMemoryEngine, TextureEngine);
	backGroundImages->setTextureID(0);
	unhighlightedOptions->setTextureID(2);
	highlightedOptions->setTextureID(3);
	printf("done\n");	


	//Setup the view port matrix
	MatrixManager->setMode(VIEW);
	MatrixManager->setView((float)sps2UScreenGetWidth(), (float)sps2UScreenGetHeight(), 2048.0f, 2048.0f, (float)(1 << 24));
	MatrixManager->dump("View.txt");
	
	//Setup the projection matrix
	MatrixManager->setMode(PROJECTION);
	MatrixManager->setProjection(60.0f, 4.0f / 3.0f, -20.0f, -2000.0f);
	MatrixManager->dump("Projection.txt");

	MatrixManager->setProjectionScale(2048.0f, 2048.0f, ((float)0xFFFFFF) / 32.0f);
	

	//Setup the camera(ie the world matrix)
	//vectorf cameraEye = {0.0f, 0.0f, 10.0f, 1.0f };
	//vectorf cameraLookAt = {0.0f, 0.0f, -10.0f, 1.0f};
	//vectorf cameraUp = { 0.0f, 1.0f, 0.0f, 1.0f};
	MatrixManager->setMode(CAMERA);
	MatrixManager->loadIdentity();
	//MatrixManager->setCamera(cameraEye, cameraLookAt, cameraUp);
	MatrixManager->dump("Camera.txt");

	//Setup color matrix(test)
	vectorf color0 = {0.4f, 0.4f, 0.4f, 1.0f};
	vectorf color1 = {0.4f, 0.4f, 0.4f, 1.0f};  
	vectorf color2 = {0.4f, 0.4f, 0.4f, 1.0f};
	vectorf ambient = {0.2f, 0.2f, 0.2f, 0.0f};
	MatrixManager->setMode(COLOR);
	MatrixManager->setColor(color0, color1, color2, ambient);
	MatrixManager->dump("Color.txt");

	//Setup light matrix(test)
	vectorf lightd0 = {1.0f, 0.0f, 0.3f, 0.0f};
	vectorf lightd1 = {0.0f, 1.0f, 0.3f, 0.0f};
	vectorf lightd2 = {0.0f, 0.0f, 1.0f, 0.0f};
	
	MatrixManager->setMode(LIGHT);
	MatrixManager->setLight(lightd0, lightd1, lightd2);
	MatrixManager->dump("Light.txt");


	playerCollision = new Collision_Detection(625, NULL, mainCharacterPosition);
	npcCollision = new Collision_Detection(625, NULL, npcPosition);


	KeyframeEngine->buildCharacter(0,0);
	KeyframeEngine->buildCharacter(0,0);
	LevelEngine->load("data/maps/Level01.bsp");

	return 1;
}

int shutDownGame()
{

	//Shut down the joypad library
	DmaMemoryScheduler->dump("DmaSche.txt");

	printf("Drawing Finished\n");
	//Shutdown the video
	sps2UScreenShutdown();


	printf("Delete Input Engine: ");
	delete InputEngine;
	printf("done\n");

	printf("Delete Game Clock Engine: ");
	delete GameClockEngine;
	printf("done\n");

	printf("Delete Menu Models: ");
	delete backGroundImages;
	delete unhighlightedOptions;
	delete highlightedOptions;
	printf("done\n");


	printf("Delete Keyframe Engine: ");
	//KeyframeEngine->getSecondPacket(0)->dump("KeyframeModel01.txt");
	delete KeyframeEngine;
	printf("done\n");

	printf("Delete Level Engine: ");
	LevelEngine->getPacket(0)->dump("TestMap.txt"); 
	delete LevelEngine;
	printf("done\n");
	
	printf("Deleting VU Manager: ");
	delete VUManager;
	printf("done\n");

	printf("Deleting Texture Engine: ");
	TextureEngine->getPacket(0)->dump("Texture01.txt");
	TextureEngine->getPacket(1)->dump("Texture02.txt");
	delete TextureEngine;
	printf("done\n");

	printf("Deleting Matrix Manager: ");
	delete MatrixManager;
	printf("done\n");

	printf("Delete Collision Detection: ");
	delete playerCollision;
	delete npcCollision;
	printf("done\n");

	printf("Deleting Memory Engine: ");
	DmaMemoryScheduler->dump("Scheduler.txt");
	delete DmaMemoryScheduler;

	//Must delete the memory manager last.
	delete DmaMemoryEngine;
	printf("done\n");

	return 1;
}