/**
 * \file 	communication.c
 * \brief 	Manages UDP/TCP communications
 * \author 	Lady team
 * \version 1.0
 * \date 	4 December 2014
 *
 */
/**********************************************************************************/
/* Libraries														      		  */
/**********************************************************************************/
#include "communication.h"

/**********************************************************************************/
/* Global variables 															  */
/**********************************************************************************/

/**********************************************************************************/
/* Procedures														      		  */
/**********************************************************************************/
/**
 * \fn 		T_comm* communication_initiate(T_protocol I_protocol, char* I_IP_addr_client, char* I_IP_addr_server, int I_port_client, int I_port_server, T_state I_state)
 * \brief 	Create and initialize a UDP/TCP socket
 *
 * \param 	I_protocol 	Protocol configuration 
 * \param
 * \param
 * \param
 * \param 	I_port  	Choosen port
 * \param 	I_state  	Socket status configuration
 * \return 				Communication structure
 */
T_comm* communication_initiate(T_protocol I_protocol, const char* I_IP_addr_client, const char* I_IP_addr_server, int I_port_client, int I_port_server, T_state I_state)
{
	/* Declaration */
	T_comm*  			communication = (T_comm*)malloc(sizeof(T_comm));
	socklen_t 			socket_lenght;
	int 				flags;


	/* Initialization */
	communication->server = NULL;
	communication->client = NULL;

	/* Save the protocol */
	communication->protocol = I_protocol;
	/* Generate a socket */
	switch(I_protocol)
	{
		case TCP:
			LOG_WriteLevel(LOG_INFO, "communication : initiating a TCP communication");
			/* Instantiate a server */
			communication->server 		= (T_socket*)malloc(sizeof(T_socket));
			communication->server->id 	= socket(PF_INET , SOCK_STREAM, IPPROTO_TCP);

			if(communication->server->id == -1)
		   	{
		   		perror("\n\rsocket()");
		   	}
		   	/* Socket created */
		   	else
		   	{
		   		//Zero out socket address
  				memset(&communication->server->parameters, 0, sizeof(communication->server->parameters));

		   		/* Socket configuration */
		   		communication->server->parameters.sin_addr.s_addr 	= htonl(INADDR_ANY);
			    communication->server->parameters.sin_family 		= AF_INET;
			    communication->server->parameters.sin_port 			= htons(I_port_server);
			 
			 	/* bind socket to port */
			 	LOG_WriteLevel(LOG_INFO, "communication : waiting a connection from a TCP client");
			    if( bind(communication->server->id, (struct sockaddr*)&communication->server->parameters, sizeof(communication->server->parameters)) == -1)
			    {
			        perror("\n\rbind()");
			        /* Desallocation */
			        free(communication->server);
			        communication->server = NULL;
			    }
			    else
			    {
		    		/* Waiting for a communication */
	    			if(listen(communication->server->id, 10u) == -1)
	    			{
	    				perror("\n\rlisten()");
	    			}
	    			else
	    			{
	    				/* Instantiate a client */
	    				communication->client 				= (T_socket*)malloc(sizeof(T_socket));
						socket_lenght = sizeof(communication->client->parameters);
	    				/* Accepting the communication */
					    communication->client->id 	= accept(communication->server->id, NULL, NULL);
					    if(communication->client->id == -1)
					    {
					        perror("\n\raccept()");
					    }
	    			}
			    }
		   	}
			break;

		case UDP:
			LOG_WriteLevel(LOG_INFO, "communication : initiating a UDP communication");
			/* Instantiate a client */
			communication->client 		= (T_socket*)malloc(sizeof(T_socket));
			communication->client->id 	= socket(AF_INET ,SOCK_DGRAM, IPPROTO_UDP);
			/* Instantiate a server */
			communication->server 		= (T_socket*)malloc(sizeof(T_socket));

			if(communication->client->id == -1)
		   	{
		   		perror("\n\rsocket()");
		   	}
		   	/* Socket created */
		   	else
		   	{
		   		/* Client */
		   		//Zero out socket address
  				memset(&communication->client->parameters, 0, sizeof(communication->client->parameters));
		   		//Set parameters 
		   		communication->client->parameters.sin_addr.s_addr 	= inet_addr(I_IP_addr_client); //htonl(INADDR_ANY);
			    communication->client->parameters.sin_family 		= AF_INET;
			    communication->client->parameters.sin_port 			= htons(I_port_client);

			    /* Server */
			    //Zero out socket address
  				memset(&communication->server->parameters, 0, sizeof(communication->server->parameters));
		   		//Set parameters 
		   		communication->server->parameters.sin_addr.s_addr 	= inet_addr(I_IP_addr_server);
			    communication->server->parameters.sin_family 		= AF_INET;
			    communication->server->parameters.sin_port 			= htons(I_port_server);
			 
			 	/* Bind the client socket */
			    if( bind(communication->client->id, (struct sockaddr*)&communication->client->parameters, sizeof(communication->client->parameters)) == -1)
			    {
			        perror("\n\rbind()");
			        /* Desallocation */
			        free(communication->client);
			        communication->client = NULL;
			    }
		   	}
			break;
	}

	/* Test if an error occured */
	if((communication->client == NULL) || (communication->server == NULL))
	{
		LOG_WriteLevel(LOG_INFO, "communication : bind error");
		free(communication);
		communication = NULL;
		LOG_WriteLevel(LOG_ERROR, "communication : communication establishment aborted");
	}
	else
	{
		/* Set the socket as non-blocking */
	    if((I_state ==  NON_BLOCKING) && (communication->client->id != -1))
	   	{
	   		flags 		= fcntl(communication->client->id, F_GETFL);
			fcntl(communication->client->id, F_SETFL, flags | O_NONBLOCK);
	   	}
	}

    

    return(communication);
}

/**
 * \fn 		T_error socket_sendString(int I_emitter_id, struct 	sockaddr_in* I_parameters, char* I_message)
 * \brief 	Send an 8-bit ASCII string through a packet
 *
 * \param   
 * \param 	
 * \param 	I_message  		String to be sent
 * \return 					ERROR: Transmission error, NO_ERROR: Success
 */
T_error socket_sendString(T_protocol I_protocol, int I_emitter_id, struct sockaddr_in* I_parameters, char* I_message)
{
	/* Declaration */
	socklen_t 			lenght_param 	= (socklen_t)sizeof(struct sockaddr_in);
	T_error 			error 			= NO_ERROR;


	switch(I_protocol)
	{
		case TCP:
			if (write(I_emitter_id, I_message, strlen(I_message)+1) == -1)
		    {
		        perror("write()");
		        error = ERROR;
		    }
		    else
		    {
		#ifdef PRINT_TCP_DATA_SENT
		    	LOG_WriteLevel(LOG_DEBUG, "communication : TCP string sent: %.*s", strlen(I_message)-1u , I_message);
		#endif
		    }
			break;

		case UDP:
			if (sendto(I_emitter_id, I_message, strlen(I_message)+1, 0, (struct sockaddr*) I_parameters, lenght_param) == -1)
		    {
		        perror("sendto()");
		        error = ERROR;
		    }
		    else
		    {
		#ifdef PRINT_UDP_DATA_SENT
		    	LOG_WriteLevel(LOG_DEBUG, "communication : UDP string sent to %s:%d: %.*s", 
		    					inet_ntoa(I_parameters->sin_addr), 
		    					(int)ntohs(I_parameters->sin_port),
		    					strlen(I_message)-1u , I_message);
		#endif
		    }
			break;
	}

    return(error);
}

/**
 * \fn 		T_error socket_sendBytes(int I_emitter_id, struct 	sockaddr_in* I_parameters, unsigned char* I_bytes, unsigned char I_lenght)
 * \brief 	Send an 8-bit array of provided lenght through a packet
 *
 * \param
 * \param 	
 * \param 	I_bytes  		Array to be sent
 * \param 	I_lenght  		Lenght of the array
 * \return 					ERROR: Transmission error, NO_ERROR: Success
 */
T_error socket_sendBytes(T_protocol I_protocol, int I_emitter_id, struct sockaddr_in* I_parameters, unsigned char* I_bytes, unsigned char I_lenght)
{
	/* Declaration */
	socklen_t 			lenght_param 	= (socklen_t)sizeof(struct sockaddr_in);
	T_error 			error 			= NO_ERROR;

	char				frame_sent[(I_lenght*3u) + 1u];
	char 				byte_ascii[4u];
	unsigned char		index;
	unsigned char  		index_frame = 0u;


	switch(I_protocol)
	{
		case TCP:
			if (write(I_emitter_id, I_bytes, I_lenght) == -1)
		    {
		        perror("write()");
		        error = ERROR;
		    }
		    else
		    {
		#ifdef PRINT_TCP_DATA_SENT
		    	/* printf */
		    	for(index = 0u; index < I_lenght; index++)
		    	{
		    		sprintf(byte_ascii, "%x ", I_bytes[index]);
		    		strcpy(&frame_sent[index_frame], byte_ascii);
		    		index_frame = strlen(frame_sent);
		    	}
		    	LOG_WriteLevel(LOG_DEBUG, "communication : TCP bytes sent: %s", frame_sent);
		#endif
		    }
			break;

		case UDP:
			if (sendto(I_emitter_id, I_bytes, I_lenght, 0u, (struct sockaddr*) I_parameters, lenght_param) == -1)
		    {
		        perror("\n\rsendto()");
		        error = ERROR;
		    }
		    else
		    {
		#ifdef PRINT_UDP_DATA_SENT
		    	/* printf */
		    	for(index = 0u; index < I_lenght; index++)
		    	{
		    		sprintf(byte_ascii, "%x ", I_bytes[index]);
		    		strcpy(&frame_sent[index_frame], byte_ascii);
		    		index_frame = strlen(frame_sent);
		    	}
		    	LOG_WriteLevel(LOG_DEBUG, "communication : UDP bytes sent to %s:%d: %s", 
		    					inet_ntoa(I_parameters->sin_addr), 
		    					(int)ntohs(I_parameters->sin_port),
		    					frame_sent);
		#endif
		    }
			break;
	}

    return(error);
}

/**
 * \fn 		T_reception_state socket_readPacket(int I_receiver_id, struct sockaddr_in* I_parameters, void* O_data, int I_lenght, T_state I_state)
 * \brief 	Read data from UDP/TCP communication
 *
 * \param 
 * \param 	 
 * \param 	O_data  		Expected data to be read
 * \param 	I_lenght  		Lenght of the expected data
 * \param 	I_state  		State of the reception
 * \return 					NO_PACKET_RECEIVED: No problem, PACKET_RECEIVED: Success, RECEPTION_ERROR: Something went wrong
 */
T_reception_state socket_readPacket(T_protocol I_protocol, int I_receiver_id, struct sockaddr_in* I_parameters, void* O_data, int I_lenght, T_state I_state)
{
	/* Declaration */
	socklen_t 			lenght_param 	= (socklen_t)sizeof(struct sockaddr_in);
	T_reception_state	state 			= RECEPTION_ERROR;
	int 				lenght_message;


	switch(I_protocol)
	{
		case TCP:
			lenght_message = read(I_receiver_id, O_data, I_lenght);
			break;

		case UDP:
			switch(I_state)
		    {
		    	case BLOCKING:
		    		lenght_message = recvfrom(I_receiver_id, O_data, I_lenght, 0u, (struct sockaddr*) I_parameters, &lenght_param);
		    		break;

		    	case NON_BLOCKING:
		    		lenght_message = recvfrom(I_receiver_id, O_data, I_lenght, MSG_DONTWAIT, (struct sockaddr*) I_parameters, &lenght_param);
		    		break;
		    }
			break;
	}
    
	if (lenght_message < 0)
	{
		state = NO_PACKET_RECEIVED;
	}
	else
	{
		if (lenght_message > 0)
		{
			state = PACKET_RECEIVED;
		}
		else
		{
			state = RECEPTION_ERROR;
		}
	}

	return(state);
}

/**
 * \fn 		void socket_close(T_socket* I_socket)
 * \brief 	Close a socket
 *
 * \param 	I_socket 	Socket structure to be closed  
 */
void socket_close(T_socket* I_socket)
{
	/* Close the socket */
	LOG_WriteLevel(LOG_INFO, "communication : closing socket %d", I_socket->id);
	close(I_socket->id);
	free(I_socket);
}
