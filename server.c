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

	int socketDialogue1;
	int socketDialogue2;
	struct sockaddr_in pointDeRencontreDistant;
	char messageRecu1[LG_MESSAGE];
	char messageRecu2[LG_MESSAGE];
	char messageEnvoi1[LG_MESSAGE];
	char messageEnvoi2[LG_MESSAGE];
	int ecrits, lus; /* nb d’octets ecrits et lus */
	int retour;

    // Variables jeu
    char *word; // Mot a deviné
    char allLetters[26] = "abcdefghijklmnopqrstuvwxyz"; // Tableau qui contient toutes les lettres de l'alphabet (lettre jouable)
    char usedLetter1[26] = {}; // Tableau qui contient les lettres que le joueur propose
    char usedLetter2[26] = {}; // Tableau qui contient les lettres que le joueur propose
    int numberOfUsedLetter1 = 0;
    int numberOfUsedLetter2 = 0;

    char letter1 = '$';
    char letter2 = '$';
    int attempts1 = 0; // Nombre d'essai(s)
    int attempts2 = 0; // Nombre d'essai(s)
	int end1 = 0;
	int end2 = 0;

    int maxLife = 5; // Nombre de vies

    int current_life1 = 0; // Nombre de vie perdu
    int current_life2 = 0; // Nombre de vie perdu
    
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
	pointDeRencontreLocal.sin_port = htons(5050); // = 5000 ou plus
	
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
	
    memset(messageRecu1, 0x00, LG_MESSAGE*sizeof(char));
	memset(messageRecu2, 0x00, LG_MESSAGE*sizeof(char));
    printf("\nAttente d’une demande de connexion (quitter avec Ctrl-C)\n\n");

    // c’est un appel bloquant
    socketDialogue1 = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
    if (socketDialogue1 < 0) {
        perror("accept");
        close(socketDialogue1);
        close(socketEcoute);
        exit(-4);
    }

	printf("Le client 1 vient de se connecter\n\n");
	printf("Attente d'un deuxième client\n\n");

	socketDialogue2 = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
    if (socketDialogue2 < 0) {
        perror("accept");
        close(socketDialogue2);
        close(socketEcoute);
        exit(-4);
    }

    printf("Le client 2 vient de se connecter\n\n");
    printf("Le jeu peut commencer\n\n");

    //---------------------------------------------------------------------
    // Boucle du jeu

	while(1){

		//---------------------------------
        // Envoie de le message au client 2

		strcpy(messageEnvoi2,prepareToSend(word,usedLetter2,allLetters,letter2,&numberOfUsedLetter2,attempts2,current_life2,maxLife, &end2));

		if (end2 == 0 || end2 == 1)
		{
			// On envoie des données vers le client (cf. protocole)
			ecrits = write(socketDialogue2, messageEnvoi2, strlen(messageEnvoi2)); 
			switch(ecrits){
				case -1 : /* une erreur ! */
					perror("write");
					close(socketDialogue2);
				case 0 :  /* la socket est fermée */
					fprintf(stderr, "La socket a été fermée par le client !\n\n");
					close(socketDialogue2);
				default:  /* envoi de n octets */
					printf("\nMessage envoyé : %s\n\n", messageEnvoi2);
					if (end2 == 1)
					{
						end2 = 2;
					}
			}

			//--------------------------------------------------
			// Reception et traitement de la réponse du client 2
			if (end2 == 0)
			{
				// On réception les données du client (cf. protocole)
				lus = read(socketDialogue2, messageRecu2, LG_MESSAGE*sizeof(char)); // ici appel bloquant
				switch(lus) {
					case -1 : /* une erreur ! */ 
						perror("read"); 
						close(socketDialogue2); 
						exit(-5);
					case 0  : /* la socket est fermée */
						fprintf(stderr, "La socket a été fermée par le client !\n\n");
						close(socketDialogue2);
					default:  /* réception de n octets */
						printf("\nMessage reçu : %s \n\n", messageRecu2);
				}

				letter2 = messageRecu2[0]; 

				switch (letterAlreadyUsed(letter2, usedLetter2)){
				case 0:
					if (strchr(word, letter2) == NULL && strchr(allLetters,letter2)!=NULL) { // Si le mot de contient pas la lettre choisie
						current_life2 ++;
					}
				}  
			}
			
			attempts2 ++;
		}

        //---------------------------------
        // Envoie de le message au client 1

        strcpy(messageEnvoi1,prepareToSend(word,usedLetter1,allLetters,letter1,&numberOfUsedLetter1,attempts1,current_life1,maxLife,&end1));

		if (end1 == 0 || end1 == 1){
			// On envoie des données vers le client (cf. protocole)
			ecrits = write(socketDialogue1, messageEnvoi1, strlen(messageEnvoi1)); 
			switch(ecrits){
				case -1 : /* une erreur ! */
					perror("write");
					close(socketDialogue1);
					exit(-6);
				case 0 :  /* la socket est fermée */
					fprintf(stderr, "La socket a été fermée par le client !\n\n");
					close(socketDialogue1);
					exit(-7);
				default:  /* envoi de n octets */
					printf("\nMessage envoyé : %s\n\n", messageEnvoi1);
					if (end1==1)
					{
						end2 = 2;
					}
			}
		

			//--------------------------------------------------
			// Reception et traitement de la réponse du client 1
			
			if (end1 == 0)
			{
				// On réception les données du client (cf. protocole)
				lus = read(socketDialogue1, messageRecu1, LG_MESSAGE*sizeof(char)); // ici appel bloquant
				switch(lus) {
					case -1 : /* une erreur ! */ 
						perror("read"); 
						close(socketDialogue1); 
						exit(-5);
					case 0  : /* la socket est fermée */
						fprintf(stderr, "La socket a été fermée par le client !\n\n");
						close(socketDialogue1);
						exit(-8);
					default:  /* réception de n octets */
						printf("\nMessage reçu : %s \n\n", messageRecu1);
				}

				letter1 = messageRecu1[0]; 

				switch (letterAlreadyUsed(letter1, usedLetter1)){
				case 0:
					if (strchr(word, letter1) == NULL && strchr(allLetters,letter1)!=NULL) { // Si le mot de contient pas la lettre choisie
						current_life1 ++;
					}
				}  
			}
			
			
			attempts1 ++;
		}
		
	}

	// On ferme la ressource avant de quitter
   	close(socketEcoute);
	return 0; 
}
