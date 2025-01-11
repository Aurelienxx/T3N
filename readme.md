Auteurs: Corbeau Baptiste; Dusannier Léothen; Fontaine Aurélien; Piquet Edwyn

Compilez les fichiers et lancez le serveur
Lors de la connexion, entrez J pour être joueur ou S pour être spectateur.

Pour les spectateurs :
Il faut choisir la partie que vous souhaitez visionner en rentrant son ID.
Il faut attendre la fin d'un tour (que les 2 joueurs aient joué) pour commencer à visionner.

Pour les joueurs :
Attendre qu'une autre personne se connecte et choisisse d'être joueur pour lancer la partie.
Le joueur 1 commence avec les X, il lui suffit de rentrer la position à laquelle il veut jouer entre 1 et 9, positions qui sont affichées pour éviter tout quiproquo.
Ensuite le joueur 2 joue et vice-versa jusqu'à la fin de la partie.

Requirements :
Lancer le programme sur Linux (on ne garantit pas son fonctionnement sur d'autre OS).
Posséder un compilateur C.

Pour compiler : 
gcc serveur_base_tcp.c -o serveur
gcc client_base_tcp.c -o client

Pour exécuter : 
./serveur
./client 127.0.0.1 5000