#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
Permet de vérifier si une lettre a déjà été utlisée.
1 : Vrai
0 : Faux
*/
int letterAlreadyUsed(char letter, char *usedLetter[26]){
    int i;
    for (i = 0; i < 26; i++) {
        if (usedLetter[i] != NULL && *usedLetter[i] == letter) {
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
int didPlayerWin(){
    return 0;
}

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

void displayUsedLetter(char *usedLetter[26]){
    printf("\n\nLettre déjà utilisée : ");
    for (int i = 0; i < 26; i++)
    {
        if (usedLetter[i] != (void *)0)
        {
            printf("%s, ", usedLetter[i]);
        }
    }
    printf("\n");
}

int addLetterToUsedLetter(char *usedLetter[26], char letter, int index){
    usedLetter[index] = &letter;
    return index + 1;
}

/*
Permet d'afficher le mot, un '_' si la lettre n'a pas encore été utilisée sinon la lettre
*/
void display(char *word, char *usedLetter[26]){
    int isUsed;
    printf("\n\n");
    //printf("---------------------");
    //printf("\n");
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
    //printf("\n");
    //printf("---------------------");
    printf("\n\n");
}

int main(int argc, char const *argv[]){

    // Variables
    char *word; // Mot a deviné
    char *usedLetter[26] = {NULL}; // Tableau qui contient les lettres que le joueur propose
    int numberOfUsedLettre = 0;

    char choosenLetter; // Lettre choisie par le joueur
    int attempts = 0; // Nombre d'essai(s)

    int badTry = 0; // Nombre d'erreur(s)
    int maxLife = 5; // Nombre de vies

    int current_life = 0;

    if (argc>0) {
		word = argv[1];
	} else {
		printf("USAGE : %s MOT",argv[0]);
		exit(-1);
	}

    do{
        displayLife(current_life, maxLife);
        display(word, usedLetter);
        printf("CHOIX LETTRE : ");
        scanf("%c", &choosenLetter);

        //printf("\nLETTRE CHOISIE : %c", choosenLetter);

        switch (letterAlreadyUsed(choosenLetter, usedLetter)){
        case 1:
            printf("La lettre '%c' a déjà été utilisée\n", choosenLetter);
            continue;
        case 0:
            if (strchr(word, choosenLetter) == NULL) { // Si le mot de contient pas la lettre choisie
                current_life ++;
            }

            numberOfUsedLettre = addLetterToUsedLetter(usedLetter, choosenLetter, numberOfUsedLettre);

            displayUsedLetter(usedLetter);
            continue;
        }
        
        attempts ++;
    } while(didPlayerWin()==0);

    return 0;
}