/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/
#include "defines.h"
#include "mmu.h"
#include "i386.h"


/* Atributos paginas                                                          */
/* -------------------------------------------------------------------------- */
#define PAG_PRESENTE    0x00000001
#define PAG_SOLO_LEC    0x00000000
#define PAG_LEC_ESC     0x00000002


/* Variables globales                                                         */
/* -------------------------------------------------------------------------- */
uint dir_tarea[2][2] = { {DIR_TAREA_A1, DIR_TAREA_A2},
		                 {DIR_TAREA_B1, DIR_TAREA_B2} };

uint ind_free_page = DIR_PDIR_KERNEL;

uint comun_dir_0;
uint comun_dir_1;


/* Funciones                                                                  */
/* -------------------------------------------------------------------------- */

// copia los bytes de una pagina a otra
void mmu_copiar_pagina(uint src, uint dst){
	uint i = 0;
	while(i < 1024){
		*((uint *)(dst + i*4)) = *((uint *)(src + i*4));;
		i++;

	}
}

// inicializa variables y estructuras globales para el manejo de memoria
void mmu_inicializar(){
	// inicializo el contador de paginas libres en el ox1000
	ind_free_page = MAPA_BASE_FISICA_LIBRE;

	// Las paginas q tiene en comun los perros de cada jugador
	comun_dir_0 = mmu_proxima_pagina_fisica_libre();
	comun_dir_1 = mmu_proxima_pagina_fisica_libre();
}

// crea el directorio, las paginas, copia el codigo e inicializa el stack
uint mmu_inicializar_memoria_perro(perro_t *perro){
    uint task_directory;
    uint dir_fisica, dir_virtual;

    // se pide una nueva pagina para crear un nuevo mapa de memoria
    task_directory = mmu_proxima_pagina_fisica_libre();

    mmu_inicializar_pagina(task_directory);

    // identity mapping del nuevo mapa de memoria
	mmu_identity_mapping(task_directory);

    // se obtiene la direccion fisica donde se copiara la tarea
    dir_fisica  = mmu_xy2fisica(perro->x, perro->y);

	// mapeo la direccion virtual con la cual se accede directamente al codigo
    // direccion fisica obtenida
	mmu_mapear_pagina(DIR_VIRTUAL_TAREA, task_directory, dir_fisica, PAG_LEC_ESC | PAG_PRESENTE);
	
    // mapeo la direccion virtual auxliar del mapa de memoria actual con la
    // direccion fisica obtenida
    mmu_mapear_pagina(DIR_VIRTUAL_AUX, rcr3(), dir_fisica, PAG_LEC_ESC | PAG_PRESENTE);

    // mapeo la direccion virtual auxiliar del nuevo mapa de memoria con una
    // direccion fisica cualquiera (en este caso con la base del mapa)
    mmu_mapear_pagina(DIR_VIRTUAL_AUX, task_directory, MAPA_BASE_FISICA, PAG_LEC_ESC | PAG_PRESENTE);

	// mapeo la direccion virtual que registra el area del mapa que la tarea
    // visita al principio con la direccion fisica obtenida
    dir_virtual = mmu_xy2virtual(perro->x, perro->y);

    mmu_mapear_pagina(dir_virtual, task_directory, dir_fisica, PAG_SOLO_LEC | PAG_PRESENTE);

	// mapeo el directorio virtual compartido
	dir_fisica = perro->jugador->index == 0 ? comun_dir_0 : comun_dir_1;
	mmu_mapear_pagina(DIR_VIRTUAL_AREA_COMP, task_directory, dir_fisica, PAG_LEC_ESC | PAG_PRESENTE);

    // copio el codigo de la tarea
    dir_fisica = dir_tarea[perro->jugador->index][perro->tipo];
    
    mmu_copiar_pagina(dir_fisica, DIR_VIRTUAL_AUX);

	return task_directory;
}

// setea en cero todos los bytes
void mmu_inicializar_pagina(uint pagina){
	uint ind  = 0;
	while(ind < 1024){
		* ((uint *) (pagina + (ind*4))) = (uint) 0;
		ind++;
	}
}

// identity mapping
void mmu_identity_mapping(uint directory){
	// Indentity mapping kernel + mememoria libre 	
	uint dir  = 0;
	while(dir < 0x400000){
		// En la dir lineal no pongo la pagina porque la va a pedir cuando mapee
		mmu_mapear_pagina(dir , directory , dir , PAG_LEC_ESC | PAG_PRESENTE);
		dir += PAGE_SIZE;
	}
}

// pide una pagina para usar de directorio. Luego inicializa las entradas que iran con identity mapping.
uint mmu_inicializar_dir_kernel(){
	uint cr3 = mmu_proxima_pagina_fisica_libre();

	//  clear directory
	mmu_inicializar_pagina(cr3);

	// identity mapping
	mmu_identity_mapping(cr3);

    // se mapea una direccion virtual auxiliar en la cual se registraran
    // direcciones fisicas que se quieran acceder cuando se crea un nuevo mapa
    // de memoria
    // se inicializa esta dirección vitual con cualquier direccion fisica (en
    // este caso con la base del mapa)
    mmu_mapear_pagina(DIR_VIRTUAL_AUX, cr3, MAPA_BASE_FISICA, PAG_LEC_ESC | PAG_PRESENTE);

	return cr3;
}

// devuelve la proxima pagina libre del area libre del kernel
uint mmu_proxima_pagina_fisica_libre(){
 	uint res = ind_free_page;
 	ind_free_page += PAGE_SIZE;
 	return res;
}

//
void mmu_mapear_pagina (uint virtual, uint cr3, uint fisica, uint attrs){
	uint directory_11_0 = virtual;
	directory_11_0 = (directory_11_0 >> 22) * 4;
	uint* directory = (uint*)(cr3 + (directory_11_0));
	uint page_31_12 ;
	if(*(directory) % 2 == 0){
		page_31_12 = mmu_proxima_pagina_fisica_libre();
		mmu_inicializar_pagina( page_31_12);
		*directory = (uint) ((page_31_12 & 0xfffff000) + (PAG_LEC_ESC | PAG_PRESENTE)); 
	}else{
		page_31_12 = ( (*directory) & 0xfffff000);
	} 
	uint page_11_0 = virtual;
	page_11_0 = (page_11_0 << 10);
	page_11_0 = (page_11_0 >> 22) * 4;
	uint* page = (uint*)  ( page_31_12  + (page_11_0));
	*(page) = (uint)(fisica + attrs);		
	tlbflush();
}

//
void mmu_unmapear_pagina(uint virtual, uint cr3){
	uint directory_11_0 = virtual;
	directory_11_0 = (directory_11_0 >> 22) * 4;
	uint* directory = (uint*)(cr3 + (directory_11_0));
	 
	if(*(directory) % 2 != 0){
		uint page_31_12 = ( (*directory) & 0xfffff000);
		uint page_11_0 = virtual;
		page_11_0 = (page_11_0 << 10);
		page_11_0 = (page_11_0 >> 22) * 4;
		uint* page = (uint*)  ( page_31_12  + (page_11_0));

		*(page) = (uint) (((*page) >> 1) << 1) ;
		tlbflush();
	}
}

// transforma coordenadas (x,y) en direcciones fisicas
uint mmu_xy2fisica(uint x, uint y) {
    return MAPA_BASE_FISICA + game_xy2lineal(x,y) * PAGE_SIZE;
}

// transforma coordenadas (x,y) en direcciones virtuales
uint mmu_xy2virtual(uint x, uint y) {
    return MAPA_BASE_VIRTUAL + game_xy2lineal(x,y) * PAGE_SIZE;
}

// debe remapear y copiar el codigo
void mmu_mover_perro(perro_t *perro, int viejo_x, int viejo_y) {
    uint dir_fisica_ant, dir_fisica_nue, dir_virtual_nue;
    uint cr3;
    
    // se obtienen las direcciones
    dir_fisica_ant  = mmu_xy2fisica(viejo_x, viejo_y);
    dir_fisica_nue  = mmu_xy2fisica(perro->x, perro->y);

    dir_virtual_nue = mmu_xy2virtual(perro->x, perro->y);

    // se copia el codigo del perro a la nueva posicion en el mapa
    mmu_copiar_pagina(dir_fisica_ant, dir_fisica_nue);

    // actualizo el mapeo de direcciones (en area de visitados y acceso directo)
    cr3 = rcr3();

    mmu_mapear_pagina(dir_virtual_nue, cr3, dir_fisica_nue, PAG_SOLO_LEC |PAG_PRESENTE);
    mmu_mapear_pagina(DIR_VIRTUAL_TAREA, cr3, dir_fisica_nue, PAG_LEC_ESC |PAG_PRESENTE);
}

//
uint mmu_test_ejercicio4() {
    jugador_t jugador;
    perro_t perro;

    jugador.index = 0;

    perro.jugador = &jugador;
    perro.x = 20;
    perro.y = 34;
    perro.tipo = 0;

    return mmu_inicializar_memoria_perro(&perro);
}
