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



uint mmu_inicializar_dir_kernel(){
	uint cr3 = mmu_proxima_pagina_fisica_libre();
	//  clear directory
	uint ind_directory = 0 ;
	while(ind_directory < 1024){
		// En la dir lineal no pongo la pagina porque la va a pedir cuando mapee
		*((uint*)(cr3 + (ind_directory*4))) = (uint) 0;
		ind_directory++;
	}

	ind_directory = 0 ;
	// Indentity mapping kernel + mememoria libre 	
	uint ind_page_table  = 0;
	while(ind_page_table < 1024){
		// En la dir lineal no pongo la pagina porque la va a pedir cuando mapee
		uint dir_lineal = ((ind_directory  << 22 )  + (ind_page_table << 12) )   ;
		mmu_mapear_pagina(dir_lineal , cr3 , dir_lineal  , 0x3);
		ind_page_table++;
	}
		
	return cr3 ;

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
