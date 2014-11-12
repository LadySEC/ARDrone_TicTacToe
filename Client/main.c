/* Useful libraries */
#include <stdio.h>			// Standard C ANSI
#include <stdlib.h>
#include <pthread.h>		// POSIX library for thread management
#include <malloc.h>			// Dynamic allocation
//#include <semaphore.h>	
#include <sys/socket.h>		// UDP/TCP
#include <netinet/in.h>
#include <string.h> 
#include <unistd.h>			// usleep
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>

#include <time.h>
#include <signal.h>

#include "global.h"

/**********************************************************************************/
/* Global variables 															  */
/**********************************************************************************/

/* Sockets */
int socket_AT;
int socket_NAV;
/* Buffer */
char 	commandBuffer[NB_MAX_BITS_COMMAND][NB_MAX_COMMANDS]; 
int 	nb_received_commands 	= 0u;
int 	sequence_number 		= 1u;
/* Mutexes */
pthread_mutex_t mutex_seqNum = PTHREAD_MUTEX_INITIALIZER;
/* States */
unsigned char 	flying 			= 0u;
unsigned int 	navdata[20u];

/**********************************************************************************/
/* Threads & Procedures														      */
/**********************************************************************************/

/* Procedures */
void die(char *s)
{
    perror(s);
    exit(1);
}

int socket_init(protocol p, int port)
{
	/* Declaration */
	struct 	sockaddr_in client;
	int 				socket_id;

	/* Generate a socket */
	switch(p)
	{
		case TCP:
			socket_id = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
			break;

		case UDP:
			socket_id = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP);
			break;
	}

    if(socket_id == -1)
   	{
   		die("socket");
   	}
   	/* Socket created */
   	else
   	{
   		/* zero out the structure */
    	memset((char *) &client, 0, sizeof(client));

   		client.sin_addr.s_addr 	= htonl(INADDR_ANY); 
	    client.sin_family 		= AF_INET;
	    client.sin_port 		= htons(port);
	 
	 	/* bind socket to port */
	    if( bind(socket_id, (struct sockaddr*)&client, sizeof(client) ) == -1)
	    {
	        die("bind");
	    }
   	}

    return(socket_id);
}

void sendFrame(int socket_id, int port_dest, char* message)
{
	/* Declaration */
	struct 	sockaddr_in server;

	/* zero out the structure */
	memset((char *) &server, 0, sizeof(server));

	server.sin_addr.s_addr 	= htonl(INADDR_ANY); 
    server.sin_family 		= AF_INET;
    server.sin_port 		= htons(port_dest);

	if (sendto(socket_id, message, strlen(message)+1, 0, (struct sockaddr*) &server, sizeof(server)) == -1)
    {
        die("sendto()");
    }
}

void sendBytes(int socket_id, int port_dest, unsigned char* bytes, unsigned char lenght)
{
	/* Declaration */
	struct 	sockaddr_in server;

	/* zero out the structure */
	memset((char *) &server, 0, sizeof(server));

	server.sin_addr.s_addr 	= htonl(INADDR_ANY); 
    server.sin_family 		= AF_INET;
    server.sin_port 		= htons(port_dest);

	if (sendto(socket_id, bytes, lenght, 0, (struct sockaddr*) &server, sizeof(server)) == -1)
    {
        die("sendto()");
    }
}

unsigned char readFrame(int socket_id, int port_dest, unsigned int* data, unsigned char lenght, function_state state)
{
	/* Declaration */
	struct 	sockaddr_in server;
	unsigned int 		lenght_server = sizeof(server);
	unsigned char 		error = 0u;


	/* zero out the structure */
	memset((char *) &server, 0, sizeof(server));

	server.sin_addr.s_addr 	= htonl(INADDR_ANY); 
    server.sin_family 		= AF_INET;
    server.sin_port 		= htons(port_dest);

    if(state == BLOCKING)
    {
    		if (recvfrom(socket_id, data, lenght, 0u, (struct sockaddr*) &server, &lenght_server) == -1)
    		{
        		die("recvfrom()");
        		error = 1u;
    		}
    }
    else
    {
    		if (recvfrom(socket_id, data, lenght, MSG_DONTWAIT, (struct sockaddr*) &server, &lenght_server) == -1) 
    		{
        		error = 1u;
    		}
    }

    return(error);
}

void ATcommand_generate(char* frame, ATcommands command, word32bits* array, char strings[NB_MAX_STRING_ARG][NB_MAX_CHAR])
{
	/* Reset the frame */
	memset((char *) frame, 0, sizeof(frame));
	/* Generate correct AT command */
	switch(command)
	{
		case REF:
			sprintf(frame,"%s=%d,%d\r",commands[command], sequence_number, array[0u].integer);
			break;

		case PCMD:
			sprintf(frame,"%s=%d,%d,%d,%d,%d,%d\r",commands[command], sequence_number, 
					array[0u].integer, array[1u].integer, array[2u].integer, array[3u].integer, array[4u].integer);
			break;

		case PCMD_MAG:
			sprintf(frame,"%s=%d,%d,%d,%d,%d,%d,%d,%d\r",commands[command], sequence_number, 
					array[0u].integer, array[1u].integer, array[2u].integer, array[3u].integer, array[4u].integer, array[5u].integer, array[6u].integer);
			break;

		case FTRIM:
			sprintf(frame,"%s=%d\r",commands[command], sequence_number);
			break;

		case CONFIG:
			sprintf(frame,"%s=%d,%s,%s\r",commands[command], sequence_number,
					strings[0u], strings[1u]);
			break;

		case CONFIG_IDS:
			sprintf(frame,"%s=%d,%s,%s,%s\r",commands[command], sequence_number,
					strings[0u], strings[1u], strings[2u]);
			break;	

		case COMWDG:
			sprintf(frame,"%s=%d\r",commands[command], sequence_number);
			break;

		case CALIB:
			sprintf(frame,"%s=%d,%d\r",commands[command], sequence_number, array[0u].integer);
			break;		

		case CTRL:
			sprintf(frame,"%s=%d,%d\r",commands[command], sequence_number, array[0u].integer);
			break;	
	}

	pthread_mutex_lock(&mutex_seqNum);
	/* Update sequence number */
	sequence_number++;
	pthread_mutex_unlock(&mutex_seqNum);
}

void ATcommand_send(ATorders order)
{
	char 			frame[NB_MAX_BITS_COMMAND];
	char 			sub_frame[NB_MAX_BITS_COMMAND];
	/* At commands management */
	word32bits 		ATarguments[NB_MAX_UNION_ARG];
	char 			ATstrings[NB_MAX_STRING_ARG][NB_MAX_CHAR];
	int 			sendToBuffer = 1u, print = 1u;
	unsigned char 	bytes[5u];

	/* Build the frame */
	switch(order)
	{
		case CALIBRATION:
			ATcommand_generate(frame, FTRIM, ATarguments, ATstrings);
			sendToBuffer = 0u;
			sendFrame(socket_AT,5556,frame);
			break;

		case TAKEOFF:
			ATarguments[0u].integer = TAKEOFF_COMMAND;
			ATcommand_generate(frame, REF, ATarguments, ATstrings);
			break;

		case LANDING:
			ATarguments[0u].integer = LANDING_COMMAND;
			ATcommand_generate(frame, REF, ATarguments, ATstrings);
			break;

		case EMERGENCY:
			ATarguments[0u].integer = EMERGENCY_COMMAND;
			ATcommand_generate(frame, REF, ATarguments, ATstrings);
			sendToBuffer = 0u;
			sendFrame(socket_AT,5556,frame);
			break;

		case HOVERING:
			ATarguments[0u].integer = 1u;
			ATarguments[1u].integer = 0u;
			ATarguments[2u].integer = 0u;
			ATarguments[3u].integer = 0u;
			ATarguments[4u].integer = 0u;
			ATarguments[5u].integer = 0u;
			ATarguments[6u].integer = 0u;
			sendToBuffer = 0u;
			ATcommand_generate(frame, PCMD_MAG, ATarguments, ATstrings);
			sendFrame(socket_AT,5556,frame);
			break;

		case YAW_RIGHT:
			ATarguments[0u].integer = 1u;
			ATarguments[1u].integer = 0u;
			ATarguments[2u].integer = 0u;
			ATarguments[3u].integer = 0u;
			ATarguments[4u].floating = 0.5f;
			ATarguments[5u].integer = 0u;
			ATarguments[6u].integer = 0u;
			ATcommand_generate(frame, PCMD_MAG, ATarguments, ATstrings);
			break;	

		case YAW_LEFT:
			ATarguments[0u].integer = 1u;
			ATarguments[1u].integer = 0u;
			ATarguments[2u].integer = 0u;
			ATarguments[3u].integer = 0u;
			ATarguments[4u].floating = -0.5f;
			ATarguments[5u].integer = 0u;
			ATarguments[6u].integer = 0u;
			ATcommand_generate(frame, PCMD_MAG, ATarguments, ATstrings);
			break;	

		case PITCH_UP:
			ATarguments[0u].integer = 1u;
			ATarguments[1u].integer = 0u;
			ATarguments[2u].floating = 0.5f;
			ATarguments[3u].integer = 0u;
			ATarguments[4u].integer = 0u;
			ATarguments[5u].integer = 0u;
			ATarguments[6u].integer = 0u;
			ATcommand_generate(frame, PCMD_MAG, ATarguments, ATstrings);
			break;	

		case PITCH_DOWN:
			ATarguments[0u].integer = 1u;
			ATarguments[1u].integer = 0u;
			ATarguments[2u].floating = -0.5f;
			ATarguments[3u].integer = 0u;
			ATarguments[4u].integer = 0u;
			ATarguments[5u].integer = 0u;
			ATarguments[6u].integer = 0u;
			ATcommand_generate(frame, PCMD_MAG, ATarguments, ATstrings);
			break;	

		case VERTICAL_UP:
			ATarguments[0u].integer = 1u;
			ATarguments[1u].integer = 0u;
			ATarguments[2u].integer = 0u;
			ATarguments[3u].floating = 0.5f;
			ATarguments[4u].integer = 0u;
			ATarguments[5u].integer = 0u;
			ATarguments[6u].integer = 0u;
			ATcommand_generate(frame, PCMD_MAG, ATarguments, ATstrings);
			break;	

		case VERTICAL_DOWN:
			ATarguments[0u].integer = 1u;
			ATarguments[1u].integer = 0u;
			ATarguments[2u].integer = 0u;
			ATarguments[3u].floating = -0.5f;
			ATarguments[4u].integer = 0u;
			ATarguments[5u].integer = 0u;
			ATarguments[6u].integer = 0u;
			ATcommand_generate(frame, PCMD_MAG, ATarguments, ATstrings);
			break;

		case INIT_NAVDATA:
			strcpy(ATstrings[0u], "\"general:navdata_demo\"");
			strcpy(ATstrings[1u], "\"FALSE\"");
			ATcommand_generate(frame, CONFIG, ATarguments, ATstrings);
			sendToBuffer = 0u;
			sendFrame(socket_AT,5556,frame);
			break;

		case CONFIGURATION_IDS:
			strcpy(ATstrings[0u], session_id);
			strcpy(ATstrings[1u], profile_id);
			strcpy(ATstrings[2u], application_id);
			ATcommand_generate(frame, CONFIG_IDS, ATarguments, ATstrings);
			sendToBuffer = 0u;
			sendFrame(socket_AT,5556,frame);
			break;

		case LED_ANIMATION:
			strcpy(ATstrings[0u], "\"leds:leds_anim\"");
			strcpy(ATstrings[1u], "\"3,1073741824,2\"");
			ATcommand_generate(frame, CONFIG, ATarguments, ATstrings);
			sendToBuffer = 0u;
			sendFrame(socket_AT,5556,frame);
			break;

		case ACK_COMMAND:
			ATarguments[0u].integer = 0u;
			ATcommand_generate(frame, CTRL, ATarguments, ATstrings);
			sendToBuffer = 0u;
			sendFrame(socket_AT,5556,frame);
			break;

		case NAVDATA_REQUEST:
			bytes[0u] 		= 1u;
			bytes[1u] 		= 0u;
			bytes[2u] 		= 0u;
			bytes[3u] 		= 0u;
			bytes[4u] 		= 0u;
			sendToBuffer 	= 0u;
			print 			= 0u;
			sendBytes(socket_NAV,5554,bytes,5u);
			printf("\n\rData sent: 01 00 00 00 00");
			break;

		case RESET_WATCHDOG:
			sendToBuffer = 0u;
			ATcommand_generate(frame, COMWDG, ATarguments, ATstrings);
			sendFrame(socket_AT,5556,frame);
			break;

		case INIT_CONFIG:
			sendToBuffer 	= 0u;
			print 			= 0u;
			printf("\n\r%s:", orders[order]);
			/* Build config_ids arg */
			strcpy(ATstrings[0u], session_id);
			strcpy(ATstrings[1u], profile_id);
			strcpy(ATstrings[2u], application_id);

			/* Session id */
			ATcommand_generate(frame, CONFIG_IDS, ATarguments, ATstrings);
			sendFrame(socket_AT,5556,frame);
			printf("\n\rAT command sent: %s", frame);
			strcpy(ATstrings[0u], "\"custom:session_id\"");
			strcpy(ATstrings[1u], session_id);
			ATcommand_generate(sub_frame, CONFIG, ATarguments, ATstrings);
			sendFrame(socket_AT,5556,sub_frame);
			printf("\n\rAT command sent: %s", sub_frame);

			/* profile id */
			ATcommand_generate(frame, CONFIG_IDS, ATarguments, ATstrings);
			sendFrame(socket_AT,5556,frame);
			printf("\n\rAT command sent: %s", frame);
			strcpy(ATstrings[0u], "\"custom:profile_id\"");
			strcpy(ATstrings[1u], profile_id);
			ATcommand_generate(sub_frame, CONFIG, ATarguments, ATstrings);
			sendFrame(socket_AT,5556,sub_frame);
			printf("\n\rAT command sent: %s", sub_frame);

			/* application id */
			ATcommand_generate(frame, CONFIG_IDS, ATarguments, ATstrings);
			sendFrame(socket_AT,5556,frame);
			printf("\n\rAT command sent: %s", frame);
			strcpy(ATstrings[0u], "\"custom:application_id\"");
			strcpy(ATstrings[1u], application_id);
			ATcommand_generate(sub_frame, CONFIG, ATarguments, ATstrings);
			sendFrame(socket_AT,5556,sub_frame);
			printf("\n\rAT command sent: %s", sub_frame);
			break;
	}

	/* Add the frame to the buffer */
	if(sendToBuffer == 1u)
	{
		printf("\n\r%s:", orders[order]);
		strcpy(commandBuffer[nb_received_commands], frame);
		if(nb_received_commands < (NB_MAX_COMMANDS - 1))
		{
			nb_received_commands++;
		}
		else
		{
			nb_received_commands = 0u;
		}
	}
	else
	{	if(print == 1u)
		{
			/* Echo */
			printf("\n\r%s:", orders[order]);
			printf("\n\rAT command sent: %s", frame);
		}
	}
}

void Mode_raw(int Activer)
{
    static struct termios Cooked;
    static int            Raw_Actif = 0;
    struct termios 		  Raw;

    if(Raw_Actif == Activer)
        return;

    if(Activer)
    {
        tcgetattr(STDIN_FILENO, &Cooked);

        Raw = Cooked;
        cfmakeraw(&Raw);
        tcsetattr(STDIN_FILENO, TCSANOW, &Raw);
    }
    else
        tcsetattr(STDIN_FILENO, TCSANOW, &Cooked);

    Raw_Actif = Activer;
}

int kbhit(void)
{
    struct timeval Tv = {0, 0};
    fd_set         Readfds;

    FD_ZERO(&Readfds);
    FD_SET(STDIN_FILENO, &Readfds);

    return select(STDIN_FILENO + 1, &Readfds, NULL, NULL, &Tv) == 1;
}

unsigned char _getch(void)
{
	unsigned char character = 0;


	character = getchar();		/* Lecture du caractere */

	/* Gestion des 3 octets codant un caractere special */
	if(character == 0x1B)
	{
		character = getchar();
		if(character ==	0x5B)
			character = getchar();
	}

	return(character);
}



/* Threads */
void* empty_commandBuffer(void* arg)
{
	int 	nb_sent_commands = 0u;

	printf("\n\rDebut du thread");

	while(1)
	{
		if(((nb_received_commands == 0u) && (nb_sent_commands > 0u)) || (nb_sent_commands < nb_received_commands))
		{
			/* Send the frame through local host */
			sendFrame(socket_AT, 5556, commandBuffer[nb_sent_commands]);
			/* Echo */
			printf("\n\rAT command sent: %s", commandBuffer[nb_sent_commands]);
			/* Update */
			if(nb_sent_commands < (NB_MAX_COMMANDS - 1))
			{
				nb_sent_commands++;
			}
			else
			{
				nb_sent_commands = 0u;
			}
		}
		else
		{
			#if 1
			if(flying == 1u)
			{
				ATcommand_send(HOVERING);
			}
			#endif
		}
	
		/* Wait */
		usleep(BUFFER_TEMPO);
	}

  	return NULL;
}

void* reset_watchdog (void* arg)
{
	while(1)
	{
		/* Wait */
		usleep(WATCHDOG_TEMPO);
		/* Reset Watchdog */
		ATcommand_send(RESET_WATCHDOG);
	}
}

/**********************************************************************************/
/* Main program													      			  */
/**********************************************************************************/
int main (int argc, char *argv[])
{
	/* Declarations */
	unsigned char  	key_pressed  	= 0;
	unsigned int   	key_selected	= 0;
	int 			flags;
	/* Thread */
	pthread_t 		thread1;
	pthread_t 		thread2;

	/* Client version */
	printf("Client v0.3\n\r");

	/* Create a socket */
	socket_AT 	= socket_init(UDP, 15213u);
	socket_NAV 	= socket_init(UDP, 15214u);
	flags 		= fcntl(socket_NAV, F_GETFL);
	fcntl(socket_NAV, F_SETFL, flags | O_NONBLOCK);

	/* Initiate the identifiers */
	ATcommand_send(INIT_CONFIG);
	ATcommand_send(CONFIGURATION_IDS);
	ATcommand_send(LED_ANIMATION);

	/* Initiate navdata */
	ATcommand_send(NAVDATA_REQUEST);
	system("iptables -t nat -F");
	system("iptables -t nat -A POSTROUTING -p UDP --sport 15214 -j SNAT --to 127.0.0.1:5554");
	system("iptables -t nat -A PREROUTING -p UDP -d 127.0.0.1 --dport 5554 -j DNAT --to 127.0.0.1:15214");
	usleep(100000);
	readFrame(socket_NAV, 15214u, navdata, 16u, NON_BLOCKING);

	printf("\n\rNAVDATA: %x %x %x %x | %x | %x %x %f %f %f %x %f %f %f | %x %x", 
			navdata[0u], navdata[1u], navdata[2u], navdata[3u], 
			navdata[4u], navdata[5u], navdata[6u], (double)navdata[7u], (double)navdata[8u], (double)navdata[9u], navdata[10u],
			(double)navdata[11u], (double)navdata[12u], (double)navdata[13u], navdata[14u], navdata[15u]);

	ATcommand_send(CONFIGURATION_IDS);
	ATcommand_send(INIT_NAVDATA);
	usleep(100000);
	readFrame(socket_NAV, 15214u, navdata, 16u, NON_BLOCKING);

	printf("\n\rNAVDATA: %x %x %x %x | %x | %x %x %f %f %f %x %f %f %f | %x %x", 
			navdata[0u], navdata[1u], navdata[2u], navdata[3u], 
			navdata[4u], navdata[5u], navdata[6u], (double)navdata[7u], (double)navdata[8u], (double)navdata[9u], navdata[10u],
			(double)navdata[11u], (double)navdata[12u], (double)navdata[13u], navdata[14u], navdata[15u]);

	ATcommand_send(ACK_COMMAND);

	/* Initialize the thread */
	pthread_create (&thread1, NULL, empty_commandBuffer, NULL);
	pthread_create (&thread2, NULL, reset_watchdog, NULL);

	/* Activate the terminal for brut mode */
	Mode_raw(1);

	do
	{
		/* Test */
        key_pressed = kbhit();
        if(key_pressed)
        {
            /* Read the selected key */
            key_selected = _getch();
	        switch(key_selected)
	        {
	            case UP_KEY	:
	            	ATcommand_send(PITCH_DOWN);
                	break;

	            case DOWN_KEY :
	            	ATcommand_send(PITCH_UP);
                	break;

                case LEFT_KEY	:
	            	ATcommand_send(YAW_LEFT);
                	break;

	            case RIGHT_KEY :
	            	ATcommand_send(YAW_RIGHT);
                	break;

                case ENTER_KEY :
                	ATcommand_send(CONFIGURATION_IDS);
                	ATcommand_send(LED_ANIMATION);

	            	if(flying == 0u)
	            	{
	            		/* Calibrate the drone */
						ATcommand_send(CALIBRATION);
						sleep(2);
	            		ATcommand_send(TAKEOFF);
	            		sleep(5);
	            		flying = 1u;
	            	}
	            	else
	            	{
	            		ATcommand_send(LANDING);
	            		flying = 0u;
	            		/* Security */
	            		usleep(1000000);
	            	}
                	break;

                case SPACE_KEY :
	            	ATcommand_send(VERTICAL_UP);
                	break;

                case BACKSPACE_KEY :
                	ATcommand_send(VERTICAL_DOWN);
                	break;

                case L_KEY :
                	ATcommand_send(EMERGENCY);
                	break;
	        }
        }

        /* Try to read a navdata */
		if(readFrame(socket_NAV, 15214u, navdata, 16u, NON_BLOCKING) == 0u)
		{
			printf("\n\rNAVDATA: %x %x %x %x | %x | %x %x %f %f %f %x %f %f %f | %x %x", 
			navdata[0u], navdata[1u], navdata[2u], navdata[3u], 
			navdata[4u], navdata[5u], navdata[6u], (double)navdata[7u], (double)navdata[8u], (double)navdata[9u], navdata[10u],
			(double)navdata[11u], (double)navdata[12u], (double)navdata[13u], navdata[14u], navdata[15u]);
		}

        /* Empty the output buffer */
		fflush(stdout);
	}
	while(key_selected != CTRL_C_KEY);

	printf("\n\rEnd");

	/* Activate the terminal for brut mode */
	Mode_raw(0);

	/* Close current threads */
   	pthread_cancel(thread1);
   	pthread_cancel(thread2);

    /* Close the socket */
	close(socket_AT);
	close(socket_NAV);

	return(0);
}
