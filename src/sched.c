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
//int ind_a = 0;
//int ind_b = 0;
int ind_a = -1;
int ind_b = -1;

void sched_inicializar()
{
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


int sched_buscar_indice_tarea(uint gdt_index) {
    return MAX_CANT_TAREAS_VIVAS;
}

// busca la proxima tarea libre
/*int sched_buscar_tarea_libre(){
	//perro_t* p = sched_tarea_actual() ;
	int ant = scheduler.current;
	int res;
	
	if ((1 < ant) && (ant < 9)){
		ind_a = ant;
		res = sched_buscar_perro_libre(ind_b,9);
		if (res == 0){
            breakpoint();
			res = sched_buscar_perro_libre( ind_a , 1);
		}

	}else{
		ind_b = ant;
		res = sched_buscar_perro_libre(ind_a,1);
		if (res == 0){
			res = sched_buscar_perro_libre( ind_b , 9);
		}

	}

	return res;
}*/

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

/*
// devuelve un perro libre indicando un rango de valores
int sched_buscar_perro_libre(int dsd, int base){
	int i = 0;
	int no_hay_tarea = 1;
	while( no_hay_tarea == 1 && i < MAX_CANT_PERROS_VIVOS){
		i++;
		if(scheduler.tasks[base +(dsd + i % 8 )].perro->libre){
			no_hay_tarea = 0;
		}
	}

	if (no_hay_tarea  == 1){
		return 0;
	}else{
		return base +(dsd + i % 8 );
	}
}*/

perro_t* sched_tarea_actual(){
    return scheduler.tasks[scheduler.current].perro;
}

void sched_agregar_tarea(perro_t *perro){
	perro->libre = FALSE;
}

void sched_remover_tarea(perro_t *perro){
	perro->libre = TRUE;
}


uint sched_proxima_a_ejecutar(){
	//scheduler.current = sched_buscar_tarea_libre();    
    //return scheduler.current;

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


ushort sched_atender_tick(){
    game_atender_tick(game_perro_actual);

    // se determina cual es la proxima tarea a ejecutar
	scheduler.current = sched_proxima_a_ejecutar();
    game_perro_actual = sched_tarea_actual();

    return scheduler.tasks[scheduler.current].gdt_index << 3;
}


