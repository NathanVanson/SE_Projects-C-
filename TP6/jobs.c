#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <signal.h>  
#include "jobs.h"

pid_t fore_pid;
pid_t back_pid; 

// cette fonction va nous permettrre de gerer SIGINT et SIGHUP
void handler(int signum) {
    /* SIGINT sera redirige vers le processus principal, ce qui vas
    permettre à l'utilisateur d'utiliser CRTL + C pour arreter un job 
    principal sans stopper le shell*/
    if (signum == SIGINT) {    
        if(fore_pid > 0) { 
            kill(fore_pid, SIGINT); 
        } 
    } else if (signum == SIGHUP) { 
        /* Cette fois ci on va devoir terminer les processus en foreground 
        et en background. On va devoir donc check pour les 2 */
        if (fore_pid > 0) {
            kill(fore_pid, SIGHUP);
        } if (back_pid > 0) {
            kill(back_pid, SIGHUP);
        }
        exit(EXIT_SUCCESS);
    }
}

// cette fois ci, on va regarde lorsqu'on a le signal SIGCHLD (un pid enfant)
void child_handler(int signum, siginfo_t *siginfo, void* unused) {
    if (back_pid == siginfo->si_pid) {
        int status;
        waitpid(siginfo->si_pid, &status, 0);
        write(STDOUT_FILENO, "\nBackground Job Exited with Succes\n", 36);
        back_pid = 0;
    }
}

// on va maintenant creer une fonction qui s occupe de configurer les mask
void set_mask(void) {
    // on va gerer SIGCHLD
    struct sigaction death_child;
    memset(&death_child, 0, sizeof(death_child));
    death_child.sa_sigaction = child_handler;

    // le flag sa_SIGINFO permet d'utiliser sa_sigaction
    // comme un handler, death_child etant seulement pour SGCHLD
    death_child.sa_flags = SA_SIGINFO;
    sigaction(SIGCHLD, &death_child, NULL);

    // signa est utilise pour gerer SIGINT and SIGHUP 
    struct sigaction signa;
    memset(&signa, 0, sizeof(signa));
    signa.sa_handler = handler;
    signa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &signa, NULL);
    sigaction(SIGHUP, &signa, NULL);

    sigset_t mask_parent;
    sigemptyset(&mask_parent);
    sigaddset(&mask_parent, SIGTERM);
    sigaddset(&mask_parent, SIGQUIT);
    sigprocmask(SIG_BLOCK, &mask_parent, NULL);

}
