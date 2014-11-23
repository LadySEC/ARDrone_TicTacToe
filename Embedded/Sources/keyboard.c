/**
 * \file    keyboard.c
 * \brief   Process keyboard entries
 * \author  Lady team
 * \version 1.0
 * \date    21 november 2014
 *
 */
#include "keyboard.h"

/**********************************************************************************/
/* Global variables 															  */
/**********************************************************************************/

/**********************************************************************************/
/* Threads & Procedures														      */
/**********************************************************************************/
/**
 * \fn      void keyboard_rawMode(T_bool I_enable)
 * \brief   Enables user's keyboard entries in the shell
 *
 * \param   I_enable    TRUE: enable, FALSE: disable
 */
void keyboard_rawMode(T_bool I_enable)
{
    static struct termios Cooked;
    static int            Raw_enabled = 0u;
    struct termios 		  Raw;

    if(Raw_enabled == I_enable)
        return;

    if(I_enable == TRUE)
    {
        tcgetattr(STDIN_FILENO, &Cooked);

        Raw = Cooked;
        cfmakeraw(&Raw);
        tcsetattr(STDIN_FILENO, TCSANOW, &Raw);
    }
    else
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &Cooked);
    }

    Raw_enabled = (int)I_enable;
}

/**
 * \fn      int keyboard_hit(void)
 * \brief   Informs if a key has just been pressed by the user
 *
 * \return  1: Key pressed, 0: Nothing has happened 
 */
int keyboard_hit(void)
{
    struct timeval Tv = {0, 0};
    fd_set         Readfds;

    FD_ZERO(&Readfds);
    FD_SET(STDIN_FILENO, &Readfds);

    return select(STDIN_FILENO + 1, &Readfds, NULL, NULL, &Tv) == 1;
}

/**
 * \fn      unsigned char keyboard_getchar(void)
 * \brief   Process a special character from a keyboard entry 
 *
 * \return  Character value  
 */
unsigned char keyboard_getchar(void)
{
	unsigned char character = 0;

	character = getchar();

	/* Bytes management for reading special characters */
	if(character == 0x1B)
	{
		character = getchar();
		if(character ==	0x5B)
			character = getchar();
	}

	return(character);
}




/**********************************************************************************/
/* Threads			                               		          */
/**********************************************************************************/
/**
 * \fn      void*  kb_thread_drone_controller(void * args)
 * \brief   Thread that control the drone through the keyboard interface
 *
 * \param 	arg 	Input argument 
 * \return  		Nothing
 *
 * This thread read the keyboards input
 * And then process them to control the drone 
 */
void*  kbd_thread_drone_controller(void * args)
{

    /* Declarations */
    unsigned char  	key_pressed  	= 0;
    unsigned int   	key_selected	= 0;
    unsigned int 	counter;

    /* Activate the terminal for raw mode */
    keyboard_rawMode(TRUE);
    /* Infinite loop */
    do
    {
        /* Test */
        key_pressed = keyboard_hit();

        if(key_pressed)
        {
            /* Read the selected key */
            key_selected = keyboard_getchar();

            switch(key_selected)
            {
                case ENTER_KEY :
                    if(ATcommand_enoughBattery() == TRUE)
                    {
                        if(ATcommand_FlyingState() == FALSE)
                        {
                            /* Enable the bottom camera */
                            ATcommand_process(CONFIGURATION_IDS);
                            ATcommand_process(ENABLE_VISION);
                            /* Flat trim */
                            ATcommand_process(TRIM);
                            sleep(2u);
                            /* Take off */
                            ATcommand_process(TAKEOFF);
                            /* Wait the flying state */
                            while(ATcommand_FlyingState() != TRUE);
                        }
                        else
                        {
                            /* Landing */
                            ATcommand_process(LANDING);
                            /* Wait the landing state */
                            while(ATcommand_FlyingState() != FALSE);
                            /* Disable the bottom camera */
                            ATcommand_process(CONFIGURATION_IDS);
                            ATcommand_process(DISABLE_VISION);
                        }
                    }
                    else
                    {
                        /* Not enough battery to takeoff */
                        ATcommand_process(CONFIGURATION_IDS);
                        ATcommand_process(LED_ANIMATION);
                    }
                    break;

                case UP_KEY	:
                    for(counter = 0u; counter < NB_ORDER_OCCUR; counter++)
                    {
                        ATcommand_process(PITCH_DOWN);
                    }
                    break;

                case DOWN_KEY :
                    for(counter = 0u; counter < NB_ORDER_OCCUR; counter++)
                    {
                        ATcommand_process(PITCH_UP);
                    }
                    break;

                case LEFT_KEY	:
                    for(counter = 0u; counter < NB_ORDER_OCCUR; counter++)
                    {
                        ATcommand_process(YAW_LEFT);
                    }
                    break;

                case RIGHT_KEY :
                    for(counter = 0u; counter < NB_ORDER_OCCUR; counter++)
                    {
                        ATcommand_process(YAW_RIGHT);
                    }
                    break;

                case Z_KEY :
                    for(counter = 0u; counter < NB_ORDER_OCCUR; counter++)
                    {
                        ATcommand_process(VERTICAL_UP);
                    }
                    break;

                case S_KEY :
                    for(counter = 0u; counter < NB_ORDER_OCCUR; counter++)
                    {
                        ATcommand_process(VERTICAL_DOWN);
                    }
                    break;

                case Q_KEY :
                    for(counter = 0u; counter < NB_ORDER_OCCUR; counter++)
                    {
                        ATcommand_process(ROLL_LEFT);
                    }
                    break;

                case D_KEY :
                    for(counter = 0u; counter < NB_ORDER_OCCUR; counter++)
                    {
                        ATcommand_process(ROLL_RIGHT);
                    }
                    break;

                case SPACE_KEY :
                    ATcommand_process(TRIM);
                    break;

                    /* Sequence test */
                case BACKSPACE_KEY :
                    for(counter = 0u; counter < 10u; counter++)
                    {
                        ATcommand_process(VERTICAL_UP);
                    }
                    for(counter = 0u; counter < NB_ORDER_OCCUR; counter++)
                    {
                        ATcommand_process(VERTICAL_DOWN);
                    }
                    for(counter = 0u; counter < 10u; counter++)
                    {
                        ATcommand_process(PITCH_DOWN);
                    }
                    for(counter = 0u; counter < 10u; counter++)
                    {
                        ATcommand_process(YAW_LEFT);
                    }
                    for(counter = 0u; counter < 10u; counter++)
                    {
                        ATcommand_process(PITCH_DOWN);
                    }
                    break;

                case A_KEY :
                    /* Disable the bottom camera */
                    ATcommand_process(CONFIGURATION_IDS);
                    ATcommand_process(DISABLE_VISION);
                    break;

                case L_KEY :
                    ATcommand_process(CONFIGURATION_IDS);
                    ATcommand_process(LED_ANIMATION);
                    break;

                case E_KEY :
                    ATcommand_process(EMERGENCY);
                    break;
            }
        }

        /* Empty the output buffer */
        fflush(stdout);
    }
    while(key_selected != CTRL_C_KEY);

    printf("\n\rEnd");

    /* Disable the raw mode */
    keyboard_rawMode(FALSE);

}

