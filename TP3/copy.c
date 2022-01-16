#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#define BUFFERSIZE 1024

void CopyFiles(char* dir_src, char* dir_dest)
{
    DIR* dirSrc = opendir(dir_src); //On lit le fichier source
    if (dirSrc) //Le fichier de source lu est un répertoire
    {
        struct dirent* dirbase;
        while ((dirbase = readdir(dirSrc)) != NULL)
        {
            char* m = dirbase->d_name; //On récupère le nom du fichier
            if (strcmp(m, ".") != 0 && strcmp(m, "..") != 0) //On vérifie que le fichier en cours n'est pas . ou ..
            {
                char newpath[512];
                sprintf(newpath, "%s%s%s", dir_src, "/", m);
                CopyFiles(newpath, dir_dest); /* Appel Récursif de la fonction */
            }
        }

        if (closedir(dirSrc)) // On fini enfin par la fermeture du répertoire
        {
            fprintf(stderr, "Couldn't close '%s': %s\n", dir_src, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        int fileSrc = open(dir_src, O_RDONLY); //On lit le fichier source
        if (fileSrc == -1)
        {
            fprintf(stderr, "Can't open the file '%s': %s\n", dir_src, strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (dir_dest[0] == '/') //On lit le fichier destinataire
        {
            DIR* dirDest = opendir(dir_dest); //C'est un répertoire, on vérifie s'il existe
            if (!dirDest)
                mkdir(dir_dest, 0777);

            if (closedir(dirDest)) // On fini enfin par la fermeture du répertoire
            {
                fprintf(stderr, "Couldn't close '%s': %s\n", dir_dest, strerror(errno));
                exit(EXIT_FAILURE);
            }

            int i;
            for (i = 1; i <= strlen(dir_dest); i++) //On récupère les informations du fichier source pour les ajouter dans le fichier destinataire
            {
                dir_dest[(i - 1)] = dir_dest[i];
            }

            strcat(dir_dest, "/");
            strcat(dir_dest, dir_src);
        }

        int fileDest = open(dir_dest, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR); //On lit le fichier destinataire
        if (fileDest == -1)
        {
            fprintf(stderr, "Can't open the file '%s': %s\n", dir_dest, strerror(errno));
            exit(EXIT_FAILURE);
        }

        //On écrit dans le fichier destinataire
        int count;
        char buf[BUFFERSIZE];
        while ((count = read(fileSrc, buf, BUFFERSIZE)) > 0)
        {
            if (write(fileDest, buf, count) != count)
            {
                fprintf(stderr, "Error to write data in '%s': %s\n", dir_dest, strerror(errno));
                exit(EXIT_FAILURE);
            }
        }

        close(fileSrc);
        close(fileDest);
    }
}
