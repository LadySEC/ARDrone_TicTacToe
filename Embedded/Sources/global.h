/**
 * \file 	global.h
 * \brief 	Common declarations required for the whole project
 * \author 	Lady team
 * \version 1.0
 * \date 	4 December 2014
 *
 */
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

/**********************************************************************************/
/* Libraries														      		  */
/**********************************************************************************/
#include <stdio.h>			// Standard C ANSI
#include <stdlib.h>
#include <unistd.h>			// usleep
#include <pthread.h>	    // POSIX library for thread management
#include <signal.h>

/**********************************************************************************/
/* Preprocessors commands														  */
/**********************************************************************************/
/* Enable functions */
#define ENABLE_KEYBOARD
#define ENABLE_SUPERVISOR
#define ENABLE_CALCUL_ORDER
//#define ENABLE_CONFIG_VIDEO
//#define ENABLE_DRONE_CONTROL_KEYBOARD

/* Debug */
//#define DEBUG_NAVDATA
#define DEBUG_MISSION
//#define PRINT_TCPUDP_DATA_SENT
#define PRINT_TCP_DATA_RECEIVED

/**********************************************************************************/
/* Constants						      			                              */
/**********************************************************************************/

#define POS_TOLERANCE	120
#define POS_MAX			360

/**********************************************************************************/
/* Types													      				  */
/**********************************************************************************/
/**
 * \struct 	T_bool
 * \brief 	Defines a boolean state
 */
typedef enum 
{
	FALSE 	= 0u,	/*!< FALSE state */
	TRUE  	= 1u  	/*!< TRUE state */
}T_bool;

/**
 * \struct 	T_error
 * \brief 	Defines an error state
 */
typedef enum 
{
	NO_ERROR = 0u,	/*!< Everything works */
	ERROR 	 = 1u  	/*!< An error has occured */
}T_error;

#endif //! _GLOBAL_H_
