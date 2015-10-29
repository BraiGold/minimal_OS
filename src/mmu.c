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
	uint cr3 = mmu_proxima_pagina_fisica_libre_directory() ;
	mmu_inicializar_pagina(cr3);

	if (index_jugador == 0){
		perro->x = 0;
		perro->y = 0;
		if (index_tipo == 0){
			dir_perro= 0x10000;
		}else{
			dir_perro= 0x11000;
		}
	}else{
		perro->x = 78;
		perro->y = 34;
		if (index_tipo == 0){
			dir_perro= 0x12000;	
		}else{
			dir_perro= 0x13000;
		}
	}
	// copio en la primera pagina el codigo del perro
	// uint dir_perro_area_libre = mmu_proxima_pagina_fisica_libre_directory() ;
	// mmu_inicializar_pagina( dir_perro_area_libre);
	 
	mmu_copiar_pagina(dir_perro, dir_perro_area_libre);
	// seteo propiedades del perro
	perro->tipo = index_tipo;
	perro->libre = false;
	perro->huesos = 0;

	// Mapeo el codigo del perro a la posicion de la cucha cucha
	uint ind_page_table  = 0;
	uint dir_lineal = MAPA_BASE_VIRTUAL;
	uint dir_fisica = MAPA_BASE_FISICA;
	while(ind_page_table < 1024){
		// En la dir lineal no pongo la pagina porque la va a pedir cuando mapee
		dir_fisica += 4;
		dir_lineal += 4;
		mmu_mapear_pagina(dir_lineal , cr3 , dir_fisica, 0x3);
		ind_page_table++;
	}
	

	return cr3;
	

}

void mmu_inicializar_pagina(uint pagina){
	uint ind  = 0;
	while(ind < 1024){
		* ((uint *) (pagina + (ind*4))) = (uint) 0;
		ind++;
	}
}

void mmu_identity_mapping(uint  ind_directory, uint cr3){
	// Indentity mapping kernel + mememoria libre 	
	uint ind_page_table  = 0;
	while(ind_page_table < 1024){
		// En la dir lineal no pongo la pagina porque la va a pedir cuando mapee
		uint dir_lineal = ((ind_directory  << 22 )  + (ind_page_table << 12) )   ;
		mmu_mapear_pagina(dir_lineal , cr3 , dir_lineal  , 0x3);
		ind_page_table++;
	}
}

uint mmu_inicializar_dir_kernel(){
	uint cr3 = mmu_proxima_pagina_fisica_libre_directory();
	//  clear directory
	mmu_inicializar_pagina( cr3);

	// identity mapping

	mmu_identity_mapping(0,cr3);

	// inicializo el contador de paginas libres en el ox1000
	ind_free_page = MAPA_BASE_FISICA_LIBRE;

	return cr3 ;

}

uint mmu_proxima_pagina_fisica_libre_directory(){
 	
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
		page_31_12 = mmu_proxima_pagina_fisica_libre_directory();
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
	} 
	
}
