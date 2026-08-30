#include <gb/gb.h>
#include <stdio.h>

#define MAP_W 20
#define MAP_H 18

UINT8 player_x = 10;
UINT8 player_y = 9;

/*
    LITTLE WILDS
    v0.2 - graphical overworld prototype

    Tile legend:
    0 = grass
    1 = path
    2 = tree
    3 = water
    4 = flower
*/

/* Simple 20 x 18 overworld */
const unsigned char meadow_map[MAP_H][MAP_W] = {
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
    {2,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,2},
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}
};

/* Draw a very simple tile using Game Boy background tile data */
void draw_tile(UINT8 tile, UINT8 x, UINT8 y) {
    unsigned char data[16];

    UINT8 i;

    for (i = 0; i < 16; i++) {
        data[i] = 0x00;
    }

    if (tile == 1) {
        for (i = 0; i < 16; i++) {
            data[i] = 0x55;
        }
    }

    if (tile == 2) {
        data[0] = 0x18;
        data[1] = 0x18;
        data[2] = 0x3C;
        data[3] = 0x3C;
        data[4] = 0x7E;
        data[5] = 0x7E;
        data[6] = 0xFF;
        data[7] = 0xFF;
        data[8] = 0x7E;
        data[9] = 0x7E;
        data[10] = 0x3C;
        data[11] = 0x3C;
    }

    if (tile == 3) {
        data[2] = 0xAA;
        data[3] = 0x55;
        data[6] = 0xAA;
        data[7] = 0x55;
        data[10] = 0xAA;
        data[11] = 0x55;
    }

    if (tile == 4) {
        data[6] = 0x18;
        data[7] = 0x18;
        data[8] = 0x3C;
        data[9] = 0x3C;
    }

    set_bkg_data(tile, 1, data);
}

/* Build the background */
void build_map(void) {
    UINT8 x;
    UINT8 y;

    for (x = 0; x < 5; x++) {
        draw_tile(x, 0, 0);
    }

    for (y = 0; y < MAP_H; y++) {
        for (x = 0; x < MAP_W; x++) {
            set_bkg_tile_xy(x, y, meadow_map[y][x]);
        }
    }
}

/* Draw the player as a tiny 8-bit character */
void draw_player(void) {
    unsigned char player_tile[16];

    player_tile[0]  = 0x18;
    player_tile[1]  = 0x18;
    player_tile[2]  = 0x3C;
    player_tile[3]  = 0x3C;
    player_tile[4]  = 0x7E;
    player_tile[5]  = 0x7E;
    player_tile[6]  = 0xDB;
    player_tile[7]  = 0xFF;
    player_tile[8]  = 0xFF;
    player_tile[9]  = 0xDB;
    player_tile[10] = 0x18;
    player_tile[11] = 0x18;
    player_tile[12] = 0x24;
    player_tile[13] = 0x24;
    player_tile[14] = 0x42;
    player_tile[15] = 0x42;

    set_sprite_data(0, 1, player_tile);
    set_sprite_tile(0, 0);
    move_sprite(0, player_x * 8 + 4, player_y * 8 + 12);
}

/* Can the player walk here? */
UINT8 can_walk(UINT8 x, UINT8 y) {
    if (x >= MAP_W || y >= MAP_H)
        return 0;

    if (meadow_map[y][x] == 2)
        return 0;

    if (meadow_map[y][x] == 3)
        return 0;

    return 1;
}

/* Simple title screen */
void title_screen(void) {
    printf("\n\n");
    printf(" LITTLE WILDS\n\n");
    printf(" THE MEADOW OF\n");
    printf(" ECHOES AWAITS\n\n");
    printf(" PRESS A\n");

    waitpad(J_A);
    waitpadup();

    cls();
}

/* Main overworld */
void overworld(void) {
    UINT8 keys;
    UINT8 nx;
    UINT8 ny;

    build_map();
    SHOW_BKG;
    SHOW_SPRITES;

    draw_player();

    while (1) {
        keys = joypad();

        nx = player_x;
        ny = player_y;

        if (keys & J_UP)
            ny--;
        else if (keys & J_DOWN)
            ny++;
        else if (keys & J_LEFT)
            nx--;
        else if (keys & J_RIGHT)
            nx++;

        if (nx != player_x || ny != player_y) {
            if (can_walk(nx, ny)) {
                player_x = nx;
                player_y = ny;
                draw_player();
            }

            waitpadup();
        }

        wait_vbl_done();
    }
}

void main(void) {
    DISPLAY_ON;

    title_screen();
    overworld();

    while (1) {
        wait_vbl_done();
    }
}