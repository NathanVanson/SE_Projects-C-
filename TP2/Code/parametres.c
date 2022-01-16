#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <assert.h>
#include "parametres.h"

// fonction permettant de gerer les options passes sur le terminal. On agit avec les pointeurs

void options(int argc, char *argv[], int *is_file, char *hash_method[])
{
    int opt;

    assert(is_file);
    assert(hash_method);

// boucle while qui pour chaque parametres passes va remplacer les valeurs par defaut
// getopt va recuperer les caracteres et switch en fonction de l'argument passe (ici les options sont f et t que l'on a passe)
    while ((opt = getopt(argc, argv, "ft:")) != -1) {
        switch (opt) {
        case 'f':
            *is_file = 1; // cas ou on a fourni le flag f
            break;
        case 't':
            *hash_method = optarg ; // cas ou l'on doit changer le type de hash utilise
            break;
        default: 
            fprintf(stderr, "Usage: %s [-f] [-t hash name] \n",argv[0]); // cas ou les argument passe ne sont pas les bons
            exit(EXIT_FAILURE);
        }
    }
    // Cas ou aucun fichier ni chaine de caractere a ete trouve
   if (optind >= argc) {
        fprintf(stderr, "Pas de string ou fichier a hash trouve \n");
        exit(EXIT_FAILURE);
    }


}



