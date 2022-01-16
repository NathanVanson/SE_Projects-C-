#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h> 
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>


int main(int argc, char *argv[]){

    const char * name = "shared_memory";
    const char * smph1 = "fill"; // ce semaphore est utilisé lorsque l'etagere est pleine
    const char * smph2 = "available"; // ce semaphore est utilisé lorsque il y a de la place sur l'etagere
    const char * smph3 = "mutex"; 

    int shm_fd; // descripteur de fichier de la memoire partagée
    int * shelf; // initialisation de l'etagere
    int loop = 10; // loop est a 10 car c'est une boucle qui va etre repete 10 fois (= le nombre de pizza)

    sem_t * fill, * available, * mutex; // initialisation des variables dans la struct sem_t

    // on va créer un segment de memoire partagee
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if(shm_fd == -1){
        fprintf(stderr, "Error during the creation of the shared memory: %s\n", strerror(errno));
    }

    // on va configurer la taille du segment de memoire partagée
    if((ftruncate(shm_fd, sizeof(int))) == -1) {
        fprintf(stderr, "Error when configuring shared memory size: %s\n", strerror(errno));
    }

    // on va mapper le segment de memoire partagee (etagere) dans l'espace d'adresse du processus
    shelf = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shelf == MAP_FAILED){
        fprintf(stderr, "Error during the mapping of the memory: %s\n", strerror(errno));
    }

    // Creation/Ouverture des semaphores
    // Apres la cuisson d'une pizza
    fill = sem_open(smph1, O_CREAT, 0666, 0); // man sem_open

    // Apres que le serveur a pris une pizza sur l'etagere, au debut on initialise le fait qu'il y a 3 pizza sur l'etagere
    available = sem_open(smph2, O_CREAT, 0666, 3);

    mutex = sem_open(smph3, O_CREAT, 0666, 1);
    printf("\nit's time to start cooking pizza! \n");

    while(loop--){
        sem_wait(available); // on lock le semaphore de available (man sem_wait)
        sleep(rand()%2+1); // on prend un temps aleatoire pour cuisiner une pizza
        sem_wait(mutex); // on lock le semaphore de mutex
        (*shelf)++; // on incremente le nombre de pizza sur l'etagere
        sem_post(mutex); // on unlock le semaphore de mutex (man sem post)
        printf("Cook: I finished cooking a pizza, there are now %d pizza on the shelf\n", * shelf);
        printf("Cook: %d pizzas left to cook\n", loop);
        sem_post(fill); // on unlock le semaphore de fill

        // Condition si y a 3 pizza sur l'etagere
        if(*shelf == 3){
            printf("Cook: Time to take a break, the shelf is full\n");
        }
    }
    printf("Cook: All pizza are done. %d are left on the shelf.\n", *shelf);

    // on va fermer et unlink tous les semaphores
    sem_close(fill);
    sem_close(available);
    sem_close(mutex);
    sem_unlink(smph1);
    sem_unlink(smph2);
    sem_unlink(smph3);

    // on va fermer et unlink la memoire partagée
    munmap(shelf, sizeof(int));
    close(shm_fd);
    shm_unlink(name);
    return 0;
}
