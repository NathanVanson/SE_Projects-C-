#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <assert.h>
#include "parametres.h"
#include "calcul_hash.h"

// fonction main qui agit en fonction de ce que notre utilisateur veut

int main(int argc, char *argv[]){
    int is_file = 0;
    char *hash_method = "sha1";

    options(argc, argv, &is_file, &hash_method);
    printf("%s , %d\n", hash_method, is_file);

    //on check si c est un fichier
    if(is_file){
        // boucle qui repete l'action pour chaque fichier demande (au cas ou plusieurs fichier ont ete demande)
        while(optind < argc){
            Hash_File(argv[optind], hash_method);
            optind++; 
        }
        
    }else{ // sinon on utilise la fonction qui nous permet de calculer des strings
        Hash_String(argc, argv, hash_method);
    }

    
}



