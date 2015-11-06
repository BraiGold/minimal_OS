
#include "game.h"
#include "mmu.h"
#include "screen.h"
#include "tss.h"


// realiza inicialización básica de un perro. El perro aun no está vivo ni por lanzarse. Setea jugador, indice, etc
void game_perro_inicializar(perro_t *perro, jugador_t *j, uint index, uint id)
{
	perro->id      = id;
    perro->index   = index;
    perro->jugador = j;
	perro->libre   = TRUE;

//	~~~ completar si es necesario ~~~

}

// toma un perro ya existente libre y lo recicla seteando x e y a la cucha.
// luego inicializa su mapeo de memoria, tss, lo agrega al sched y pinta la pantalla
void game_perro_reciclar_y_lanzar(perro_t *perro, uint tipo)
{
	jugador_t *j = perro->jugador;

	perro->x = j->x_cucha;
	perro->y = j->y_cucha;
	perro->tipo = tipo;
	perro->libre = FALSE;

	// ahora debo llamar a rutinas que inicialicen un nuevo mapa de
	// memoria para el nuevo perro, que carguen su tss correspondiente,
	// lo scheduleen y finalmente lo pinten en pantalla

	// ~~~ completar ~~~

}

// el perro descargó sus huesos o realizó una acción no válida y caputó, hay que sacarlo del sistema.
void game_perro_termino(perro_t *perro)
{
    screen_borrar_perro(perro);
    perro->libre = TRUE;
}

// transforma código de dirección en valores x e y 
uint game_dir2xy(/* in */ direccion dir, /* out */ int *x, /* out */ int *y)
{
	switch (dir)
	{
		case IZQ: *x = -1; *y =  0; break;
		case DER: *x =  1; *y =  0; break;
		case ABA: *x =  0; *y =  1; break;
		case ARR: *x =  0; *y = -1; break;
    	default: return -1;
	}

	return 0;
}

uint game_hay_perro_amigo(perro_t *perro, int x, int y) {
    jugador_t *jugador;
    perro_t *perro_amigo;
    int i;

    jugador = perro->jugador;

    for(i = 0; i < MAX_CANT_PERROS_VIVOS; i++) {
        perro_amigo = &(jugador->perros[i]);
        if(perro_amigo->libre == FALSE && perro_amigo->x == x && perro_amigo->y == y) {
            return TRUE;
        }
    }
    return FALSE;
}

// recibe una direccion y un perro, al cual debe mover en esa dirección
// *** viene del syscall mover ***
uint game_perro_mover(perro_t *perro, direccion dir)
{
    uint ant_x, ant_y;
    int dst_x, dst_y;

    // se copia temporalmente la posición (x,y) del perro
    ant_x = perro->x;
    ant_y = perro->y;
    dst_x = perro->x;
    dst_y = perro->y;

    // usando la variables temporales se determinan los nuevos valores (x,y)
    switch(dir) {
        case  ARR: dst_y++; break;
        case  ABA: dst_y--; break;
        case  DER: dst_x++; break;
        case  IZQ: dst_x--; break;
        case AQUI: break;
    }

    // si la posicion destino es valida y no hay ningun perro amigo ahi
    if(game_es_posicion_valida(dst_x,dst_y) && !game_hay_perro_amigo(perro,dst_x,dst_y)) {
        // se actualiza posicion del perro
        perro->x = dst_x;
        perro->y = dst_y;

        // se remapea y copia el codigo del perro
        mmu_mover_perro(perro, ant_x, ant_y);

        // se actualizan la visualizacion de las posiciones en el mapa
        screen_actualizar_posicion_mapa(ant_x, ant_y);
        screen_actualizar_posicion_mapa(dst_x, dst_y);
    }

    return 0x42;
}

// recibe un perro, el cual debe cavar en su posición
// *** viene del syscall cavar ***
uint game_perro_cavar(perro_t *perro) {
    int i;

    // un perro no puede cargar mas de 10 huesos
    if(perro->huesos == 10) {
        return 0;
    }

    // Si el perro esta parado sobre un escondite entonces se pone a cavar
    for(i = 0; i < ESCONDITES_CANTIDAD; i++) {
        if(escondites[i][0] == perro->x && escondites[i][1] == perro->y) {
            perro->huesos++;
            escondites[i][3]--;
        }
    }

	return 0;
}

// recibe un perro, devueve la dirección del hueso más cercano
// *** viene del syscall olfatear ***
uint game_perro_olfatear(perro_t *perro) {
	int x_actual_diff = 1000, y_actual_diff = 1000;


	int i;
	for (i = 0; i < ESCONDITES_CANTIDAD; i++)
	{
		if (escondites[i][2] == 0) continue;

		int diff_x = escondites[i][0] - (int)perro->x;
		int diff_y = escondites[i][1] - (int)perro->y;

		if (x_actual_diff * x_actual_diff + y_actual_diff * y_actual_diff > diff_x * diff_x + diff_y * diff_y)
		{
			x_actual_diff = diff_x;
			y_actual_diff = diff_y;
		}
   	}

	if (x_actual_diff == 0 && y_actual_diff == 0)
		return AQUI;

	if (x_actual_diff * x_actual_diff > y_actual_diff * y_actual_diff)
	{
		return x_actual_diff > 0 ? DER : IZQ;
	}
	else 
	{
		return y_actual_diff > 0 ? ABA : ARR;
	}

    return 0;
}


// chequea si el perro está en la cucha y suma punto al jugador o lo manda a dormir
void game_perro_ver_si_en_cucha(perro_t *perro) {
	if (perro->x != perro->jugador->x_cucha || perro->y != perro->jugador->y_cucha)
		return;

	if (perro->huesos == 0)
		return;

	game_jugador_anotar_punto(perro->jugador);
	perro->huesos--;
	if (perro->huesos == 0)
		game_perro_termino(perro);
}

// devuelve la ultima orden recibida
uint game_perro_recibir_orden(perro_t *perro) {
    jugador_t *jugador;

    jugador = perro->jugador;

    return jugador->orden << 16 | jugador->y << 8 | jugador->x;
}

