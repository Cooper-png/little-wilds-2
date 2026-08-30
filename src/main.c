#include <gb/gb.h>
#include <stdio.h>

/*
    LITTLE WILDS
    v0.3 - First playable RPG slice

    Controls:
      D-Pad = move
      A     = interact / confirm
      B     = cancel

    Original Game Boy project.
*/

#define MAP_W 20
#define MAP_H 18

/* Tile IDs */
#define TILE_GRASS  0
#define TILE_PATH   1
#define TILE_TREE   2
#define TILE_WATER  3
#define TILE_FLOWER 4
#define TILE_GRASS2 5

/* Player */
UINT8 player_x = 9;
UINT8 player_y = 8;

/* Current area */
UINT8 area = 0;

/* Simple monster data */
typedef struct {
    const char *name;
    UINT8 max_hp;
    UINT8 attack;
    UINT8 level;
} Monster;

Monster player_monster = {"FENLO", 20, 7, 3};

/* -------------------------------------------------
   TILE GRAPHICS
   ------------------------------------------------- */

const unsigned char tile_grass[16] = {
    0x00,0x00,
    0x00,0x00,
    0x10,0x00,
    0x00,0x01,
    0x00,0x00,
    0x08,0x00,
    0x00,0x00,
    0x01,0x00
};

const unsigned char tile_path[16] = {
    0x00,0x00,
    0x55,0x55,
    0x00,0x00,
    0x55,0x55,
    0x00,0x00,
    0x55,0x55,
    0x00,0x00,
    0x55,0x55
};

const unsigned char tile_tree[16] = {
    0x18,0x18,
    0x3C,0x3C,
    0x7E,0x7E,
    0xFF,0xFF,
    0xFF,0xFF,
    0x7E,0x7E,
    0x18,0x18,
    0x18,0x18
};

const unsigned char tile_water[16] = {
    0xAA,0x55,
    0x00,0x00,
    0x55,0xAA,
    0x00,0x00,
    0xAA,0x55,
    0x00,0x00,
    0x55,0xAA,
    0x00,0x00
};

const unsigned char tile_flower[16] = {
    0x00,0x00,
    0x18,0x18,
    0x3C,0x3C,
    0x18,0x18,
    0x18,0x18,
    0x3C,0x3C,
    0x18,0x18,
    0x00,0x00
};

const unsigned char tile_grass2[16] = {
    0x00,0x00,
    0x08,0x00,
    0x00,0x04,
    0x20,0x00,
    0x00,0x08,
    0x04,0x00,
    0x00,0x20,
    0x10,0x00
};

/* -------------------------------------------------
   MAPS
   ------------------------------------------------- */

/* Village */
const unsigned char village[MAP_H][MAP_W] = {
 {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,2,2,2,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,2,4,2,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,2,2,2,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}
};

/* Meadow */
const unsigned char meadow[MAP_H][MAP_W] = {
 {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
 {2,0,0,0,0,5,0,0,0,1,1,0,5,0,0,0,0,0,0,2},
 {2,0,0,5,0,0,0,0,0,1,1,0,0,0,0,5,0,0,0,2},
 {2,0,0,0,0,0,0,5,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,5,0,0,0,0,0,0,1,1,0,0,5,0,0,0,0,0,2},
 {2,0,0,0,0,0,5,0,0,1,1,0,0,0,0,0,5,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,5,0,0,0,0,0,0,2},
 {2,0,0,5,0,0,0,0,0,1,1,0,0,0,0,5,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,5,0,2},
 {2,0,5,0,0,0,0,0,0,1,1,0,0,0,5,0,0,0,0,2},
 {2,0,0,0,0,0,5,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,5,0,0,0,0,5,0,2},
 {2,0,0,5,0,0,0,0,0,1,1,0,0,0,0,0,5,0,0,2},
 {2,0,0,0,0,0,0,5,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,5,0,0,0,0,0,0,1,1,0,0,5,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,5,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}
};

/* -------------------------------------------------
   LOAD GRAPHICS
   ------------------------------------------------- */

void load_tiles(void) {
    set_bkg_data(TILE_GRASS,1,tile_grass);
    set_bkg_data(TILE_PATH,1,tile_path);
    set_bkg_data(TILE_TREE,1,tile_tree);
    set_bkg_data(TILE_WATER,1,tile_water);
    set_bkg_data(TILE_FLOWER,1,tile_flower);
    set_bkg_data(TILE_GRASS2,1,tile_grass2);
}

/* -------------------------------------------------
   PLAYER SPRITE
   ------------------------------------------------- */

const unsigned char player_sprite[16] = {
    0x18,0x18,
    0x3C,0x3C,
    0x7E,0x7E,
    0xDB,0xFF,
    0xFF,0xDB,
    0x7E,0x7E,
    0x3C,0x3C,
    0x24,0x24
};

void setup_player(void) {
    set_sprite_data(0,1,player_sprite);
    set_sprite_tile(0,0);
    move_sprite(0,player_x*8+4,player_y*8+12);
}

/* -------------------------------------------------
   MAP DRAWING
   ------------------------------------------------- */

void draw_map(void) {
    UINT8 x;
    UINT8 y;
    const unsigned char (*map)[MAP_W];

    if (area == 0)
        map = village;
    else
        map = meadow;

    for (y=0;y<MAP_H;y++) {
        for (x=0;x<MAP_W;x++) {
            set_bkg_tile_xy(x,y,map[y][x]);
        }
    }
}

/* -------------------------------------------------
   COLLISION
   ------------------------------------------------- */

UINT8 blocked(UINT8 x, UINT8 y) {
    const unsigned char (*map)[MAP_W];

    if (x >= MAP_W || y >= MAP_H)
        return 1;

    if (area == 0)
        map = village;
    else
        map = meadow;

    if (map[y][x] == TILE_TREE)
        return 1;

    if (map[y][x] == TILE_WATER)
        return 1;

    return 0;
}

/* -------------------------------------------------
   DIALOGUE
   ------------------------------------------------- */

void wait_a(void) {
    waitpad(J_A);
    waitpadup();
}

void say(const char *a, const char *b) {
    cls();

    printf("\n");
    printf("%s\n",a);

    if (b != 0) {
        printf("\n%s\n",b);
    }

    printf("\n\nA: CONTINUE");

    wait_a();

    draw_map();
}

/* -------------------------------------------------
   BATTLE
   ------------------------------------------------- */

void battle(void) {
    UINT8 hp = 12;
    UINT8 player_hp = player_monster.max_hp;
    UINT8 damage;

    cls();

    printf("\n");
    printf("A WILD BRINN\n");
    printf("\n");
    printf("BRINN  HP:%u\n",hp);
    printf("\n");
    printf("%s  HP:%u\n",player_monster.name,player_hp);
    printf("\n");
    printf("A: ATTACK\n");
    printf("B: RUN\n");

    while (1) {
        UINT8 k = waitpad(J_A | J_B);

        if (k & J_B) {
            waitpadup();
            printf("\nESCAPED!");
            wait_a();
            draw_map();
            return;
        }

        if (k & J_A) {
            waitpadup();

            damage = player_monster.attack;

            if (damage > hp)
                hp = 0;
            else
                hp -= damage;

            cls();

            printf("\n");
            printf("%s ATTACKS!\n",player_monster.name);
            printf("\n");
            printf("BRINN HP: %u\n",hp);

            if (hp == 0) {
                printf("\nBRINN FLED!\n");
                wait_a();
                draw_map();
                return;
            }

            printf("\nBRINN ATTACKS!\n");

            if (player_hp > 3)
                player_hp -= 3;
            else
                player_hp = 1;

            printf("\nYOUR HP: %u\n",player_hp);
            printf("\nA: CONTINUE");

            wait_a();

            cls();

            printf("\n");
            printf("BRINN\n");
            printf("HP: %u\n\n",hp);

            printf("%s\n",player_monster.name);
            printf("HP: %u\n\n",player_hp);

            printf("A: ATTACK\n");
            printf("B: RUN\n");
        }
    }
}

/* -------------------------------------------------
   NPC
   ------------------------------------------------- */

void talk(void) {
    if (area == 0) {
        say("OLD MARA:", "The meadow is quiet today.");
        say("OLD MARA:", "If you hear singing,\ndon't follow it.");
    } else {
        say("A STRANGE VOICE:", "The wilds remember\nthose who enter.");
    }
}

/* -------------------------------------------------
   INPUT / MOVEMENT
   ------------------------------------------------- */

void move_player(UINT8 dx, UINT8 dy) {
    UINT8 nx = player_x + dx;
    UINT8 ny = player_y + dy;

    if (!blocked(nx,ny)) {
        player_x = nx;
        player_y = ny;

        move_sprite(
            0,
            player_x*8+4,
            player_y*8+12
        );
    }
}

/* -------------------------------------------------
   AREA TRANSITION
   ------------------------------------------------- */

void check_area(void) {
    if (area == 0 && player_x == 19) {
        area = 1;
        player_x = 1;
        player_y = 9;

        draw_map();

        say(
            "MEADOW OF ECHOES",
            "The village disappears\nbehind the trees."
        );
    }
}

/* -------------------------------------------------
   TITLE
   ------------------------------------------------- */

void title_screen(void) {
    cls();

    printf("\n\n");
    printf("   LITTLE WILDS\n\n");
    printf("  THE MEADOW OF\n");
    printf("     ECHOES\n\n");
    printf(" A TINY ADVENTURE\n\n");
    printf("      A: START");

    wait_a();
}

/* -------------------------------------------------
   MAIN
   ------------------------------------------------- */

void main(void) {
    UINT8 keys;

    DISPLAY_OFF;

    load_tiles();

    SHOW_BKG;
    SHOW_SPRITES;

    title_screen();

    DISPLAY_ON;

    draw_map();
    setup_player();

    while (1) {

        keys = joypad();

        if (keys & J_UP) {
            move_player(0,255);
            waitpadup();
        }
        else if (keys & J_DOWN) {
            move_player(0,1);
            waitpadup();
        }
        else if (keys & J_LEFT) {
            move_player(255,0);
            waitpadup();
        }
        else if (keys & J_RIGHT) {
            move_player(1,0);
            waitpadup();
        }
        else if (keys & J_A) {

            waitpadup();

            /*
                Village: talk to Mara.
                Meadow: A starts a battle.
            */

            if (area == 0) {
                talk();
            }
            else {
                battle();
            }
        }

        check_area();

        wait_vbl_done();
    }
}