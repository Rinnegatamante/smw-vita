#include "global.h"
#include "input.h"
#include <vitasdk.h>

CPlayerInput::CPlayerInput()
{
	for(int iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		for(int iKey = 0; iKey < NUM_KEYS; iKey++)
		{
			outputControls[iPlayer].keys[iKey].fPressed = false;
			outputControls[iPlayer].keys[iKey].fDown = false;
		}
	}

	iPressedKey = 0;

	fUsingMouse = false;
}

void CPlayerInput::CheckIfMouseUsed()
{
	fUsingMouse = false;
}

//Pass in 0 for game and 1 for menu
//Clear old button pushed states
void CPlayerInput::ClearPressedKeys(short iGameState)
{
	for(int iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		CInputControl * inputControl = &inputControls[iPlayer]->inputGameControls[iGameState];
		COutputControl * outputControl = &outputControls[iPlayer];

		for(int iKey = 0; iKey < NUM_KEYS; iKey++)
		{
			outputControl->keys[iKey].fPressed = false;
		}
	}

	iPressedKey = 0;
}

//Clear all button pushed and down states
//Call this when switching from menu to game
void CPlayerInput::ResetKeys()
{
	for(int iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		for(int iKey = 0; iKey < NUM_KEYS; iKey++)
		{
			outputControls[iPlayer].keys[iKey].fPressed = false;
			outputControls[iPlayer].keys[iKey].fDown = false;
		}
	}

	iPressedKey = 0;
}

//Called during game loop to read input events and see if 
//configured keys were pressed.  If they were, then turn on 
//key flags to be used by game logic
//iGameState == 0 for in game and 1 for menu

void setState(bool state, COutputControl *outputControl, int iKey) {
	if (state) {
		if(!outputControl->keys[iKey].fDown)
			outputControl->keys[iKey].fPressed = true;
			
		outputControl->keys[iKey].fDown = true;
	}else outputControl->keys[iKey].fDown = false;
}

void CPlayerInput::Update(SDL_Event event, short iGameState)
{
	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		CInputControl * inputControl;
		COutputControl * outputControl;
		short iPlayerID = iPlayer;
		short iDeviceID = DEVICE_KEYBOARD;
		
		SceCtrlData pad;
		SceCtrlPortInfo info;
		sceCtrlGetControllerPortInfo(&info);
		if (info.port[iPlayer > 0 ? (iPlayer + 1) : iPlayer] == SCE_CTRL_TYPE_UNPAIRED) continue;
		sceCtrlPeekBufferPositive(iPlayer > 0 ? (iPlayer + 1) : iPlayer, &pad, 1);
		
		outputControl = &outputControls[iPlayer];
		iDeviceID = inputControls[iPlayer]->iDevice;
		
		//Ignore input for cpu controlled players
		if(iGameState == 0 && game_values.playercontrol[iPlayer] != 1)
			continue;
		
		if (iGameState == 0) { // Game
			setState((pad.buttons & SCE_CTRL_LEFT) || (pad.lx < 80), outputControl, 0);
			setState((pad.buttons & SCE_CTRL_RIGHT) || (pad.lx > 180), outputControl, 1);
			setState((pad.buttons & SCE_CTRL_UP) || (pad.ly < 80) || (pad.buttons & SCE_CTRL_CROSS), outputControl, 2);
			setState((pad.buttons & SCE_CTRL_DOWN) || (pad.ly > 180), outputControl, 3);
			setState(pad.buttons & SCE_CTRL_SQUARE, outputControl, 4);
			setState(pad.buttons & SCE_CTRL_SELECT, outputControl, 6);
			setState(pad.buttons & SCE_CTRL_START, outputControl, 7);
			setState(pad.buttons & SCE_CTRL_LTRIGGER, outputControl, 8);
			setState(pad.buttons & SCE_CTRL_RTRIGGER, outputControl, 9);
			setState(pad.buttons & SCE_CTRL_TRIANGLE, outputControl, 10);
			setState(pad.buttons & SCE_CTRL_CIRCLE, outputControl, 11);
		} else {
			setState((pad.buttons & SCE_CTRL_UP) || (pad.ly < 80), outputControl, 0);
			setState((pad.buttons & SCE_CTRL_DOWN) || (pad.ly > 180), outputControl, 1);
			setState((pad.buttons & SCE_CTRL_LEFT) || (pad.lx < 80), outputControl, 2);
			setState((pad.buttons & SCE_CTRL_RIGHT) || (pad.lx > 180), outputControl, 3);
			setState(pad.buttons & SCE_CTRL_CROSS, outputControl, 4);
			setState(pad.buttons & SCE_CTRL_CIRCLE, outputControl, 5);
			setState(pad.buttons & SCE_CTRL_SQUARE, outputControl, 6);
			setState(pad.buttons & SCE_CTRL_TRIANGLE, outputControl, 7);
			setState(pad.buttons & SCE_CTRL_LTRIGGER, outputControl, 8);
			setState(pad.buttons & SCE_CTRL_RTRIGGER, outputControl, 9);
			setState(pad.buttons & SCE_CTRL_START, outputControl, 10);
			setState(pad.buttons & SCE_CTRL_SELECT, outputControl, 11);
		}

		//This line might be causing input from some players not to be read
		//if(fFound)
			//break;
	}
}

