#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <syscall.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include "builtin.h"

/* Cette fonction va prendre le path et, avec chdir(), si l'input
est "cd", va changer le repertoire courant */

void cd_func(char* path) {
    int cd_res = chdir(path); // on change le repertoire
    
    /* chdir() retourne -1 en cas d'echec, on va bien tester qu'il n y a donc pas d'echec
    et si oui quelle est l'erreur */

    if (cd_res == -1){
        printf("%s\n", strerror(errno));
    }
}

/* Cette fonction va nous permettre de quitter notre shell
Comme c'est un processus, il suffit de communiquer au pid que le processus 
doit se terminer */

void exit_shell(void) {
    int pid = getpid(); // on recupere le pid
    
    printf("exiting shell...\n");
    kill(pid, SIGHUP); // SIGHUP va terminera tous les processus en background
}

/* on va creer une fonction qui configure le mask de l'enfant
et qui vas rediriger sa sortie standard a lire vers /dev/null, afin d'eviter
d'eventuel conflits avec le shell. Il va devoir ignorer SIGINT. */

void task_background_config() {
    int redirect = open("/dev/null", O_RDWR);
    // creer une nouvelle variable pour eviter conflit avec shell
    // dup2 va dupliquer notre descirpteur de fichier

    int duplicate = dup2(redirect, STDIN_FILENO);
    printf("%d", duplicate);
    close(redirect);

    // on va configurer les signaux a masquer chez l'enfant
    // pour qu il ignore SIGINT

    sigset_t mask_enfant;
    sigemptyset(&mask_enfant);
    sigaddset(&mask_enfant, SIGINT);
    sigprocmask(SIG_BLOCK, &mask_enfant, NULL);
}