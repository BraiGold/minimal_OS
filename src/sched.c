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
int ind_a = 0;
int ind_b = 0;

void sched_inicializar()
{
    uint i, j;

	scheduler.current = 0;

    // Primera tarea del scheduler es la tarea IDLE
	scheduler.tasks[0].gdt_index = GDT_IDX_TSS_IDLE;

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


int sched_buscar_tarea_libre(){
	//perro_t* p = sched_tarea_actual() ;
	int ant = scheduler.current;
	int res;
	
	if (1 < ant < 9){
		ind_a = ant;
		res = buscar_perro_libre(ind_b,9);
		if (res == 0){
			res = buscar_perro_libre( ind_a , 1)
		}

	}else{
		ind_b = ant;
		res = buscar_perro_libre(ind_a,1);
		if (res == 0){
			res = buscar_perro_libre( ind_b , 9)
		}

	}

	return res;
    
}

int buscar_perro_libre( int dsd , int base ){
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
}

perro_t* sched_tarea_actual(){
    return scheduler.tasks[scheduler.current].perro;
}

void sched_agregar_tarea(perro_t *perro){
	perro->libre = 1;
}

void sched_remover_tarea(perro_t *perro){
	perro->libre = 0;
}


uint sched_proxima_a_ejecutar(){
	scheduler.current = sched_buscar_tarea_libre(); 
    return scheduler.current;
}


ushort sched_atender_tick(){
	sched_proxima_a_ejecutar();
	
    return scheduler.tasks[scheduler.current].gdt_index << 3;
}


