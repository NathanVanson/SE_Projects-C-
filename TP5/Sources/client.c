#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define TOO_LOW 1
#define TOO_HIGH 2
#define WIN 3
#define LOSE 4

int main(int argc, char *argv[]) {
    
    int port = strtol(argv[2], NULL, 10);

    struct sockaddr_in address; // structure de l'adresse du client

    // on va checker si l'adresse IP est valide
    memset( &address, 0, sizeof(address)); // On initialise l'adresse

    int adresse_ip = inet_pton(AF_INET, "127.0.0.1", &(address.sin_addr));
    if( adresse_ip <= 0){
        fprintf(stderr, "Couldn't use IP adress: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    // Creation du socket client
    int client = socket(AF_INET, SOCK_STREAM, 0);
    // on test si la creation a bien marche
    if(client == -1) {
        printf("Error during the creation of the Client Socket \n");
        printf("%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // On connecte le client et on verifie qu'il n y a pas eu d'erreur pendant la creation
    int connection = connect(client, (struct sockaddr *) &address, sizeof(address));
    if(connection == -1){
        printf("Connection Error \n");
        printf("%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // On fait un premier envoi contentant le minimum et le maximum du nombre a deviner
    unsigned char min, max;
    // On va lire le min et le max, chacun sur un octet
    read(client, &min, 1);
    read(client, &max, 1);
    printf("Time to play! Guess a number between %u and %u\n", min,max);

    // creation d'une boucle qui agit tant que le client n'as pas trouve le nombre ou a atteint le nombre d'essais max
    while(1) {
        int input;

        scanf("%d", &input); // On recupere la valeur que l'utilisateur a tape
        unsigned char input_mod;
        input_mod = (unsigned char) input; // si le nombre est trop grand, on vas faire un modulo
        write(client, NULL, 1); // le premier octet est ignore
        write(client, &input_mod, 1); // cet octet contient  la proposition du client
        printf("Sending Proposition: %d\n", input_mod);

        // on recupere la reponse du server
        unsigned char cmd, random_number;
        read(client, &cmd, 1);
        read(client, &random_number, 1);

        if (cmd == WIN) { // cas ou le client a devine
            printf("I won!\n");
            close(client); // on ferme le client
            break;
        }else{
            if (cmd == LOSE){ // Si le client a depassse le nombre max d essais et n as pas trouve le bon nombre
                printf("Answer was %u. You lost\n", random_number);
                close(client);
                exit(EXIT_SUCCESS);
            }else if (cmd == TOO_LOW){ // Si le nombre est plus petit
                printf("Too low\n");
            }else if (cmd == TOO_HIGH){ // si le nombre est plus grand
                printf("Too high\n");
            }
        }
    }
    return 0;

}
