#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

// on definit des valeurs d'entier non signes pour l'echange entre serveur et client
#define TOO_LOW 1
#define TOO_HIGH 2
#define WIN 3
#define LOSE 4
#define NB_ESSAIS 10

unsigned char nb_aleatoire(unsigned char min, unsigned char max); // prototype de notre fonction

int main(int argc, char *argv[]) {
    struct sockaddr_in address; // structure du server

    unsigned char min = 0;
    unsigned char max = 100;

    // on verifie l entree de l utilisateur pour que le port puisse etre reelemment utilise
    int port = strtol(argv[1], NULL, 10);
    if (port < 1024 || port > 65535) {
        fprintf(stderr, "Chosen port is out of range (try between 1024 and 65535)\n");
        exit(EXIT_FAILURE); // on sort avec une erreur
    }


    memset(&address, 0, sizeof(address)); // on initialise l'adresse
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    // Creation du socket, et on verifie que sa creation n as pas eu d erreur (socket cree en IPv4)
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSock == -1){
        printf("Error during the creation of the socket\n");
        exit(EXIT_FAILURE); // si erreur, on quitte le programme avec une failure
    }

    // on attribue une adresse IP au serveur
    int lier = bind(serverSock, (struct sockaddr*) &address, sizeof(address));
    if(lier == -1){ // on verifie que l adresse Ip utilise n'est pas deja utilise par un autre processus et/ou ne peux pas etre utilise
        fprintf(stderr, "Couldn't bind socket to adress: %s\n", strerror(errno));
    } 

    printf("Server listening on port %i \n", port);

    // Notre serveur pourra au maximum ecouter 5 utilisateur
    int queue = 4; // avec une queue, notre server est de type passif
    int ecoute = listen(serverSock, queue);
    if(ecoute == -1){
        printf("Error during the listening\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        struct sockaddr_in clientAddress; // Initialisation du client
        unsigned int clientLength = sizeof(clientAddress);
        int clientSock = accept(serverSock, (struct sockaddr *) &clientAddress, &clientLength); // on accepte la connection

        printf("Client %d with adress %s:%d is connected\n", clientSock, inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

        // on va utiliser fork() pour creer un nouveau processus a chaque fois qu'un nouveau client se connecte
        // on va veiller aussi a ce qu on reste pas avec des processus zombies

        pid_t pid = fork(); // pid_t se trouve dans l include sys/types.h

        if ( pid > 0) {
            // Code du Parent
            waitpid(pid, NULL, 0); // l'execution va etre suspendue jusqu'à ce que l'enfant se termine
        } else if (pid == 0) { 
            // Code de l'enfant
            // on va creer un nouveau processus avec fork() pour eviter les processus zombies
            pid_t pid2 = fork();
            if (pid2 == 0) {
                // on peux maintenant jouer !!

                unsigned char random_number = nb_aleatoire(min, max); // creation du nombre aleatoire
                printf("Selected value for client %d: %u\n", clientSock, random_number); 

                // on va communiquer le maximum et le minimum
                write(clientSock, &min, 1);
                write(clientSock, &max, 1);

                unsigned char essais = 0; // on initialise le nombre d'essais pour pouvoir les incrementer plus tard
                unsigned char cmd;

                while(1){
                    // Si on a fait trop d'essais
                    if(essais > 10) {
                        cmd = LOSE; // le client a perdu
                        write(clientSock, &cmd, 1); // on notifie le client
                        write(clientSock, &random_number, 1);
                        printf("Client %d lost\n", clientSock);
                        exit(EXIT_SUCCESS); 
                    }

                    // on va lire l'input de l utilisateur
                    unsigned char input;
                    read(clientSock, NULL, 1); // on a pas besoin du premier octet, on vas tout simplement l'ignorer
                    read(clientSock, &input, 1); //on a ici la reponse

                    printf("Client %d proposes %u\n", clientSock, input);

                    
                    if (input == random_number){ // Si le client a devine
                        printf("Client %d wins!\n", clientSock); 
                        cmd = WIN; // le client a gagne
                        write(clientSock, &cmd, 1); // on lui dit qu'il a gagne
                        write(clientSock, &random_number, 1);
                        exit(EXIT_SUCCESS);
                    }else{
                        essais = essais + 1; // on incremente le nombre d'essais
                        if(input > random_number) { // Si l'input est trop grand
                            cmd = TOO_HIGH;
                            write(clientSock, &cmd, 1);
                            write(clientSock, &random_number, 1);
                        }else { // si c est trop petit
                            cmd = TOO_LOW; 
                            write(clientSock, &cmd, 1);
                            write(clientSock, &random_number, 1);
                        }
                    }
                }

            } exit(0); // on pense bien à terminer l'enfant (cf: correction faite car avant l'enfant)
                       // agissait comme le processus principal et les zombies etaient mal gerés
        }else {
            fprintf(stderr, "Error during the creation of the process: %s", strerror(errno)); // cas ou une erreur a eu lieu
        }
    }
    

    return 0;

}

/* J'utilise rand car j'ai eu des probleme avec dev/urandom */
// J'ai donc prefere une fonction qui cree des nombre aleatoire plutot qu'elle aille les chercher
unsigned char nb_aleatoire(unsigned char min, unsigned char max){
    srand(time(0));

    int num = (rand() % (max - min + 1)) + min; // on genere un nombre dans notre interval

    return num;
}

