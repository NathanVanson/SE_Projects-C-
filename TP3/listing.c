#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include "listing.h"

void RecursiveFuncList(char* dir_entry)
{
    struct dirent* dirbase;
    DIR* dir = opendir(dir_entry);
    if (!dir) // Dans le cas ou une erreur se produit lors de l'ouverture
    {
        fprintf(stderr, "Can't open the directory '%s': %s\n", dir_entry, strerror(errno));
        exit(EXIT_FAILURE);
    }

    int stat(const char* path, struct stat* buf); //Appel de la méthode pour la référencer en mémoire
    struct stat infos;
    stat(dir_entry, &infos); //On récupère les informations du fichier en cours de lecture

    char roles[10];
    void lireDroitFichier(char* dir_entry, char rols[10]); //Appel de la méthode pour la référencer en mémoire
    lireDroitFichier(dir_entry, roles); //On récupère tous les droits du fichier

    printf("%s %d %s %s \n", roles, (int)infos.st_blksize, ctime(&infos.st_atime), dir_entry); //Affichage de toutes les informations du fichier (droits, taille, date d'accès, chemin)

    // Boucle concernant toute les information que l'énoncé demande pour avoir en output les infos demandés
    while ((dirbase = readdir(dir)) != NULL)
    {
        char* m = dirbase->d_name; //On récupère le nom du fichier
        if (strcmp(m, ".") != 0 && strcmp(m, "..") != 0) //On vérifie que le fichier en cours n'est pas . ou ..
        {
            char newpath[512];
            sprintf(newpath, "%s%s%s", dir_entry, "/", m); //On récupère le chemin correct du fichier en cours de lecture
            stat(newpath, &infos); //On récupère les informations du fichier en cours de lecture
            if (S_ISDIR(infos.st_mode))
            {
                //Le fichier en cours de lecture est un répertoire. On appelle la méthode en récursif
                sprintf(newpath, "%s%s%s", dir_entry, "/", m);
                RecursiveFuncList(newpath); /* Appel Récursif de la fonction */
            }
            else
            {
                lireDroitFichier(newpath, roles);  //On récupère tous les droits du fichier

                char pathFile[512];
                sprintf(pathFile, "%s%s%s", dir_entry, "/", m);
                printf("%s %d %s %s \n", roles, (int)infos.st_blksize, ctime(&infos.st_atime), pathFile); //Affichage de toutes les informations du fichier (droits, taille, date d'accès, chemin)
            }
        }
    }

    if (closedir(dir)) // On fini enfin par la fermeture du répertoire
    {
        fprintf(stderr, "Couldn't close '%s': %s\n", dir_entry, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

//Méthode permettant de lire les droits d'un fichier
void lireDroitFichier(char* dir_entry, char roles[10])
{
    struct stat role;

    int stat(const char* path, struct stat* buf); //Appel de la méthode pour la référencer en mémoire
    stat(dir_entry, &role);

    memset(roles, 0, 10); //Mise en mémoire de la variable roles

    //On vérifie quel type de fichier on est en train de lire
    if (S_ISREG(role.st_mode))
        roles[0] = '-';
    else if (S_ISDIR(role.st_mode))
        roles[0] = 'd';
    else if (S_ISLNK(role.st_mode))
        roles[0] = 'l';

    //On vérifie les droits pour ce fichier
    if (role.st_mode & S_IRUSR)
        strcat(roles, "r");
    else
        strcat(roles, "-");

    if (role.st_mode & S_IWUSR)
        strcat(roles, "w");
    else
        strcat(roles, "-");

    if (role.st_mode & S_IXUSR)
        strcat(roles, "x");
    else
        strcat(roles, "-");

    if (role.st_mode & S_IRGRP)
        strcat(roles, "r");
    else
        strcat(roles, "-");

    if (role.st_mode & S_IWGRP)
        strcat(roles, "w");
    else
        strcat(roles, "-");

    if (role.st_mode & S_IXGRP)
        strcat(roles, "x");
    else
        strcat(roles, "-");

    if (role.st_mode & S_IROTH)
        strcat(roles, "r");
    else
        strcat(roles, "-");

    if (role.st_mode & S_IWOTH)
        strcat(roles, "w");
    else
        strcat(roles, "-");

    //if (role.st_mode & S_IXOTH) strcat(roles, "x"); else strcat(roles, "-");
}
