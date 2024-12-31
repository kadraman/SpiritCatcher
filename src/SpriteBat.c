#include "Banks/SetAutoBank.h"
#include "main.h"

#include <gb/gb.h>

#include "Scroll.h"
#include "SpriteManager.h"

const UINT8 anim_bat_move[] = {3, 0, 1, 2};
const UINT8 anim_bat_attack[] = {3, 3, 4, 5};
const INT8 batDist = 64u;
struct EnemyInfo
{
    UINT8 wait;
    BYTE goingUp;
    INT8 dist;
};

void START() {
	struct EnemyInfo* data = (struct EnemyInfo*)THIS->custom_data;
	data->goingUp = 1;
    data->dist = batDist;
	SetSpriteAnim(THIS, anim_bat_move, 5u);
	THIS->lim_x = 160u;
	THIS->lim_y = 160u;
}

void UPDATE() {
	struct EnemyInfo* data = (struct EnemyInfo*)THIS->custom_data;
	if (data->wait == 2)
    {
		data->wait = 0;
		data->dist--;

		if (data->dist == 0)
        {
            data->goingUp = !data->goingUp;
            data->dist = batDist;
        }
		if (data->goingUp)
        {
            //SetSpriteAnim(THIS, anim_bat_up, 10u);
            if (TranslateSprite(THIS,0,-1))
            {
                data->goingUp = !data->goingUp;
                data->dist = batDist;
            }
        }
        else
        {
            //SetSpriteAnim(THIS, anim_bat_down, 15u);
            if (TranslateSprite(THIS,0,1))
            {
                data->goingUp = !data->goingUp;
                data->dist = batDist;
            }
        }
    } 
	data->wait++;
}

void DESTROY() {
}