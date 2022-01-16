#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <openssl/evp.h>
#include <sys/stat.h>
#include "calcul_hash.h"

// fonction pour le hashing d'un fichier (inspiré de man 3 EVP_Digest)

void Hash_File(const char *file_name, const char *hash_name)
 {
     // Creation de variable et ouverture du fichier
    FILE *file;
    file = fopen(file_name, "r");
    char *message = NULL;
    size_t len = 0;

    
    // creation d'un buffer permettant de savoir si le fichier souhaite existe ou non
    struct stat buffer;
    int exist = stat(file_name, &buffer);
    if(exist != 0) {
        printf("Le fichier %s n'est pas un fichier valide\n", file_name);
        exit(EXIT_FAILURE);
    }

    // Partie calcul du hash

    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len, i;

    md = EVP_get_digestbyname(hash_name);

    // conditions pour savoir si le type de hash demande est valide ou non
    if (!md) {
        printf("Le hash %s est inconnu\n", hash_name);
        exit(1);
    }else{ //Si le hash est valide, on calcule le hash demande
        mdctx = EVP_MD_CTX_new(); // on alloue et on renvoie un "context de digest"
        EVP_DigestInit_ex(mdctx, md, NULL); // on configure le context de digest à utiliser

        // boucle while pour update le hash de sorte a avoir un unique hash pour toutes les lignes du fichier
        while((getline(&message, &len, file)) != -1)
        {
            EVP_DigestUpdate(mdctx, message, strlen(message)); // on va hash le nb de octets de donnés dans le contex
        }
        
        // Une fois le hash calcule, on l'affiche sur le terminal
        EVP_DigestFinal_ex(mdctx, md_value, &md_len); // on recupere la valeur du digest de mdctx et la place dans md_value
        EVP_MD_CTX_free(mdctx);

        // on imprime la valeur du digest
        for (i = 0; i < md_len; i++)
            printf("%02x", md_value[i]);
            printf("\n");
        fclose(file);
    }
 
 }

// Fonction pour calculer le hash d'une chaine de caractere
// cette fonction marche quasiment de la meme facon que la fonction precedente
void Hash_String(int argc, char *argv[], const char *hash_name)
 {
     EVP_MD_CTX *mdctx;
     const EVP_MD *md;
     unsigned char md_value[EVP_MAX_MD_SIZE];
     unsigned int md_len, i;
     
    
    // meme chose que pour la fonction au dessus
     md = EVP_get_digestbyname(hash_name);
     if (!md) {
         printf("Le hash %s est inconnu\n", hash_name);
         exit(1);
     }

     mdctx = EVP_MD_CTX_new(); // on initialise
     EVP_DigestInit_ex(mdctx, md, NULL);

    // Boucle while pour update le hash pour chaque chaine de caractere
    while(optind < argc)
    {
        EVP_DigestUpdate(mdctx, argv[optind], strlen(argv[optind]));

        // pour ne pas considerer le dernier espace du saut de ligne avoir le bon hash
        if(optind != argc -1){
            EVP_DigestUpdate(mdctx, " ", strlen(" ")); 
        }
        
        printf("%s ", argv[optind]);
        optind++;

    }
    // meme chose que pour la fonction au dessus
    
     EVP_DigestFinal_ex(mdctx, md_value, &md_len);

     EVP_MD_CTX_free(mdctx); // on libere la memoire utilise

     printf("Le hash correspond a: ");
     for (i = 0; i < md_len; i++)
         printf("%02x", md_value[i]);
     printf("\n");

     exit(0);
 }    