#!/bin/bash

#-------------------------------------------------#
#                  VARIABLES                      #                                         
#-------------------------------------------------#

rep_Stockage_Image=$1
rep_Writing_Image=$2
resolution_Image_Optionel=$3

#-------------------------------------------------#
#                  FONCTIONS                      #
#-------------------------------------------------#

# Vérification des arguments si on en a au moins deux.

function verfication() {

    if [ "$#" -lt 2 ] ; then
        echo "Le nombre d'arguments en entrée est insuffisant"
    else
        echo "Le nombre d'arguements en entrée est suffisant"
        exit
    fi

}

# Création d'un répertoire d'arrivée dans le cas ou ce dernier serait inexistant.

function rep_Arrivee() {
    
    if [ -e $rep_Writing_Image ] ; then 
        echo "Le répertoire existe bel et bien"
    else 
        echo "On crée un nouveau répertoire du nom de :"
        mkdir -p $rep_Writing_Image
    fi  

}

# Suppressions des espaces, d'apostrophes, ainsi que des guillemets des noms de fichier dans l'image d'arrivée.

function suppressions_Caractere() {
    
    special_caractere=']°,!?/.·"«»()[@…'
    mv $1 $rep_Stockage_Image/$(echo $(basename $1)| sed -r "s/['"${special_caractere}"']//g" ) #Va enveler tous les caractères spéciaux des noms de fichier dans l'image d'arrivée

}

# Arrivée de l'image en format PNG

function format_PNG() {

    if [[ $(file --mime-type -b $1) = image/* ]] ; then 
        if [ -z $resolution ] ; then
            convert $1 $rep_Stockage_Image/$(basename ${1%.*}).png
        else   
            convert -resize $resolution $1 $rep_Stockage_Image/$(basename ${1%.*}).png
        fi
    else 
        echo "'$1' ne correspond pas à une image"
    fi

}

#-------------------------------------------------#
#                     MAIN                        #
#-------------------------------------------------#

function script() {

    for file in "$rep_Stockage_Image"/* ; do 
    suppressions_Caractere "$file"
    format_PNG "$file"
    if [[ $file == *.png ]] ; then 
        cp $file $rep_Writing_Image 
    else
        echo "Le '$file' ne peut pas être copié"
    fi

done
}