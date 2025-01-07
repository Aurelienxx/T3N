#include <stdio.h>
#include <stdlib.h>

void position_alea(char grille[3][3], int *pos_x, int *pos_y) {
    int vide[9][2];
    int compteur = 0;

    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            if (grille[x][y] == ' ') {
                vide[compteur][0] = x;
                vide[compteur][1] = y;
                compteur++;
            }
        }
    }

    unsigned int seed = (unsigned int)&compteur;
    srand(seed);

    int choix = (rand() % compteur);

    *pos_x = vide[choix][0];
    *pos_y = vide[choix][1];
}

int main() {
    char grille[3][3] = {
        {' ', ' ', 'O'},
        {'X', ' ', ' '},
        {' ', ' ', ' '}
    };

    int x, y;
    position_alea(grille, &x, &y);

    printf("Position aléatoire sélectionnée : (%d, %d)\n", x, y);

    return 0;
}
