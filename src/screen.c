/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "screen.h"
#include "game.h"

#define POSICION_RELOJES_F   46
#define POSICION_RELOJES_C_A 4
#define POSICION_RELOJES_C_B 60

#define reloj_size 4

/*
================================================================================
                               ~~~ constantes ~~~
================================================================================
*/

static ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;

const char reloj[] = "|/-\\";

const char *label_datos[8]     = {"eax","ebx","ecx","edx","esi","edi","ebp","esp"};
const char *label_ejecucion[2] = {"eip","eflags"};
const char *label_segmentos[6] = {"cs","ds","es","fs","gs","ss"};
const char *label_control[4]   = {"cr0","cr2","cr3","cr4"};
const char *label_stack[1]     = {"stack"};

/*
================================================================================
                               ~~~ variables ~~~
================================================================================
*/

// ??
extern int ultimo_cambio;

// jugadores
extern jugador_t jugadorA, jugadorB;

// copia de pantalla
ca aux_screen[VIDEO_FILS][VIDEO_COLS];

// modo debug
datos32_t datos;
segmentos32_t segmentos;
ejecucion32_t ejecucion;
control32_t control;
stack32_t stack;

/*
================================================================================
                               ~~~ funciones ~~~
================================================================================
*/

// tick del reloj global
void screen_actualizar_reloj_global()
{
    static uint reloj_global = 0;

    reloj_global = (reloj_global + 1) % reloj_size;

    screen_pintar(reloj[reloj_global], C_BW, 49, 79);
}

// pinta un "pixel" de la pantalla
void screen_pintar(uchar c, uchar color, uint fila, uint columna)
{
    p[fila][columna].c = c;
    p[fila][columna].a = color;
}

// imprime un string en pantalla
void print(const char * text, uint x, uint y, ushort attr) {
    int i;
    for (i = 0; text[i] != 0; i++)
     {
        screen_pintar(text[i], attr, y, x);

        x++;
        if (x == VIDEO_COLS) {
            x = 0;
            y++;
        }
    }
}

// imprime un numero en hexa en pantalla
void print_hex(uint numero, int size, uint x, uint y, ushort attr) {
    int i;
    char hexa[8];
    char letras[16] = "0123456789ABCDEF";
    hexa[0] = letras[ ( numero & 0x0000000F ) >> 0  ];
    hexa[1] = letras[ ( numero & 0x000000F0 ) >> 4  ];
    hexa[2] = letras[ ( numero & 0x00000F00 ) >> 8  ];
    hexa[3] = letras[ ( numero & 0x0000F000 ) >> 12 ];
    hexa[4] = letras[ ( numero & 0x000F0000 ) >> 16 ];
    hexa[5] = letras[ ( numero & 0x00F00000 ) >> 20 ];
    hexa[6] = letras[ ( numero & 0x0F000000 ) >> 24 ];
    hexa[7] = letras[ ( numero & 0xF0000000 ) >> 28 ];
    for(i = 0; i < size; i++) {
        p[y][x + size - i - 1].c = hexa[i];
        p[y][x + size - i - 1].a = attr;
    }
}

// imprime un numero decimal en pantalla
void print_dec(uint numero, int size, uint x, uint y, ushort attr) {
    int i;
    char letras[16] = "0123456789";

    for(i = 0; i < size; i++) {
        int resto  = numero % 10;
        numero = numero / 10;
        p[y][x + size - i - 1].c = letras[resto];
        p[y][x + size - i - 1].a = attr;
    }
}

// pinta un rectangulo en pantalla
void screen_pintar_rect(uchar letra, uchar color, int fila, int columna, int alto, int ancho)
{
    int f, c;
    for (f = fila; f < fila + alto; f++)
    {
        for (c = columna; c < columna+ancho; c++)
            screen_pintar(letra, color, f, c);
    }
}

// pinta una linea horizontal
void screen_pintar_linea_h(uchar c, uchar color, int fila, int columna, int ancho)
{
    screen_pintar_rect(c, color, fila, columna, 1, ancho);
}

// pinta una linea vertical
void screen_pintar_linea_v(uchar c, uchar color, int fila, int columna, int alto)
{
    screen_pintar_rect(c, color, fila, columna, alto, 1);
}

// pinta el mapa, los huesos, los jugadores, etc
void screen_inicializar()
{
    screen_pintar_rect(' ', C_BG_BLACK | C_FG_WHITE, 0, 0, VIDEO_FILS, VIDEO_COLS);
    screen_pintar_rect(' ', C_BG_RED   | C_FG_WHITE, 45, 33, 5, 7);
    screen_pintar_rect(' ', C_BG_BLUE  | C_FG_WHITE, 45, 40, 5, 7);


    screen_pintar_relojes();
    screen_pintar_puntajes();

    int i, j;
    for (i = 0; i < MAPA_ALTO; i++)
    {
        for (j = 0; j < MAPA_ANCHO; j++)
            //screen_pintar( ' ', C_BG_LIGHT_GREY, i+1, j );
            screen_actualizar_posicion_mapa(j, i);
    }
}

// pinta los puntajes en el rect??ngulo de abajo
void screen_pintar_puntajes()
{
    print_dec(jugadorA.puntos, 3, 33+2, 45+2, C_BG_RED  | C_FG_WHITE);
    print_dec(jugadorB.puntos, 3, 40+2, 45+2, C_BG_BLUE | C_FG_WHITE);
}

// helper: color de un jugador
uchar screen_color_jugador(jugador_t *j)
{
    if (j == NULL)
        return C_FG_LIGHT_GREY;

    if (j->index == JUGADOR_A)
        return C_FG_RED;
    else
        return C_FG_BLUE;
}

// helper: caracter segun tipo de perro
uchar screen_caracter_perro(uint tipo)
{
    if (tipo == TIPO_1) return '1';
    if (tipo == TIPO_2) return '2';
    while(1){};
    return '?';
}

// pinta el reloj de un perro
void screen_pintar_reloj_perro(perro_t *perro)
{
	jugador_t *j = perro->jugador;
    uint columna = j->index == JUGADOR_A ? POSICION_RELOJES_C_A : POSICION_RELOJES_C_B;

    uchar col_libre = C_BG_BLACK | screen_color_jugador(j);

    uchar c, col;
    if (perro->libre == TRUE) {
        c = 'X';
        col = col_libre;
    } else {
        c = reloj[perro->indice_reloj];
        col = C_BW;
    }

    screen_pintar('1' + perro->index, C_BW, POSICION_RELOJES_F    , columna + perro->index * 2);
    screen_pintar(                 c,  col, POSICION_RELOJES_F + 2, columna + perro->index * 2);
}

// pinta los relojes de los perros de un jugador
void screen_pintar_reloj_perros(jugador_t *j)
{
    int i;
    for(i = 0; i < MAX_CANT_PERROS_VIVOS; i++)
        screen_pintar_reloj_perro(&j->perros[i]);
}

// pinta los relojes de todos los perros de cada jugador
void screen_pintar_relojes()
{
    screen_pintar_reloj_perros(&jugadorA);
    screen_pintar_reloj_perros(&jugadorB);
}

// se actualiza el tick del reloj de un perro
void screen_actualizar_reloj_perro(perro_t *perro)
{
    perro->indice_reloj = (perro->indice_reloj + 1) % reloj_size;
    screen_pintar_reloj_perro(perro);
}

// se actualiza el tick del reloj de todos lo perros
void screen_actualizar_relojes_perro() {
    int i;
    perro_t *perro;

    for(i = 0; i < MAX_CANT_PERROS_VIVOS; i++) {
        perro = &jugadorA.perros[i];

        if(perro->libre == FALSE) {
            perro->indice_reloj = (perro->indice_reloj + 1) % reloj_size;
        }
    }

    for(i = 0; i < MAX_CANT_PERROS_VIVOS; i++) {
        perro = &jugadorB.perros[i];

        if(perro->libre == FALSE) {
            perro->indice_reloj = (perro->indice_reloj + 1) % reloj_size;
        }
    }
}

// pinta un perro
void screen_pintar_perro(perro_t *perro)
{
    uchar c     = screen_caracter_perro(perro->tipo);
    uchar color = C_MAKE_BG(screen_color_jugador(perro->jugador)) | C_FG_WHITE;

    screen_pintar(c, color, perro->y+1, perro->x);
}

// borra un perro
void screen_borrar_perro(perro_t *perro)
{
    screen_pintar('.', C_BG_GREEN | C_FG_BLACK, perro->y+1, perro->x);
    screen_actualizar_posicion_mapa(perro->x, perro->y);
}

// pinta un jugador
void screen_pintar_jugador(jugador_t *j)
{
    uchar c     = 'A' + j->index;
    uchar color = C_MAKE_BG(screen_color_jugador(j)) | C_FG_WHITE;

    screen_pintar(c, color, j->y+1, j->x);
}

// borra un jugador
void screen_borrar_jugador(jugador_t *j)
{
    screen_pintar('.', C_BG_GREEN | C_FG_BLACK, j->y+1, j->x);
    screen_actualizar_posicion_mapa(j->x, j->y);
}

// pinta un escondite
void screen_pintar_escondite(uint valor, uint x, uint y) {
    uchar c     = screen_caracter_tesoro(valor);
    uchar color = C_BG_GREEN | C_FG_BLACK;

    screen_pintar(c, color, y+1, x);
}

// pinta una cucha
void screen_pintar_cucha(uint x, uint y) {
    uchar c     = 'x';
    uchar color = C_BG_GREEN | C_FG_BLACK;

    screen_pintar(c, color, y+1, x);
}

// pinta el fondo del mapa
void screen_pintar_fondo(uint x, uint y) {
    uchar c     = ' ';
    uchar color = C_BG_GREEN | C_FG_BLACK;

    screen_pintar(c, color, y+1, x);
}

// helper: caracter segun estado de escondite
uchar screen_caracter_tesoro(uint valor) {
    if (valor > 100)
        return 'O';
    else
        return 'o';
}

// pinta en la posicion indicada el elemento del juego que corresponda
void screen_actualizar_posicion_mapa(uint x, uint y)
{
    uint valor;
    perro_t *perro;

    //bg    = C_BG_GREEN;
    valor = game_huesos_en_posicion(x, y);
    perro = game_perro_en_posicion(x, y);

    // se pinta un perro
    if(perro != NULL) {
        screen_pintar_perro(perro);
        return;
    }

    // se pinta un escondite
    if(valor > 0) {
        screen_pintar_escondite(valor, x, y);
        return;
    }

    // se pinta la cucha
    if((jugadorA.x_cucha == x && jugadorA.y_cucha == y) || (jugadorB.x_cucha == x && jugadorB.y_cucha == y)) {
        screen_pintar_cucha(x, y);
        return;
    }

    screen_pintar_fondo(x, y);
}

// pinta un cartel con el ganador del juego para luego terminar la ejecucion
void screen_stop_game_show_winner(jugador_t *j) {
    int offy = 14; //(50/2 - 11);
    int offx = 20; //(80/2 - 20);

    int ancho = 40;
    int alto = 17;

    uchar color = screen_color_jugador(j);

    screen_pintar_rect(' ', C_MAKE_BG(color) | C_FG_WHITE, offy, offx, alto, ancho);

    offy++; offx++; alto -= 2; ancho-=2;

    screen_pintar_rect('*', C_BW, offy, offx, alto, ancho);

    offy++; offx++; alto -= 2; ancho-=2;

    screen_pintar_rect(' ', C_BG_LIGHT_GREY | C_FG_BLACK, offy, offx, alto, ancho);

    print("EL GANADOR ES EL JUGADOR",   offx+5  , offy+3 , C_BG_LIGHT_GREY | C_FG_BLACK);

    if(j == NULL)      print("EMPATE", offx+14, offy+6, C_MAKE_BG(color) | C_FG_BLACK);
    if(j == &jugadorA) print("<-- A",  offx+15, offy+6, C_BG_LIGHT_GREY  | color);
    if(j == &jugadorB) print("B -->",  offx+15, offy+6, C_BG_LIGHT_GREY  | color);
    // a partir de aca se termina el unviverso (STOP GAME)
    __asm __volatile( "cli\n" : : : );
    while(1){}
}

// copia el estado de la pantalla actual
void screen_guardar_pantalla() {
    uint i, j;
    
    i = 0;
    while(i< VIDEO_FILS ){
        j = 0;
        while(j < VIDEO_COLS ){
            aux_screen[i][j].c = p[i][j].c;
            aux_screen[i][j].a = p[i][j].a;
            j++;
        }
        i++;
    }
}

// recupera el estado de la pantalla guardada
void screen_cargar_pantalla() {
    uint i, j;
    
    i = 0;
    while(i< VIDEO_FILS ){
        j = 0;
        while(j < VIDEO_COLS ){
            p[i][j].c = aux_screen[i][j].c;
            p[i][j].a = aux_screen[i][j].a;
            j++;
        }
        i++;
    }
}

// imprime registros por pantalla
void screen_imprimir_registros() {
    int i;
    uint x1, x2, y;
    uint *p_uint;
    ushort *p_ushort;

    // se imprime el cuadro
    screen_pintar_linea_h(' ', C_BG_BLACK,  7, 26, 28);
    screen_pintar_linea_h(' ', C_BG_BLACK, 42, 26, 28);
    
    screen_pintar_linea_v(' ', C_BG_BLACK, 7, 25, 36);
    screen_pintar_linea_v(' ', C_BG_BLACK, 7, 54, 36);

    screen_pintar_linea_h(' ', C_BG_RED, 8, 26, 28);

    screen_pintar_rect(' ', C_BG_LIGHT_GREY, 9, 26, 33, 28);

    // se imprimen los labels
    x1 = 27;
    x2 = 31;
    y  = 10;

    p_uint = (uint *) &datos;

    for(i = 0; i < 8; i++) {
        print(label_datos[i], x1, y, C_BG_LIGHT_GREY | C_FG_BLACK);
        print_hex(p_uint[i], 8, x2, y, C_BG_LIGHT_GREY | C_FG_WHITE);
        y = y + 2;
    }

    print(label_ejecucion[0], x1, y, C_BG_LIGHT_GREY | C_FG_BLACK);
    print_hex(ejecucion.eip, 8, x2, y, C_BG_LIGHT_GREY | C_FG_WHITE);
    y = y + 2;

    x1 = x1 + 1;

    p_ushort = (ushort *) &segmentos;

    for(i = 0; i < 6; i++) {
        print(label_segmentos[i], x1, y, C_BG_LIGHT_GREY | C_FG_BLACK);
        print_hex(p_ushort[i], 4, x2, y, C_BG_LIGHT_GREY | C_FG_WHITE);
        y = y + 2;
    }


    x2 = 35;

    print(label_ejecucion[1], x1, y, C_BG_LIGHT_GREY | C_FG_BLACK);
    print_hex(ejecucion.eflags, 8, x2, y, C_BG_LIGHT_GREY | C_FG_WHITE);
    y = y + 2;

    x1 = 41;
    x2 = 45;
    y  = 10;

    p_uint = (uint *) &control;

    for(i = 0; i < 4; i++) {
        print(label_control[i], x1, y, C_BG_LIGHT_GREY | C_FG_BLACK);
        print_hex(p_uint[i], 8, x2, y, C_BG_LIGHT_GREY | C_FG_WHITE);
        y = y + 2;
    }

    p_uint = (uint *) &stack;

    print(label_stack[0], x1, y, C_BG_LIGHT_GREY | C_FG_BLACK);
    y = y + 2;

    for(i = 0; i < 5; i++) {
        print_hex(p_uint[i], 8, x2, y, C_BG_LIGHT_GREY | C_FG_WHITE);
        y = y+1;
    }
}

// test impresion
void screen_test() {
    screen_pintar_rect(' ', C_BG_BLACK | C_FG_WHITE, 0, 0, VIDEO_FILS, VIDEO_COLS);
    screen_pintar(' ', C_BG_RED, 0, 0 );
}

// imprime el nombre del grupo en pantalla
void screen_imprimir_nombre_grupo() {
    print("Smooth Criminal", VIDEO_COLS-16, 0, C_BG_BLACK | C_FG_WHITE);
}

