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
		*(cr3 + ind_directory*4) = (uint) 0;
		ind_directory++;
	}
	ind_directory = 0 ;
	// Indentity mapping kernel + mememoria libre 	
	uint paga_table = mmu_proxima_pagina_fisica_libre();
	uint ind_page_table  = 0;
	while(ind_page_table < 0x0040000){
		// En la dir lineal no pongo la pagina porque la va a pedir cuando mapee
		uint dir_lineal = ( ind_directory  << 22 )  | ( ind_page_table  << 12 )  ;
		mmu_mapear_pagina(dir_lineal , cr3 , dir_lineal  , 0x3);
		ind_page_table+= 0x0001000;
	}
	
	
	
	

}

uint mmu_proxima_pagina_fisica_libre(){
 	ind_free_page += 4*1024;
 	return (ind_free_page - 4*1024);

}

void mmu_mapear_pagina  (uint virtual, uint cr3, uint fisica, uint attrs){

	uint ind_directory = virtual >> 22;
	uint directory = (cr3 + ind_directory*4);
	if(*(directory) % 2 == 0){
		uint page = mmu_proxima_pagina_fisica_libre();
		*directory = *directory | 0x3; 
	}else{
		uint page = *directory >> 12;
		page = page << 12;
	} 
	uint ind_page = virtual << 10;
	ind_page = virtual >> 22;

	*(page + ind_page*4) = ((fisica >> 12) << 12) | attrs;

			

}