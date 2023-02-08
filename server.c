#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include "pendu.c"

#define PORT IPPORT_USERRESERVED // = 5000 (ports >= 5000 réservés pour usage explicite)

#define LG_MESSAGE 256

int main(int argc, char *argv[]){

    // Variables serveur
	int socketEcoute;

	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;

	int socketDialogue;
	struct sockaddr_in pointDeRencontreDistant;
	char messageRecu[LG_MESSAGE];
	char messageEnvoi[LG_MESSAGE];
	int ecrits, lus; /* nb d’octets ecrits et lus */
	int retour;

    // Variables jeu
    char *word; // Mot a deviné
    char allLetters[26] = "abcdefghijklmnopqrstuvwxyz"; // Tableau qui contient toutes les lettres de l'alphabet (lettre jouable)
    char usedLetter[26] = {}; // Tableau qui contient les lettres que le joueur propose
    int numberOfUsedLetter = 0;

    char letter = '$';
    int attempts = 0; // Nombre d'essai(s)

    int badTry = 0; // Nombre d'erreur(s)
    int maxLife = 5; // Nombre de vies

    int current_life = 0; // Nombre de vie perdu
    
    if (argc>1) {
		word = argv[1];
	} else {
		printf("USAGE : %s MOT",argv[0]);
		exit(-1);
	}

    // --------------------------------------------------
    // Création / init du socket

	// Crée un socket de communication
	socketEcoute = socket(PF_INET, SOCK_STREAM, 0); 
	// Teste la valeur renvoyée par l’appel système socket() 
	if(socketEcoute < 0){
		perror("socket"); // Affiche le message d’erreur 
	exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", socketEcoute); // On prépare l’adresse d’attachement locale

	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant le point d'écoute local)
	longueurAdresse = sizeof(pointDeRencontreLocal);

	memset(&pointDeRencontreLocal, 0x00, longueurAdresse); pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // attaché à toutes les interfaces locales disponibles
	pointDeRencontreLocal.sin_port = htons(5051); // = 5000 ou plus
	
	// On demande l’attachement local de la socket
	if((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0) {
		perror("bind");
		exit(-2); 
	}
	printf("Socket attachée avec succès !\n");

	// On fixe la taille de la file d’attente à 5 (pour les demandes de connexion non encore traitées)
	if(listen(socketEcoute, 5) < 0){
   		perror("listen");
   		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");

    //---------------------------------------------------------------------
    // Attente d'une conexion
	
    memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
    printf("\nAttente d’une demande de connexion (quitter avec Ctrl-C)\n\n");

    // c’est un appel bloquant
    socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
    if (socketDialogue < 0) {
        perror("accept");
        close(socketDialogue);
        close(socketEcoute);
        exit(-4);
    }

    printf("Un client vient de se connecter\n\n");

    //---------------------------------------------------------------------
    // Boucle du jeu

	while(1){

        //--------------------------------
        // Envoie de le message au client

        strcpy(messageEnvoi,prepareToSend(word,usedLetter,allLetters,letter,&numberOfUsedLetter,attempts,current_life,maxLife));

		// On envoie des données vers le client (cf. protocole)
		ecrits = write(socketDialogue, messageEnvoi, strlen(messageEnvoi)); 
		switch(ecrits){
			case -1 : /* une erreur ! */
				  perror("write");
   				  close(socketDialogue);
   				  exit(-6);
			case 0 :  /* la socket est fermée */
				  fprintf(stderr, "La socket a été fermée par le client !\n\n");
				  close(socketDialogue);
				  return 0;
			default:  /* envoi de n octets */
   				  printf("Message envoyé : %s\n\n", messageEnvoi);
		}

		if (strcmp(&messageEnvoi,"Vous avez gagné !") == 0 || strcmp(&messageEnvoi,"Dommange vous avez perdu :(") == 0)
		{
			close(socketDialogue); 
			exit(-7);
		}
		

        //-------------------------------------------------
        // Reception et traitement de la réponse du client
		
		// On réception les données du client (cf. protocole)
		lus = read(socketDialogue, messageRecu, LG_MESSAGE*sizeof(char)); // ici appel bloquant
		switch(lus) {
			case -1 : /* une erreur ! */ 
				  perror("read"); 
				  close(socketDialogue); 
				  exit(-5);
			case 0  : /* la socket est fermée */
				  fprintf(stderr, "La socket a été fermée par le client !\n\n");
   				  close(socketDialogue);
   				  return 0;
			default:  /* réception de n octets */
				  printf("Message reçu : %s \n\n", messageRecu);
		}

        letter = messageRecu[0]; 

        switch (letterAlreadyUsed(letter, usedLetter)){
        case 0:
            if (strchr(word, letter) == NULL && strchr(allLetters,letter)!=NULL) { // Si le mot de contient pas la lettre choisie
                current_life ++;
            }
        }  
        attempts ++;
	}


	// On ferme la ressource avant de quitter
   	close(socketEcoute);
	return 0; 
}
