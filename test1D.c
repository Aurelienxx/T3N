#include <stdio.h>
#include <stdlib.h>

void position_alea(char grille[9], int *pos_x) {
    int vide[9];
    int compteur = 0;

    for (size_t i = 0; i < 9; i++) {
        if (grille[i] == ' ') {
            vide[compteur] = i;
            compteur++;
        }
    }

    unsigned int seed = (unsigned int)&compteur;
    srand(seed);

    int choix = (rand() % compteur);

    *pos_x = vide[choix];
}

int main() {
    char grille[9] = {
        ' ', ' ', 'O',
        'X', ' ', ' ',
        ' ', ' ', ' '
    };

    int pos;
    position_alea(grille, &pos);

    printf("Position : (%d)\n", pos);

    return 0;
}
