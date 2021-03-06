/*
 * cisynth_eth.c
 *
 *  Created on: May 31, 2020
 *      Author: zhonx
 */

#include "cisynth_ifft.h"
#include "synth.h"
#include "times_base.h"
#include "cis.h"
#include "wave_generation.h"
#include "wave_sommation.h"
#include "config.h"
#include "stdio.h"
#include "arm_math.h"
#include "menu.h"
#include "ssd1362.h"
#include "stdbool.h"
#include "pcm5102.h"

extern __IO uint32_t synth_process_cnt;
static void cisynth_ifft_SetHint(void);
//static int16_t *cisBuff = NULL;

/**
 * @brief  The application entry point.
 * @retval int
 */
int cisynth_ifft(void)
{
	uint8_t FreqStr[256] = {0};
	uint32_t cis_color = 0;

	printf("------ BW IFFT MODE START -----\n");
	printf("-------------------------------\n");

	ssd1362_clearBuffer();

	synth_IfftInit();

	cisynth_ifft_SetHint();

	//	cis_Test();

	/* Infinite loop */
	static uint32_t start_tick;
	uint32_t latency;
	int32_t i = 0;
	int32_t j = 0;

	while (1)
	{
		start_tick = HAL_GetTick();
		while ((synth_process_cnt) < (SAMPLING_FREQUENCY / DISPLAY_REFRESH_FPS))
		{
			synth_AudioProcess(IFFT_MODE);
		}

		latency = HAL_GetTick() - start_tick;
		sprintf((char *)FreqStr, "%dHz", (int)((synth_process_cnt * 1000) / latency));
		synth_process_cnt = 0;

		ssd1362_drawRect(0, DISPLAY_AERA1_Y1POS, DISPLAY_MAX_X_LENGTH / 2 - 1, DISPLAY_AERA1_Y2POS, 3, false);
		ssd1362_drawRect(DISPLAY_MAX_X_LENGTH / 2 + 1, DISPLAY_AERA1_Y1POS, DISPLAY_MAX_X_LENGTH, DISPLAY_AERA1_Y2POS, 4, false);
		ssd1362_drawRect(0, DISPLAY_AERA2_Y1POS, DISPLAY_MAX_X_LENGTH, DISPLAY_AERA2_Y2POS, 3, false);
		ssd1362_drawRect(0, DISPLAY_AERA3_Y1POS, DISPLAY_MAX_X_LENGTH, DISPLAY_AERA3_Y2POS, 8, false);

		for (i = 0; i < ((DISPLAY_MAX_X_LENGTH / 2) - 1); i++)
		{
			ssd1362_drawPixel(i, DISPLAY_AERA1_Y1POS + (DISPLAY_AERAS1_HEIGHT / 2) + (pcm5102_GetAudioData(i * 2) / 2048) - 1, 10, false);
			ssd1362_drawPixel(i + (DISPLAY_MAX_X_LENGTH / 2) + 1, DISPLAY_AERA1_Y1POS + (DISPLAY_AERAS1_HEIGHT / 2) + (pcm5102_GetAudioData(i * 2 + 1) / 2048) - 1, 10, false);
		}

		for (i = 0; i < (DISPLAY_MAX_X_LENGTH); i++)
		{
			//			cis_color = cis_GetBuffData((i * ((float)cis_GetEffectivePixelNb() / (float)DISPLAY_MAX_X_LENGTH))) >> 12;
			cis_color = synth_GetImageData((i * ((float)cis_GetEffectivePixelNb() / (float)DISPLAY_MAX_X_LENGTH))) >> 12;
			ssd1362_drawPixel(DISPLAY_MAX_X_LENGTH - 1 - i, DISPLAY_AERA2_Y1POS + DISPLAY_AERAS2_HEIGHT - DISPLAY_INTER_AERAS_HEIGHT - (cis_color) - 1, 15, false);

			ssd1362_drawVLine(DISPLAY_MAX_X_LENGTH - 1 - i, DISPLAY_AERA3_Y1POS + 1, DISPLAY_AERAS3_HEIGHT - 2, cis_color, false);
		}
		ssd1362_drawRect(200, DISPLAY_HEAD_Y1POS, DISPLAY_MAX_X_LENGTH, DISPLAY_HEAD_Y2POS, 4, false);
		ssd1362_drawString(200, 1, (int8_t*)FreqStr, 15, 8);
		ssd1362_writeFullBuffer();

		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		//		j++;
		//		if (j%2)
		//			cis_LedsOff();
		//		else
		//			cis_LedsOn();
	}
}
/**
 * @brief  Display Audio demo hint
 * @param  None
 * @retval None
 */
static void cisynth_ifft_SetHint(void)
{
	/* Set Audio header description */
	ssd1362_drawRect(0, DISPLAY_HEAD_Y1POS, DISPLAY_MAX_X_LENGTH, DISPLAY_HEAD_Y2POS, 4, false);
	ssd1362_drawString(100, 1, (int8_t *)"CISYNTH 3", 0xF, 8);
	ssd1362_drawString(0, 1, (int8_t *)"BW ifft", 0xF, 8);
	ssd1362_writeFullBuffer();
}
