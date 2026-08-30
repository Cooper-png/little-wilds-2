#include <gb/gb.h>
#include <stdio.h>

/*
    LITTLE WILDS
    Expanded Game Boy RPG
    Target: GBDK

    Controls:
      D-PAD = Move
      A     = Interact / Select
      B     = Menu / Back

    Progression:
      Bramblewick
          |
       Meadow
          |
       Forest
          |
        Mine
          |
        Ruins
          |
       Shrine

    The game is deliberately compact so we can spend ROM
    on gameplay rather than enormous amounts of duplicated code.
*/

#define MAP_W 20
#define MAP_H 18

#define VILLAGE 0
#define MEADOW  1
#define FOREST  2
#define MINE    3
#define RUINS   4
#define SHRINE  5

#define TILE_GRASS  0
#define TILE_PATH   1
#define TILE_TREE   2
#define TILE_WATER  3
#define TILE_FLOWER 4
#define TILE_TALL   5
#define TILE_HOUSE  6
#define TILE_ROCK   7
#define TILE_DOOR   8
#define TILE_CAVE   9
#define TILE_SIGN   10

#define MOVE_STRIKE 0
#define MOVE_GUARD  1
#define MOVE_BITE   2
#define MOVE_VINE   3
#define MOVE_SPARK  4
#define MOVE_WIND   5
#define MOVE_CRUSH  6
#define MOVE_GLOW   7

#define MAX_MONSTERS 12
#define PARTY_SIZE 3

#define ITEM_POTION 0
#define ITEM_BALL   1
#define ITEM_HERB   2

typedef struct {
    const char *name;
    UINT8 hp;
    UINT8 max_hp;
    UINT8 atk;
    UINT8 def;
    UINT8 spd;
    UINT8 level;
    UINT8 xp;
    UINT8 move1;
    UINT8 move2;
    UINT8 owned;
} Monster;

typedef struct {
    const char *name;
    UINT8 power;
} Move;

/* -------------------------------------------------
   MOVES
   ------------------------------------------------- */

const Move move_data[8] = {
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

Monster monsters[MAX_MONSTERS] = {

    /* name      hp max atk def spd lv xp m1 m2 owned */

    {"FENLO",    24,24, 7,5,7, 3,0,
        MOVE_STRIKE,MOVE_GUARD,1},

    {"MOSSEN",   20,20, 6,7,4, 2,0,
        MOVE_VINE,MOVE_GUARD,0},

    {"ZAPPIT",   18,18, 8,3,9, 3,0,
        MOVE_SPARK,MOVE_BITE,0},

    {"BRINN",    14,14, 5,3,6, 2,0,
        MOVE_BITE,MOVE_GUARD,0},

    {"MOSSEL",   18,18, 5,6,3, 2,0,
        MOVE_VINE,MOVE_GUARD,0},

    {"RILL",     22,22, 7,6,5, 3,0,
        MOVE_STRIKE,MOVE_WIND,0},

    {"GRUFF",    28,28, 8,7,2, 4,0,
        MOVE_BITE,MOVE_CRUSH,0},

    {"EMBERN",   17,17, 9,3,8, 4,0,
        MOVE_SPARK,MOVE_STRIKE,0},

    {"THORN",    25,25, 7,8,4, 4,0,
        MOVE_VINE,MOVE_CRUSH,0},

    {"WISP",     20,20,10,4,10,5,0,
        MOVE_GLOW,MOVE_WIND,0},

    {"BRAMBLE",  32,32,11,9,5, 6,0,
        MOVE_VINE,MOVE_CRUSH,0},

    {"ECHO",     38,38,12,10,12,7,0,
        MOVE_GLOW,MOVE_SPARK,0}
};

/* -------------------------------------------------
   SIMPLE TILE GRAPHICS
   ------------------------------------------------- */

const unsigned char tiles[11][16] = {

    /* grass */
    {
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00
    },

    /* path */
    {
        0x55,0x55,0x55,0x55,
        0x55,0x55,0x55,0x55,
        0x55,0x55,0x55,0x55,
        0x55,0x55,0x55,0x55
    },

    /* tree */
    {
        0x18,0x18,0x3C,0x3C,
        0x7E,0x7E,0xFF,0xFF,
        0xFF,0xFF,0x7E,0x7E,
        0x3C,0x3C,0x18,0x18
    },

    /* water */
    {
        0xAA,0x55,0xAA,0x55,
        0x55,0xAA,0x55,0xAA,
        0xAA,0x55,0xAA,0x55,
        0x55,0xAA,0x55,0xAA
    },

    /* flower */
    {
        0x00,0x18,0x18,0x3C,
        0x3C,0x18,0x18,0x00,
        0x00,0x18,0x18,0x3C,
        0x3C,0x18,0x18,0x00
    },

    /* tall grass */
    {
        0x11,0x11,0x22,0x22,
        0x11,0x11,0x22,0x22,
        0x11,0x11,0x22,0x22,
        0x11,0x11,0x22,0x22
    },

    /* house */
    {
        0xFF,0xFF,0x81,0x81,
        0xBD,0xBD,0xA5,0xA5,
        0xBD,0xBD,0x81,0x81,
        0xFF,0xFF,0xFF,0xFF
    },

    /* rock */
    {
        0x00,0x00,0x18,0x18,
        0x3C,0x3C,0x7E,0x7E,
        0x7E,0x7E,0x3C,0x3C,
        0x18,0x18,0x00,0x00
    },

    /* door */
    {
        0xFF,0xFF,0x81,0x81,
        0x99,0x99,0x99,0x99,
        0x99,0x99,0x99,0x99,
        0x81,0x81,0xFF,0xFF
    },

    /* cave */
    {
        0xFF,0xFF,0xAA,0xAA,
        0x55,0x55,0xFF,0xFF,
        0xAA,0xAA,0x55,0x55,
        0xFF,0xFF,0xAA,0xAA
    },

    /* sign */
    {
        0x00,0x18,0x18,0x18,
        0x3C,0x3C,0x7E,0x7E,
        0x7E,0x7E,0x3C,0x3C,
        0x18,0x18,0x18,0x00
    }
};

/* -------------------------------------------------
   SPRITES
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

const unsigned char npc_sprite[16] = {
    0x3C,0x3C,
    0x42,0x42,
    0xA5,0xA5,
    0x81,0x81,
    0x7E,0x7E,
    0x18,0x18,
    0x24,0x24,
    0x42,0x42
};

/* -------------------------------------------------
   GAME STATE
   ------------------------------------------------- */

UINT8 area = VILLAGE;

UINT8 player_x = 9;
UINT8 player_y = 15;

UINT8 story = 0;
UINT8 flags = 0;

UINT8 party[PARTY_SIZE] = {0,255,255};
UINT8 party_count = 1;

UINT8 potions = 3;
UINT8 balls = 6;
UINT8 herbs = 2;

UINT8 steps = 0;

UINT8 battle_enemy = 0;
UINT8 battle_enemy_hp = 0;
UINT8 battle_player_hp = 0;

UINT8 guarding = 0;

/* -------------------------------------------------
   MAP DATA

   The maps deliberately share a lot of structure.
   This keeps the ROM much smaller than storing
   huge independent maps.
   ------------------------------------------------- */

const unsigned char maps[6][MAP_W * MAP_H] = {

    /* VILLAGE */
    {
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
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
    },

    /* MEADOW */
    {
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
    },

    /* FOREST */
    {
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
    },

    /* MINE */
    {
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
    },

    /* RUINS */
    {
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
    },

    /* SHRINE */
    {
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
    }
};

/* -------------------------------------------------
   TEXT HELPERS
   ------------------------------------------------- */

void wait_a(void)
{
    waitpad(J_A);
    waitpadup();
}

void pause_text(void)
{
    printf("\n\nA: CONTINUE");
    wait_a();
}

void message(const char *a, const char *b)
{
    cls();

    printf("\n%s", a);

    if(b)
        printf("\n\n%s", b);

    pause_text();
}

/* -------------------------------------------------
   AREA NAMES
   ------------------------------------------------- */

const char *area_name(void)
{
    switch(area)
    {
        case VILLAGE: return "BRAMBLEWICK";
        case MEADOW:  return "MEADOW";
        case FOREST:  return "WHISPER FOREST";
        case MINE:    return "OLD MINE";
        case RUINS:   return "SUNKEN RUINS";
        default:      return "ECHO SHRINE";
    }
}

/* -------------------------------------------------
   MAP FUNCTIONS
   ------------------------------------------------- */

const unsigned char *get_map(void)
{
    return maps[area];
}

UINT8 tile_at(UINT8 x, UINT8 y)
{
    if(x >= MAP_W || y >= MAP_H)
        return TILE_TREE;

    return get_map()[(UINT16)y * MAP_W + x];
}

UINT8 blocked(UINT8 tile)
{
    if(tile == TILE_TREE)
        return 1;

    if(tile == TILE_WATER)
        return 1;

    if(tile == TILE_HOUSE)
        return 1;

    if(tile == TILE_ROCK)
        return 1;

    if(tile == TILE_CAVE)
        return 1;

    return 0;
}

void show_player(void)
{
    move_sprite(
        0,
        (player_x << 3) + 4,
        (player_y << 3) + 12
    );
}

void hide_npc(void)
{
    move_sprite(1,0,0);
}

void draw_map(void)
{
    set_bkg_tiles(
        0,
        0,
        MAP_W,
        MAP_H,
        get_map()
    );

    show_player();
    hide_npc();

    if(area == VILLAGE)
        move_sprite(1,52,44);
}

/* -------------------------------------------------
   PARTY
   ------------------------------------------------- */

Monster *lead_monster(void)
{
    return &monsters[party[0]];
}

void heal_lead(void)
{
    Monster *m = lead_monster();

    m->hp = m->max_hp;
    battle_player_hp = m->max_hp;
}

void add_xp(UINT8 amount)
{
    Monster *m = lead_monster();

    m->xp += amount;

    if(m->xp >= 10)
    {
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

void enter_area(UINT8 new_area)
{
    area = new_area;

    player_x = 9;
    player_y = 16;

    steps = 0;

    draw_map();

    cls();

    printf("\n%s", area_name());

    if(area == MEADOW)
        printf("\n\nThe wind moves through the grass.");

    if(area == FOREST)
        printf("\n\nThe trees swallow the sunlight.");

    if(area == MINE)
        printf("\n\nSomething drips in the darkness.");

    if(area == RUINS)
        printf("\n\nThe stones feel strangely warm.");

    if(area == SHRINE)
        printf("\n\nA pale light waits ahead.");

    pause_text();

    draw_map();
}

/* -------------------------------------------------
   STORY
   ------------------------------------------------- */

void mara(void)
{
    if(story == 0)
    {
        message(
            "OLD MARA",
            "The meadow has been quiet for weeks."
        );

        message(
            "OLD MARA",
            "Now it whispers after sunset."
        );

        message(
            "OLD MARA",
            "Take Fenlo with you."
        );

        story = 1;
    }
    else
    {
        message(
            "OLD MARA",
            "The path beyond the forest leads underground."
        );
    }
}

void sign_message(void)
{
    if(area == VILLAGE)
        message("SIGN","MEADOW -> SOUTH");

    else if(area == MEADOW)
        message("SIGN","FOREST -> NORTH");

    else if(area == FOREST)
        message("SIGN","OLD MINE -> NORTH");

    else if(area == MINE)
        message("SIGN","RUINS -> NORTH");

    else if(area == RUINS)
        message("SIGN","SHRINE -> NORTH");

    else
        message("STONE","THE WILD REMEMBERS.");
}

/* -------------------------------------------------
   ENCOUNTERS
   ------------------------------------------------- */

UINT8 wild_for_area(void)
{
    if(area == MEADOW)
    {
        if((steps & 7) == 0)
            return 5;

        if((steps & 3) == 0)
            return 4;

        return 3;
    }

    if(area == FOREST)
    {
        if((steps & 7) == 0)
            return 9;

        if((steps & 3) == 0)
            return 8;

        return 6;
    }

    if(area == MINE)
    {
        if((steps & 3) == 0)
            return 10;

        return 5;
    }

    if(area == RUINS)
    {
        if((steps & 3) == 0)
            return 11;

        return 9;
    }

    return 3;
}

UINT8 encounter_allowed(void)
{
    if(area == VILLAGE)
        return 0;

    if(area == SHRINE)
        return 0;

    if(area == MEADOW || area == FOREST)
    {
        if(tile_at(player_x,player_y) != TILE_TALL)
            return 0;
    }

    return 1;
}

/* -------------------------------------------------
   BATTLE
   ------------------------------------------------- */

UINT8 calculate_damage(
    Monster *attacker,
    Monster *defender,
    UINT8 move
)
{
    UINT16 damage;

    if(move == MOVE_GUARD)
        return 0;

    damage =
        attacker->atk +
        move_data[move].power;

    damage += attacker->level;

    if(defender->def < damage)
        damage -= defender->def / 2;
    else
        damage = 1;

    if(damage > 255)
        damage = 255;

    return (UINT8)damage;
}

void battle_screen(Monster *enemy)
{
    cls();

    printf(
        "\nWILD %s LV%u",
        enemy->name,
        enemy->level
    );

    printf(
        "\nHP %u/%u",
        battle_enemy_hp,
        enemy->max_hp
    );

    printf(
        "\n\n%s LV%u",
        lead_monster()->name,
        lead_monster()->level
    );

    printf(
        "\nHP %u/%u",
        battle_player_hp,
        lead_monster()->max_hp
    );
}

UINT8 battle_menu(void)
{
    UINT8 key;

    while(1)
    {
        printf("\n\nA ATTACK");
        printf("\nUP ITEM");
        printf("\nDOWN BALL");
        printf("\nB RUN");

        key = waitpad(
            J_A |
            J_B |
            J_UP |
            J_DOWN
        );

        waitpadup();

        if(key & J_A)
            return 0;

        if(key & J_UP)
            return 1;

        if(key & J_DOWN)
            return 2;

        if(key & J_B)
            return 3;
    }
}

void enemy_attack(Monster *enemy)
{
    UINT8 damage;

    damage = calculate_damage(
        enemy,
        lead_monster(),
        enemy->move1
    );

    if(guarding)
    {
        damage >>= 1;
        guarding = 0;
    }

    if(damage >= battle_player_hp)
        battle_player_hp = 0;
    else
        battle_player_hp -= damage;
}

UINT8 try_capture(void)
{
    UINT8 chance;

    if(!balls)
        return 0;

    balls--;

    chance = 35;

    if(battle_enemy_hp <
       monsters[battle_enemy].max_hp / 2)
        chance = 65;

    if(
        (UINT8)(
            steps +
            player_x +
            player_y +
            battle_enemy * 7
        ) < chance
    )
        return 1;

    return 0;
}

void catch_monster(void)
{
    Monster *m = &monsters[battle_enemy];

    m->owned = 1;

    if(party_count < PARTY_SIZE)
    {
        party[party_count] = battle_enemy;
        party_count++;

        message(
            "CAUGHT!",
            m->name
        );
    }
    else
    {
        message(
            "CAUGHT!",
            "Your party is full."
        );
    }
}

void win_battle(void)
{
    UINT8 xp;

    xp = monsters[battle_enemy].level + 3;

    cls();

    printf(
        "\n%s defeated!",
        monsters[battle_enemy].name
    );

    printf(
        "\n\nXP +%u",
        xp
    );

    pause_text();

    add_xp(xp);

    draw_map();
}

void lose_battle(void)
{
    message(
        "FENLO FAINTED",
        "You return to Bramblewick."
    );

    heal_lead();

    area = VILLAGE;

    player_x = 9;
    player_y = 15;

    draw_map();
}

/* -------------------------------------------------
   FULL BATTLE LOOP
   ------------------------------------------------- */

void battle(void)
{
    Monster *enemy;
    UINT8 option;
    UINT8 move;
    UINT8 damage;

    battle_enemy = wild_for_area();

    enemy = &monsters[battle_enemy];

    battle_enemy_hp = enemy->max_hp;

    battle_player_hp =
        lead_monster()->hp;

    guarding = 0;

    while(1)
    {
        battle_screen(enemy);

        option = battle_menu();

        /* RUN */
        if(option == 3)
        {
            message(
                "ESCAPE",
                "You got away."
            );

            draw_map();
            return;
        }

        /* ITEM */
        if(option == 1)
        {
            if(potions)
            {
                potions--;

                battle_player_hp += 8;

                if(
                    battle_player_hp >
                    lead_monster()->max_hp
                )
                    battle_player_hp =
                        lead_monster()->max_hp;

                message(
                    "POTION",
                    "Your wild recovered."
                );
            }
            else
            {
                message(
                    "BAG",
                    "No potions."
                );
            }

            enemy_attack(enemy);
        }

        /* BALL */
        else if(option == 2)
        {
            if(!balls)
            {
                message(
                    "BAG",
                    "No wild balls."
                );

                continue;
            }

            if(try_capture())
            {
                catch_monster();
                draw_map();
                return;
            }

            message(
                "WILD BALL",
                "The wild broke free!"
            );

            enemy_attack(enemy);
        }

        /* ATTACK */
        else
        {
            move = lead_monster()->move1;

            if((steps & 3) == 0)
                move = lead_monster()->move2;

            if(move == MOVE_GUARD)
            {
                guarding = 1;

                message(
                    "GUARD",
                    "You brace yourself."
                );
            }
            else
            {
                damage =
                    calculate_damage(
                        lead_monster(),
                        enemy,
                        move
                    );

                if(damage >= battle_enemy_hp)
                    battle_enemy_hp = 0;
                else
                    battle_enemy_hp -= damage;

                cls();

                printf(
                    "\n%s used %s!",
                    lead_monster()->name,
                    move_data[move].name
                );

                printf(
                    "\n\nEnemy HP: %u",
                    battle_enemy_hp
                );

                pause_text();
            }

            if(battle_enemy_hp == 0)
            {
                win_battle();
                return;
            }

            enemy_attack(enemy);
        }

        if(battle_player_hp == 0)
        {
            lose_battle();
            return;
        }
    }
}

/* -------------------------------------------------
   SPECIAL EVENTS
   ------------------------------------------------- */

void forest_event(void)
{
    if(story >= 2)
        return;

    story = 2;

    message(
        "THE FOREST",
        "Something large moves between the trees."
    );

    message(
        "THE FOREST",
        "The path north is open."
    );
}

void mine_event(void)
{
    if(story >= 3)
        return;

    story = 3;

    message(
        "OLD MINE",
        "The tunnels continue beneath the earth."
    );
}

void ruins_event(void)
{
    if(story >= 4)
        return;

    story = 4;

    message(
        "SUNken RUINS",
        "A familiar symbol is carved into the stone."
    );

    message(
        "SUNken RUINS",
        "Something answers from below."
    );
}

void shrine_event(void)
{
    if(story < 4)
    {
        message(
            "SHRINE",
            "The entrance is sealed."
        );

        return;
    }

    message(
        "ECHO SHRINE",
        "The air becomes completely still."
    );

    message(
        "ECHO SHRINE",
        "Something awakens."
    );

    /*
       Force the final monster instead of using
       the normal random encounter.
    */

    battle_enemy = 11;

    battle_enemy_hp =
        monsters[11].max_hp;

    battle_player_hp =
        lead_monster()->max_hp;

    battle();

    if(monsters[11].hp)
    {
        message(
            "THE ECHO",
            "The shrine falls silent."
        );

        message(
            "LITTLE WILDS",
            "You completed the journey."
        );
    }
}

/* -------------------------------------------------
   INTERACTION
   ------------------------------------------------- */

void interact(void)
{
    UINT8 tile;

    tile = tile_at(
        player_x,
        player_y
    );

    if(area == VILLAGE)
    {
        if(
            player_x >= 5 &&
            player_x <= 7 &&
            player_y >= 2 &&
            player_y <= 4
        )
        {
            mara();
            return;
        }

        sign_message();
        return;
    }

    if(area == SHRINE)
    {
        shrine_event();
        return;
    }

    if(tile == TILE_SIGN)
    {
        sign_message();
        return;
    }

    if(
        tile == TILE_TALL ||
        area == MINE ||
        area == RUINS
    )
    {
        battle();
        return;
    }

    message(
        area_name(),
        "Nothing answers."
    );
}

/* -------------------------------------------------
   MOVEMENT
   ------------------------------------------------- */

void check_exit(void)
{
    if(
        player_y != 17 ||
        (player_x != 9 &&
         player_x != 10)
    )
        return;

    if(area == VILLAGE)
        enter_area(MEADOW);

    else if(area == MEADOW)
        enter_area(VILLAGE);

    else if(area == FOREST)
        enter_area(MEADOW);

    else if(area == MINE)
        enter_area(FOREST);

    else if(area == RUINS)
        enter_area(MINE);

    else if(area == SHRINE)
        enter_area(RUINS);
}

void move_player(UINT8 direction)
{
    UINT8 nx = player_x;
    UINT8 ny = player_y;

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

    show_player();

    check_exit();

    if(area == FOREST)
        forest_event();

    if(area == MINE)
        mine_event();

    if(area == RUINS)
        ruins_event();

    if(encounter_allowed())
    {
        steps++;

        if(steps >= 4)
        {
            steps = 0;
            battle();
        }
    }
}

/* -------------------------------------------------
   PARTY SCREEN
   ------------------------------------------------- */

void party_screen(void)
{
    UINT8 i;

    cls();

    printf("\nPARTY\n");
    printf("----------------");

    for(i = 0; i < party_count; i++)
    {
        Monster *m =
            &monsters[party[i]];

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

        printf(
            "\nXP %u",
            m->xp
        );
    }

    pause_text();

    draw_map();
}

/* -------------------------------------------------
   BAG
   ------------------------------------------------- */

void bag(void)
{
    UINT8 key;

    while(1)
    {
        cls();

        printf("\nBAG");
        printf("\n----------------");

        printf("\nPOTION  x%u",potions);
        printf("\nBALL    x%u",balls);
        printf("\nHERB    x%u",herbs);

        printf("\n\nA HEAL");
        printf("\nB BACK");

        key =
            waitpad(J_A | J_B);

        waitpadup();

        if(key & J_B)
        {
            draw_map();
            return;
        }

        if(key & J_A)
        {
            if(potions)
            {
                potions--;

                heal_lead();

                message(
                    "POTION",
                    "Fenlo is fully healed."
                );
            }
            else
            {
                message(
                    "BAG",
                    "No potions."
                );
            }
        }
    }
}

/* -------------------------------------------------
   STATUS
   ------------------------------------------------- */

void status_screen(void)
{
    Monster *m =
        lead_monster();

    cls();

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
        "\n\nCAUGHT %u/%u",
        party_count,
        MAX_MONSTERS
    );

    pause_text();

    draw_map();
}

/* -------------------------------------------------
   MAIN MENU
   ------------------------------------------------- */

void menu(void)
{
    UINT8 key;

    while(1)
    {
        cls();

        printf("\nMENU");
        printf("\n----------------");

        printf("\nA BAG");
        printf("\nUP PARTY");
        printf("\nDOWN STATUS");
        printf("\nB CLOSE");

        key =
            waitpad(
                J_A |
                J_B |
                J_UP |
                J_DOWN
            );

        waitpadup();

        if(key & J_B)
        {
            draw_map();
            return;
        }

        if(key & J_A)
        {
            bag();
            return;
        }

        if(key & J_UP)
        {
            party_screen();
            return;
        }

        if(key & J_DOWN)
        {
            status_screen();
            return;
        }
    }
}

/* -------------------------------------------------
   INTRO
   ------------------------------------------------- */

void intro(void)
{
    cls();

    printf("\n\n");
    printf(" LITTLE WILDS");

    printf("\n\n");
    printf(" THE MEADOW");
    printf(" OF ECHOES");

    printf("\n\n");
    printf(" A: START");

    wait_a();

    message(
        "BRAMBLEWICK",
        "Morning arrives quietly."
    );

    message(
        "YOU",
        "Something feels different today."
    );

    message(
        "OLD MARA",
        "Take Fenlo with you."
    );
}

/* -------------------------------------------------
   INITIALIZATION
   ------------------------------------------------- */

void initialize(void)
{
    UINT8 i;

    area = VILLAGE;

    player_x = 9;
    player_y = 15;

    story = 0;
    flags = 0;

    party[0] = 0;
    party[1] = 255;
    party[2] = 255;

    party_count = 1;

    potions = 3;
    balls = 6;
    herbs = 2;

    steps = 0;

    for(i = 0; i < MAX_MONSTERS; i++)
        monsters[i].owned = 0;

    monsters[0].owned = 1;

    heal_lead();
}

/* -------------------------------------------------
   GRAPHICS
   ------------------------------------------------- */

void initialize_graphics(void)
{
    set_bkg_data(
        0,
        11,
        tiles
    );

    set_sprite_data(
        0,
        1,
        player_sprite
    );

    set_sprite_data(
        1,
        1,
        npc_sprite
    );

    set_sprite_tile(
        0,
        0
    );

    set_sprite_tile(
        1,
        1
    );
}

/* -------------------------------------------------
   OVERWORLD
   ------------------------------------------------- */

void overworld(void)
{
    UINT8 key;

    while(1)
    {
        key = joypad();

        if(key & J_UP)
        {
            move_player(0);
            waitpadup();
        }

        else if(key & J_DOWN)
        {
            move_player(1);
            waitpadup();
        }

        else if(key & J_LEFT)
        {
            move_player(2);
            waitpadup();
        }

        else if(key & J_RIGHT)
        {
            move_player(3);
            waitpadup();
        }

        else if(key & J_A)
        {
            waitpadup();
            interact();
        }

        else if(key & J_B)
        {
            waitpadup();
            menu();
        }

        vsync();
    }
}

/* -------------------------------------------------
   MAIN
   ------------------------------------------------- */

void main(void)
{
    DISPLAY_OFF;

    initialize();

    initialize_graphics();

    SHOW_BKG;
    SHOW_SPRITES;

    intro();

    draw_map();

    DISPLAY_ON;

    overworld();
}