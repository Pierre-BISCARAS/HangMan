#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


/*
Permet de vérifier si une lettre a déjà été utlisée.
1 : Vrai
0 : Faux
*/
int letterAlreadyUsed(char letter, char usedLetter[26]){
    int i;
    for (i = 0; i < 26; i++) {
        if (usedLetter[i] == letter) {
            return 1;
        }
    }
    return 0;
}

/*
Permet de vérifier si le joueur a gagné.
1 : Vrai
0 : Faux (La partie n'est pas terminée)
-1 : Le joueur a perdu
*/
int didPlayerWin(int life, int maxLife, char *word, char usedLetter[26]){
    int counter = 0;
    if (life >= maxLife) // Le joueur n'a plus de vie
    {
        return -1;
    }
    for (int i = 0; i < strlen(word); i++)
    {
        if (letterAlreadyUsed(word[i],usedLetter))
        {
            counter++;
        }
        
    }
    if (counter == (int) strlen(word))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
Permet l'affichage de la vie restante
*/
void displayLife(int life, int maxLife){
    printf("\n");
    printf("[ ");
    for (int i = 0; i < life; i++){
        printf("X ");
    }
    for (int i = 0; i < (maxLife - life); i++){
        printf("O ");
    }
    printf("]");
}

/*
Permet l'affichage des lettres déjà utilisées
*/
void displayUsedLetter(char usedLetter[26]){
    printf("\n\nLettre déjà utilisée : ");
    for (int i = 0; i < 26; i++)
    {
        if (usedLetter[i] != '\0')
        {
            printf("%c, ", usedLetter[i]);
        }
    }
    printf("\n");
}

/*
Ajoute une lettre dans le tableau des lettres utilisées et incrémente l'index (numberOfUsedLetter)
*/
void addLetterToUsedLetter(char usedLetter[26], char letter, int *index){
    usedLetter[*index] = letter;
    *index += 1;
}

/*
Permet d'afficher le mot, un '_' si la lettre n'a pas encore été utilisée sinon la lettre
*/
void display(char *word, char usedLetter[26]){
    int isUsed;
    printf("\n\n");

    // Parcours de chaque lettre du mot et si elle est dans la liste des lettres utilisées alors l'afficher
    for (int i = 0; i < strlen(word); i++)
    {
        isUsed = letterAlreadyUsed(word[i], usedLetter);
        if (isUsed == 1)
        {
            printf("%c ",word[i]);
        } else {
            printf("_ ");
        }
    }
    printf("\n\n");
}

/*
Permet de stocker dans un tableau un message formaté comme avec la fonction display()
*/
char* prepareToSend(char *word, char usedLetter[26],char allLetters[26], char letter, int *numberOfUsedLetter, int try,int current_life, int maxLife, int* end){
    char *toSend = (char *)malloc(100 * sizeof(char));
    char* message;
    memset(toSend, 0, 100 * sizeof(char));
    sprintf(toSend,"\nNombre de d'essais restants : %i\nMot a deviner :",(maxLife - current_life));
    if (try == 0)
    {
        for (int i = 0; i < strlen(word); i++)
            {
                if(letterAlreadyUsed(word[i],usedLetter)==1){
                    toSend[strlen(toSend)] = word[i];
                    strcat(toSend," ");
                } else {
                    strcat(toSend,"_ ");
                }
            }
    }
    else
    {
        switch (letterAlreadyUsed(letter, usedLetter)){
        case 1:
            return ("\nLa lettre a déjà été utilisée");
        case 0:
            if (strchr(allLetters,letter)==NULL)
            {
                return ("\nLe caractère n'est pas utilisable");
            }
            addLetterToUsedLetter(usedLetter,letter,numberOfUsedLetter);
            for (int i = 0; i < strlen(word); i++)
            {
                if(letterAlreadyUsed(word[i],usedLetter)==1){
                    toSend[strlen(toSend)] = word[i];
                    strcat(toSend," ");
                } else {
                    strcat(toSend,"_ ");
                }
            }
        } 
    }
    if (didPlayerWin(current_life, maxLife, word, usedLetter)==1)
    {
        message = "\nVous avez gagné ! \nLe mot était :";
        char *target = (char *)malloc(100 * sizeof(char));

        sprintf(target, "%s %s", message, word);

        *end=1;

        return(target);
    } else if (didPlayerWin(current_life, maxLife, word, usedLetter)==-1)
    {

        message = "\nDommage vous avez perdu :( \nLe mot était :";
        char *target = (char *)malloc(100 * sizeof(char));

        sprintf(target, "%s %s", message, word);

        *end=1;

        return(target);
    }

    return toSend;
}
