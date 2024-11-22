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
 * @file   : task_menu_attribute.h
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

#ifndef TASK_INC_TASK_MENU_ATTRIBUTE_H_
#define TASK_INC_TASK_MENU_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* Menu Statechart - State Transition Table */
/* 	------------------------+-----------------------+-----------------------+-----------------------+------------------------
 * 	| Current               | Event                 |                       | Next                  |                       |
 * 	| State                 | (Parameters)          | [Guard]               | State                 | Actions               |
 * 	|=======================+=======================+=======================+=======================+=======================|
 * 	| ST_MEN_XX_IDLE        | EV_MEN_MEN_ACTIVE     |                       | ST_MEN_XX_ACTIVE      |                       |
 * 	|                       |                       |                       |                       |                       |
 * 	|-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * 	| ST_MEN_XX_ACTIVE      | EV_MEN_MEN_IDLE       |                       | ST_MEN_XX_IDLE        |                       |
 * 	|                       |                       |                       |                       |                       |
 * 	------------------------+-----------------------+-----------------------+-----------------------+------------------------
 */

/* Events to excite Task Menu */
typedef enum task_menu_ev {
	EV_BTN_ESCAPE_IDLE,
	EV_BTN_ESCAPE_ACTIVE,
	EV_BTN_MENU_IDLE,
	EV_BTN_MENU_ACTIVE,
	EV_BTN_ENTER_IDLE,
	EV_BTN_ENTER_ACTIVE,
	EV_BTN_NEXT_IDLE,
	EV_BTN_NEXT_ACTIVE
	// El botón "Escape" hace que vuelvas al menú anterior.
	// El botón "Menú" hace que avances desde el menú inicial al menú de selección del motor.
	// El botón "Enter" hace que se fije el parámetro seleccionado.
	// El botón "Next" cambia el valor del parámetro.
} task_menu_ev_t;

/* State of Task Menu */
typedef enum task_menu_st {
	ST_MAIN_MENU, 		// Menú inicial, por defecto
	ST_MENU_1, 			// Menú de selección de motor. Su internal_state indica el motor a utilizar.
	ST_MENU_2,			// Menú de selección de parámetro a variar. Su internal_state indica el parametro del motor seleccionado a modificar
	ST_MENU_3_POWER,		// Menú de ajuste del parámetro "Encendido". Su internal_state indica si el motor está encendido o apagado
	ST_MENU_3_SPEED,		// Menú de ajuste del parámetro "Velocidad". Su internal_state indica la velocidad del motor (varía de 0 a 9).
	ST_MENU_3_SPIN		// Menú de ajuste del parámetro "Sentido de rotación". Su internal_state indica el sentido de giro del motor.
} task_menu_st_t;

typedef struct
{
	uint32_t			tick;
	task_menu_st_t		state;				// Indica el estado actual
	task_menu_ev_t		event;				// Indica el evento actual
	uint32_t 			internal_state;		// Indica el valor de l parámetro del estado actual
	bool				flag;				// Indica si el estado fue procesado o no.
} task_menu_dta_t;

typedef struct {
	char id[17];
	char power[17];
	char speed[17];
	char spin[17];
} motor_t;




/********************** external data declaration ****************************/
extern task_menu_dta_t task_menu_dta;

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_MENU_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
