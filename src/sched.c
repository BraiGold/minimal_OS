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

void sched_inicializar()
{
	scheduler.current = 0;
	uint i = 0;
	int gdt_index = GDT_IDX_TSS_IDLE;
	scheduler.tasks[i].gdt_index = gdt_index;

	gdt_index ++;
	while(i < (2 * MAX_CANT_PERROS_VIVOS) ){
		scheduler.tasks[i].gdt_index = i;
		scheduler.tasks[i].perro->id = gdt_index;
		
		i ++ ;
		gdt_index ++ ;
	}



}


int sched_buscar_indice_tarea(uint gdt_index) {

    return MAX_CANT_TAREAS_VIVAS;
}


int sched_buscar_tarea_libre(){
	perro_t* p = sched_tarea_actual() ;
	int i = p->index;
	int cont = 0;
	int no_hay_tarea = 0;
	while( no_hay_tarea == 0 && cont < MAX_CANT_TAREAS_VIVAS){
		cont ++ ;
		i = (i + 8) % MAX_CANT_PERROS_VIVOS;
		if(scheduler.tasks[i + 1].perro->libre){
			no_hay_tarea = 1;
		}
	}
	if (no_hay_tarea  == 1)
	{
		return 0;
	}else{
		return i + 1;
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
	
    return scheduler.tasks[scheduler.current].gdt_index;
}


