// 
// "main.h"
// 
// 
#ifndef _MAIN_H_
#define _MAIN_H_






void MoveSprite(PS2Sprite_t * pSP, float Delta, int Axis);
void AnimateSprite(PS2Sprite_t * pSP, float Count);
void GetUserInput(void);
void DrawFrame(char * pMemStart, PS2Sprite_t * pSP, DmacControl_t * pD);
void GameLogic(PS2Sprite_t * pSP);



#endif // _MAIN_H_
