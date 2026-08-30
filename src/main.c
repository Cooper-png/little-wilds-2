#include <gb/gb.h>
#include <stdio.h>
#include <stdint.h>

/*
    LITTLE WILDS
    Compact Game Boy RPG
    GBDK 4.4 compatible

    Controls:
      D-PAD : Move
      A     : Interact / Attack
      B     : Menu / Run
      UP    : Party
      DOWN  : Status
*/

#define MAP_W 20
#define MAP_H 18

#define VILLAGE 0
#define MEADOW  1
#define FOREST  2
#define MINE    3
#define RUINS   4
#define SHRINE  5

#define GRASS  0
#define PATH   1
#define TREE   2
#define WATER  3
#define FLOWER 4
#define TALL   5
#define HOUSE  6
#define ROCK   7
#define CAVE   9
#define SIGN   10

#define MOVE_STRIKE 0
#define MOVE_GUARD  1
#define MOVE_BITE   2
#define MOVE_VINE   3
#define MOVE_SPARK  4
#define MOVE_WIND   5
#define MOVE_CRUSH  6
#define MOVE_GLOW   7

#define PARTY_SIZE 3
#define MAX_MONSTERS 12

typedef struct {
    const char *name;
    uint8_t power;
} Move;

typedef struct {
    const char *name;
    uint8_t hp;
    uint8_t max_hp;
    uint8_t atk;
    uint8_t def;
    uint8_t spd;
    uint8_t level;
    uint8_t xp;
    uint8_t move1;
    uint8_t move2;
    uint8_t owned;
} Monster;

/* -------------------------------------------------
   MOVES
   ------------------------------------------------- */

const Move moves[8] = {
    {"STRIKE", 6},
    {"GUARD",  0},
    {"BITE",   7},
    {"VINE",   7},
    {"SPARK",  8},
    {"WIND",   6},
    {"CRUSH", 10},
    {"GLOW",   9}
};

/* -------------------------------------------------
   MONSTERS
   ------------------------------------------------- */

Monster mons[MAX_MONSTERS] = {
    {"FENLO",   24,24,7,5,7,3,0,MOVE_STRIKE,MOVE_GUARD,1},
    {"MOSSEN",  20,20,6,7,4,2,0,MOVE_VINE,MOVE_GUARD,0},
    {"ZAPPIT",  18,18,8,3,9,3,0,MOVE_SPARK,MOVE_BITE,0},
    {"BRINN",   14,14,5,3,6,2,0,MOVE_BITE,MOVE_GUARD,0},
    {"MOSSEL",  18,18,5,6,3,2,0,MOVE_VINE,MOVE_GUARD,0},
    {"RILL",    22,22,7,6,5,3,0,MOVE_STRIKE,MOVE_WIND,0},
    {"GRUFF",   28,28,8,7,2,4,0,MOVE_BITE,MOVE_CRUSH,0},
    {"EMBERN",  17,17,9,3,8,4,0,MOVE_SPARK,MOVE_STRIKE,0},
    {"THORN",   25,25,7,8,4,4,0,MOVE_VINE,MOVE_CRUSH,0},
    {"WISP",    20,20,10,4,10,5,0,MOVE_GLOW,MOVE_WIND,0},
    {"BRAMBLE", 32,32,11,9,5,6,0,MOVE_VINE,MOVE_CRUSH,0},
    {"ECHO",    38,38,12,10,12,7,0,MOVE_GLOW,MOVE_SPARK,0}
};

/* -------------------------------------------------
   MAPS
   ------------------------------------------------- */

/*
   IMPORTANT:
   Maps are stored as uint8_t arrays.
   They are copied into VRAM with set_bkg_tiles().
*/

const uint8_t village_map[MAP_W * MAP_H] = {
    2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,6,6,6,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,6,8,6,0,1,1,0,0,0,0,0,0,0,0,2,
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

const uint8_t meadow_map[MAP_W * MAP_H] = {
    2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,
    2,0,0,5,5,5,0,0,0,1,1,0,0,0,5,5,5,0,0,2,
    2,0,5,5,5,5,0,0,0,1,1,0,0,5,5,5,5,0,0,2,
    2,0,5,0,0,5,0,0,0,1,1,0,0,5,0,0,5,0,0,2,
    2,0,5,0,0,5,0,0,0,1,1,0,0,5,0,0,5,0,0,2,
    2,0,5,0,0,5,0,0,0,1,1,0,0,5,0,0,5,0,0,2,
    2,0,5,5,5,5,0,0,0,1,1,0,0,5,5,5,5,0,0,2,
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

const uint8_t forest_map[MAP_W * MAP_H] = {
    2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,
    2,2,0,0,5,0,0,2,2,1,1,2,2,0,0,5,0,0,2,2,
    2,0,0,5,5,5,0,0,2,1,1,2,0,0,5,5,5,0,0,2,
    2,0,5,5,0,5,5,0,0,1,1,0,0,5,5,0,5,5,0,2,
    2,0,5,0,0,0,5,0,0,1,1,0,0,5,0,0,0,5,0,2,
    2,0,5,0,7,0,5,0,0,1,1,0,0,5,0,7,0,5,0,2,
    2,0,5,5,5,5,5,0,0,1,1,0,0,5,5,5,5,5,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,2,0,0,5,0,0,2,2,1,1,2,2,0,0,5,0,0,2,2,
    2,0,0,5,5,5,0,0,2,1,1,2,0,0,5,5,5,0,0,2,
    2,0,5,5,0,5,5,0,0,1,1,0,0,5,5,0,5,5,0,2,
    2,0,5,0,0,0,5,0,0,1,1,0,0,5,0,0,0,5,0,2,
    2,0,5,0,7,0,5,0,0,1,1,0,0,5,0,7,0,5,0,2,
    2,0,5,5,5,5,5,0,0,1,1,0,0,5,5,5,5,5,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2
};

const uint8_t mine_map[MAP_W * MAP_H] = {
    2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,
    2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2,
    2,9,9,7,9,9,9,9,9,1,1,9,9,9,7,9,9,9,9,2,
    2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2,
    2,9,7,9,9,9,9,7,9,1,1,9,9,9,9,9,7,9,9,2,
    2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2,
    2,9,9,9,9,7,9,9,9,1,1,9,9,7,9,9,9,9,9,2,
    2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2,
    2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2,
    2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2,
    2,9,9,7,9,9,9,9,9,1,1,9,9,9,7,9,9,9,9,2,
    2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2,
    2,9,7,9,9,9,9,7,9,1,1,9,9,9,9,9,7,9,9,2,
    2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2,
    2,9,9,9,9,7,9,9,9,1,1,9,9,7,9,9,9,9,9,2,
    2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2,
    2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2,
    2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2
};

const uint8_t ruins_map[MAP_W * MAP_H] = {
    2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,
    2,0,0,0,7,0,0,0,0,1,1,0,0,0,7,0,0,0,0,2,
    2,0,7,0,0,0,7,0,0,1,1,0,0,7,0,0,7,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,7,0,0,7,0,0,1,1,0,0,0,7,0,0,7,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,7,0,0,0,7,0,0,1,1,0,0,7,0,0,7,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,7,0,0,0,7,0,0,1,1,0,0,7,0,0,7,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,7,0,0,7,0,0,1,1,0,0,0,7,0,0,7,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,7,0,0,0,7,0,0,1,1,0,0,7,0,0,7,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2
};

const uint8_t shrine_map[MAP_W * MAP_H] = {
    2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,7,0,0,0,1,1,0,0,0,7,0,0,0,0,2,
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
    2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2,
    2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2
};

/* -------------------------------------------------
   GAME STATE
   ------------------------------------------------- */

uint8_t area = VILLAGE;
uint8_t player_x = 9;
uint8_t player_y = 15;

uint8_t story = 0;
uint8_t steps = 0;

uint8_t party[PARTY_SIZE] = {0,255,255};
uint8_t party_count = 1;

uint8_t potions = 3;
uint8_t balls = 6;

uint8_t battle_enemy;
uint8_t battle_enemy_hp;
uint8_t battle_player_hp;
uint8_t guarding;

/* -------------------------------------------------
   BASIC UI
   ------------------------------------------------- */

void wait_a(void) {
    while(!(joypad() & J_A)) {
        vsync();
    }

    waitpadup();
}

void clear_screen(void) {
    fill_bkg_rect(0,0,20,18,0);
}

void pause_text(void) {
    printf("\n\nA: CONTINUE");
    wait_a();
}

void message(const char *line1, const char *line2) {
    clear_screen();

    printf("\n%s",line1);

    if(line2)
        printf("\n\n%s",line2);

    pause_text();
}

/* -------------------------------------------------
   MAP ACCESS
   ------------------------------------------------- */

const uint8_t *current_map(void) {
    switch(area) {
        case VILLAGE: return village_map;
        case MEADOW:  return meadow_map;
        case FOREST:  return forest_map;
        case MINE:    return mine_map;
        case RUINS:   return ruins_map;
        default:      return shrine_map;
    }
}

uint8_t tile_at(uint8_t x, uint8_t y) {
    if(x >= MAP_W || y >= MAP_H)
        return TREE;

    return current_map()[(uint16_t)y * MAP_W + x];
}

uint8_t blocked(uint8_t tile) {
    if(tile == TREE)  return 1;
    if(tile == WATER) return 1;
    if(tile == HOUSE) return 1;
    if(tile == ROCK)  return 1;
    if(tile == CAVE)  return 1;

    return 0;
}

const char *area_name(void) {
    switch(area) {
        case VILLAGE: return "BRAMBLEWICK";
        case MEADOW:  return "MEADOW";
        case FOREST:  return "WHISPER FOREST";
        case MINE:    return "OLD MINE";
        case RUINS:   return "SUNKEN RUINS";
        default:      return "ECHO SHRINE";
    }
}

/* -------------------------------------------------
   GRAPHICS
   ------------------------------------------------- */

const uint8_t player_tile[16] = {
    0x18,0x18,
    0x3C,0x3C,
    0x7E,0x7E,
    0xDB,0xFF,
    0xFF,0xDB,
    0x7E,0x7E,
    0x3C,0x3C,
    0x24,0x24
};

const uint8_t npc_tile[16] = {
    0x3C,0x3C,
    0x42,0x42,
    0xA5,0xA5,
    0x81,0x81,
    0x7E,0x7E,
    0x18,0x18,
    0x24,0x24,
    0x42,0x42
};

void graphics_init(void) {
    set_sprite_data(0,1,player_tile);
    set_sprite_data(1,1,npc_tile);

    set_sprite_tile(0,0);
    set_sprite_tile(1,1);

    SHOW_BKG;
    SHOW_SPRITES;
}

void draw_player(void) {
    move_sprite(
        0,
        (player_x << 3) + 8,
        (player_y << 3) + 16
    );
}

void draw_map(void) {
    const uint8_t *map = current_map();

    set_bkg_tiles(
        0,
        0,
        MAP_W,
        MAP_H,
        map
    );

    draw_player();

    move_sprite(1,0,0);
}

/* -------------------------------------------------
   PARTY
   ------------------------------------------------- */

Monster *lead(void) {
    return &mons[party[0]];
}

void heal_party(void) {
    uint8_t i;

    for(i=0;i<party_count;i++)
        mons[party[i]].hp =
            mons[party[i]].max_hp;
}

void give_xp(uint8_t amount) {
    Monster *m = lead();

    m->xp += amount;

    if(m->xp >= 10) {
        m->xp -= 10;

        m->level++;
        m->max_hp += 3;
        m->atk++;
        m->def++;
        m->spd++;

        m->hp = m->max_hp;

        message(
            "LEVEL UP!",
            m->name
        );
    }
}

/* -------------------------------------------------
   AREA TRANSITIONS
   ------------------------------------------------- */

void enter_area(uint8_t new_area) {
    area = new_area;

    player_x = 9;
    player_y = 16;
    steps = 0;

    draw_map();

    message(
        area_name(),
        "A new path lies ahead."
    );

    draw_map();
}

/* -------------------------------------------------
   STORY
   ------------------------------------------------- */

void story_event(void) {
    if(area == VILLAGE && story == 0) {

        message(
            "OLD MARA",
            "The meadow has been quiet."
        );

        message(
            "OLD MARA",
            "Now it whispers at night."
        );

        message(
            "OLD MARA",
            "Take Fenlo with you."
        );

        story = 1;
    }

    else if(area == FOREST && story == 1) {

        message(
            "WHISPER FOREST",
            "Something moves beyond the trees."
        );

        message(
            "WHISPER FOREST",
            "The northern path opens."
        );

        story = 2;
    }

    else if(area == MINE && story == 2) {

        message(
            "OLD MINE",
            "The tunnel descends deeper."
        );

        story = 3;
    }

    else if(area == RUINS && story == 3) {

        message(
            "SUNKEN RUINS",
            "Ancient symbols cover the walls."
        );

        message(
            "SUNKEN RUINS",
            "Something answers below."
        );

        story = 4;
    }
}

/* -------------------------------------------------
   WILD ENCOUNTERS
   ------------------------------------------------- */

uint8_t wild_monster(void) {
    if(area == MEADOW)
        return (steps & 1) ? 3 : 4;

    if(area == FOREST)
        return (steps & 1) ? 6 : 8;

    if(area == MINE)
        return (steps & 1) ? 5 : 10;

    if(area == RUINS)
        return (steps & 1) ? 9 : 11;

    return 3;
}

uint8_t can_encounter(void) {
    if(area == VILLAGE)
        return 0;

    if(area == SHRINE)
        return 0;

    if(
        (area == MEADOW || area == FOREST) &&
        tile_at(player_x,player_y) != TALL
    )
        return 0;

    return 1;
}

/* -------------------------------------------------
   BATTLE DAMAGE
   ------------------------------------------------- */

uint8_t damage_calc(
    Monster *attacker,
    Monster *defender,
    uint8_t move
) {
    uint16_t damage;

    if(move == MOVE_GUARD)
        return 0;

    damage =
        attacker->atk +
        moves[move].power +
        attacker->level;

    if(damage > defender->def)
        damage -= defender->def / 2;
    else
        damage = 1;

    if(damage > 255)
        damage = 255;

    return (uint8_t)damage;
}

/* -------------------------------------------------
   BATTLE UI
   ------------------------------------------------- */

void battle_status(Monster *enemy) {
    clear_screen();

    printf(
        "\nWILD %s",
        enemy->name
    );

    printf(
        "\nLV %u",
        enemy->level
    );

    printf(
        "\nHP %u/%u",
        battle_enemy_hp,
        enemy->max_hp
    );

    printf(
        "\n\n%s",
        lead()->name
    );

    printf(
        "\nLV %u",
        lead()->level
    );

    printf(
        "\nHP %u/%u",
        battle_player_hp,
        lead()->max_hp
    );
}

uint8_t battle_menu(void) {
    uint8_t key;

    clear_screen();

    printf("\nA ATTACK");
    printf("\nUP POTION");
    printf("\nDOWN BALL");
    printf("\nB RUN");

    while(1) {

        key = joypad();

        if(key & J_A) {
            waitpadup();
            return 0;
        }

        if(key & J_UP) {
            waitpadup();
            return 1;
        }

        if(key & J_DOWN) {
            waitpadup();
            return 2;
        }

        if(key & J_B) {
            waitpadup();
            return 3;
        }

        vsync();
    }
}

/* -------------------------------------------------
   BATTLE ACTIONS
   ------------------------------------------------- */

void enemy_attack(Monster *enemy) {
    uint8_t damage;

    damage =
        damage_calc(
            enemy,
            lead(),
            enemy->move1
        );

    if(guarding) {
        damage >>= 1;
        guarding = 0;
    }

    if(damage >= battle_player_hp)
        battle_player_hp = 0;
    else
        battle_player_hp -= damage;
}

uint8_t capture_attempt(void) {
    uint8_t chance;

    if(!balls)
        return 0;

    balls--;

    chance = 25;

    if(
        battle_enemy_hp <
        mons[battle_enemy].max_hp / 2
    )
        chance = 60;

    /*
       Deterministic pseudo-random behaviour.
       Good enough for a tiny GB prototype.
    */
    if(
        (steps +
         player_x +
         player_y +
         battle_enemy * 7) % 100
        < chance
    )
        return 1;

    return 0;
}

void capture_enemy(void) {
    Monster *m = &mons[battle_enemy];

    m->owned = 1;

    if(party_count < PARTY_SIZE) {

        party[party_count] = battle_enemy;
        party_count++;

        message(
            "CAUGHT!",
            m->name
        );
    }
    else {
        message(
            "CAUGHT!",
            "Your party is full."
        );
    }
}

void battle_win(void) {
    uint8_t xp =
        mons[battle_enemy].level + 3;

    clear_screen();

    printf(
        "\n%s DEFEATED!",
        mons[battle_enemy].name
    );

    printf(
        "\n\nXP +%u",
        xp
    );

    pause_text();

    give_xp(xp);

    draw_map();
}

void battle_lose(void) {
    message(
        "FENLO FAINTED",
        "You return to Bramblewick."
    );

    heal_party();

    area = VILLAGE;
    player_x = 9;
    player_y = 15;

    draw_map();
}

/* -------------------------------------------------
   BATTLE LOOP
   ------------------------------------------------- */

void battle(void) {
    Monster *enemy;
    uint8_t option;
    uint8_t move;
    uint8_t damage;

    enemy =
        &mons[battle_enemy];

    battle_enemy_hp =
        enemy->max_hp;

    battle_player_hp =
        lead()->hp;

    guarding = 0;

    while(1) {

        battle_status(enemy);

        option = battle_menu();

        /* RUN */
        if(option == 3) {

            message(
                "ESCAPED",
                "You got away."
            );

            draw_map();
            return;
        }

        /* POTION */
        if(option == 1) {

            if(!potions) {

                message(
                    "BAG",
                    "No potions."
                );

                continue;
            }

            potions--;

            battle_player_hp += 8;

            if(
                battle_player_hp >
                lead()->max_hp
            )
                battle_player_hp =
                    lead()->max_hp;

            message(
                "POTION",
                "HP restored."
            );

            enemy_attack(enemy);
        }

        /* BALL */
        else if(option == 2) {

            if(!balls) {

                message(
                    "BAG",
                    "No wild balls."
                );

                continue;
            }

            if(capture_attempt()) {

                capture_enemy();

                draw_map();
                return;
            }

            message(
                "WILD BALL",
                "It broke free!"
            );

            enemy_attack(enemy);
        }

        /* ATTACK */
        else {

            move = lead()->move1;

            if(
                (steps & 3) == 0
            )
                move = lead()->move2;

            if(move == MOVE_GUARD) {

                guarding = 1;

                message(
                    "GUARD",
                    "You brace yourself."
                );
            }

            else {

                damage =
                    damage_calc(
                        lead(),
                        enemy,
                        move
                    );

                if(damage >= battle_enemy_hp)
                    battle_enemy_hp = 0;
                else
                    battle_enemy_hp -= damage;

                clear_screen();

                printf(
                    "\n%s used",
                    lead()->name
                );

                printf(
                    "\n%s!",
                    moves[move].name
                );

                printf(
                    "\n\nENEMY HP %u",
                    battle_enemy_hp
                );

                pause_text();
            }

            if(battle_enemy_hp == 0) {

                battle_win();
                return;
            }

            enemy_attack(enemy);
        }

        if(battle_player_hp == 0) {

            battle_lose();
            return;
        }
    }
}

/* -------------------------------------------------
   INTERACTION
   ------------------------------------------------- */

void interact(void) {

    if(area == VILLAGE) {

        story_event();
        return;
    }

    if(area == SHRINE) {

        if(story < 4) {

            message(
                "SHRINE",
                "The entrance is sealed."
            );

            return;
        }

        message(
            "ECHO SHRINE",
            "Something awakens."
        );

        battle_enemy = 11;

        battle();

        message(
            "THE ECHO",
            "The shrine falls silent."
        );

        return;
    }

    if(
        tile_at(player_x,player_y) == SIGN
    ) {

        message(
            area_name(),
            "The path continues north."
        );

        return;
    }

    message(
        area_name(),
        "Nothing answers."
    );
}

/* -------------------------------------------------
   EXITS
   ------------------------------------------------- */

void check_exit(void) {

    if(
        player_y != 17 ||
        (player_x != 9 &&
         player_x != 10)
    )
        return;

    if(area == VILLAGE)
        enter_area(MEADOW);

    else if(area == MEADOW)
        enter_area(FOREST);

    else if(area == FOREST)
        enter_area(MINE);

    else if(area == MINE)
        enter_area(RUINS);

    else if(area == RUINS)
        enter_area(SHRINE);

    else if(area == SHRINE)
        enter_area(RUINS);
}

/* -------------------------------------------------
   MOVEMENT
   ------------------------------------------------- */

void move_player(uint8_t direction) {

    uint8_t nx = player_x;
    uint8_t ny = player_y;

    if(direction == 0 && ny > 0)
        ny--;

    if(direction == 1 && ny < MAP_H - 1)
        ny++;

    if(direction == 2 && nx > 0)
        nx--;

    if(direction == 3 && nx < MAP_W - 1)
        nx++;

    if(
        blocked(
            tile_at(nx,ny)
        )
    )
        return;

    player_x = nx;
    player_y = ny;

    draw_player();

    check_exit();

    story_event();

    if(can_encounter()) {

        steps++;

        /*
           Encounter roughly every few
           steps while standing in tall grass.
        */
        if(steps >= 5) {

            steps = 0;

            battle_enemy =
                wild_monster();

            battle();
        }
    }
}

/* -------------------------------------------------
   PARTY SCREEN
   ------------------------------------------------- */

void party_screen(void) {

    uint8_t i;

    clear_screen();

    printf("\nPARTY");
    printf("\n----------------");

    for(i=0;i<party_count;i++) {

        Monster *m =
            &mons[party[i]];

        printf(
            "\n\n%u %s",
            i + 1,
            m->name
        );

        printf(
            "\nLV %u",
            m->level
        );

        printf(
            "\nHP %u/%u",
            m->hp,
            m->max_hp
        );
    }

    pause_text();

    draw_map();
}

/* -------------------------------------------------
   BAG
   ------------------------------------------------- */

void bag(void) {

    uint8_t key;

    while(1) {

        clear_screen();

        printf("\nBAG");
        printf("\n----------------");

        printf("\nPOTION x%u",potions);
        printf("\nBALL   x%u",balls);

        printf("\n\nA HEAL");
        printf("\nB BACK");

        while(1) {

            key = joypad();

            if(key & J_A) {

                waitpadup();

                if(potions) {

                    potions--;

                    heal_party();

                    message(
                        "POTION",
                        "Your party is healed."
                    );
                }

                else {

                    message(
                        "BAG",
                        "No potions."
                    );
                }

                break;
            }

            if(key & J_B) {

                waitpadup();

                draw_map();
                return;
            }

            vsync();
        }
    }
}

/* -------------------------------------------------
   STATUS
   ------------------------------------------------- */

void status_screen(void) {

    Monster *m = lead();

    clear_screen();

    printf("\nLITTLE WILDS");
    printf("\n----------------");

    printf(
        "\nAREA: %s",
        area_name()
    );

    printf(
        "\n\n%s",
        m->name
    );

    printf(
        "\nLV %u",
        m->level
    );

    printf(
        "\nHP %u/%u",
        m->hp,
        m->max_hp
    );

    printf(
        "\nATK %u",
        m->atk
    );

    printf(
        "\nDEF %u",
        m->def
    );

    printf(
        "\nSPD %u",
        m->spd
    );

    printf(
        "\n\nPARTY %u/%u",
        party_count,
        PARTY_SIZE
    );

    pause_text();

    draw_map();
}

/* -------------------------------------------------
   MENU
   ------------------------------------------------- */

void menu(void) {

    uint8_t key;

    clear_screen();

    printf("\nMENU");
    printf("\n----------------");

    printf("\nA BAG");
    printf("\nUP PARTY");
    printf("\nDOWN STATUS");
    printf("\nB CLOSE");

    while(1) {

        key = joypad();

        if(key & J_A) {

            waitpadup();
            bag();
            return;
        }

        if(key & J_UP) {

            waitpadup();
            party_screen();
            return;
        }

        if(key & J_DOWN) {

            waitpadup();
            status_screen();
            return;
        }

        if(key & J_B) {

            waitpadup();
            draw_map();
            return;
        }

        vsync();
    }
}

/* -------------------------------------------------
   INTRO
   ------------------------------------------------- */

void intro(void) {

    clear_screen();

    printf("\n\nLITTLE WILDS");

    printf("\n\nA TINY");
    printf("\nMONSTER RPG");

    printf("\n\nPRESS A");

    wait_a();

    message(
        "BRAMBLEWICK",
        "Morning arrives quietly."
    );

    message(
        "YOU",
        "Something feels different."
    );

    message(
        "OLD MARA",
        "Take Fenlo with you."
    );
}

/* -------------------------------------------------
   INITIALIZATION
   ------------------------------------------------- */

void initialize(void) {

    uint8_t i;

    area = VILLAGE;

    player_x = 9;
    player_y = 15;

    story = 0;
    steps = 0;

    party[0] = 0;
    party[1] = 255;
    party[2] = 255;

    party_count = 1;

    potions = 3;
    balls = 6;

    for(i=0;i<MAX_MONSTERS;i++)
        mons[i].owned = 0;

    mons[0].owned = 1;

    heal_party();
}

/* -------------------------------------------------
   MAIN
   ------------------------------------------------- */

void main(void) {

    DISPLAY_OFF;

    initialize();

    graphics_init();

    intro();

    draw_map();

    DISPLAY_ON;

    while(1) {

        uint8_t key =
            joypad();

        if(key & J_UP) {

            move_player(0);
            waitpadup();
        }

        else if(key & J_DOWN) {

            move_player(1);
            waitpadup();
        }

        else if(key & J_LEFT) {

            move_player(2);
            waitpadup();
        }

        else if(key & J_RIGHT) {

            move_player(3);
            waitpadup();
        }

        else if(key & J_A) {

            waitpadup();
            interact();
        }

        else if(key & J_B) {

            waitpadup();
            menu();
        }

        vsync();
    }
}