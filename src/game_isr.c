
#include "game.h"
#include "mmu.h"
#include "tss.h"
#include "screen.h"

/// ************** RUTINAS DE ATENCION DE INTERRUPCIONES DE ************** ///
///                              ALTO NIVEL                                ///
///                  (deben ser llamadas desde isr.asm)                    ///
/// ********************************************************************** ///

// se activa o desactiva el modo debug
void game_desactive_active_debug() {
    if (debug_time == FALSE) {
        debug_time = TRUE;
    } else {
        debug_time = FALSE;
        if(exploto_algo == TRUE) {
            exploto_algo = FALSE;
            ya_impresa   = FALSE;
            screen_cargar_pantalla();
        }
    }
}

// se indica que se debe mostrar la ventana con la informacion de los registros
void game_activar_ventana_debug(){
    if(debug_time == TRUE) {
        if(ya_impresa == FALSE){
            screen_guardar_pantalla();
        }
        exploto_algo = TRUE;
    }
}

// gasta un rato en un ciclo infinito, util para hacer pausas y debuguear
void wait(int pseudosecs){
	int count;
	for (count = 0; count < pseudosecs * 1000000; count++) {}
}

uint game_syscall_manejar(uint syscall, uint param1)
{
    switch(syscall) {
        // moverse
        case 0x1:
            return game_perro_mover(game_perro_actual, param1);

        // cavar
        case 0x2:
            return game_perro_cavar(game_perro_actual);
            break;

        // olfatear
        case 0x3:
            return game_perro_olfatear(game_perro_actual);
            break;

        // recibir orden
        case 0x4:
            return game_perro_recibir_orden(game_perro_actual);
            break;
    }

    return 0x42;
}

// ~~~ debe atender la interrupción de reloj para actualizar la pantalla y terminar si es hora,
// ~~~ recibe el perro que está corriendo actualmente
void game_atender_tick()
{
    // actualizo reloj global
    screen_actualizar_reloj_global();

    if(exploto_algo == FALSE) {
        // actualizo relojes de perros
        screen_actualizar_relojes_perro();
        screen_pintar_relojes();

        // el perro esta en la cucha...
        if(game_perro_actual != NULL) {
            game_perro_ver_si_en_cucha(game_perro_actual);
        }

        // determino si es hora de terminar el juego
        game_terminar_si_es_hora();

    } else {
        if(ya_impresa == FALSE) {
            ya_impresa = TRUE;
            screen_imprimir_registros();
        }
    }
}


#define KB_w        0x11 // 0x91
#define KB_s        0x1f // 0x9f
#define KB_a        0x1e // 0x9e
#define KB_d        0x20 // 0xa0

#define KB_i        0x17 // 0x97
#define KB_k        0x25 // 0xa5
#define KB_j        0x24 // 0xa4
#define KB_l        0x26 // 0xa6


#define KB_q        0x10 // 0x90
#define KB_e        0x12 // 0x92

#define KB_u        0x16 // 0x96
#define KB_o        0x18 // 0x98

#define KB_z        0x2C // ?
#define KB_x        0x2D // ?
#define KB_c        0x2E // ?

#define KB_b        0x30 // ?
#define KB_n        0x31 // ?
#define KB_m        0x32 // ?

#define KB_y        0x15 // ?

#define KB_shiftL   0x2a // 0xaa
#define KB_shiftR   0x36 // 0xb6


// ~~~ debe atender la interrupción de teclado, se le pasa la tecla presionada
void game_atender_teclado(unsigned char tecla)
{
    if (exploto_algo == FALSE || tecla == KB_y){

        switch (tecla){
            /*case KB_w: screen_pintar('W', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;
            case KB_s: screen_pintar('S', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;
            case KB_a: screen_pintar('A', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;
            case KB_d: screen_pintar('D', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;

            case KB_i: screen_pintar('I', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;
            case KB_k: screen_pintar('K', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;
            case KB_j: screen_pintar('J', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;
            case KB_l: screen_pintar('L', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;

            case KB_q: screen_pintar('Q', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;
            case KB_e: screen_pintar('E', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;

            case KB_u: screen_pintar('U', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;
            case KB_o: screen_pintar('O', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;

            case KB_z: screen_pintar('Z', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;
            case KB_x: screen_pintar('X', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;
            case KB_c: screen_pintar('C', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;

            case KB_b: screen_pintar('B', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;
            case KB_n: screen_pintar('N', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;
            case KB_m: screen_pintar('M', C_BG_BLACK | C_FG_WHITE, 0, VIDEO_COLS-2); break;

            case KB_y: desactive_active_debug(); break;

            case KB_shiftL: screen_pintar(tecla, C_BG_BLACK | C_FG_WHITE, VIDEO_COLS-2, 0); break;
            case KB_shiftR: screen_pintar(tecla, C_BG_BLACK | C_FG_WHITE, VIDEO_COLS-2, 0); break;*/

            case KB_y: game_desactive_active_debug(); break;

            case KB_q: game_jugador_lanzar_perro(&jugadorA, TIPO_1, 0, 0); break;
            case KB_e: game_jugador_lanzar_perro(&jugadorA, TIPO_2, 0, 0); break;

            case KB_u: game_jugador_lanzar_perro(&jugadorB, TIPO_1, 0, 0); break;
            case KB_o: game_jugador_lanzar_perro(&jugadorB, TIPO_2, 0, 0); break;

            case KB_a: game_jugador_moverse(&jugadorA, -1,  0); break;
            case KB_d: game_jugador_moverse(&jugadorA,  1,  0); break;
            case KB_w: game_jugador_moverse(&jugadorA,  0, -1); break;
            case KB_s: game_jugador_moverse(&jugadorA,  0,  1); break;

            case KB_j: game_jugador_moverse(&jugadorB, -1,  0); break;
            case KB_l: game_jugador_moverse(&jugadorB,  1,  0); break;
            case KB_i: game_jugador_moverse(&jugadorB,  0, -1); break;
            case KB_k: game_jugador_moverse(&jugadorB,  0,  1); break;

            case KB_z: game_jugador_dar_orden(&jugadorA, 0); break;
            case KB_x: game_jugador_dar_orden(&jugadorA, 1); break;
            case KB_c: game_jugador_dar_orden(&jugadorA, 2); break;

            case KB_b: game_jugador_dar_orden(&jugadorB, 0); break;
            case KB_n: game_jugador_dar_orden(&jugadorB, 1); break;
            case KB_m: game_jugador_dar_orden(&jugadorB, 2); break;

            default: break;
        }
    }
}

