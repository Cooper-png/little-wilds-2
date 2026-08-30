#include <gb/gb.h>
#include <stdio.h>

/*
    LITTLE WILDS
    ALPHA 2 - STABLE BUILD

    GBDK-2020 4.4.0
    Target: Game Boy

    Controls:
      D-Pad = Move
      A     = Talk / Attack
      B     = Heal in village

    Areas:
      Bramblewick
      Meadow of Echoes
      Whisper Woods
*/

/* =================================================
   CONSTANTS
   ================================================= */

#define MAP_W 20
#define MAP_H 18

#define AREA_VILLAGE 0
#define AREA_MEADOW  1
#define AREA_FOREST  2

#define TILE_GRASS  0
#define TILE_PATH   1
#define TILE_TREE   2
#define TILE_WATER  3
#define TILE_TALL   4
#define TILE_ROCK   5
#define TILE_HOUSE  6
#define TILE_DOOR   7

#define MON_FENLO   0
#define MON_MOSSEN  1
#define MON_ZAPPIT  2
#define MON_BRINN   3

/* =================================================
   MONSTER DATA
   ================================================= */

typedef struct {
    const char *name;
    UINT8 max_hp;
    UINT8 hp;
    UINT8 attack;
    UINT8 defense;
    UINT8 level;
    UINT8 xp;
} Monster;

Monster player_mon = {
    "FENLO",
    30,
    30,
    8,
    5,
    3,
    0
};

Monster wild_mossen = {
    "MOSSEN",
    18,
    18,
    5,
    4,
    2,
    0
};

Monster wild_zappit = {
    "ZAPPIT",
    20,
    20,
    7,
    3,
    3,
    0
};

Monster wild_brinn = {
    "BRINN",
    24,
    24,
    7,
    5,
    3,
    0
};

/* =================================================
   GAME STATE
   ================================================= */

UINT8 current_area = AREA_VILLAGE;

UINT8 player_x = 9;
UINT8 player_y = 15;

UINT8 story_step = 0;
UINT8 random_value = 37;

/* =================================================
   TILES
   ================================================= */

const unsigned char tile_data[9 * 16] = {

    /* 0 - GRASS */
    0x00,0x00,
    0x00,0x00,
    0x08,0x00,
    0x00,0x08,
    0x00,0x00,
    0x10,0x00,
    0x00,0x00,
    0x00,0x00,

    /* 1 - PATH */
    0x00,0x00,
    0x18,0x18,
    0x00,0x00,
    0x18,0x18,
    0x00,0x00,
    0x18,0x18,
    0x00,0x00,
    0x18,0x18,

    /* 2 - TREE */
    0x18,0x00,
    0x3C,0x18,
    0x7E,0x3C,
    0xFF,0x7E,
    0xFF,0x7E,
    0x7E,0x3C,
    0x3C,0x18,
    0x18,0x18,

    /* 3 - WATER */
    0x00,0x00,
    0x18,0x18,
    0x00,0x00,
    0x18,0x18,
    0x00,0x00,
    0x18,0x18,
    0x00,0x00,
    0x18,0x18,

    /* 4 - TALL GRASS */
    0x10,0x00,
    0x10,0x10,
    0x18,0x00,
    0x10,0x10,
    0x10,0x00,
    0x10,0x10,
    0x18,0x00,
    0x10,0x10,

    /* 5 - ROCK */
    0x00,0x00,
    0x18,0x00,
    0x3C,0x18,
    0x7E,0x3C,
    0x7E,0x3C,
    0x3C,0x18,
    0x18,0x00,
    0x00,0x00,

    /* 6 - HOUSE */
    0x18,0x18,
    0x3C,0x3C,
    0x7E,0x7E,
    0xFF,0xFF,
    0xFF,0x81,
    0xC3,0x81,
    0xC3,0x81,
    0xFF,0xFF,

    /* 7 - DOOR */
    0x3C,0x3C,
    0x42,0x42,
    0x42,0x42,
    0x42,0x42,
    0x42,0x42,
    0x42,0x42,
    0x7E,0x7E,
    0x7E,0x7E,

    /* 8 - PLAYER */
    0x3C,0x3C,
    0x7E,0x7E,
    0xFF,0xFF,
    0xDB,0xDB,
    0xFF,0xFF,
    0x7E,0x7E,
    0x3C,0x3C,
    0x18,0x18
};

/* =================================================
   MAP DATA
   ================================================= */

const unsigned char village_map[MAP_W * MAP_H] = {

    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,

    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,6,6,6,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,6,7,6,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,6,6,6,0,1,1,0,0,0,0,0,0,0,0,2,

    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,

    2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2
};

const unsigned char meadow_map[MAP_W * MAP_H] = {

    2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,

    2,0,4,4,0,0,0,0,0,1,1,0,0,0,4,4,0,0,0,2,
    2,0,4,4,4,0,0,0,0,1,1,0,0,4,4,4,0,0,0,2,
    2,0,4,0,4,0,0,0,0,1,1,0,0,4,0,4,0,0,0,2,
    2,0,4,0,4,0,0,0,0,1,1,0,0,4,0,4,0,0,0,2,
    2,0,4,4,4,0,0,0,0,1,1,0,0,4,4,4,0,0,0,2,

    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,

    2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2
};

const unsigned char forest_map[MAP_W * MAP_H] = {

    2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,

    2,2,0,0,0,2,0,0,0,1,1,0,0,0,2,0,0,0,2,2,
    2,0,0,2,0,0,0,2,0,1,1,0,2,0,0,0,2,0,0,2,
    2,0,2,2,0,0,0,0,0,1,1,0,0,0,0,2,2,0,0,2,
    2,0,0,2,0,2,0,0,0,1,1,0,0,2,0,0,2,0,0,2,
    2,0,0,0,0,0,0,2,0,1,1,0,2,0,0,0,0,0,0,2,

    2,0,2,0,0,0,0,0,0,1,1,0,0,0,0,0,2,0,0,2,
    2,0,0,0,2,0,2,0,0,1,1,0,0,2,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,2,0,0,0,2,
    2,0,2,0,0,2,0,0,0,1,1,0,0,0,2,0,0,0,0,2,
    2,0,0,0,0,0,0,2,0,1,1,0,2,0,0,0,0,2,0,2,
    2,0,0,2,0,0,0,0,0,1,1,0,0,0,2,0,0,0,0,2,
    2,0,0,0,0,2,0,0,0,1,1,0,0,0,0,0,2,0,0,2,
    2,0,2,0,0,0,0,0,0,1,1,0,0,2,0,0,0,0,0,2,
    2,0,0,0,0,0,2,0,0,1,1,0,0,0,0,2,0,0,0,2,
    2,0,0,2,0,0,0,0,0,1,1,0,0,0,2,0,0,0,0,2,
    2,0,0,0,0,0,0,2,0,1,1,0,0,0,0,0,0,2,0,2,

    2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2
};

/* =================================================
   SOUND
   ================================================= */

void sound_init(void) {
    NR52_REG = 0x80;
    NR50_REG = 0x77;
    NR51_REG = 0xFF;
}

void sound_move(void) {
    NR21_REG = 0x80;
    NR22_REG = 0xF2;
    NR23_REG = 0x90;
    NR24_REG = 0x86;
}

void sound_hit(void) {
    NR41_REG = 0x08;
    NR42_REG = 0xF3;
    NR43_REG = 0x25;
    NR44_REG = 0xC0;
}

/* =================================================
   RANDOM
   ================================================= */

UINT8 random_number(void) {

    random_value++;

    random_value ^= DIV_REG;

    random_value =
        (UINT8)(
            (random_value << 1) |
            (random_value >> 7)
        );

    return random_value;
}

/* =================================================
   MAP FUNCTIONS
   ================================================= */

const unsigned char *get_map(void) {

    if(current_area == AREA_VILLAGE)
        return village_map;

    if(current_area == AREA_MEADOW)
        return meadow_map;

    return forest_map;
}

UINT8 get_tile(UINT8 x, UINT8 y) {

    const unsigned char *map;

    if(x >= MAP_W)
        return TILE_TREE;

    if(y >= MAP_H)
        return TILE_TREE;

    map = get_map();

    return map[
        ((UINT16)y * MAP_W) + x
    ];
}

UINT8 blocked(UINT8 tile) {

    if(tile == TILE_TREE)
        return 1;

    if(tile == TILE_WATER)
        return 1;

    if(tile == TILE_ROCK)
        return 1;

    if(tile == TILE_HOUSE)
        return 1;

    return 0;
}

/* =================================================
   DRAWING
   ================================================= */

void draw_map(void) {

    set_bkg_tiles(
        0,
        0,
        MAP_W,
        MAP_H,
        get_map()
    );
}

void draw_player(void) {

    set_bkg_tile_xy(
        player_x,
        player_y,
        8
    );
}

/*
   Redraw the tile underneath the player.
*/

void redraw_player_tile(
    UINT8 old_x,
    UINT8 old_y
) {

    set_bkg_tile_xy(
        old_x,
        old_y,
        get_tile(old_x, old_y)
    );

    draw_player();
}

/* =================================================
   INPUT
   ================================================= */

void wait_a(void) {

    while(!(joypad() & J_A))
        wait_vbl_done();

    waitpadup();
}

void wait_b(void) {

    while(!(joypad() & J_B))
        wait_vbl_done();

    waitpadup();
}

/* =================================================
   MESSAGE
   ================================================= */

void message(const char *text) {

    printf("\n\n%s\n\n", text);
    printf("A: CONTINUE");

    wait_a();

    draw_map();
    draw_player();
}

/* =================================================
   BATTLE
   ================================================= */

UINT8 battle(Monster *enemy) {

    UINT8 damage;
    UINT8 enemy_damage;
    UINT8 keys;

    while(1) {

        printf("\n\n================");
        printf("\nWILD %s", enemy->name);

        printf("\nHP %u/%u",
            enemy->hp,
            enemy->max_hp
        );

        printf("\n\nFENLO");

        printf("\nHP %u/%u",
            player_mon.hp,
            player_mon.max_hp
        );

        printf("\n\nA ATTACK");
        printf("\nB RUN");

        while(1) {

            wait_vbl_done();

            keys = joypad();

            if(keys & J_A) {

                waitpadup();

                damage =
                    player_mon.attack;

                if(enemy->defense < damage)
                    damage -= enemy->defense;
                else
                    damage = 1;

                if(damage > enemy->hp)
                    damage = enemy->hp;

                enemy->hp -= damage;

                sound_hit();

                printf(
                    "\n\nFENLO HIT %u!",
                    damage
                );

                delay(40);

                break;
            }

            if(keys & J_B) {

                waitpadup();

                if(random_number() & 1) {

                    message(
                        "ESCAPED!"
                    );

                    enemy->hp =
                        enemy->max_hp;

                    return 0;
                }

                message(
                    "COULDN'T ESCAPE!"
                );

                break;
            }
        }

        if(enemy->hp == 0) {

            enemy->hp =
                enemy->max_hp;

            player_mon.xp += 3;

            message(
                "WILD MONSTER DEFEATED!"
            );

            if(player_mon.xp >= 10) {

                player_mon.xp -= 10;

                player_mon.level++;

                player_mon.max_hp += 4;
                player_mon.attack++;
                player_mon.defense++;

                player_mon.hp =
                    player_mon.max_hp;

                message(
                    "FENLO LEVEL UP!"
                );
            }

            return 1;
        }

        enemy_damage =
            enemy->attack;

        if(enemy_damage <= player_mon.defense)
            enemy_damage = 1;
        else
            enemy_damage -= player_mon.defense;

        if(enemy_damage > player_mon.hp)
            enemy_damage = player_mon.hp;

        player_mon.hp -= enemy_damage;

        sound_hit();

        printf(
            "\n\n%s HIT %u!",
            enemy->name,
            enemy_damage
        );

        delay(40);

        if(player_mon.hp == 0) {

            player_mon.hp =
                player_mon.max_hp;

            message(
                "FENLO FAINTED!"
            );

            return 0;
        }
    }
}

/* =================================================
   ENCOUNTERS
   ================================================= */

void encounter(void) {

    UINT8 tile;
    UINT8 roll;

    if(current_area != AREA_MEADOW)
        return;

    tile =
        get_tile(
            player_x,
            player_y
        );

    if(tile != TILE_TALL)
        return;

    roll =
        random_number() & 7;

    if(roll != 0)
        return;

    if(random_number() & 1) {

        wild_mossen.hp =
            wild_mossen.max_hp;

        battle(&wild_mossen);

    } else {

        wild_zappit.hp =
            wild_zappit.max_hp;

        battle(&wild_zappit);
    }

    draw_map();
    draw_player();
}

/* =================================================
   AREA TRANSITIONS
   ================================================= */

void transition_area(void) {

    if(current_area == AREA_VILLAGE) {

        if(player_y == 17 &&
           player_x >= 9 &&
           player_x <= 10) {

            current_area =
                AREA_MEADOW;

            player_x = 9;
            player_y = 1;

            draw_map();
            draw_player();

            message(
                "MEADOW OF ECHOES"
            );
        }

        return;
    }

    if(current_area == AREA_MEADOW) {

        if(player_y == 0 &&
           player_x >= 9 &&
           player_x <= 10) {

            current_area =
                AREA_VILLAGE;

            player_x = 9;
            player_y = 16;

            draw_map();
            draw_player();

            return;
        }

        if(player_y == 17 &&
           player_x >= 9 &&
           player_x <= 10) {

            current_area =
                AREA_FOREST;

            player_x = 9;
            player_y = 1;

            draw_map();
            draw_player();

            message(
                "WHISPER WOODS"
            );
        }

        return;
    }

    if(current_area == AREA_FOREST) {

        if(player_y == 0 &&
           player_x >= 9 &&
           player_x <= 10) {

            current_area =
                AREA_MEADOW;

            player_x = 9;
            player_y = 16;

            draw_map();
            draw_player();
        }
    }
}

/* =================================================
   MOVEMENT
   ================================================= */

void move_player(UINT8 keys) {

    INT8 new_x;
    INT8 new_y;

    UINT8 old_x;
    UINT8 old_y;

    new_x =
        (INT8)player_x;

    new_y =
        (INT8)player_y;

    old_x =
        player_x;

    old_y =
        player_y;

    if(keys & J_UP)
        new_y--;

    else if(keys & J_DOWN)
        new_y++;

    else if(keys & J_LEFT)
        new_x--;

    else if(keys & J_RIGHT)
        new_x++;

    else
        return;

    /*
       Allow the player to stand on the
       transition border.
    */

    if(new_x < 0 ||
       new_x >= MAP_W)
        return;

    if(new_y < 0 ||
       new_y >= MAP_H)
        return;

    if(blocked(
        get_tile(
            (UINT8)new_x,
            (UINT8)new_y
        )
    ))
        return;

    player_x =
        (UINT8)new_x;

    player_y =
        (UINT8)new_y;

    sound_move();

    redraw_player_tile(
        old_x,
        old_y
    );

    encounter();

    transition_area();
}

/* =================================================
   VILLAGE INTERACTION
   ================================================= */

void village_interaction(void) {

    if(current_area != AREA_VILLAGE)
        return;

    /*
       House healing area.
    */

    if(player_x >= 5 &&
       player_x <= 7 &&
       player_y >= 2 &&
       player_y <= 4) {

        player_mon.hp =
            player_mon.max_hp;

        message(
            "FENLO IS FULLY HEALED."
        );

        return;
    }

    if(story_step == 0) {

        story_step = 1;

        message(
            "MARA: THE MEADOW IS QUIET."
        );

        message(
            "MARA: TOO QUIET."
        );

        message(
            "MARA: BE CAREFUL OUT THERE."
        );

        return;
    }

    message(
        "MARA: STAY SAFE."
    );
}

/* =================================================
   TITLE
   ================================================= */

void title_screen(void) {

    printf("\n\n");
    printf("    LITTLE WILDS");
    printf("\n\n");
    printf(" A TINY MONSTER RPG");
    printf("\n\n");
    printf("       ALPHA 2");
    printf("\n\n");
    printf("      PRESS A");

    wait_a();
}

/* =================================================
   MAIN
   ================================================= */

void main(void) {

    UINT8 keys;

    DISPLAY_OFF;

    sound_init();

    set_bkg_data(
        0,
        9,
        tile_data
    );

    BGP_REG =
        DMG_PALETTE(
            DMG_BLACK,
            DMG_DARK_GRAY,
            DMG_LITE_GRAY,
            DMG_WHITE
        );

    title_screen();

    draw_map();
    draw_player();

    DISPLAY_ON;

    while(1) {

        wait_vbl_done();

        keys =
            joypad();

        if(keys & J_A) {

            waitpadup();

            village_interaction();

            continue;
        }

        if(keys & J_B) {

            waitpadup();

            if(current_area == AREA_VILLAGE) {

                player_mon.hp =
                    player_mon.max_hp;

                message(
                    "FENLO WAS HEALED."
                );
            }

            continue;
        }

        move_player(keys);
    }
}