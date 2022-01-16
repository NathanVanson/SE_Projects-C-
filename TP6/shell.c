#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <syscall.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include "builtin.h"
#include "jobs.h"

extern pid_t fore_pid, back_pid;

int main(int argc, char* argv[]){
    if(argc != 1) {
        fprintf(stderr, "Too much arguments: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    while(1){

        // on va set les masks pour que les signaux soit bien geres
        set_mask();

        // Affichage du shell avec la directory et le username (utilisation des couleurs presente dans stdio.h)

        printf("\033[0;33m"); // on met le user en jaune pour differencier du shell predefini de Unix
        printf("%s@%s: ", getenv("USER"), getenv("HOSTNAME")); // On affiche le username 
        printf("\033[0;36m"); // On prend du cyan pour differencier le directory du username
        printf("%s", getcwd(NULL, 0)); // affiche le username
        printf("\033[0;36m");
        printf("$ ");
        printf("\033[0m"); // On reset la couleur a blanc pour que l'input utilisateur soit blanc
        fflush(stdout);
 
        char *input = malloc(1024); // on alloue de la memoire à l'input
        fgets(input, 1024, stdin); // on recupere l'entree utilisateur

        input[strcspn(input, "\n")] = 0; // on enleve le saut de ligne pour que l'input soit la commande voulue

        // On vas faire le parsing pour pouvoir faire fonctionner nos fonctions builtin 
        // et pour que le shell comprenne quelle sont les commandes tapes
        int argcount = 0;
        char * _argv[20] = {};

        char * token = strtok(input, " ");

        while(token != NULL) {
            _argv[argcount] = malloc(sizeof(char)*strlen(token));
            strcpy(_argv[argcount], token); 

            token = strtok(NULL, " ");
            argcount += 1;
        }

        // on verifie l'entree utilisateur (cf correction pour eviter de segfault si l'utilisateur entre une ligne vide)
        if (argcount == 0) {continue;}

        /* Une fois que l'input a ete parse, on peux maintenant utiliser nos
        fonctions de builtin à l'aide de le function strcmp, qui va comparer 
        les deux string */
        
        if(strcmp(_argv[0], "cd") == 0){ // cas ou l'on veux changer de repertoire
            cd_func(_argv[1]);
        }else if (strcmp(_argv[0], "exit") == 0){ // cas ou l'on veux quitter le shell
            exit_shell();
        } else {
            // cas ou on doit executer un programme en fond
            int background = 0; // cette variable va nous permettre de savoir si des programme s'executent en fond
            if(_argv[argcount - 1][0] == '&') {
                // on enleve le &
                free(_argv[argcount -1]);
                _argv[argcount - 1] = NULL;
                argcount = argcount -1;
                background = 1; // on dit que la tache a faire est en background
            }
            // si c'est pas un builtin, il faut fork et l'enfant va pouvoir executer le programme
            pid_t pid = fork();
            // on check qu on est bien dans l'enfant
            if (pid == 0) {
                // on va configurer l'enfant afin qu'il ignore SIGINT 
                // (si c'est une background task)
                if(background == 1){
                    task_background_config();
                }

                // si c'est pas une tache en background, on utilise execvp
                int res = execvp(_argv[0], _argv);
                if (res == -1){
                    fprintf(stderr, "%s: command not found\n", _argv[0]);
                    exit(EXIT_FAILURE);
                }else{
                    exit(EXIT_SUCCESS);
                }


            } else if (pid > 0){ // parent
                if(background == 0) {
                    fore_pid = pid;
                    int status = 0; // on check le satus du programme a sa sortie

                    int res = waitpid(pid, &status, 0);

                    if(res > 0) { // si cela est vrai, cela signifie qu'on a bien termine l'enfant
                        if (WIFSIGNALED(status)){
                            printf("Foreground terminated by signal \n");

                        printf("Foreground job exited with code %d\n", status);
                        fore_pid = 0;
                        continue;
                        }
                    // sinon si waitpid n as pas marche, on recommence
                    }else if (res == -1) {
                        printf("Error during exit of foreground job (status: %d) \n", status);
                    }

                }else if (background == 1) { // si c'est une tache en background
                    back_pid = pid;
                }

            } else {
                fprintf(stderr, "Couldn't execute processus");
                exit(EXIT_FAILURE);
            }

        }

       free(input); // on oublie pas de liberer le memoire (cf correction qui prendrait beaucoup de memoire en reserve a long terme)
        
    }
    
    
}