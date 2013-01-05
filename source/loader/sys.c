#include <stdio.h>
#include <ogcsys.h>
#include <stdlib.h>

#include "sys.h"
#include "gecko.h"

/* Constants */
#define CERTS_LEN	0x280

/* Variables */
static const char certs_fs[] ATTRIBUTE_ALIGN(32) = "/sys/cert.sys";
static bool reset = false;
static bool shutdown = false;
static bool return_to_menu = false;

bool Sys_Exiting(void)
{
	return reset || shutdown;
}

void Sys_Test(void)
{
	if (reset)
		Sys_Reboot();
	else if (shutdown)
		Sys_Shutdown();
}

void Sys_ExitToWiiMenu(bool b)
{
	return_to_menu = b;
}

void Sys_Exit(int ret)
{
	if (return_to_menu)
		Sys_LoadMenu();
	else
		exit(ret);
}

void __Sys_ResetCallback(void)
{
	reset = true;
	/* Reboot console */
//	Sys_Reboot();
}

void __Sys_PowerCallback(void)
{
	shutdown = true;
	/* Poweroff console */
//	Sys_Shutdown();
}


void Sys_Init(void)
{
	/* Initialize video subsytem */
//	VIDEO_Init();

	/* Set RESET/POWER button callback */
	SYS_SetResetCallback(__Sys_ResetCallback);
	SYS_SetPowerCallback(__Sys_PowerCallback);
}

void Sys_Reboot(void)
{
	/* Restart console */
	STM_RebootSystem();
}

void Sys_Shutdown(void)
{
	/* Poweroff console */
	if(CONF_GetShutdownMode() == CONF_SHUTDOWN_IDLE) {
		s32 ret;

		/* Set LED mode */
		ret = CONF_GetIdleLedMode();
		if(ret >= 0 && ret <= 2)
			STM_SetLedMode(ret);

		/* Shutdown to idle */
		STM_ShutdownToIdle();
	} else {
		/* Shutdown to standby */
		STM_ShutdownToStandby();
	}
}

void Sys_LoadMenu(void)
{
	/* Return to the Wii system menu */
	SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
}

bool ModeChecked = false;
bool DolphinMode = false;
bool Sys_DolphinMode(void)
{
	if(ModeChecked)
		return DolphinMode;

	/* Thanks to skidau for that code! */
	u32 ifpr11 = 0x12345678;
	u32 ifpr12 = 0x9abcdef0;
	u32 ofpr1 = 0x00000000;
	u32 ofpr2 = 0x00000000;
	asm volatile (
		"lwz 3,%[ifpr11]\n\t"
		"stw 3,8(1)\n\t"
		"lwz 3,%[ifpr12]\n\t"
		"stw 3,12(1)\n\t"

		"lfd 1,8(1)\n\t"
		"frsqrte	1, 1\n\t"
		"stfd 	1,8(1)\n\t"

		"lwz 	3,8(1)\n\t" 
		"stw	3, %[ofpr1]\n\t"
		"lwz 	3,12(1)\n\t" 
		"stw	3, %[ofpr2]\n\t"

		:
		 [ofpr1]"=m" (ofpr1)
		,[ofpr2]"=m" (ofpr2)
		:
		 [ifpr11]"m" (ifpr11)
		,[ifpr12]"m" (ifpr12)

	);
	if(ofpr1 != 0x56cc62b2)
	{
		gprintf("Dolphin-Emu\n");
		DolphinMode = true;
	}
	else
	{
		gprintf("Real Wii\n");
		DolphinMode = false;
	}
	ModeChecked = true;
	return DolphinMode;
}