#include <gb/gb.h>
#include <stdio.h>

/*
    LITTLE WILDS
    ALPHA 2

    Controls:
    D-Pad = Move
    A     = Interact / battle
    B     = Heal at village

    This version focuses on:
    - Working maps
    - Working transitions
    - Proper collision
    - Wild encounters
    - Simple battles
    - HP persistence
    - XP / leveling
    - Sound effects
*/

/* -------------------------------------------------
   CONSTANTS
   ------------------------------------------------- */

#define MAP_W 20
#define MAP_H 18

#define VILLAGE 0
#define MEADOW  1
#define FOREST  2
#define MINE    3
#define RUINS   4
#define SHRINE  5

#define TILE_GRASS 0
#define TILE_PATH  1
#define TILE_TREE  2
#define TILE_WATER 3
#define TILE_TALL  4
#define TILE_ROCK  5
#define TILE_HOUSE 6
#define TILE_DOOR  7
#define TILE_CAVE  8

#define MONSTER_PLAYER 0
#define MONSTER_MOSSEN 1
#define MONSTER_ZAPPIT 2
#define MONSTER_BRINN  3
#define MONSTER_GRUFF  4
#define MONSTER_WISP   5
#define MONSTER_ECHO   6

#define BATTLE_LOSE 0
#define BATTLE_WIN  1
#define BATTLE_RUN  2

/* -------------------------------------------------
   MONSTER
   ------------------------------------------------- */

typedef struct {
    const char *name;
    UINT8 max_hp;
    UINT8 hp;
    UINT8 attack;
    UINT8 defense;
    UINT8 level;
    UINT8 xp;
} Monster;

Monster monster_data[7] = {

    /* Player */
    {
        "FENLO",
        30,
        30,
        8,
        6,
        3,
        0
    },

    {
        "MOSSEN",
        20,
        20,
        6,
        6,
        2,
        0
    },

    {
        "ZAPPIT",
        18,
        18,
        8,
        3,
        3,
        0
    },

    {
        "BRINN",
        22,
        22,
        7,
        4,
        3,
        0
    },

    {
        "GRUFF",
        30,
        30,
        9,
        7,
        4,
        0
    },

    {
        "WISP",
        24,
        24,
        10,
        4,
        5,
        0
    },

    {
        "ECHO",
        45,
        45,
        12,
        8,
        7,
        0
    }
};

/* -------------------------------------------------
   GAME STATE
   ------------------------------------------------- */

UINT8 current_area = VILLAGE;

UINT8 player_x = 9;
UINT8 player_y = 15;

UINT8 steps = 0;
UINT8 story_progress = 0;

UINT8 random_state = 37;

/* -------------------------------------------------
   TILE GRAPHICS
   ------------------------------------------------- */

/*
   Each Game Boy tile uses 16 bytes.
*/

const unsigned char tiles[9 * 16] = {

    /* 0 GRASS */
    0x00,0x00,
    0x00,0x00,
    0x08,0x00,
    0x00,0x08,
    0x00,0x00,
    0x10,0x00,
    0x00,0x00,
    0x00,0x00,

    /* 1 PATH */
    0x00,0x00,
    0x18,0x18,
    0x00,0x00,
    0x18,0x18,
    0x00,0x00,
    0x18,0x18,
    0x00,0x00,
    0x18,0x18,

    /* 2 TREE */
    0x18,0x00,
    0x3C,0x18,
    0x7E,0x3C,
    0xFF,0x7E,
    0xFF,0x7E,
    0x7E,0x3C,
    0x3C,0x18,
    0x18,0x18,

    /* 3 WATER */
    0x00,0x00,
    0x18,0x18,
    0x00,0x00,
    0x18,0x18,
    0x00,0x00,
    0x18,0x18,
    0x00,0x00,
    0x18,0x18,

    /* 4 TALL GRASS */
    0x10,0x00,
    0x10,0x10,
    0x18,0x00,
    0x10,0x10,
    0x10,0x00,
    0x10,0x10,
    0x18,0x00,
    0x10,0x10,

    /* 5 ROCK */
    0x00,0x00,
    0x18,0x00,
    0x3C,0x18,
    0x7E,0x3C,
    0x7E,0x3C,
    0x3C,0x18,
    0x18,0x00,
    0x00,0x00,

    /* 6 HOUSE */
    0x18,0x18,
    0x3C,0x3C,
    0x7E,0x7E,
    0xFF,0xFF,
    0xFF,0x81,
    0xC3,0x81,
    0xC3,0x81,
    0xFF,0xFF,

    /* 7 DOOR */
    0x3C,0x3C,
    0x42,0x42,
    0x42,0x42,
    0x42,0x42,
    0x42,0x42,
    0x42,0x42,
    0x7E,0x7E,
    0x7E,0x7E,

    /* 8 CAVE */
    0x00,0x00,
    0x18,0x18,
    0x3C,0x3C,
    0x7E,0x7E,
    0xFF,0xFF,
    0xFF,0xFF,
    0xFF,0xFF,
    0xFF,0xFF
};

/* -------------------------------------------------
   MAPS
   ------------------------------------------------- */

/*
   Maps are deliberately simple and rectangular.
   Every map contains exactly 360 tiles.
*/

const unsigned char village_map[360] = {

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

const unsigned char meadow_map[360] = {

    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    2,0,0,4,4,0,0,0,0,1,1,0,0,0,4,4,0,0,0,2,
    2,0,4,4,4,4,0,0,0,1,1,0,0,4,4,4,4,0,0,2,
    2,0,4,0,0,4,0,0,0,1,1,0,0,4,0,0,4,0,0,2,
    2,0,4,0,0,4,0,0,0,1,1,0,0,4,0,0,4,0,0,2,
    2,0,4,4,4,4,0,0,0,1,1,0,0,4,4,4,4,0,0,2,
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

/* -------------------------------------------------
   SOUND
   ------------------------------------------------- */

void sound_init(void) {

    NR52_REG = 0x80;
    NR50_REG = 0x77;
    NR51_REG = 0xFF;
}

void sound_blip(void) {

    NR21_REG = 0x80;
    NR22_REG = 0xF2;
    NR23_REG = 0x90;
    NR24_REG = 0x86;

    delay(30);
}

void sound_attack(void) {

    NR11_REG = 0x80;
    NR12_REG = 0xF3;
    NR13_REG = 0x40;
    NR14_REG = 0x86;

    delay(40);

    NR13_REG = 0x90;
    NR14_REG = 0x86;

    delay(40);
}

void sound_hit(void) {

    NR41_REG = 0x08;
    NR42_REG = 0xF3;
    NR43_REG = 0x25;
    NR44_REG = 0xC0;

    delay(45);
}

void sound_victory(void) {

    NR21_REG = 0x80;
    NR22_REG = 0xF2;

    NR23_REG = 0x80;
    NR24_REG = 0x86;

    delay(35);

    NR23_REG = 0xB0;
    NR24_REG = 0x86;

    delay(35);

    NR23_REG = 0xE0;
    NR24_REG = 0x86;

    delay(50);
}

/* -------------------------------------------------
   RANDOM NUMBER
   ------------------------------------------------- */

UINT8 random_number(void) {

    random_state++;

    random_state ^= DIV_REG;

    random_state =
        (random_state << 1) |
        (random_state >> 7);

    return random_state;
}

/* -------------------------------------------------
   MAP HELPERS
   ------------------------------------------------- */

const unsigned char *get_current_map(void) {

    if(current_area == VILLAGE)
        return village_map;

    return meadow_map;
}

UINT8 get_tile(
    UINT8 x,
    UINT8 y
) {

    const unsigned char *map;

    if(x >= MAP_W)
        return TILE_TREE;

    if(y >= MAP_H)
        return TILE_TREE;

    map = get_current_map();

    return map[
        ((UINT16)y * MAP_W) + x
    ];
}

UINT8 is_blocked(UINT8 tile) {

    if(tile == TILE_TREE)
        return 1;

    if(tile == TILE_WATER)
        return 1;

    if(tile == TILE_ROCK)
        return 1;

    if(tile == TILE_HOUSE)
        return 1;

    if(tile == TILE_CAVE)
        return 1;

    return 0;
}

/* -------------------------------------------------
   MAP DRAWING
   ------------------------------------------------- */

void draw_map(void) {

    const unsigned char *map;

    map = get_current_map();

    set_bkg_tiles(
        0,
        0,
        MAP_W,
        MAP_H,
        map
    );
}

/* -------------------------------------------------
   TEXT
   ------------------------------------------------- */

void wait_a(void) {

    while(!(joypad() & J_A))
        wait_vbl_done();

    waitpadup();
}

void message(
    const char *text
) {

    printf("\n\n%s\n\n",text);
    printf("A: CONTINUE");

    wait_a();
}

/* -------------------------------------------------
   BATTLE
   ------------------------------------------------- */

UINT8 battle(
    UINT8 enemy_id
) {

    Monster *player;
    Monster *enemy;

    UINT8 damage;
    UINT8 enemy_damage;

    player =
        &monster_data[MONSTER_PLAYER];

    enemy =
        &monster_data[enemy_id];

    battle_enemy_hp =
        enemy->max_hp;

    if(player->hp == 0)
        player->hp = player->max_hp;

    while(1) {

        printf("\n\n----------------");
        printf("\nWILD %s",enemy->name);
        printf("\nHP %u/%u",
            enemy->hp,
            enemy->max_hp
        );

        printf("\n\nFENLO");
        printf("\nHP %u/%u",
            player->hp,
            player->max_hp
        );

        printf("\n\nA: ATTACK");
        printf("\nB: RUN");

        while(1) {

            UINT8 keys;

            wait_vbl_done();

            keys = joypad();

            if(keys & J_A) {

                waitpadup();

                sound_attack();

                damage =
                    player->attack;

                if(damage < 1)
                    damage = 1;

                if(enemy->defense < damage)
                    damage -= enemy->defense;
                else
                    damage = 1;

                if(damage > enemy->hp)
                    damage = enemy->hp;

                enemy->hp -= damage;

                sound_hit();

                printf(
                    "\n\nFENLO dealt %u!",
                    damage
                );

                delay(40);

                break;
            }

            if(keys & J_B) {

                waitpadup();

                if(enemy_id == MONSTER_ECHO) {

                    message(
                        "You cannot escape."
                    );

                    break;
                }

                if(random_number() & 1) {

                    message(
                        "You escaped!"
                    );

                    enemy->hp =
                        enemy->max_hp;

                    return BATTLE_RUN;
                }

                message(
                    "You couldn't escape!"
                );

                break;
            }
        }

        if(enemy->hp == 0) {

            enemy->hp =
                enemy->max_hp;

            player->xp += 3;

            sound_victory();

            message(
                "WILD MONSTER DEFEATED!"
            );

            if(player->xp >= 10) {

                player->xp -= 10;

                player->level++;

                player->max_hp += 4;
                player->attack += 1;
                player->defense += 1;

                player->hp =
                    player->max_hp;

                sound_victory();

                message(
                    "FENLO LEVELLED UP!"
                );
            }

            return BATTLE_WIN;
        }

        enemy_damage =
            enemy->attack;

        if(enemy_damage <= player->defense)
            enemy_damage = 1;
        else
            enemy_damage -= player->defense;

        if(enemy_damage > player->hp)
            enemy_damage = player->hp;

        player->hp -= enemy_damage;

        sound_hit();

        printf(
            "\n\n%s dealt %u!",
            enemy->name,
            enemy_damage
        );

        delay(50);

        if(player->hp == 0) {

            player->hp =
                player->max_hp;

            message(
                "FENLO fainted!"
            );

            return BATTLE_LOSE;
        }
    }
}

/* -------------------------------------------------
   ENCOUNTER
   ------------------------------------------------- */

void try_encounter(void) {

    UINT8 tile;
    UINT8 roll;
    UINT8 enemy;

    if(current_area != MEADOW)
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

    enemy =
        MONSTER_MOSSEN;

    if(random_number() & 1)
        enemy = MONSTER_ZAPPIT;

    sound_blip();

    printf(
        "\n\nA wild %s appeared!",
        monster_data[enemy].name
    );

    delay(70);

    battle(enemy);

    draw_map();
}

/* -------------------------------------------------
   AREA TRANSITION
   ------------------------------------------------- */

void change_area(void) {

    if(current_area == VILLAGE) {

        if(player_y == 17) {

            current_area = MEADOW;

            player_x = 9;
            player_y = 1;

            sound_victory();

            draw_map();

            message(
                "MEADOW OF ECHOES"
            );
        }

        return;
    }

    if(current_area == MEADOW) {

        if(player_y == 0) {

            current_area = VILLAGE;

            player_x = 9;
            player_y = 16;

            sound_blip();

            draw_map();

            message(
                "BRAMBLEWICK"
            );

            return;
        }

        if(player_y == 17) {

            current_area = FOREST;

            player_x = 9;
            player_y = 1;

            sound_blip();

            draw_map();

            message(
                "WHISPER WOODS"
            );

            return;
        }
    }

    if(current_area == FOREST) {

        if(player_y == 0) {

            current_area = MEADOW;

            player_x = 9;
            player_y = 16;

            sound_blip();

            draw_map();

            message(
                "MEADOW OF ECHOES"
            );
        }
    }
}

/* -------------------------------------------------
   MOVEMENT
   ------------------------------------------------- */

void move_player(UINT8 keys) {

    INT8 new_x;
    INT8 new_y;

    new_x =
        (INT8)player_x;

    new_y =
        (INT8)player_y;

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

    if(new_x < 0)
        return;

    if(new_y < 0)
        return;

    if(new_x >= MAP_W)
        return;

    if(new_y >= MAP_H)
        return;

    if(
        is_blocked(
            get_tile(
                (UINT8)new_x,
                (UINT8)new_y
            )
        )
    )
        return;

    player_x =
        (UINT8)new_x;

    player_y =
        (UINT8)new_y;

    steps++;

    sound_blip();

    try_encounter();

    change_area();
}

/* -------------------------------------------------
   VILLAGE INTERACTION
   ------------------------------------------------- */

void village_interaction(void) {

    if(current_area != VILLAGE)
        return;

    if(
        player_x >= 5 &&
        player_x <= 7 &&
        player_y >= 2 &&
        player_y <= 4
    ) {

        monster_data[MONSTER_PLAYER].hp =
            monster_data[MONSTER_PLAYER].max_hp;

        message(
            "FENLO is fully healed."
        );

        return;
    }

    if(story_progress == 0) {

        story_progress = 1;

        message(
            "MARA: The meadow is quiet."
        );

        message(
            "MARA: Too quiet."
        );

        message(
            "MARA: Be careful out there."
        );

        return;
    }

    message(
        "MARA: Stay safe."
    );
}

/* -------------------------------------------------
   TITLE
   ------------------------------------------------- */

void title_screen(void) {

    printf("\n\n");
    printf("   LITTLE WILDS");
    printf("\n\n");
    printf(" A TINY MONSTER RPG");
    printf("\n\n");
    printf("      ALPHA 2");
    printf("\n\n");
    printf("      PRESS A");

    sound_blip();

    wait_a();
}

/* -------------------------------------------------
   MAIN
   ------------------------------------------------- */

void main(void) {

    UINT8 keys;

    DISPLAY_OFF;

    sound_init();

    set_bkg_data(
        0,
        9,
        tiles
    );

    BGP_REG =
        DMG_PALETTE(
            DMG_BLACK,
            DMG_DARK_GRAY,
            DMG_LITE_GRAY,
            DMG_WHITE
        );

    title_screen();

    DISPLAY_ON;

    draw_map();

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

            if(current_area == VILLAGE) {

                monster_data[MONSTER_PLAYER].hp =
                    monster_data[MONSTER_PLAYER].max_hp;

                message(
                    "FENLO was healed."
                );
            }

            continue;
        }

        move_player(keys);
    }
}