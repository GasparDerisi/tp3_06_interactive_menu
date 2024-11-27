/*
 * Copyright (c) 2023 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * @file   : task_menu.c
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/
/* Project includes. */
#include "main.h"
#include "string.h"

/* Demo includes. */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes. */
#include "board.h"
#include "app.h"
#include "task_menu_attribute.h"
#include "task_menu_interface.h"
#include "display.h"

/********************** macros and definitions *******************************/
#define G_TASK_MEN_CNT_INI			0ul
#define G_TASK_MEN_TICK_CNT_INI		0ul

#define DEL_MEN_XX_MIN				0ul
#define DEL_MEN_XX_MED				50ul
#define DEL_MEN_XX_MAX				500ul

/********************** internal data declaration ****************************/

task_menu_dta_t task_menu_dta =
	{DEL_MEN_XX_MIN, ST_MAIN_MENU, EV_BTN_ESCAPE_ACTIVE, 0, false};


motor_t motor_config[] = {
		{"Motor1: ", "OFF", "0", "L"},
		{"Motor2: ", "OFF", "0", "L"}
};
uint32_t motor_actual;


const char* str_menu_1[] = {
    "Motor1          ",
    "Motor2          "
};

uint32_t n_motores = 2;

const char* str_menu_2[] = {
    "Power           ",
    "Speed           ",
	"Spin            "
};

uint32_t n_param = 3;

const char* str_menu_3_power[] = {
    "OFF             ",
    "ON              "
};

uint32_t n_power = 2;

const char* str_menu_3_spin[] = {
    "L",
    "R"
};

uint32_t n_spin = 2;
uint32_t n_speed = 10;

char str_linea_1[16] = "Menu principal";
char str_linea_2[16] = "Pulse 'menu'";




char str_linea_1_aux[16];
char str_linea_2_aux[16];



#define MENU_DTA_QTY	(sizeof(task_menu_dta)/sizeof(task_menu_dta_t))

/********************** internal functions declaration ***********************/

void clear_display(uint8_t n_linea){
	displayCharPositionWrite(0, n_linea);
	displayStringWrite("                ");
}

/********************** internal data definition *****************************/
const char *p_task_menu 		= "Task Menu (Interactive Menu)";
const char *p_task_menu_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_menu_cnt;
volatile uint32_t g_task_menu_tick_cnt;

/********************** external functions definition ************************/

void task_menu_init(void *parameters)
{
	task_menu_dta_t *p_task_menu_dta;
	task_menu_st_t	state;
	task_menu_ev_t	event;
	bool b_event;

	/* Print out: Task Initialized */
	LOGGER_LOG("  %s is running - %s\r\n", GET_NAME(task_menu_init), p_task_menu);
	LOGGER_LOG("  %s is a %s\r\n", GET_NAME(task_menu), p_task_menu_);

	g_task_menu_cnt = G_TASK_MEN_CNT_INI;

	/* Print out: Task execution counter */
	LOGGER_LOG("   %s = %lu\r\n", GET_NAME(g_task_menu_cnt), g_task_menu_cnt);

	init_queue_event_task_menu();

	/* Update Task Actuator Configuration & Data Pointer */
	p_task_menu_dta = &task_menu_dta;

	/* Print out: Task execution FSM */
	state = p_task_menu_dta->state;
	LOGGER_LOG("   %s = %lu", GET_NAME(state), (uint32_t)state);

	event = p_task_menu_dta->event;
	LOGGER_LOG("   %s = %lu", GET_NAME(event), (uint32_t)event);

	b_event = p_task_menu_dta->flag;
	LOGGER_LOG("   %s = %s\r\n", GET_NAME(b_event), (b_event ? "true" : "false"));

    displayInit(DISPLAY_CONNECTION_GPIO_4BITS);

    displayCharPositionWrite(0, 0);
	displayStringWrite(str_linea_1);

	displayCharPositionWrite(0, 1);
	displayStringWrite(str_linea_2);

	HAL_GPIO_WritePin(LED_A_PORT, LED_A_PIN, LED_A_ON);

	g_task_menu_tick_cnt = G_TASK_MEN_TICK_CNT_INI;
}


void task_menu_update(void *parameters)
{
	task_menu_dta_t *p_task_menu_dta;
	bool b_time_update_required = false;

	/* Update Task Menu Counter */
	g_task_menu_cnt++;

	/* Protect shared resource (g_task_menu_tick) */
	__asm("CPSID i");	/* disable interrupts*/
    if (G_TASK_MEN_TICK_CNT_INI < g_task_menu_tick_cnt)
    {
    	g_task_menu_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts*/

    while (b_time_update_required)
    {
		/* Protect shared resource (g_task_menu_tick) */
		__asm("CPSID i");	/* disable interrupts*/
		if (G_TASK_MEN_TICK_CNT_INI < g_task_menu_tick_cnt)
		{
			g_task_menu_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts*/

    	/* Update Task Menu Data Pointer */
		p_task_menu_dta = &task_menu_dta;

    	if (DEL_MEN_XX_MIN < p_task_menu_dta->tick)
		{
			p_task_menu_dta->tick--;
		}
		else
		{
			HAL_GPIO_TogglePin(LED_A_PORT, LED_A_PIN);

			p_task_menu_dta->tick = DEL_MEN_XX_MAX;

			if (true == any_event_task_menu())
			{
				p_task_menu_dta->flag = true;
				p_task_menu_dta->event = get_event_task_menu();
			}


			switch (p_task_menu_dta->state)
			{
				case ST_MAIN_MENU:

					strcpy(str_linea_1, motor_config[0].id);
					strcat(str_linea_1, motor_config[0].power);
					strcat(str_linea_1, ",");
					strcat(str_linea_1, motor_config[0].speed);
					strcat(str_linea_1, ",");
					strcat(str_linea_1, motor_config[0].spin);

					strcpy(str_linea_2, motor_config[1].id);
					strcat(str_linea_2, motor_config[1].power);
					strcat(str_linea_2, ",");
					strcat(str_linea_2, motor_config[1].speed);
					strcat(str_linea_2, ",");
					strcat(str_linea_2, motor_config[1].spin);

					if ((p_task_menu_dta->flag == true) && (p_task_menu_dta->event == EV_BTN_MENU_ACTIVE))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MENU_1;
					}

					break;


				case ST_MENU_1:
					strcpy (str_linea_1, "Enter/Next/Esc");
					strcpy(str_linea_2, str_menu_1[p_task_menu_dta->internal_state]);
					if ((p_task_menu_dta->flag == true) && (p_task_menu_dta->event == EV_BTN_ESCAPE_ACTIVE))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MAIN_MENU;
					}

					if ((p_task_menu_dta->flag == true) && (p_task_menu_dta->event == EV_BTN_NEXT_ACTIVE))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->internal_state = (p_task_menu_dta->internal_state + 1) % n_motores;
						p_task_menu_dta->state = ST_MENU_1;
						strcpy(str_linea_2, str_menu_1[p_task_menu_dta->internal_state]);
					}

					if ((p_task_menu_dta->flag == true) && (p_task_menu_dta->event == EV_BTN_ENTER_ACTIVE))
					{

						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MENU_2;
						motor_actual = p_task_menu_dta->internal_state;
						p_task_menu_dta->internal_state = 0;
					}

					break;


				case ST_MENU_2:
					strcpy (str_linea_1, "Enter/Next/Esc  ");
					strcpy(str_linea_2, str_menu_2[p_task_menu_dta->internal_state]);
					if ((p_task_menu_dta->flag == true) && (p_task_menu_dta->event == EV_BTN_ESCAPE_ACTIVE))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MENU_1;
					}

					if ((p_task_menu_dta->flag == true) && (p_task_menu_dta->event == EV_BTN_NEXT_ACTIVE))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->internal_state = (p_task_menu_dta->internal_state + 1) % n_param;
						p_task_menu_dta->state = ST_MENU_2;
						strcpy(str_linea_2, str_menu_2[p_task_menu_dta->internal_state]);
					}

					if ((p_task_menu_dta->flag == true) && (p_task_menu_dta->event == EV_BTN_ENTER_ACTIVE))
					{
						p_task_menu_dta->flag = false;
						switch (p_task_menu_dta->internal_state){
							case (0):
									p_task_menu_dta->state = ST_MENU_3_POWER;
							break;

							case (1):
									p_task_menu_dta->state = ST_MENU_3_SPEED;
							break;

							case (2):
									p_task_menu_dta->state = ST_MENU_3_SPIN;
							break;
						}
						p_task_menu_dta->internal_state = 0;
					}
					break;


				case ST_MENU_3_POWER:
					strcpy (str_linea_1, "Enter/Next/Esc  ");
					strcpy(str_linea_2, str_menu_3_power[p_task_menu_dta->internal_state]);
					if ((p_task_menu_dta->flag == true) && (p_task_menu_dta->event == EV_BTN_ESCAPE_ACTIVE))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MENU_2;
					}

					if ((p_task_menu_dta->flag == true) && (p_task_menu_dta->event == EV_BTN_NEXT_ACTIVE))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->internal_state = (p_task_menu_dta->internal_state + 1) % n_power;
						p_task_menu_dta->state = ST_MENU_3_POWER;
						strcpy(str_linea_2, str_menu_3_power[p_task_menu_dta->internal_state]);
					}

					if ((p_task_menu_dta->flag == true) && (p_task_menu_dta->event == EV_BTN_ENTER_ACTIVE))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MAIN_MENU;
						strncpy(motor_config[motor_actual].power, str_menu_3_power[p_task_menu_dta->internal_state], 3);
						p_task_menu_dta->internal_state = 0;

					}

					break;


				case ST_MENU_3_SPEED:
					strcpy (str_linea_1, "Enter/Next/Esc  ");
					sprintf(str_linea_2, "%lu", p_task_menu_dta->internal_state);
					if ((p_task_menu_dta->flag == true) && (p_task_menu_dta->event == EV_BTN_ESCAPE_ACTIVE))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MENU_2;
					}

					if ((p_task_menu_dta->flag == true) && (p_task_menu_dta->event == EV_BTN_NEXT_ACTIVE))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->internal_state = (p_task_menu_dta->internal_state + 1) % n_speed;
						p_task_menu_dta->state = ST_MENU_3_SPEED;
						sprintf(str_linea_2, "%lu", p_task_menu_dta->internal_state);
					}

					if ((p_task_menu_dta->flag == true) && (p_task_menu_dta->event == EV_BTN_ENTER_ACTIVE))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MAIN_MENU;
						sprintf(motor_config[motor_actual].speed, "%lu",p_task_menu_dta->internal_state);
						p_task_menu_dta->internal_state = 0;
					}
					break;


				case ST_MENU_3_SPIN:
					strcpy (str_linea_1, "Enter/Next/Esc  ");
					strcpy(str_linea_2, str_menu_3_spin[p_task_menu_dta->internal_state]);
					if ((p_task_menu_dta->flag == true) && (p_task_menu_dta->event == EV_BTN_ESCAPE_ACTIVE))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MENU_2;
					}

					if ((p_task_menu_dta->flag == true) && (p_task_menu_dta->event == EV_BTN_NEXT_ACTIVE))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->internal_state = (p_task_menu_dta->internal_state + 1) % n_spin;
						p_task_menu_dta->state = ST_MENU_3_SPIN;
						strcpy(str_linea_2, str_menu_3_spin[p_task_menu_dta->internal_state]);
					}

					if ((p_task_menu_dta->flag == true) && (p_task_menu_dta->event == EV_BTN_ENTER_ACTIVE))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MAIN_MENU;
						strcpy(motor_config[motor_actual].spin, str_menu_3_spin[p_task_menu_dta->internal_state]);
						p_task_menu_dta->internal_state = 0;
					}
					break;


				default:

					p_task_menu_dta->tick  = DEL_MEN_XX_MIN;
					p_task_menu_dta->state = ST_MAIN_MENU;
					p_task_menu_dta->event = EV_BTN_ESCAPE_ACTIVE;
					p_task_menu_dta->internal_state = 0;
					p_task_menu_dta->flag  = false;

					break;
			}

			str_linea_1[15] = '\0';
			str_linea_2[15] = '\0';

			if (strcmp(str_linea_1, str_linea_1_aux) != 0){
				clear_display(0);
				strcpy(str_linea_1_aux, str_linea_1);

				displayCharPositionWrite(0, 0);
				displayStringWrite(str_linea_1);
			}

			if (strcmp(str_linea_2, str_linea_2_aux) != 0){
				clear_display(1);
				strcpy(str_linea_2_aux, str_linea_2);

				displayCharPositionWrite(0, 1);
				displayStringWrite(str_linea_2);
			}




		}
	}
}

/********************** end of file ******************************************/
