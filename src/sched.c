/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/


#include "sched.h"
#include "i386.h"
#include "screen.h"
#include "defines.h"

sched_t scheduler;

int ind_a = -1; // indice de tareas del jugador A
int ind_b = -1; // indice de tareas del jugador B

// se inicializan las esctructuras del scheduler
void sched_inicializar()
{
    /*
     * El scheduler se arma de una manera sencilla: todas las tareas ya tienen
     * una posición determinada en el scheduler de modo que para liberar o
     * agregar una tarea del mismo debemos determinar si el perro asociado esta
     * libre o no.
     * Considerenmos, ademas, que nunca liberamos la tarea IDLE.
     */
    uint i, j;

	scheduler.current = 0;

    // Primera tarea del scheduler es la tarea IDLE
	scheduler.tasks[0].gdt_index = GDT_IDX_TSS_IDLE;
    scheduler.tasks[0].perro     = NULL;

    i = 1;
    
    // Agrego los perros del jugador A al scheduler
    j = 0;
	while( j < MAX_CANT_PERROS_VIVOS ){
		scheduler.tasks[i].gdt_index = jugadorA.perros[j].id;
        scheduler.tasks[i].perro     = &jugadorA.perros[j];
		i++;
        j++;
	}

    // Agrego los perros del jugador B al scheduler
    j = 0;
	while( j < MAX_CANT_PERROS_VIVOS ){
		scheduler.tasks[i].gdt_index = jugadorB.perros[j].id;
        scheduler.tasks[i].perro     = &jugadorB.perros[j];
		i++;
        j++;
	}
}

// busca la proxima tarea libre del jugador A
int sched_buscar_tarea_libre_a() {
    return sched_buscar_tarea_libre(1, &ind_a);
}

// busca la proxima tarea libre del jugador B
int sched_buscar_tarea_libre_b() {
    return sched_buscar_tarea_libre(9, &ind_b);
}

// busca la proxima tarea libre para un determinado jugador
int sched_buscar_tarea_libre(int base, int *indice) {
    int i;

    (*indice)++;

    for(i = (*indice); i < MAX_CANT_PERROS_VIVOS; i++) {
        if(scheduler.tasks[base+i].perro->libre == FALSE) {
            (*indice) = i;
            return base + i;
        }
    }

    (*indice) = -1;

    return (*indice);
}

// debe devolver el perro correspondiente a la tarea que está corriendo actualmente
perro_t* sched_tarea_actual() {
    return scheduler.tasks[scheduler.current].perro;
}

// debe agregar una tarea al scheduler
void sched_agregar_tarea(perro_t *perro) {
	perro->libre = FALSE;
}

// debe remover tareas del scheduler
void sched_remover_tarea(perro_t *perro) {
	perro->libre = TRUE;
}

// determina la tarea que debe ser ejecutada
uint sched_proxima_a_ejecutar() {
    int i;

    // si la tarea actual es una tarea perro del jugador A
    if( scheduler.current > 0 && scheduler.current < 9 ) {
        i = sched_buscar_tarea_libre_b();

        if(i != -1) {
            // es el turno de una tarea del jugador B
            return i;
        }

        i = sched_buscar_tarea_libre_a();

        if(i != -1) {
            // es el turno de una tarea del jugador A
            return i;
        }

    // si la tarea actual es una tarea perro del jugador B o es la tarea IDLE
    } else {
        i = sched_buscar_tarea_libre_a();

        if(i != -1) {
            // es el turno de una tarea del jugador A
            return i;
        }

        i = sched_buscar_tarea_libre_b();

        if(i != -1) {
            // es el turno de una tarea del jugador B
            return i;
        }
    }

    // es el turno de la tarea IDLE
    return 0;
}

// debe avisar al juego que hubo un tick (para que haga cosas del juego) y luego
// configurarse para pasar a la siguiente tarea (devuelve el segmento con el que
// debe hacerse el salto de tarea)
ushort sched_atender_tick(){
    uint gdt_index;

    // se determina proxima tarea en caso que debug_time no este activo
    if(exploto_algo == FALSE) {
        // se determina cual es la proxima tarea a ejecutar
	    scheduler.current = sched_proxima_a_ejecutar();
        game_perro_actual = sched_tarea_actual();
    }

    // se obtiene el indice de la GDT de la tarea actual
    gdt_index = scheduler.tasks[scheduler.current].gdt_index;

    // se devuelve el selector correspondiente
    return SELECTOR_SEGMENTO(gdt_index, IT_GDT, RPL_0);
}


