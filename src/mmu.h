/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "defines.h"
#include "game.h"

// devuelve la proxima pagina libre del area libre del kernel
uint mmu_proxima_pagina_fisica_libre();

// inicializa variables y estructuras globales para el manejo de memoria
void mmu_inicializar();

// setea en cero todos los bytes
void mmu_inicializar_pagina(uint pagina);

// identity mapping
void mmu_identity_mapping( uint directory);

// copia los bytes de una pagina a otra
void mmu_copiar_pagina(uint src, uint dst);

// pide una pagina para usar de directorio. Luego inicializa las entradas que iran con identity mapping.
uint mmu_inicializar_dir_kernel();

// transforma coordenadas (x,y) en direcciones fisicas
uint mmu_xy2fisica(uint x, uint y);

// transforma coordenadas (x,y) en direcciones virtuales
uint mmu_xy2virtual(uint x, uint y);

// crea el directorio, las paginas, copia el codigo e inicializa el stack
uint mmu_inicializar_memoria_perro(perro_t *perro);

// debe remapear y copiar el codigo
void mmu_mover_perro(perro_t *perro, int viejo_x, int viejo_y);


void mmu_mapear_pagina  (uint virtual, uint cr3, uint fisica, uint attrs);
void mmu_unmapear_pagina(uint virtual, uint cr3);

// Se inicializa el mapa de memoria de una tarea perro y se lo intercambia con
// el del kernel
uint mmu_test_ejercicio4();

#endif	/* !__MMU_H__ */
