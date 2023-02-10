#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include "pendu.c"

#define LG_MESSAGE 256

int main(int argc, char *argv[]){
    int descripteurSocket;
	struct sockaddr_in sockaddrDistant;
	socklen_t longueurAdresse;

	char messageEnvoi[LG_MESSAGE]; // Lettre choisie par le joueur
	char messageRecu[LG_MESSAGE]; // buffer stockant le message reçu
	int nb; /* nb d’octets écrits et lus */

    char ip_dest[16];
	int  port_dest;

	// Variables jeu
	char *word= malloc(sizeof(char) * 256); // Mot a deviné
	char allLetters[26] = "abcdefghijklmnopqrstuvwxyz"; // Tableau qui contient toutes les lettres de l'alphabet (lettre jouable)
	char usedLetter[26] = {}; // Tableau qui contient les lettres que le joueur propose
	int numberOfUsedLetter = 0;

	char letter = '$';
	int attempts = 0; // Nombre d'essai(s)

	int maxLife = 5; // Nombre de vies

	int current_life = 0; // Nombre de vie perdu

    if (argc>1) { // si il y a au moins 2 arguments passés en ligne de commande, récupération ip, port et message
		strncpy(ip_dest,argv[1],16);
		sscanf(argv[2],"%d",&port_dest);
	}else{
		printf("Nombre d'argument insuffisant");
		exit(-1);
	}

    // Crée un socket de communication
	descripteurSocket = socket(AF_INET, SOCK_STREAM, 0);
	// Teste la valeur renvoyée par l’appel système socket()
	if(descripteurSocket < 0){
		perror("Erreur en création de la socket..."); // Affiche le message d’erreur
		exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée! (%d)\n", descripteurSocket);

    // Remplissage de sockaddrDistant (structure sockaddr_in identifiant la machine distante)
	// Obtient la longueur en octets de la structure sockaddr_in
	longueurAdresse = sizeof(sockaddrDistant);
	// Initialise à 0 la structure sockaddr_in
	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &sockaddrDistant
	memset(&sockaddrDistant, 0x00, longueurAdresse);
	// Renseigne la structure sockaddr_in avec les informations du serveur distant
	sockaddrDistant.sin_family = AF_INET;
	// On choisit le numéro de port d’écoute du serveur
	sockaddrDistant.sin_port = htons(port_dest);
	// On choisit l’adresse IPv4 du serveur
	inet_aton(ip_dest, &sockaddrDistant.sin_addr);
    
    // Débute la connexion vers le processus serveur distant
	if((connect(descripteurSocket, (struct sockaddr *)&sockaddrDistant,longueurAdresse)) == -1){
		perror("Erreur de connection avec le serveur distant...");
		close(descripteurSocket);
		exit(-2); // On sort en indiquant un code erreur
	}
	printf("Connexion au serveur %s:%d réussie!\n",ip_dest,port_dest);


	/* Reception des données du serveur */
	switch(nb = read(descripteurSocket, messageRecu, LG_MESSAGE)) {
		case -1 : /* une erreur ! */
			perror("Erreur de lecture...");
			close(descripteurSocket);
			exit(-4);
		case 0 : /* la socket est fermée */
		fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
			return 0;
		default: /* réception de n octets */
			messageRecu[nb]='\0';
			printf("%s\n", messageRecu);
	}

	
	int isFirst = strcmp(messageRecu,"A vous de choisir le mot\n") == 0 ? 1 : 0;

	if (isFirst == 1)
	{
		printf("Entrez votre mot : ");

		scanf("%s", word);
	}

	
	

    while (descripteurSocket!=0){

		if (isFirst == 1)
		{
			strcpy(messageEnvoi,prepareToSend(word,usedLetter,allLetters,letter,&numberOfUsedLetter,attempts,current_life,maxLife));

			// On envoie des données vers le client (cf. protocole)
			nb = write(descripteurSocket, messageEnvoi, strlen(messageEnvoi)); 
			switch(nb){
				case -1 : /* une erreur ! */
					perror("write");
					close(descripteurSocket);
				case 0 :  /* la socket est fermée */
					fprintf(stderr, "La socket a été fermée par le client !\n\n");
					close(descripteurSocket);
				default:  /* envoi de n octets */
					printf("\nMessage envoyé : %s\n\n", messageEnvoi);

			}

			if (strstr(messageEnvoi,"\nVous avez gagné !") || strstr(messageEnvoi,"\nDommage vous avez perdu :("))
			{
				printf("Fin de la partie.");
				close(descripteurSocket); 
				exit(-9);
			} 

			//-------------------------------------------------------
			// Reception et traitement de la réponse du client joueur
			// On réception les données du client (cf. protocole)
			nb = read(descripteurSocket, messageRecu, LG_MESSAGE*sizeof(char)); // ici appel bloquant
			switch(nb) {
				case -1 : /* une erreur ! */ 
					perror("read"); 
					close(descripteurSocket); 
					exit(-5);
				case 0  : /* la socket est fermée */
					fprintf(stderr, "La socket a été fermée par le client !\n\n");
					close(descripteurSocket);
				default:  /* réception de n octets */
					printf("\nMessage reçu : %s \n\n", messageRecu);
			}

			letter = messageRecu[0]; 

			switch (letterAlreadyUsed(letter, usedLetter)){
			case 0:
				if (strchr(word, letter) == NULL && strchr(allLetters,letter)!=NULL) { // Si le mot de contient pas la lettre choisie
					current_life ++;
				}
			}  
			attempts ++;

		} else {

			/* Reception des données du serveur */
			switch(nb = read(descripteurSocket, messageRecu, LG_MESSAGE)) {
				case -1 : /* une erreur ! */
					perror("Erreur de lecture...");
					close(descripteurSocket);
					exit(-4);
				case 0 : /* la socket est fermée */
				fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
					return 0;
				default: /* réception de n octets */
					messageRecu[nb]='\0';
					printf("%s\n", messageRecu);
					
					if (strstr(messageRecu,"\nVous avez gagné !") || strstr(messageRecu,"\nDommage vous avez perdu :("))
					{
						close(descripteurSocket); 
						exit(-5);
					} else {
						printf("Donnez une lettre : ");
						scanf("%s", messageEnvoi);
					}
			}

			// Envoi du message
			switch(nb = write(descripteurSocket, messageEnvoi, strlen(messageEnvoi))){
				case -1 : /* une erreur ! */
						perror("Erreur en écriture...");
						close(descripteurSocket);
						exit(-3);
				case 0 : /* la socket est fermée */
					fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
					return 0;
			}
		}
    }
	close(descripteurSocket);
}