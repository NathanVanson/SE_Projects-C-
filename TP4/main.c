/* THIS PROGRAM ALLOWS TO LOCK/UNCLOCK A
   PART OF A FILE IN AN INTERACTIVE WAY. */

// Libraries

#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// °-°-°-°-° Function main °-°-°-°-°

int main(int argc, char* argv[]) {

    if (argc !=2) { // Nous permet au moins de vérifier si le nombre correct d'argument a été donnée ( ici le nom du fichier ).
        fprintf(stderr, "There are not enough arguments: It should be './verrou [file name]'");
        exit(EXIT_FAILURE);
    }

    struct flock fl; // Structure flock en prenant fl comme indicateur de variable pour tout ce que contient la structure.

        // Création et définition de toute les variables nécessaires pour le code.
        char input[50];
        int cmd, start, lenght;
        int input_start, input_lenght;
        char input_cmd, input_type;
        char input_whence = 's';
        
        // Afin d'éviter l'intéraction avec un fichier non existant
        int fd = open(argv[1],O_RDWR); // -> Ouverture du fichier.
        if(fd < 0) { // -> Cas : le fichier ne peut pas s'ouvrir.
            fprintf(stderr, "Can't open the file '%s': %s\n", argv[1], strerror(errno));
            exit(EXIT_FAILURE); // => cas d'échec.
        }

    for(;;) { /* Demander une commande de verrouillage et l'exécuter */

        /* Apparence physique du programme : Soit on tappe '?' puis on recoit les informations nécessaire pour continuer
        Soit on tappe 'exit' pour quitter le programme. */
        
        printf("Enter ? for help\n");
        printf("Enter exit to exit the program\n");

        printf("PID=%ld> ", (long) getpid());
        fflush(stdout); // Clear la sortie standard et forcer à écrire le contenu du buffer.

        fgets(input, sizeof(input), stdin); // Permet de lire l'entrée de l'utilisateur et ensuite la traiter.
        if (strcmp(input, "?\n") == 0) { // Affichage de l'interface d'aide si l'utilisateur tappe "?" => Comparaison d'input.

            printf("\n");
            printf("    Format : cmd l_type start length [ whence ( optional ) ]\n");
            printf("    'cmd ' --- 'g ' ( F_GETLK ) , 's ' ( F_SETLK ) , or 'w ' ( F_SETLKW )\n"); 
            printf("    'l_type ' --- 'r ' ( F_RDLCK ) , 'w ' ( F_WRLCK ) , or 'u ' ( F_UNLCK )\n");
            printf("    'start ' --- lock starting offset\n"); 
            printf("    'length ' --- number of bytes to lock\n");
            printf("    'whence ' --- 's' ( SEEK_SET , default ) , 'c' ( SEEK_CUR ) , or 'e' ( SEEK_END ) \n");
            printf("\n");

        } else if(strcmp(input,"exit\n") == 0) {

            close(fd);
            exit(0);

        } else {
            sscanf(input, "%c %c %d %d %c", &input_cmd, &input_type, &input_start, &input_lenght, &input_whence); // Permet de séparer l'input de l'utilisateur.
        }

        // -> Conversion de la commande 'cmd' en flag :
        switch(input_cmd) {
            case 'g':
                cmd = F_GETLK;
                break;
            case 's':
                cmd = F_SETLK;
                break;
            case 'w':
                cmd = F_SETLKW;
                break;
            default:
                break;
        }

        // Conversion de l'input 'type' en flag :
        switch(input_type) {
            case 'w':
                fl.l_type = F_WRLCK;
                break;
            case 'r':
                fl.l_type = F_RDLCK;
                break;
            case 'u':
                fl.l_type = F_UNLCK;
            default:
                break;
        }

        // On a pas besoin de convertir ces deux-là car ils se trouvent déjà dans le bon format => On a pas besoin d'utiliser un "switch case".
        fl.l_start = input_start;
        fl.l_len = input_lenght;

        // Conversion de l'input 'whence' en flag :
        switch(input_whence) {
            case 's':
                fl.l_whence = SEEK_SET;
                break;
            case 'c':
                fl.l_whence = SEEK_CUR;
            case 'e':
                fl.l_whence = SEEK_END;
                break;
            default:
                fl.l_whence = SEEK_SET; // Paramètres par défaut
                break;
        }

        int status = fcntl(fd, cmd, &fl); /* Perform request... */
        
        // Récupérer l'information du verrou.
        if (cmd == F_GETLK) { /* F_GETLK */

            // Vérifier l'état et traiter les erreurs.
            if(status == 0) {
                // Traiter les résultats et imprimer un texte informatif.
                if(fl.l_type == F_UNLCK) { // fcntl va retourner F_UNLCK si le verrou est placable.
                    printf("[PID=%ld] lock obtained \n", (long) getpid());
                } else if (fl.l_type == F_RDLCK) { // Si on a placé un readlock :
                    printf("[PID=%ld] Permissions denied by READ ('r') lock on %lu:%lu (held by PID %d)\n", (long)  getpid(), fl.l_start, fl.l_len, fl.l_pid);
                } else if (fl.l_type == F_WRLCK) { // Si on a placé un writelock :
                    printf("[PID=%ld] Permissions denied by WRITE ('w') lock on %lu:%lu (held by PID %d)\n", (long)  getpid(), fl.l_start, fl.l_len, fl.l_pid);
                }

            } else if(errno == EINVAL) { // Cas d'erreur .
                // Traiter les résultats et imprimer un texte informatif
                fprintf(stderr, "Can't get lock: %s\n", strerror(errno));
            }

        } else { /* F_SETLK, F_SETLKW */
            // Vérifier l'état et traiter les erreurs.
            if(status == 0) {
                // Traiter les résultats et imprimer un texte informatif
                if(fl.l_type == F_UNLCK) {

                    printf("[PID=%ld] unlocked : %s\n", (long)  getpid(), argv[1]); // Unlocked
                } else {

                    printf("[PID=%ld] locked : %s\n", (long)  getpid(), argv[1]); // Locked
                }
            } else if (errno == EACCES || errno == EAGAIN) { // Cas d'erreur 
                fprintf(stderr, "Couldn't access to the lock: %s\n", strerror(errno));
            }
        }
    }

    return 0;

}

/* Si on place un verrou sur un fichier dans un premier processus ouvert,
alors en ouvrant un deuxième processus, si on tappe les même paramètres qu'au 
processus précédent alors la sortie standard va nous afficher que le fichier en
question a déjà été lock. */
