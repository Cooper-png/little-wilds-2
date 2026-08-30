#include <gb/gb.h>
#include <stdio.h>

/*
====================================================
                    LITTLE WILDS
                 THE MEADOW OF ECHOES
                      VERSION 0.5

        Original Game Boy monster RPG

        CONTROLS
        D-PAD  : Move
        A      : Confirm / Interact
        B      : Cancel / Run

====================================================
*/

/* ==================================================
   CONSTANTS
   ================================================== */

#define MAP_W 20
#define MAP_H 18

#define VILLAGE 0
#define MEADOW  1
#define FOREST  2
#define CAVE    3
#define SHRINE  4

#define GRASS   0
#define PATH    1
#define TREE    2
#define WATER   3
#define FLOWER  4
#define TALL    5
#define HOUSE   6
#define ROCK    7
#define DOOR    8
#define DARK    9

#define MAX_PARTY 3
#define MAX_MONSTERS 12
#define MAX_ITEMS 8

#define ITEM_POTION 0
#define ITEM_BALL   1
#define ITEM_HERB   2

#define MOVE_STRIKE 0
#define MOVE_GUARD  1
#define MOVE_SCRATCH 2
#define MOVE_SPARK  3
#define MOVE_BITE   4
#define MOVE_VINE   5

/* ==================================================
   GLOBAL GAME STATE
   ================================================== */

UINT8 current_area = VILLAGE;

UINT8 player_x = 9;
UINT8 player_y = 15;

UINT8 story_step = 0;

UINT8 party_count = 1;

UINT8 item_potions = 3;
UINT8 item_balls = 5;
UINT8 item_herbs = 2;

UINT8 total_caught = 0;

UINT8 facing = 0;

UINT8 menu_cursor = 0;

UINT8 game_running = 1;

/* ==================================================
   MONSTER STRUCTURE
   ================================================== */

typedef struct
{
    const char *name;
    UINT8 max_hp;
    UINT8 attack;
    UINT8 defense;
    UINT8 speed;
    UINT8 level;
    UINT8 xp;
    UINT8 move1;
    UINT8 move2;
} Monster;

/* ==================================================
   MONSTER DATA
   ================================================== */

Monster player_monster =
{
    "FENLO",
    22,
    7,
    5,
    6,
    3,
    0,
    MOVE_STRIKE,
    MOVE_GUARD
};

Monster party_two =
{
    "MOSSEN",
    18,
    6,
    7,
    3,
    2,
    0,
    MOVE_VINE,
    MOVE_GUARD
};

Monster party_three =
{
    "ZAPPIT",
    16,
    8,
    3,
    8,
    3,
    0,
    MOVE_SPARK,
    MOVE_SCRATCH
};

Monster wild_brinn =
{
    "BRINN",
    13,
    5,
    3,
    5,
    2,
    0,
    MOVE_BITE,
    MOVE_GUARD
};

Monster wild_mossel =
{
    "MOSSEL",
    17,
    5,
    6,
    3,
    2,
    0,
    MOVE_VINE,
    MOVE_GUARD
};

Monster wild_zappit =
{
    "ZAPPIT",
    11,
    8,
    2,
    8,
    3,
    0,
    MOVE_SPARK,
    MOVE_SCRATCH
};

Monster wild_rill =
{
    "RILL",
    20,
    6,
    6,
    4,
    3,
    0,
    MOVE_STRIKE,
    MOVE_GUARD
};

Monster wild_gruff =
{
    "GRUFF",
    25,
    8,
    7,
    2,
    4,
    0,
    MOVE_BITE,
    MOVE_STRIKE
};

Monster wild_ember =
{
    "EMBERN",
    15,
    9,
    3,
    7,
    4,
    0,
    MOVE_STRIKE,
    MOVE_SPARK
};

Monster wild_thorn =
{
    "THORN",
    23,
    7,
    8,
    3,
    4,
    0,
    MOVE_VINE,
    MOVE_GUARD
};

Monster wild_wisp =
{
    "WISP",
    18,
    10,
    4,
    9,
    5,
    0,
    MOVE_SPARK,
    MOVE_BITE
};

/* ==================================================
   MAP DATA
   ================================================== */

const unsigned char village[MAP_H][MAP_W] =
{
 {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,6,6,6,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,6,4,6,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,6,6,6,0,1,1,0,0,0,0,0,0,0,0,2},
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
 {2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2}
};

const unsigned char meadow[MAP_H][MAP_W] =
{
 {2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2},
 {2,0,0,5,5,5,0,0,0,1,1,0,0,0,5,5,5,0,0,2},
 {2,0,5,5,5,5,0,0,0,1,1,0,0,5,5,5,5,0,0,2},
 {2,0,5,0,0,5,0,0,0,1,1,0,0,5,0,0,5,0,0,2},
 {2,0,5,0,0,5,0,0,0,1,1,0,0,5,0,0,5,0,0,2},
 {2,0,5,0,0,5,0,0,0,1,1,0,0,5,0,0,5,0,0,2},
 {2,0,5,5,5,5,0,0,0,1,1,0,0,5,5,5,5,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,5,5,0,0,0,0,1,1,0,0,0,5,5,0,0,0,2},
 {2,0,5,5,5,0,0,0,0,1,1,0,0,5,5,0,0,0,0,2},
 {2,0,5,0,5,0,0,0,0,1,1,0,0,5,0,0,0,0,0,2},
 {2,0,5,0,5,0,0,0,0,1,1,0,0,5,0,0,0,0,0,2},
 {2,0,5,5,5,0,0,0,0,1,1,0,0,5,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2}
};

/*
    Forest
*/
const unsigned char forest[MAP_H][MAP_W] =
{
 {2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2},
 {2,2,0,0,5,0,0,2,2,1,1,2,2,0,0,5,0,0,2,2},
 {2,0,0,5,5,5,0,0,2,1,1,2,0,0,5,5,5,0,0,2},
 {2,0,5,5,0,5,5,0,0,1,1,0,0,5,5,0,5,5,0,2},
 {2,0,5,0,0,0,5,0,0,1,1,0,0,5,0,0,0,5,0,2},
 {2,0,5,0,7,0,5,0,0,1,1,0,0,5,0,7,0,5,0,2},
 {2,0,5,5,5,5,5,0,0,1,1,0,0,5,5,5,5,5,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,2,0,0,5,0,0,2,2,1,1,2,2,0,0,5,0,0,2,2},
 {2,0,0,5,5,5,0,0,2,1,1,2,0,0,5,5,5,0,0,2},
 {2,0,5,5,0,5,5,0,0,1,1,0,0,5,5,0,5,5,0,2},
 {2,0,5,0,0,0,5,0,0,1,1,0,0,5,0,0,0,5,0,2},
 {2,0,5,0,7,0,5,0,0,1,1,0,0,5,0,7,0,5,0,2},
 {2,0,5,5,5,5,5,0,0,1,1,0,0,5,5,5,5,5,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2}
};

/*
    Cave
*/
const unsigned char cave[MAP_H][MAP_W] =
{
 {2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2},
 {2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2},
 {2,9,9,7,9,9,9,9,9,1,1,9,9,9,7,9,9,9,9,2},
 {2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2},
 {2,9,7,9,9,9,9,7,9,1,1,9,9,9,9,9,7,9,9,2},
 {2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2},
 {2,9,9,9,9,7,9,9,9,1,1,9,9,7,9,9,9,9,9,2},
 {2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2},
 {2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2},
 {2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2},
 {2,9,9,7,9,9,9,9,9,1,1,9,9,9,7,9,9,9,9,2},
 {2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2},
 {2,9,7,9,9,9,9,7,9,1,1,9,9,9,9,9,7,9,9,2},
 {2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2},
 {2,9,9,9,9,7,9,9,9,1,1,9,9,7,9,9,9,9,9,2},
 {2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2},
 {2,9,9,9,9,9,9,9,9,1,1,9,9,9,9,9,9,9,9,2},
 {2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2}
};

/*
    Shrine
*/
const unsigned char shrine[MAP_H][MAP_W] =
{
 {2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,7,0,0,0,1,1,0,0,0,7,0,0,0,0,2},
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
 {2,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,2},
 {2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2}
};

/* ==================================================
   TILE GRAPHICS
   ================================================== */

const unsigned char gfx_grass[16] =
{
 0x00,0x00,0x00,0x00,
 0x10,0x00,0x00,0x01,
 0x00,0x00,0x08,0x00,
 0x00,0x00,0x01,0x00
};

const unsigned char gfx_path[16] =
{
 0x00,0x00,0x55,0x55,
 0x00,0x00,0x55,0x55,
 0x00,0x00,0x55,0x55,
 0x00,0x00,0x55,0x55
};

const unsigned char gfx_tree[16] =
{
 0x18,0x18,0x3C,0x3C,
 0x7E,0x7E,0xFF,0xFF,
 0xFF,0xFF,0x7E,0x7E,
 0x18,0x18,0x18,0x18
};

const unsigned char gfx_water[16] =
{
 0xAA,0x55,0x00,0x00,
 0x55,0xAA,0x00,0x00,
 0xAA,0x55,0x00,0x00,
 0x55,0xAA,0x00,0x00
};

const unsigned char gfx_flower[16] =
{
 0x00,0x00,0x18,0x18,
 0x3C,0x3C,0x18,0x18,
 0x18,0x18,0x3C,0x3C,
 0x18,0x18,0x00,0x00
};

const unsigned char gfx_tall[16] =
{
 0x11,0x11,0x22,0x22,
 0x11,0x11,0x22,0x22,
 0x11,0x11,0x22,0x22,
 0x11,0x11,0x22,0x22
};

const unsigned char gfx_house[16] =
{
 0xFF,0xFF,0x81,0x81,
 0xBD,0xBD,0xA5,0xA5,
 0xBD,0xBD,0x81,0x81,
 0x81,0x81,0xFF,0xFF
};

const unsigned char gfx_rock[16] =
{
 0x00,0x00,0x18,0x18,
 0x3C,0x3C,0x7E,0x7E,
 0x7E,0x7E,0x3C,0x3C,
 0x18,0x18,0x00,0x00
};

const unsigned char gfx_door[16] =
{
 0xFF,0xFF,0x81,0x81,
 0x81,0x81,0x99,0x99,
 0x99,0x99,0x99,0x99,
 0x81,0x81,0xFF,0xFF
};

const unsigned char gfx_dark[16] =
{
 0xFF,0xFF,0xAA,0xAA,
 0x55,0x55,0xFF,0xFF,
 0xAA,0xAA,0x55,0x55,
 0xFF,0xFF,0xAA,0xAA
};

/* ==================================================
   SPRITES
   ================================================== */

const unsigned char gfx_player[16] =
{
 0x18,0x18,
 0x3C,0x3C,
 0x7E,0x7E,
 0xDB,0xFF,
 0xFF,0xDB,
 0x7E,0x7E,
 0x3C,0x3C,
 0x24,0x24
};

const unsigned char gfx_npc[16] =
{
 0x3C,0x3C,
 0x42,0x42,
 0xA5,0xA5,
 0x81,0x81,
 0x7E,0x7E,
 0x18,0x18,
 0x24,0x24,
 0x42,0x42
};

const unsigned char gfx_monster[16] =
{
 0x00,0x00,
 0x3C,0x3C,
 0x7E,0x7E,
 0xFF,0xFF,
 0xFF,0xFF,
 0x7E,0x7E,
 0x3C,0x3C,
 0x00,0x00
};

/* ==================================================
   GRAPHICS INITIALIZATION
   ================================================== */

void load_graphics(void)
{
    set_bkg_data(GRASS,1,gfx_grass);
    set_bkg_data(PATH,1,gfx_path);
    set_bkg_data(TREE,1,gfx_tree);
    set_bkg_data(WATER,1,gfx_water);
    set_bkg_data(FLOWER,1,gfx_flower);
    set_bkg_data(TALL,1,gfx_tall);
    set_bkg_data(HOUSE,1,gfx_house);
    set_bkg_data(ROCK,1,gfx_rock);
    set_bkg_data(DOOR,1,gfx_door);
    set_bkg_data(DARK,1,gfx_dark);

    set_sprite_data(0,1,gfx_player);
    set_sprite_data(1,1,gfx_npc);
    set_sprite_data(2,1,gfx_monster);
}

/* ==================================================
   MAP SELECTION
   ================================================== */

UINT8 get_tile(UINT8 x, UINT8 y)
{
    if(x >= MAP_W || y >= MAP_H)
        return TREE;

    if(current_area == VILLAGE)
        return village[y][x];

    if(current_area == MEADOW)
        return meadow[y][x];

    if(current_area == FOREST)
        return forest[y][x];

    if(current_area == CAVE)
        return cave[y][x];

    return shrine[y][x];
}

/* ==================================================
   MAP DRAWING
   ================================================== */

void draw_map(void)
{
    UINT8 x;
    UINT8 y;

    for(y=0;y<MAP_H;y++)
    {
        for(x=0;x<MAP_W;x++)
        {
            set_bkg_tile_xy(
                x,
                y,
                get_tile(x,y)
            );
        }
    }

    move_sprite(
        0,
        player_x*8+4,
        player_y*8+12
    );

    if(current_area == VILLAGE)
        move_sprite(1,52,44);
    else
        move_sprite(1,0,0);
}

/* ==================================================
   PLAYER SPRITE
   ================================================== */

void setup_sprites(void)
{
    set_sprite_tile(0,0);
    set_sprite_tile(1,1);

    move_sprite(
        0,
        player_x*8+4,
        player_y*8+12
    );

    move_sprite(1,52,44);
}

/* ==================================================
   COLLISION
   ================================================== */

UINT8 blocked(UINT8 x, UINT8 y)
{
    UINT8 tile;

    if(x >= MAP_W || y >= MAP_H)
        return 1;

    tile = get_tile(x,y);

    if(tile == TREE)
        return 1;

    if(tile == WATER)
        return 1;

    if(tile == HOUSE)
        return 1;

    if(tile == ROCK)
        return 1;

    return 0;
}

/* ==================================================
   TEXT
   ================================================== */

void wait_a(void)
{
    waitpad(J_A);
    waitpadup();
}

void message(
    const char *a,
    const char *b
)
{
    cls();

    printf("\n");
    printf("%s\n",a);

    if(b != 0)
    {
        printf("\n%s\n",b);
    }

    printf("\nA: CONTINUE");

    wait_a();

    draw_map();
}

/* ==================================================
   TITLE
   ================================================== */

void title_screen(void)
{
    cls();

    printf("\n\n");
    printf("   LITTLE WILDS\n\n");
    printf(" THE MEADOW OF\n");
    printf("     ECHOES\n\n");
    printf("----------------\n");
    printf("\n");
    printf("A: START");

    wait_a();
}

/* ==================================================
   AREA NAMES
   ================================================== */

const char *area_name(void)
{
    if(current_area == VILLAGE)
        return "BRAMBLEWICK";

    if(current_area == MEADOW)
        return "MEADOW";

    if(current_area == FOREST)
        return "FOREST";

    if(current_area == CAVE)
        return "OLD MINE";

    return "ECHO SHRINE";
}

/* ==================================================
   AREA TRANSITIONS
   ================================================== */

void go_meadow(void)
{
    current_area = MEADOW;

    player_x = 9;
    player_y = 16;

    draw_map();

    message(
        "MEADOW OF ECHOES",
        "The grass whispers\nin the wind."
    );
}

void go_village(void)
{
    current_area = VILLAGE;

    player_x = 9;
    player_y = 16;

    draw_map();

    message(
        "BRAMBLEWICK",
        "You return home."
    );
}

void go_forest(void)
{
    current_area = FOREST;

    player_x = 9;
    player_y = 16;

    draw_map();

    message(
        "WHISPERING FOREST",
        "The trees seem to\nlean toward you."
    );
}

void go_cave(void)
{
    current_area = CAVE;

    player_x = 9;
    player_y = 16;

    draw_map();

    message(
        "THE OLD MINE",
        "Cold air rises from\nthe darkness."
    );
}

void go_shrine(void)
{
    current_area = SHRINE;

    player_x = 9;
    player_y = 16;

    draw_map();

    message(
        "ECHO SHRINE",
        "Something is waiting\nat the center."
    );
}

/* ==================================================
   EXIT CHECKING
   ================================================== */

void check_exits(void)
{
    if(player_y != 17)
        return;

    if(player_x != 9 && player_x != 10)
        return;

    if(current_area == VILLAGE)
    {
        go_meadow();
        return;
    }

    if(current_area == MEADOW)
    {
        go_village();
        return;
    }

    if(current_area == FOREST)
    {
        go_meadow();
        return;
    }

    if(current_area == CAVE)
    {
        go_forest();
        return;
    }

    if(current_area == SHRINE)
    {
        go_cave();
        return;
    }
}

/* ==================================================
   NPC
   ================================================== */

UINT8 near_mara(void)
{
    if(current_area != VILLAGE)
        return 0;

    if(player_x == 6 && player_y == 5)
        return 1;

    if(player_x == 5 && player_y == 4)
        return 1;

    if(player_x == 7 && player_y == 4)
        return 1;

    return 0;
}

void mara_dialogue(void)
{
    if(story_step == 0)
    {
        message(
            "OLD MARA:",
            "You look ready for\na little adventure."
        );

        message(
            "OLD MARA:",
            "The meadow has been\nacting strangely."
        );

        message(
            "OLD MARA:",
            "Find out why."
        );

        story_step = 1;

        return;
    }

    if(story_step == 1)
    {
        message(
            "OLD MARA:",
            "The forest lies beyond\nthe meadow."
        );

        message(
            "OLD MARA:",
            "Something old sleeps\nbeneath these lands."
        );

        return;
    }

    if(story_step >= 2)
    {
        message(
            "OLD MARA:",
            "Keep going.\nYou're close now."
        );
    }
}

/* ==================================================
   MONSTER NAME DISPLAY
   ================================================== */

void show_monster(Monster *m)
{
    printf("%s",m->name);
}

/* ==================================================
   DAMAGE
   ================================================== */

UINT8 calculate_damage(
    Monster *attacker,
    Monster *defender,
    UINT8 move
)
{
    UINT8 damage;

    damage = attacker->attack;

    if(move == MOVE_STRIKE)
        damage += 2;

    if(move == MOVE_SCRATCH)
        damage += 1;

    if(move == MOVE_SPARK)
        damage += 3;

    if(move == MOVE_BITE)
        damage += 2;

    if(move == MOVE_VINE)
        damage += 2;

    if(damage > defender->defense)
        damage -= defender->defense / 2;

    else
        damage = 1;

    return damage;
}

/* ==================================================
   BATTLE DISPLAY
   ================================================== */

void battle_display(
    Monster *wild,
    UINT8 wild_hp,
    UINT8 player_hp
)
{
    cls();

    printf("\n");
    printf("WILD %s\n",wild->name);

    printf("LV%u HP:%u/%u\n",
        wild->level,
        wild_hp,
        wild->max_hp
    );

    printf("\n");

    printf("%s\n",player_monster.name);

    printf("LV%u HP:%u/%u\n",
        player_monster.level,
        player_hp,
        player_monster.max_hp
    );

    printf("\n");
    printf("A ATTACK\n");
    printf("B RUN\n");
}

/* ==================================================
   EXPERIENCE
   ================================================== */

void gain_xp(UINT8 amount)
{
    player_monster.xp += amount;

    if(player_monster.xp >= 10)
    {
        player_monster.xp = 0;

        player_monster.level++;

        player_monster.max_hp += 3;
        player_monster.attack++;
        player_monster.defense++;

        cls();

        printf("\n");
        printf("%s GREW!\n\n",
            player_monster.name
        );

        printf("NOW LEVEL %u\n",
            player_monster.level
        );

        printf("\nA: CONTINUE");

        wait_a();
    }
}

/* ==================================================
   CAPTURE
   ================================================== */

UINT8 attempt_capture(
    Monster *wild,
    UINT8 wild_hp
)
{
    UINT8 chance;

    if(item_balls == 0)
        return 0;

    item_balls--;

    chance = 35;

    if(wild_hp < wild->max_hp / 2)
        chance = 65;

    /*
        Small deterministic pseudo-random effect.
    */
    if((player_x + player_y + wild_hp) % 100 < chance)
        return 1;

    return 0;
}

/* ==================================================
   WILD MONSTER SELECTION
   ================================================== */

Monster *select_wild(void)
{
    static UINT8 encounter_counter = 0;

    encounter_counter++;

    if(current_area == MEADOW)
    {
        if(encounter_counter % 4 == 0)
            return &wild_zappit;

        if(encounter_counter % 3 == 0)
            return &wild_mossel;

        return &wild_brinn;
    }

    if(current_area == FOREST)
    {
        if(encounter_counter % 4 == 0)
            return &wild_ember;

        if(encounter_counter % 3 == 0)
            return &wild_thorn;

        return &wild_gruff;
    }

    if(current_area == CAVE)
    {
        if(encounter_counter % 3 == 0)
            return &wild_wisp;

        return &wild_rill;
    }

    return &wild_brinn;
}

/* ==================================================
   BATTLE
   ================================================== */

void battle(void)
{
    Monster *wild;

    UINT8 wild_hp;
    UINT8 player_hp;

    UINT8 input;
    UINT8 damage;

    wild = select_wild();

    wild_hp = wild->max_hp;
    player_hp = player_monster.max_hp;

    while(1)
    {
        battle_display(
            wild,
            wild_hp,
            player_hp
        );

        input = waitpad(J_A | J_B);

        waitpadup();

        if(input & J_B)
        {
            cls();

            printf("\n");
            printf("You escaped safely.\n");

            printf("\nA: CONTINUE");

            wait_a();

            draw_map();

            return;
        }

        if(input & J_A)
        {
            damage = calculate_damage(
                &player_monster,
                wild,
                player_monster.move1
            );

            if(damage >= wild_hp)
                wild_hp = 0;
            else
                wild_hp -= damage;

            cls();

            printf("\n");
            printf("%s attacks!\n",
                player_monster.name
            );

            printf("\n");
            printf("%s HP: %u",
                wild->name,
                wild_hp
            );

            if(wild_hp == 0)
            {
                printf("\n\n");
                printf("The wild %s fled!",
                    wild->name
                );

                gain_xp(5);

                if(story_step == 1)
                    story_step = 2;

                printf("\n\nA: CONTINUE");

                wait_a();

                draw_map();

                return;
            }

            printf("\n\n");
            printf("%s attacks!",
                wild->name
            );

            damage = calculate_damage(
                wild,
                &player_monster,
                wild->move1
            );

            if(damage >= player_hp)
                player_hp = 1;
            else
                player_hp -= damage;

            printf("\n");
            printf("YOUR HP: %u",
                player_hp
            );

            printf("\n\nA: CONTINUE");

            wait_a();

            if(player_hp <= 1)
            {
                cls();

                printf("\n");
                printf("You retreat from\nthe battle.");

                printf("\n\nA: CONTINUE");

                wait_a();

                draw_map();

                return;
            }
        }
    }
}

/* ==================================================
   ENCOUNTERS
   ================================================== */

void check_encounter(void)
{
    static UINT8 steps = 0;

    if(current_area == VILLAGE)
        return;

    if(current_area == SHRINE)
        return;

    if(get_tile(player_x,player_y) != TALL)
        return;

    steps++;

    if(steps >= 5)
    {
        steps = 0;

        battle();
    }
}

/* ==================================================
   HEALING
   ================================================== */

void heal_party(void)
{
    player_monster.max_hp = player_monster.max_hp;

    message(
        "MARA:",
        "Your wild is feeling\nmuch better."
    );
}

/* ==================================================
   ITEMS
   ================================================== */

void use_potion(void)
{
    if(item_potions == 0)
    {
        message(
            "BAG",
            "You have no potions."
        );

        return;
    }

    item_potions--;

    player_monster.max_hp += 5;

    message(
        "BAG",
        "You used a potion."
    );
}

void use_herb(void)
{
    if(item_herbs == 0)
    {
        message(
            "BAG",
            "You have no herbs."
        );

        return;
    }

    item_herbs--;

    player_monster.attack++;

    message(
        "BAG",
        "Your wild feels\nstronger."
    );
}

/* ==================================================
   BAG MENU
   ================================================== */

void bag_menu(void)
{
    UINT8 choice;

    while(1)
    {
        cls();

        printf("\n");
        printf("       BAG\n\n");

        printf("> POTION x%u\n",
            item_potions
        );

        printf("  BALL   x%u\n",
            item_balls
        );

        printf("  HERB   x%u\n",
            item_herbs
        );

        printf("\n");
        printf("A USE\n");
        printf("B EXIT");

        choice = waitpad(J_A | J_B);

        waitpadup();

        if(choice & J_B)
        {
            draw_map();
            return;
        }

        if(choice & J_A)
        {
            if(item_potions > 0)
                use_potion();
            else
                message("BAG","Nothing to use.");
        }
    }
}

/* ==================================================
   PARTY MENU
   ================================================== */

void party_menu(void)
{
    cls();

    printf("\n");
    printf("       PARTY\n\n");

    printf("1 %s\n",
        player_monster.name
    );

    printf("LV%u\n",
        player_monster.level
    );

    printf("\n");

    if(party_count >= 2)
    {
        printf("2 %s\n",
            party_two.name
        );

        printf("LV%u\n",
            party_two.level
        );
    }

    if(party_count >= 3)
    {
        printf("\n");
        printf("3 %s\n",
            party_three.name
        );

        printf("LV%u\n",
            party_three.level
        );
    }

    printf("\nA: CLOSE");

    wait_a();

    draw_map();
}

/* ==================================================
   MAIN MENU
   ================================================== */

void main_menu(void)
{
    UINT8 input;

    while(1)
    {
        cls();

        printf("\n");
        printf("       MENU\n\n");

        printf("A BAG\n");
        printf("B CLOSE\n\n");

        printf("XP: %u\n",
            player_monster.xp
        );

        input = waitpad(J_A | J_B);

        waitpadup();

        if(input & J_B)
        {
            draw_map();
            return;
        }

        if(input & J_A)
        {
            bag_menu();
            return;
        }
    }
}

/* ==================================================
   SHRINE EVENT
   ================================================== */

void shrine_event(void)
{
    if(story_step < 3)
    {
        message(
            "THE SHRINE",
            "The stone is silent."
        );

        message(
            "A WHISPER:",
            "Not yet..."
        );

        return;
    }

    message(
        "THE SHRINE",
        "The stone begins\nto glow."
    );

    message(
        "A WHISPER:",
        "You listened to the\nwilds."
    );

    message(
        "THE END",
        "The meadow remembers\nyour name."
    );

    story_step = 5;
}

/* ==================================================
   INTERACTION
   ================================================== */

void interact(void)
{
    if(current_area == VILLAGE)
    {
        if(near_mara())
        {
            mara_dialogue();
            return;
        }

        if(player_x == 9 || player_x == 10)
        {
            message(
                "BRAMBLEWICK",
                "A quiet village."
            );

            return;
        }

        message(
            "BRAMBLEWICK",
            "Nothing interesting\nhere."
        );

        return;
    }

    if(current_area == SHRINE)
    {
        shrine_event();
        return;
    }

    if(get_tile(player_x,player_y) == TALL)
    {
        battle();
        return;
    }

    message(
        area_name(),
        "The wilds are quiet."
    );
}

/* ==================================================
   MOVEMENT
   ================================================== */

void move_player(
    UINT8 dx,
    UINT8 dy
)
{
    UINT8 nx;
    UINT8 ny;

    nx = player_x + dx;
    ny = player_y + dy;

    if(blocked(nx,ny))
        return;

    player_x = nx;
    player_y = ny;

    update_player_sprite();

    check_exits();

    check_encounter();
}

/* ==================================================
   SPRITE UPDATE
   ================================================== */

void update_player_sprite(void)
{
    move_sprite(
        0,
        player_x*8+4,
        player_y*8+12
    );
}

/* ==================================================
   SAVE-STATE STYLE RESET
   ================================================== */

void reset_player_position(void)
{
    current_area = VILLAGE;

    player_x = 9;
    player_y = 15;

    draw_map();
}

/* ==================================================
   DEBUG / STATUS SCREEN
   ================================================== */

void status_screen(void)
{
    cls();

    printf("\n");
    printf(" LITTLE WILDS\n");
    printf("----------------\n");

    printf("\nAREA:\n%s\n",
        area_name()
    );

    printf("\nPLAYER:\n");
    printf("X %u  Y %u\n",
        player_x,
        player_y
    );

    printf("\nMONSTER:\n");
    printf("%s LV%u\n",
        player_monster.name,
        player_monster.level
    );

    printf("\nCAUGHT: %u\n",
        total_caught
    );

    printf("\nA: CLOSE");

    wait_a();

    draw_map();
}

/* ==================================================
   OVERWORLD LOOP
   ================================================== */

void overworld(void)
{
    UINT8 keys;

    draw_map();

    while(game_running)
    {
        keys = joypad();

        if(keys & J_UP)
        {
            facing = 0;

            move_player(0,255);

            waitpadup();
        }

        else if(keys & J_DOWN)
        {
            facing = 1;

            move_player(0,1);

            waitpadup();
        }

        else if(keys & J_LEFT)
        {
            facing = 2;

            move_player(255,0);

            waitpadup();
        }

        else if(keys & J_RIGHT)
        {
            facing = 3;

            move_player(1,0);

            waitpadup();
        }

        else if(keys & J_A)
        {
            waitpadup();

            interact();
        }

        else if(keys & J_B)
        {
            waitpadup();

            main_menu();
        }

        wait_vbl_done();
    }
}

/* ==================================================
   GAME INITIALIZATION
   ================================================== */

void initialize_game(void)
{
    current_area = VILLAGE;

    player_x = 9;
    player_y = 15;

    story_step = 0;

    party_count = 1;

    item_potions = 3;
    item_balls = 5;
    item_herbs = 2;

    total_caught = 0;

    player_monster.max_hp = 22;
    player_monster.attack = 7;
    player_monster.defense = 5;
    player_monster.level = 3;
    player_monster.xp = 0;
}

/* ==================================================
   MAIN
   ================================================== */

void main(void)
{
    DISPLAY_OFF;

    initialize_game();

    load_graphics();

    SHOW_BKG;
    SHOW_SPRITES;

    title_screen();

    setup_sprites();

    DISPLAY_ON;

    overworld();

    while(1)
    {
        wait_vbl_done();
    }
}