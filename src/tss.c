/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "mmu.h"

tss tss_inicial;
tss tss_idle;

tss tss_jugadorA[MAX_CANT_PERROS_VIVOS];
tss tss_jugadorB[MAX_CANT_PERROS_VIVOS];


// Se carga el descriptor de una tarea a la GDT
void tss_cargar_tarea_a_gdt(int pos, tss *tarea) {
    gdt[pos].limit_0_15  = 0x0067;
    gdt[pos].base_0_15   = (uint) tarea & (uint) 0xFFFF;
    gdt[pos].base_23_16  = ( (uint) tarea >> 16 ) & (uint) 0xFF;
    gdt[pos].type        = 0x09;
    gdt[pos].s           = 0x00;
    gdt[pos].dpl         = 0x00;
    gdt[pos].p           = 0x01;
    gdt[pos].limit_16_19 = 0x00;
    gdt[pos].avl         = 0x01;
    gdt[pos].l           = 0x00;
    gdt[pos].db          = 0x01;
    gdt[pos].g           = 0x00;
    gdt[pos].base_31_24  = ( (uint) tarea >> 24 ) & (uint) 0xFF;
}

// Se cargan los descriptores de todas las tareas en la GDT
void tss_inicializar() {
    int pos_gdt, id_tarea;

    // Se cargan tareas inicial e idle
    tss_cargar_tarea_a_gdt( GDT_IDX_TSS_INICIAL, &tss_inicial );
    tss_cargar_tarea_a_gdt( GDT_IDX_TSS_IDLE, &tss_idle );

    // Se carga tareas del jugador A
    pos_gdt = GDT_IDX_TSS_A1;

    for( id_tarea = 0; id_tarea < MAX_CANT_PERROS_VIVOS; id_tarea++ ) {
        tss_cargar_tarea_a_gdt( pos_gdt, &tss_jugadorA[id_tarea] );
        pos_gdt++;
    }

    // Se carga tareas del jugador B
    pos_gdt = GDT_IDX_TSS_B1;

    for( id_tarea = 0; id_tarea < MAX_CANT_PERROS_VIVOS; id_tarea++ ) {
        tss_cargar_tarea_a_gdt( pos_gdt, &tss_jugadorB[id_tarea] );
        pos_gdt++;
    }
}

// Se inicializa la TSS de la tarea IDE
void tss_inicializar_idle() {
    tss_idle.ptl      = 0x0000;
    tss_idle.unused0  = 0x0000;
    tss_idle.esp0     = DIR_PILA_KERNEL;
    tss_idle.ss0      = SELECTOR_SEGMENTO(GDT_IDX_DATA0, IT_GDT, RPL_0);
    tss_idle.unused1  = 0x0000;
    tss_idle.esp1     = 0x00000000;
    tss_idle.ss1      = 0x0000;
    tss_idle.unused2  = 0x0000;
    tss_idle.esp2     = 0x00000000;
    tss_idle.ss2      = 0x0000;
    tss_idle.unused3  = 0x0000;
    tss_idle.cr3      = DIR_PDIR_KERNEL;
    tss_idle.eip      = DIR_TAREA_IDLE;
    tss_idle.eflags   = EFLAGS_INT_HAB;
    tss_idle.eax      = 0x00000000;
    tss_idle.ecx      = 0x00000000;
    tss_idle.edx      = 0x00000000;
    tss_idle.ebx      = 0x00000000;
    tss_idle.esp      = DIR_PILA_KERNEL;
    tss_idle.ebp      = DIR_PILA_KERNEL;
    tss_idle.esi      = 0x00000000;
    tss_idle.edi      = 0x00000000;
    tss_idle.es       = SELECTOR_SEGMENTO(GDT_IDX_DATA0, IT_GDT, RPL_0);
    tss_idle.unused4  = 0x0000;
    tss_idle.cs       = SELECTOR_SEGMENTO(GDT_IDX_CODE0, IT_GDT, RPL_0);
    tss_idle.unused5  = 0x0000;
    tss_idle.ss       = SELECTOR_SEGMENTO(GDT_IDX_DATA0, IT_GDT, RPL_0);
    tss_idle.unused6  = 0x0000;
    tss_idle.ds       = SELECTOR_SEGMENTO(GDT_IDX_DATA0, IT_GDT, RPL_0);
    tss_idle.unused7  = 0x0000;
    tss_idle.fs       = SELECTOR_SEGMENTO(GDT_IDX_DATA0, IT_GDT, RPL_0);
    tss_idle.unused8  = 0x0000;
    tss_idle.gs       = SELECTOR_SEGMENTO(GDT_IDX_DATA0, IT_GDT, RPL_0);
    tss_idle.unused9  = 0x0000;
    tss_idle.ldt      = 0x0000;
    tss_idle.unused10 = 0x0000;
    tss_idle.dtrap    = 0x0000;
    tss_idle.iomap    = SIN_IOMAP;
}

// Se inicializa la TSS de una tarea perro
void tss_inicializar_perro( perro_t *perro ) {
    tss *tarea;

    if( perro->jugador->index == JUGADOR_A ) {
        tarea = &tss_jugadorA[ perro->index ];
    } else {
        tarea = &tss_jugadorB[ perro->index ];
    }

    tarea->ptl      = 0x0000;
    tarea->unused0  = 0x0000;
    tarea->esp0     = mmu_proxima_pagina_fisica_libre() + PAGE_SIZE;
    tarea->ss0      = SELECTOR_SEGMENTO(GDT_IDX_DATA0, IT_GDT, RPL_0);
    tarea->unused1  = 0x0000;
    tarea->esp1     = 0x00000000;
    tarea->ss1      = 0x0000;
    tarea->unused2  = 0x0000;
    tarea->esp2     = 0x00000000;
    tarea->ss2      = 0x0000;
    tarea->unused3  = 0x0000;
    tarea->cr3      = mmu_inicializar_memoria_perro( perro );
    tarea->eip      = DIR_VIRTUAL_TAREA;
    tarea->eflags   = EFLAGS_INT_HAB;
    tarea->eax      = 0x00000000;
    tarea->ecx      = 0x00000000;
    tarea->edx      = 0x00000000;
    tarea->ebx      = 0x00000000;
    tarea->esp      = DIR_VIRTUAL_TAREA + PAGE_SIZE - 12;
    tarea->ebp      = DIR_VIRTUAL_TAREA + PAGE_SIZE - 12;
    tarea->esi      = 0x00000000;
    tarea->edi      = 0x00000000;
    tarea->es       = SELECTOR_SEGMENTO(GDT_IDX_DATA3, IT_GDT, RPL_3);
    tarea->unused4  = 0x0000;
    tarea->cs       = SELECTOR_SEGMENTO(GDT_IDX_CODE3, IT_GDT, RPL_3);;
    tarea->unused5  = 0x0000;
    tarea->ss       = SELECTOR_SEGMENTO(GDT_IDX_DATA3, IT_GDT, RPL_3);
    tarea->unused6  = 0x0000;
    tarea->ds       = SELECTOR_SEGMENTO(GDT_IDX_DATA3, IT_GDT, RPL_3);
    tarea->unused7  = 0x0000;
    tarea->fs       = SELECTOR_SEGMENTO(GDT_IDX_DATA3, IT_GDT, RPL_3);
    tarea->unused8  = 0x0000;
    tarea->gs       = SELECTOR_SEGMENTO(GDT_IDX_DATA3, IT_GDT, RPL_3);
    tarea->unused9  = 0x0000;
    tarea->ldt      = 0x0000;
    tarea->unused10 = 0x0000;
    tarea->dtrap    = 0x0000;
    tarea->iomap    = SIN_IOMAP;
}

// Se prueba la creación de una tarea perro devolviendo el selector de segmento
uint tss_test_ejercicio6() {
    jugador_t jugador;
    perro_t perro;
    
    jugador.index   = JUGADOR_B;
    jugador.x_cucha = 1;
    jugador.y_cucha = 1;

    perro.id      = GDT_IDX_TSS_B1;
    perro.index   = 0;
    perro.jugador = &jugador;
    perro.x       = jugador.x_cucha;
    perro.y       = jugador.y_cucha;
    perro.tipo    = TIPO_2;

    tss_inicializar_perro(&perro);
    
    return SELECTOR_SEGMENTO(perro.id, IT_GDT, RPL_0);
}
