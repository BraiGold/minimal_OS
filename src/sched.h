/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#ifndef __SCHED_H__
#define __SCHED_H__

#include "game.h"

#define MAX_CANT_TAREAS_VIVAS   (MAX_CANT_PERROS_VIVOS * 2)

/*
================================================================================
                       ~~~ estructuras del scheduler ~~~
================================================================================
*/

// cada tarea del scheduler debe estar determinada de antemano
typedef struct sched_task_t
{
    uint gdt_index;
	perro_t *perro;
} sched_task_t;

// el scheduler posee un arreglo de tareas (cada una puede estar libre o asignada)
typedef struct sched_t
{
    sched_task_t tasks[MAX_CANT_TAREAS_VIVAS+1];
    ushort current;
} sched_t;

extern sched_t scheduler;

/*
================================================================================
                       ~~~ funciones del scheduler ~~~
================================================================================
*/

// se inicializan las esctructuras del scheduler
void sched_inicializar();

// debe agregar una tarea al scheduler
void sched_agregar_tarea(perro_t *perro);

// debe remover tareas del scheduler
void sched_remover_tarea(perro_t *perro);

// debe devolver el perro correspondiente a la tarea que está corriendo actualmente
perro_t* sched_tarea_actual();

// busca la proxima tarea libre del jugador A
int sched_buscar_tarea_libre_a();

// busca la proxima tarea libre del jugador B
int sched_buscar_tarea_libre_b();

// busca la proxima tarea libre de un determinado jugador
int sched_buscar_tarea_libre(int base, int *indice);

// determina la tarea que debe ser ejecutada
uint sched_proxima_a_ejecutar();

// debe avisar al juego que hubo un tick (para que haga cosas del juego) y luego
// configurarse para pasar a la siguiente tarea (devuelve el segmento con el que
// debe hacerse el salto de tarea)
ushort sched_atender_tick();

#endif	/* !__SCHED_H__ */
