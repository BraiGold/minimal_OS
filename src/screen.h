/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "colors.h"
#include "defines.h"
#include "i386.h"
#include "sched.h"
#include "game.h"

/*
================================================================================
                              ~~~ definiciones ~~~
================================================================================
*/

#define VIDEO_FILS 50
#define VIDEO_COLS 80

#define VIDEO      0x000B8000 /* direccion fisica del buffer de video */

/*
================================================================================
                              ~~~ estructuras ~~~
================================================================================
*/

/* Estructura para acceder a registros de datos */
typedef struct datos32_s {
    uint eax;
    uint ebx;
    uint ecx;
    uint edx;
    uint esi;
    uint edi;
    uint ebp;
    uint esp;
} __attribute__((__packed__)) datos32_t;

/* Estructura para acceder a registros de segmento */
typedef struct segmentos32_s {
    ushort cs;
    ushort ds;
    ushort es;
    ushort fs;
    ushort gs;
    ushort ss;
} __attribute__((__packed__)) segmentos32_t;

/* Estructura para acceder a registros de ejecucion */
typedef struct ejecucion32_s {
    uint eip;
    uint eflags;
} __attribute__((__packed__)) ejecucion32_t;

/* Estructura para acceder a registros de control */
typedef struct control32_s {
    uint cr0;
    uint cr2;
    uint cr3;
    uint cr4;
} __attribute__((__packed__)) control32_t;

/* Estructura para acceder a algunos elementos de una pila */
typedef struct stack32_s {
    uint stack_0;
    uint stack_1;
    uint stack_2;
    uint stack_3;
    uint stack_4;
} __attribute__((__packed__)) stack32_t;

/* Estructura de para acceder a memoria de video */
typedef struct ca_s {
    uchar c;
    uchar a;
} ca;


struct perro_t;
typedef struct perro_t perro_t;

struct jugador_t;
typedef struct jugador_t jugador_t;

/*
================================================================================
                               ~~~ funciones ~~~
================================================================================
*/


int ee_printf(const char *fmt, ...);

// pinta un "pixel" de la pantalla
void screen_pintar(uchar c, uchar color, uint fila, uint columna);

// imprime un string en pantalla
void print(const char * text, uint x, uint y, ushort attr);

// imprime un numero en hexa en pantalla
void print_hex(uint numero, int size, uint x, uint y, ushort attr);

// imprime un numero decimal en pantalla
void print_dec(uint numero, int size, uint x, uint y, ushort attr); 

// pinta un rectangulo en pantalla
void screen_pintar_rect(uchar c, uchar color, int fila, int columna, int alto, int ancho);

// pinta una linea horizontal
void screen_pintar_linea_h(uchar c, uchar color, int fila, int columna, int ancho);

// pinta una linea vertical
void screen_pintar_linea_v(uchar c, uchar color, int fila, int columna, int alto);

// pinta el mapa, los huesos, los jugadores, etc
void screen_inicializar();

// pinta los puntajes en el rectángulo de abajo
void screen_pintar_puntajes();

// tick del reloj global
void screen_actualizar_reloj_global();

// actualiza el tick del reloj de un perro
void screen_actualizar_reloj_perro (perro_t *perro);

// actualiza el tick del reloj de todos lo perros
void screen_actualizar_relojes_perro();

// helper: color de un jugador
uchar screen_color_jugador(jugador_t *j);

// helper: caracter segun tipo de perro
uchar screen_caracter_perro(uint tipo);

// helper: caracter segun estado de escondite
uchar screen_caracter_tesoro(uint valor);

// pinta un perro
void screen_pintar_perro(perro_t *perro);

// borra un perro
void screen_borrar_perro(perro_t *perro);

// pinta un jugador
void screen_pintar_jugador(jugador_t *j);

// borra un jugador
void screen_borrar_jugador(jugador_t *j);

// pinta un escondite
void screen_pintar_escondite(uint valor, uint x, uint y);

// pinta una cucha
void screen_pintar_cucha(uint x, uint y);

// pinta el fondo del mapa
void screen_pintar_fondo(uint x, uint y);

// pinta el reloj de un perro
void screen_pintar_reloj_perro(perro_t *perro);

// pinta los relojes de los perros de un jugador
void screen_pintar_reloj_perros(jugador_t *j);

// pinta los relojes de todos los perros de cada jugador
void screen_pintar_relojes();

// pinta en la posicion indicada el elemento del juego que corresponda
void screen_actualizar_posicion_mapa(uint x, uint y);

// pinta un cartel con el ganador del juego para luego terminar la ejecucion
void screen_stop_game_show_winner(jugador_t *j);

// copia el estado de la pantalla actual
void screen_guardar_pantalla();

// recupera el estado de la pantalla guardada
void screen_cargar_pantalla();

// imprime registros por pantalla
void screen_imprimir_registros();

// test impresion
void screen_test();

// imprime el nombre del grupo en pantalla
void screen_imprimir_nombre_grupo();

#endif  /* !__SCREEN_H__ */
