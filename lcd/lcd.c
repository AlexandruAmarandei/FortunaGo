/*  Author: Steve Gunn
 * Licence: This work is licensed under the Creative Commons Attribution License.
 *           View this license at http://creativecommons.org/about/licenses/
 *
 *  
 *  - Jan 2015  Modified for LaFortuna (Rev A, black edition) [KPZ]
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "font.h"
#include "ili934x.h"
#include "lcd.h"

lcd display;
uint16_t minim, size;
void init_lcd()
{
    /* Enable extended memory interface with 10 bit addressing */
    XMCRB = _BV(XMM2) | _BV(XMM1);
    XMCRA = _BV(SRE);
    DDRC |= _BV(RESET);
    DDRB |= _BV(BLC);
    _delay_ms(1);
    PORTC &= ~_BV(RESET);
    _delay_ms(20);
    PORTC |= _BV(RESET);
    _delay_ms(120);
    write_cmd(DISPLAY_OFF);
    write_cmd(SLEEP_OUT);
    _delay_ms(60);
    write_cmd_data(INTERNAL_IC_SETTING,          0x01);
    write_cmd(POWER_CONTROL_1);
        write_data16(0x2608);
    write_cmd_data(POWER_CONTROL_2,              0x10);
    write_cmd(VCOM_CONTROL_1);
        write_data16(0x353E);
    write_cmd_data(VCOM_CONTROL_2, 0xB5);
    write_cmd_data(INTERFACE_CONTROL, 0x01);
        write_data16(0x0000);
    write_cmd_data(PIXEL_FORMAT_SET, 0x55);     /* 16bit/pixel */
    set_orientation(West);
    clear_screen();
    display.x = 0;
    display.y = 0;
    display.background = WHITE;
    display.foreground = BLACK;
    write_cmd(DISPLAY_ON);
    _delay_ms(50);
    write_cmd_data(TEARING_EFFECT_LINE_ON, 0x00);
    EICRB |= _BV(ISC61);
    PORTB |= _BV(BLC);
    clear_screen();
    minim = 4099;
    if(display.width < display.height){
        minim = display.width-1;
    }
    else{
        minim = display.height-1;
    }
    size = minim/9;
    minim = size * 9;

    display_Players();  
}

void display_Players(){
    char carray[] = "Player 1:";
    char *pointerCarray = carray;
    display_string_xy(pointerCarray, minim+3, 0);
    carray[7] = '2';
    display_string_xy(pointerCarray, minim+3, 40);

}

void convertToChar(uint16_t number, char *into){

    if(number > 9999) {
        display_string("Error while displaying number");
        return;
    }
    uint16_t size = 0;
    uint16_t aux = number;
    do{
        aux /= 10;
        size++;
    }while(aux != 0);
    aux = number;
    uint16_t i;
    for(i = size; i > 0; i--){
        *(into + 4 - size +i-1) = aux%10 + '0';
        aux /= 10;
    }

}

void update_Score(uint16_t number, uint16_t player){
    char numberString[4] = "0000";
    char *p2 = numberString;
    convertToChar(number, p2);
    char toDisplay[11] = "Score: ";
    char *point = toDisplay;
    toDisplay[7] = numberString[0];
    toDisplay[8] = numberString[1];
    toDisplay[9] = numberString[2];
    toDisplay[10] = numberString[3];
    if(player == 1){
        display_string_xy(point, minim+3, 24);
    }
    if(player == 2){
        display_string_xy(point, minim+3, 64);
    }

}

void update_Taken(uint16_t number, uint16_t player){
    char numberString[4] = "0000";
    char *p2 = numberString;
    convertToChar(number, p2);
    char toDisplay[11] = "Taken: ";
    char *point = toDisplay;
    toDisplay[7] = numberString[0];
    toDisplay[8] = numberString[1];
    toDisplay[9] = numberString[2];
    toDisplay[10] = numberString[3];
    if(player == 1){
        display_string_xy(point, minim+3, 8);
    }
    if(player == 2){
        display_string_xy(point, minim+3, 48);
    }
}

void update_Stones(uint16_t number, uint16_t player){
    char numberString[4] = "0000";
    char *p2 = numberString;
    convertToChar(number, p2);
    char toDisplay[12] = "Stones: ";
    char *point = toDisplay;
    toDisplay[8] = numberString[0];
    toDisplay[9] = numberString[1];
    toDisplay[10] = numberString[2];
    toDisplay[11] = numberString[3];
    if(player == 1){
        display_string_xy(point, minim+3, 16);
    }
    if(player == 2){
        display_string_xy(point, minim+3, 56);
    }
}

void display_circle(uint16_t x, uint16_t y, uint16_t radius, uint16_t col){
    uint16_t i = 0;
    rectangle r, r2;

    for(i = 0; i < radius; i++){
        if(x < radius){
            r.left = 0;
            r.right = 1;
            r.top = 0;
            r2.top = 0;
        }else{
            r.left = x - radius + i;
            r.right = x - radius + i+1;
            r.top = y - i;
            r2.top = y - i;
        }
        if(x + radius > minim){
            r2.left = minim;
            r2.right = minim;
            r.bottom = minim;
            r2.bottom = minim;
        }else{
            r2.left = x + radius - i;
            r2.right = x + radius - i - 1; 
            r.bottom = y + i;
            r2.bottom = y + i;
        } 
        fill_rectangle(r, col);
        fill_rectangle(r2, col);
    }
     
}

void lcd_brightness(uint8_t i)
{
    /* Configure Timer 2 Fast PWM Mode 3 */
    TCCR2A = _BV(COM2A1) | _BV(WGM21) | _BV(WGM20);
    TCCR2B = _BV(CS20);
    OCR2A = i;
}

void set_orientation(orientation o)
{
    display.orient = o;
    write_cmd(MEMORY_ACCESS_CONTROL);
    if (o==North) { 
        display.width = LCDWIDTH;
        display.height = LCDHEIGHT;
        write_data(0x48);
    }
    else if (o==West) {
        display.width = LCDHEIGHT;
        display.height = LCDWIDTH;
        write_data(0xE8);
    }
    else if (o==South) {
        display.width = LCDWIDTH;
        display.height = LCDHEIGHT;
        write_data(0x88);
    }
    else if (o==East) {
        display.width = LCDHEIGHT;
        display.height = LCDWIDTH;
        write_data(0x28);
    }
    write_cmd(COLUMN_ADDRESS_SET);
    write_data16(0);
    write_data16(display.width-1);
    write_cmd(PAGE_ADDRESS_SET);
    write_data16(0);
    write_data16(display.height-1);
}



void set_frame_rate_hz(uint8_t f)
{
    uint8_t diva, rtna, period;
    if (f>118)
        f = 118;
    if (f<8)
        f = 8;
    if (f>60) {
        diva = 0x00;
    } else if (f>30) {
        diva = 0x01;
    } else if (f>15) {
        diva = 0x02;
    } else {
        diva = 0x03;
    }
    /*   !!! FIXME !!!  [KPZ-30.01.2015] */
    /*   Check whether this works for diva > 0  */
    /*   See ILI9341 datasheet, page 155  */
    period = 1920.0/f;
    rtna = period >> diva;
    write_cmd(FRAME_CONTROL_IN_NORMAL_MODE);
    write_data(diva);
    write_data(rtna);
}

void fill_rectangle(rectangle r, uint16_t col)
{
    write_cmd(COLUMN_ADDRESS_SET);
    write_data16(r.left);
    write_data16(r.right);
    write_cmd(PAGE_ADDRESS_SET);
    write_data16(r.top);
    write_data16(r.bottom);
    write_cmd(MEMORY_WRITE);
/*  uint16_t x, y;
    for(x=r.left; x<=r.right; x++)
        for(y=r.top; y<=r.bottom; y++)
            write_data16(col);
*/
    uint16_t wpixels = r.right - r.left + 1;
    uint16_t hpixels = r.bottom - r.top + 1;
    uint8_t mod8, div8;
    uint16_t odm8, odd8;
    if (hpixels > wpixels) {
        mod8 = hpixels & 0x07;
        div8 = hpixels >> 3;
        odm8 = wpixels*mod8;
        odd8 = wpixels*div8;
    } else {
        mod8 = wpixels & 0x07;
        div8 = wpixels >> 3;
        odm8 = hpixels*mod8;
        odd8 = hpixels*div8;
    }
    uint8_t pix1 = odm8 & 0x07;
    while(pix1--)
        write_data16(col);

    uint16_t pix8 = odd8 + (odm8 >> 3);
    while(pix8--) {
        write_data16(col);
        write_data16(col);
        write_data16(col);
        write_data16(col);
        write_data16(col);
        write_data16(col);
        write_data16(col);
        write_data16(col);
    }
}

void fill_rectangle_indexed(rectangle r, uint16_t* col)
{
    uint16_t x, y;
    write_cmd(COLUMN_ADDRESS_SET);
    write_data16(r.left);
    write_data16(r.right);
    write_cmd(PAGE_ADDRESS_SET);
    write_data16(r.top);
    write_data16(r.bottom);
    write_cmd(MEMORY_WRITE);
    for(x=r.left; x<=r.right; x++)
        for(y=r.top; y<=r.bottom; y++)
            write_data16(*col++);
}

void clear_screen()
{
    display.x = 0;
    display.y = 0;
    rectangle r = {0, display.width-1, 0, display.height-1};
    fill_rectangle(r, display.background);
}

void display_char(char c)
{
    uint16_t x, y;
    PGM_P fdata; 
    uint8_t bits, mask;
    uint16_t sc=display.x, ec=display.x + 4, sp=display.y, ep=display.y + 7;
    if(c == "\n"){
        y+=8; 
        return;
    }
    if (c < 32 || c > 126) return;
    fdata = (c - ' ')*5 + font5x7;
    write_cmd(PAGE_ADDRESS_SET);
    write_data16(sp);
    write_data16(ep);
    for(x=sc; x<=ec; x++) {
        write_cmd(COLUMN_ADDRESS_SET);
        write_data16(x);
        write_data16(x);
        write_cmd(MEMORY_WRITE);
        bits = pgm_read_byte(fdata++);
        for(y=sp, mask=0x01; y<=ep; y++, mask<<=1)
            write_data16((bits & mask) ? display.foreground : display.background);
    }
    write_cmd(COLUMN_ADDRESS_SET);
    write_data16(x);
    write_data16(x);
    write_cmd(MEMORY_WRITE);
    for(y=sp; y<=ep; y++)
        write_data16(display.background);

    display.x += 6;
    if (display.x >= display.width) { display.x=0; display.y+=8; }
}

void display_string(char *str)
{
    uint8_t i;
    for(i=0; str[i]; i++) 
        display_char(str[i]);
}

void display_string_xy(char *str, uint16_t x, uint16_t y)
{
    uint8_t i;
    display.x = x;
    display.y = y;
    for(i=0; str[i]; i++)
        display_char(str[i]);
}

void game_start_screen(){
    rectangle r;
    r.left = 0;
    r.right = display.width - 1;
    r.top = 0;
    r.bottom = display.height - 1;
    fill_rectangle(r, display.background);
    display_string_xy("                ,----.. ",display.width/2 - (25 * 6/2), display.height/2 - (14 / 2 * 9));
    display_string_xy("  ,----..       /   / \\ ",display.width/2 - (25 * 6/2), display.height/2 - (6 * 9));
    display_string_xy("  ,----..      /   /   \\  ",display.width/2 - (25 * 6/2), display.height/2 - (5 * 9));
    display_string_xy(" /   /   \\    /   .     :  ",display.width/2 - (25 * 6/2), display.height/2 - (4 * 9));
    display_string_xy("|   :     :  .   /   ;.  \\  ",display.width/2 - (25 * 6/2), display.height/2 - (3 * 9));
    display_string_xy(".   |  ;. / .   ;   /  ` ; ",display.width/2 - (25 * 6/2), display.height/2 - (2* 9));
    display_string_xy(".   ; /--`  ;   |  ; \\ ; | ",display.width/2 - (25 * 6/2), display.height/2 - (1* 9));
    display_string_xy(";   | ;  __ |   :  | ; | ' ",display.width/2 - (25 * 6/2), display.height/2 - (0 * 9));
    display_string_xy("|   : |.' .'.   |  ' ' ' : ",display.width/2 - (25 * 6/2), display.height/2 - (-1* 9));
    display_string_xy(".   | '_.' :'   ;  \\; /  | ",display.width/2 - (25 * 6/2), display.height/2 - ( -2 * 9));
    display_string_xy("'   ; : \\  | \\   \\  ',  / ",display.width/2 - (25 * 6/2), display.height/2 - (-3 * 9));
    display_string_xy("'   | '/  .'  ;   :    /   ",display.width/2 - (25 * 6/2), display.height/2 - (-4 * 9));
    display_string_xy("|   :    /     \\   \\ .'    ",display.width/2 - (25 * 6/2), display.height/2 - (-5 * 9));
    display_string_xy(" \\   \\ .'       `---` ",display.width/2 - (25 * 6/2), display.height/2 - (-6 * 9));
    display_string_xy("  `---`                   ",display.width/2 - (25 * 6/2), display.height/2 - (-7 * 9));

    display_string_xy("Press center key to continue!", display.width/2 - (25 * 6/2), display.height/2 - (-9 * 9));                                    

}

void display_winning_message(uint16_t winningp){

    rectangle r;
    r.left = 0;
    r.right = display.width - 1;
    r.top = 0;
    r.bottom = display.height - 1;
    fill_rectangle(r, display.background);
    if(winningp == 1){
        display_string_xy("THE WINNER IS:                        ",display.width/2 - (38 * 6/2), display.height/2 - (5 * 9));               
        display_string_xy("  _____  _                         __ ",display.width/2 - (38 * 6/2), display.height/2 - (4 * 9));
        display_string_xy(" |  __ \\| |                       /_ |",display.width/2 - (38 * 6/2), display.height/2 - (3 * 9));
        display_string_xy(" | |__) | | __ _ _   _  ___ _ __   | |",display.width/2 - (38 * 6/2), display.height/2 - (2 * 9));
        display_string_xy(" |  ___/| |/ _` | | | |/ _ \\ '__|  | |",display.width/2 - (38 * 6/2), display.height/2 - (1 * 9));
        display_string_xy(" | |    | | (_| | |_| |  __/ |     | |",display.width/2 - (38 * 6/2), display.height/2 - (0 * 9));
        display_string_xy(" |_|    |_|\\__,_|\\__, |\\___|_|     |_|",display.width/2 - (38 * 6/2), display.height/2 - (-1* 9));
        display_string_xy("                  __/ |               ",display.width/2 - (38 * 6/2), display.height/2 - (-2* 9));
        display_string_xy("                 |___/                ",display.width/2 - (38 * 6/2), display.height/2 - (-3 * 9));
        display_string_xy("Press center key to restart the game! ",display.width/2 - (38 * 6/2), display.height/2 - (-6 * 9));               
    }
    if(winningp == 2){
        display_string_xy("THE WINNER IS:                        ",display.width/2 - (38 * 6/2), display.height/2 - (5 * 9));               
        display_string_xy("  _____  _                          ___  ",display.width/2 - (38 * 6/2), display.height/2 - (4 * 9));
        display_string_xy(" |  __ \\| |                       |__ \\ ",display.width/2 - (38 * 6/2), display.height/2 - (3 * 9));
        display_string_xy(" | |__) | | __ _ _   _  ___ _ __      ) |",display.width/2 - (38 * 6/2), display.height/2 - (2 * 9));
        display_string_xy(" |  ___/| |/ _` | | | |/ _ \\ '__|    / / ",display.width/2 - (38 * 6/2), display.height/2 - (1 * 9));
        display_string_xy(" | |    | | (_| | |_| |  __/ |      / /_ ",display.width/2 - (38 * 6/2), display.height/2 - (0 * 9));
        display_string_xy(" |_|    |_|\\__,_|\\__, |\\___|_|     |____|",display.width/2 - (38 * 6/2), display.height/2 - (-1* 9));
        display_string_xy("                  __/ |               ",display.width/2 - (38 * 6/2), display.height/2 - (-2* 9));
        display_string_xy("                 |___/                ",display.width/2 - (38 * 6/2), display.height/2 - (-3 * 9));
        display_string_xy("Press center key to restart the game! ",display.width/2 - (38 * 6/2), display.height/2 - (-6 * 9));               
    }
          
}


void display_grid(){
    rectangle r, r2;

    uint16_t i;   
    r.top = 1;
    r.bottom = minim;
    r2.left = 1;
    r2.right = minim;

    for(i=0; i<=9;i++){
        uint16_t newsize = size*i;
        r.left = newsize;
        r.right = newsize+1;
        fill_rectangle(r, display.foreground);
        r2.top = newsize;
        r2.bottom = newsize+1;
        fill_rectangle(r2, display.foreground);

    } 


}



void delete_from_display(uint16_t x, uint16_t y){
    uint16_t rectanglesize = 10;
    rectangle r;

    if(size * x < rectanglesize){
        r.top = 0;
    }else{
        r.top = size * x - rectanglesize;
    }
    if(size * x + rectanglesize > minim){
        r.bottom = minim;
    }else{
     r.bottom = size * x + rectanglesize;
    }
    if(size * y < rectanglesize){
        r.left = 0;
    }else{
        r.left = size * y - rectanglesize;
    }
    if(size * y + rectanglesize > minim){
        r.right = minim;
    }else{
        r.right = size * y + rectanglesize;
    }

    fill_rectangle(r, display.background);


     rectangle r2;

     if(size * x < rectanglesize){
        r2.top = 0;
    }else{
        r2.top = size * x - rectanglesize;
    }
    if(size * x + rectanglesize > minim){
        r2.bottom = minim;
    }else{
     r2.bottom = size * x + rectanglesize;
    }

    r2.left = size * y;
    r2.right = size * y + 1;
    rectangle r3;
    r3.top = size * x;
    r3.bottom = size*x+1;
    if(size * y < rectanglesize){
        r3.left = 0;
    }else{
        r3.left = size * y - rectanglesize;
    }
    if(size * y + rectanglesize > minim){
        r3.right = minim;
    }else{
        r3.right = size * y + rectanglesize;
    }
    fill_rectangle(r3, display.foreground);
    fill_rectangle(r2, display.foreground);
    return;
}

void place_on_screen(uint16_t x, uint16_t y, uint16_t col, uint16_t rectanglesize){
    rectangle r;
    
    if(size * x < rectanglesize){
        r.top = 0;
    }else{
        r.top = size * x - rectanglesize;
    }
    if(size * x + rectanglesize > minim){
        r.bottom = minim;
    }else{
     r.bottom = size * x + rectanglesize;
    }
    if(size * y < rectanglesize){
        r.left = 0;
    }else{
        r.left = size * y - rectanglesize;
    }
    if(size * y + rectanglesize > minim){
        r.right = minim;
    }else{
        r.right = size * y + rectanglesize;
    }

    fill_rectangle(r, col);
    //display_circle(size*x, size*y, rectanglesize, col);
    return;
}