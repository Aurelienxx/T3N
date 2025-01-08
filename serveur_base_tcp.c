#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include <time.h>

#define PORT 5000 //(ports >= 5000 réservés pour usage explicite)

#define LG_MESSAGE 256

/* Choisi une position aléatoirement */
void position_alea(char grille[9], int *pos_x) {
    int vide[9];
    int nb_vide = 0;

    for (int i = 0; i < 9; i++) {
        if ((grille[i] != 'X') | (grille[i] == 'O')) {
            vide[nb_vide] = i;
            nb_vide++;
        }
    }

    int choix = (rand() % nb_vide);

    *pos_x = vide[choix];
}

/* Compte le nombre d'espace vide contenu dans le tableau */
int check_empty(char grille[9]){
	int nb_vide = 0;

	for ( int i = 0; i < 9; i++){
		if ((grille[i] != 'X') | (grille[i] == 'O')){
			nb_vide = nb_vide + 1;
		}
	}

	return nb_vide;
}

/* Verifie une victoire */
int check_victory(char tab[9], char symbol) {

    for (int i = 0; i < 9; i = i + 3) {
        if (tab[i] == symbol && tab[i + 1] == symbol && tab[i + 2] == symbol) {
            return 1; 
        }
    }

    for (int i = 0; i < 3; i++) {
        if (tab[i] == symbol && tab[i + 3] == symbol && tab[i + 6] == symbol) {
            return 1;
        }
    }

    if (tab[0] == symbol && tab[4] == symbol && tab[8] == symbol) {
        return 1; 
    }
    if (tab[2] == symbol && tab[4] == symbol && tab[6] == symbol) {
        return 1; 
    }

    return 0; 
}

/* Verfication victoire */
void verifier_etat_jeu(char tab[9], char *messageEnvoye, char joueur_actuel, char autre_joueur) {
    if (check_victory(tab, joueur_actuel)) {
        snprintf(messageEnvoye, LG_MESSAGE, "%cwins", joueur_actuel); 
    } else if (check_empty(tab) == 0) {
        snprintf(messageEnvoye, LG_MESSAGE, "%cends", joueur_actuel); 
    } else if (check_victory(tab, autre_joueur)) {
        snprintf(messageEnvoye, LG_MESSAGE, "%cwins", autre_joueur); 
    } else if (check_empty(tab) == 0) {
        snprintf(messageEnvoye, LG_MESSAGE, "%cends", autre_joueur); 
    } else {
        strncpy(messageEnvoye, "continue", LG_MESSAGE - 1); 
    }
}

int main(int argc, char *argv[]){
	int socketEcoute;

	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;

	int socketDialogue;
	int joueur1,joueur2;
	int indiceJoueur1, indiceJoueur2;
	struct sockaddr_in pointDeRencontreDistant;
	char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */
	char messageEnvoye[LG_MESSAGE];
	char tab[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
	int ecrits, lus; /* nb d’octets ecrits et lus */
	int retour;

	srand(time(NULL));

	// Crée un socket de communication
	socketEcoute = socket(AF_INET, SOCK_STREAM, 0); 
	// Teste la valeur renvoyée par l’appel système socket() 
	if(socketEcoute < 0){
		perror("socket"); // Affiche le message d’erreur 
	exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", socketEcoute); // On prépare l’adresse d’attachement locale
	//setsockopt()


	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant le point d'écoute local)
	longueurAdresse = sizeof(pointDeRencontreLocal);
	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &pointDeRencontreLocal
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse); pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // attaché à toutes les interfaces locales disponibles
	pointDeRencontreLocal.sin_port = htons(PORT); // = 5000 ou plus
	
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

	while (1){

		printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");

		printf("Attente des connexions des joueurs ...\n");
		joueur1 = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (joueur1 < 0) {
			perror("accept joueur1");
			exit(-4);
		}
		printf("Joueur 1 connecté.\n");

		joueur2 = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (joueur2 < 0) {
			perror("accept joueur2");
			exit(-5);
		}
		printf("Joueur 2 connecté.\n");

		send(joueur1, "startPlayer1", strlen("startPlayer1") + 1, 0);
		send(joueur2, "startplayer2", strlen("startplayer2") + 1, 0);
		
		// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment ! 
		while(1){
			lus = recv(joueur1, &indiceJoueur1, sizeof(indiceJoueur1), 0);
			
			tab[indiceJoueur1] = 'X';

			send(joueur2, &indiceJoueur1, sizeof(indiceJoueur1), 0);  // Envoi de la position choisi 

			verifier_etat_jeu(tab,messageEnvoye,'X','O');
			send(joueur2, messageEnvoye, strlen(messageEnvoye) + 1, 0);

			lus = recv(joueur2, &indiceJoueur2, sizeof(indiceJoueur2), 0);

			tab[indiceJoueur2] = 'O';

			send(joueur1, &indiceJoueur2, sizeof(indiceJoueur2), 0);  // Envoi de la position choisi 

			//char envoie[] = "continue";
			//strncpy(messageEnvoye,"continue", LG_MESSAGE - 1);
			//send(joueur1, envoie, strlen(envoie) + 1, 0);  // Envoi de l'état de la grille
			//send(joueur2, envoie, strlen(envoie) + 1, 0);  // Envoi de l'état de la grille

			/*
			switch(lus) {
				case -1: // une erreur !
						perror("recv");
						close(socketDialogue);
					exit(-5);
				case 0: // la socket est fermée
						fprintf(stderr, "La socket a été fermée par le client !\n");
						close(socketDialogue);
					return 0;
				default: // recuperation de la position 
						

					break;

			}
			*/
			
			verifier_etat_jeu(tab,messageEnvoye,'O','X');
			send(joueur1, messageEnvoye, strlen(messageEnvoye) + 1, 0);  
			  

		}

		close(joueur1);
		close(joueur2);

		/*
		// c’est un appel bloquant
		socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketDialogue < 0) {
   			perror("accept");
			close(socketDialogue);
   			close(socketEcoute);
   			exit(-4);
		}
		
		char message[] = "start";
		send(socketDialogue, message, strlen(message) + 1, 0);

		// On réception les données du client (cf. protocole)
		//lus = read(socketDialogue, messageRecu, LG_MESSAGE*sizeof(char)); // ici appel bloquant

		int joueur,ordinateur ;
		while (1){
			lus = recv(socketDialogue, &joueur, sizeof(joueur), 0); // Attente de la réception des données
			
			switch(lus) {
				case -1: // une erreur !
						perror("recv");
						close(socketDialogue);
					exit(-5);
				case 0: // la socket est fermée
						fprintf(stderr, "La socket a été fermée par le client !\n");
						close(socketDialogue);
					return 0;
				default: // recuperation de la position 
						tab[joueur] = 'X';

						// Verifie une victoire de 'X'
						if ( check_victory(tab,'X') ){
							strncpy(messageEnvoye,"Xwins", LG_MESSAGE - 1);
						} else {
							// verification si la grille est vide ou non 
							if (check_empty(tab) == 0){
								strncpy(messageEnvoye,"Xends", LG_MESSAGE - 1);
							} else {
								position_alea(tab, &ordinateur);
								tab[ordinateur] = 'O';

								// verification victoire de O
								if ( check_victory(tab,'O') ){
									strncpy(messageEnvoye,"Owins", LG_MESSAGE - 1);
								} else {
									if ( check_empty(tab) == 0){
										strncpy(messageEnvoye,"Oends", LG_MESSAGE - 1);
									} else {
										strncpy(messageEnvoye,"continue", LG_MESSAGE - 1);
									}
								}
							}
						}

					break;

			}
			
			send(socketDialogue, &ordinateur, sizeof(ordinateur), 0);  // Envoi de la position choisi 
			send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);  // Envoi de l'état de la grille
			
		}	
		
        close(socketDialogue);
		*/

	}
	

	// On ferme la ressource avant de quitter
   	close(socketEcoute);
	return 0; 
}
