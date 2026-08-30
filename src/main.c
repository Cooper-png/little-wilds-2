#include <gb/gb.h>
#include <stdio.h>

void wait_for_a(void) {
    while (1) {
        waitpad(J_A);
        if (joypad() & J_A) {
            waitpadup();
            return;
        }
    }
}

void title(void) {
    printf("\n\n");
    printf("   LITTLE WILDS\n\n");
    printf(" A tiny monster RPG\n\n");
    printf("   ORIGINAL GB GAME\n\n");
    printf("      PRESS A\n");

    wait_for_a();
}

void overworld(void) {
    UINT8 x = 8;
    UINT8 y = 8;
    UINT8 keys;

    printf("\n\n");
    printf("MEADOW OF ECHOES\n\n");
    printf("You enter a quiet\n");
    printf("little wilderness.\n\n");
    printf("Find the wilds!\n\n");
    printf("ARROWS: MOVE\n");
    printf("A: SEARCH\n");

    while (1) {
        keys = joypad();

        if (keys & J_UP) {
            y--;
            waitpadup();
        }
        else if (keys & J_DOWN) {
            y++;
            waitpadup();
        }
        else if (keys & J_LEFT) {
            x--;
            waitpadup();
        }
        else if (keys & J_RIGHT) {
            x++;
            waitpadup();
        }
        else if (keys & J_A) {
            waitpadup();

            printf("\nA wild FENLO\n");
            printf("appeared!\n\n");
            printf("FENLO HP: 12\n");
            printf("YOUR HP:  15\n\n");
            printf("A: ATTACK\n");

            wait_for_a();

            printf("\nFENLO was defeated!\n");
            printf("You found a wild\n");
            printf("little companion.\n");

            return;
        }
    }
}

void main(void) {
    DISPLAY_ON;

    title();
    overworld();

    printf("\n\nTHE END\n\n");
    printf("Thanks for playing!\n");

    while (1) {
        wait_vbl_done();
    }
}