#include <stdio.h>
#include <stdlib.h>
#include "listing.h"
#include "copy.h"

int main(int argc, char *argv[])
{
    // Première partie du TP : listing de fichier d'un dossier ( contenu complet )
    if(argc !=3)
    {
        RecursiveFuncList(argv[1]); //Lire l'intégralité des fichiers dans un répertoire donné
    }
    else
    {
        CopyFiles(argv[1], argv[2]); //Copie d'un fichier vers un autre fichier
    }
}
