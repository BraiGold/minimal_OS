/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/
#include "defines.h"
#include "mmu.h"
#include "i386.h"
/* Atributos paginas */
/* -------------------------------------------------------------------------- */
uint ind_free_page = PDE_DESC;

uint has_comun_dir = 0;

uint comun_dir_0;

uint comun_dir_1;

/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */

void mmu_copiar_pagina(uint src, uint dst){
	uint i = 0;
	while(i < 1024){
		*((uint *)(dst + i*4)) = *((uint *)(src + i*4));;
		i++;

	}
}

// crea el directorio, las paginas, copia el codigo e inicializa el stack
uint mmu_inicializar_memoria_perro(perro_t *perro, int index_jugador, int index_tipo){
	uint task_directory = mmu_proxima_pagina_fisica_libre() ;
	mmu_inicializar_pagina(task_directory);

	
	
	// copio en la primera pagina el codigo del perro
	// uint dir_perro_area_libre = mmu_proxima_pagina_fisica_libre_directory() ;
	// mmu_inicializar_pagina( dir_perro_area_libre);
	 
	
	// // seteo propiedades del perro
	// perro->tipo = index_tipo;
	// perro->libre = 0;
	// perro->huesos = 0;

	// Mapeo el codigo del perro a la posicion de la cucha cucha
	mmu_identity_mapping(task_directory);

	
	// Mapeo  cucha.	
	uint dir_fisica = MAPA_BASE_FISICA;
	uint dir_virtual = MAPA_BASE_VIRTUAL;
	if (index_jugador == 1){
		dir_fisica = 0x11FF000;
		dir_virtual = 0x15BF000;
	}
	mmu_mapear_pagina(dir_virtual, task_directory , dir_fisica  , 0x1);

	// Mapeo Codigo y pila a la cucha.
	dir_virtual = 0x401000;
	mmu_mapear_pagina(dir_virtual, task_directory , dir_fisica  , 0x3);
	
	// Mapeo el directorio compartido.
	dir_virtual = 0x400000;
	dir_fisica = comun_dir_0;
	if (index_jugador == 1){
		dir_fisica = comun_dir_1;	
	}
	mmu_mapear_pagina(dir_virtual, task_directory , dir_fisica  , 0x3);

	return task_directory;
	

}

void mmu_inicializar_pagina(uint pagina){
	uint ind  = 0;
	while(ind < 1024){
		* ((uint *) (pagina + (ind*4))) = (uint) 0;
		ind++;
	}
}

void mmu_identity_mapping(uint directory){
	// Indentity mapping kernel + mememoria libre 	
	uint dir  = 0;
	while(dir < 0x400000){
		// En la dir lineal no pongo la pagina porque la va a pedir cuando mapee
		mmu_mapear_pagina(dir , directory , dir , 0x3);
		dir += 0x0001000;
	}
}

uint mmu_inicializar_dir_kernel(){
	uint cr3 = mmu_proxima_pagina_fisica_libre();
	//  clear directory
	mmu_inicializar_pagina( cr3);

	// identity mapping
	mmu_identity_mapping(cr3);

	

	return cr3 ;

}

void mmu_inicializar(){
	// inicializo el contador de paginas libres en el ox1000
	
	ind_free_page = MAPA_BASE_FISICA_LIBRE;

	// Las paginas q tiene en comun los perros de cada jugador

	comun_dir_0 = mmu_proxima_pagina_fisica_libre();
	comun_dir_1 = mmu_proxima_pagina_fisica_libre();

}


uint mmu_proxima_pagina_fisica_libre(){
 	
 	uint res = ind_free_page;
 	ind_free_page += 0x0001000;
 	return res;

}

void mmu_mapear_pagina  (uint virtual, uint cr3, uint fisica, uint attrs){
	//breakpoint();
	uint directory_11_0 = virtual;
	directory_11_0 = (directory_11_0 >> 22) * 4;
	uint* directory = (uint*)(cr3 + (directory_11_0));
	uint page_31_12 ;
	if(*(directory) % 2 == 0){
		page_31_12 = mmu_proxima_pagina_fisica_libre();
		mmu_inicializar_pagina( page_31_12);
		*directory = (uint) ((page_31_12 & 0xfffff000) + 0x3); 
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

void mmu_unmapear_pagina(uint virtual, uint cr3){
	//breakpoint();
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
