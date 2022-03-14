#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>	//strlen
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr

#define MAX_BUFFER 20000

char sendM[MAX_BUFFER];

char client_message[MAX_BUFFER];

int enviar(char* men) {
	int sock;
	struct sockaddr_in server;
	char message[1000] , server_reply[2000];
	
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1) {
		printf("Could not create socket");
	}
	puts("Socket created");
	
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(9999);

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
		perror("connect failed. Error");
		return 1;
	}
	
	puts("Connected\n");
	
    
	//printf("MENSAJE: %s", men);

    //Send some data
    if( send(sock , men , MAX_BUFFER , 0) < 0) {
        puts("Send failed");
        return 1;
    } else {
        puts("send ok");
    }
		

	
	close(sock);
	return 0;
}


int recieve_info(){
    int socket_desc, client_sock, c, read_size;
	struct sockaddr_in server, client;  // https://github.com/torvalds/linux/blob/master/tools/include/uapi/linux/in.h
	
	
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1) {
		printf("Could not create socket");
	}
	puts("Socket created");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);
	
	//Bind the socket to the address and port number specified
	if( bind(socket_desc, (struct sockaddr *)&server , sizeof(server)) < 0) {
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");
	
	// Listen
    // It puts the server socket in a passive mode, where it waits for the client 
    // to approach the server to make a connection. The backlog, defines the maximum 
    // length to which the queue of pending connections for sockfd may grow. If a connection 
    // request arrives when the queue is full, the client may receive an error with an 
    // indication of ECONNREFUSED.
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	
	//accept connection from an incoming client
	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	if (client_sock < 0) {
		perror("accept failed");
		return 0;
	}
	puts("Connection accepted");
	

	memset (client_message, 0, 2000);
	//Receive a message from client
	if (recv(client_sock , client_message , 2000 , 0) > 0) {
		printf("received message: %s\n", client_message);
		//Send the message back to client
		send(client_sock , client_message , strlen(client_message), 0);
	} else {
		puts("recv failed");
		return 0;
	}

	return 1;

}

char* listarContenedores(){
    FILE *fp;
    int status;
    char path[MAX_BUFFER];
    static char conca[MAX_BUFFER];
    fp = popen("docker ps -a", "r");

    while (fgets(path, MAX_BUFFER, fp) != NULL)
        strcat(conca, path);
        // printf("%s", path);
    // printf("%s", conca);

    return conca;
    //execlp("docker", "docker", "ps", "-a", NULL);
}

void CrearContenedor(char* sistema){
    //execlp("docker", "docker", "run", "-ti", sistema, "/bin/bash", NULL);
    execlp("docker", "docker", "create", sistema, NULL);
}

void BorrarContenedor(char* sistema){
    //execlp("docker", "docker", "run", "-ti", sistema, "/bin/bash", NULL);
    execlp("docker", "docker", "rm", "-f", sistema, NULL);
}

void PararContenedor(char* sistema){
    //execlp("docker", "docker", "run", "-ti", sistema, "/bin/bash", NULL);
    execlp("docker", "docker", "stop", sistema, NULL);
}

int main()
{
    pid_t pid;

    int p[2];

    pipe(p);


    /* fork a child process */
    pid = fork ();
    if (pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed\n");
        return 1;
    }
    else if (pid == 0) { /* admin_container */

        /* Aca recibimos la info del cliente. 
        Un Int op que es las opcion que desea realizar:

            op = 1: crear contenedor
            op = 2: Listar contenedores
            op = 3: borrar contenedor
            op = 4: detener contenedor

        Y se recibe un char* param donde esta la info 
        para las opciones( el contenedor a crear, eliminar o detener)
        */
        
        char* test;
        test = malloc(MAX_BUFFER);
        int rec = recieve_info();

        if (rec == 1){
            test = client_message;

            char* token1 = strtok(test, " ");
            //printf("%s\n", token1);
            char* token2 = strtok(NULL, " ");
            //printf("%s\n", token2);
 
            
            write( p[1], test, strlen( test ) );
            close(p[1]);

            switch (token1[0])
            {
            case '1':
                //Crear
                CrearContenedor(token2);
                break;
            case '2':;
                //listar
                //*sendM = ;
                char* temp = listarContenedores();
                enviar(temp);
                break;
            case '3':
                //borrar
                BorrarContenedor(token2);
                break;
            case '4':
                //Parar
                PararContenedor(token2);
                break;
            default:
                break;
            }


            
        }else{
            printf("Algo Falla en el socket");
        }
        

    }
    else { /* Parent subscribe_host */
        /* 
        
        parent will wait for the child to complete 
        El subscribe_host, es un proceso encargado de recibir peticiones desde los hosts y lleva
        un control de los hosts disponibles para crear contenedores. Solo los agentes que
        corren en el host interactuan con el proceso subscribe_host.
        
        */
        char buffer[MAX_BUFFER];
        read( p[0], buffer, MAX_BUFFER );
        // printf(" %s", buffer);
        close(p[0]);
        wait(NULL);
        printf("Child Complete\n");
    }
}