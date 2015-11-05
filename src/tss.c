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
void tss_cargar_tarea_a_gdt( int pos, tss *tarea ) {
    gdt[pos].limit_0_15  = 0x0067;
    gdt[pos].base_0_15   = (uint) tarea & (uint) 0xFFFF;
    gdt[pos].base_23_16  = ( (uint) tarea >> 16 ) & (uint) 0xFF;
    gdt[pos].type        = 0x09;
    gdt[pos].s           = 0x00;
    gdt[pos].dpl         = 0x00;
    gdt[pos].p           = 0x01;
    gdt[pos].limit_16_19 = 0x00;
    gdt[pos].avl         = 0x00;
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
    tss_idle.esp0     = 0x00000000; // Como cargo la pila de nivel cero?
    tss_idle.ss0      = 0x0000;     // 
    tss_idle.unused1  = 0x0000;
    tss_idle.esp1     = 0x00000000;
    tss_idle.ss1      = 0x0000;
    tss_idle.unused2  = 0x0000;
    tss_idle.esp2     = 0x00000000;
    tss_idle.ss2      = 0x0000;
    tss_idle.unused3  = 0x0000;
    tss_idle.cr3      = PDE_DESC;
    tss_idle.eip      = DIR_BASE_TAREA_IDLE;
    tss_idle.eflags   = EFLAGS_INT_HAB;
    tss_idle.eax      = 0x00000000;
    tss_idle.ecx      = 0x00000000;
    tss_idle.edx      = 0x00000000;
    tss_idle.ebx      = 0x00000000;
    tss_idle.esp      = DIR_PILA_KERNEL;
    tss_idle.ebp      = DIR_PILA_KERNEL;
    tss_idle.esi      = 0x00000000;
    tss_idle.edi      = 0x00000000;
    tss_idle.es       = GDT_OFF_DATA0;
    tss_idle.unused4  = 0x0000;
    tss_idle.cs       = GDT_OFF_CODE0;
    tss_idle.unused5  = 0x0000;
    tss_idle.ss       = GDT_OFF_DATA0;
    tss_idle.unused6  = 0x0000;
    tss_idle.ds       = GDT_OFF_DATA0;
    tss_idle.unused7  = 0x0000;
    tss_idle.fs       = GDT_OFF_DATA0;
    tss_idle.unused8  = 0x0000;
    tss_idle.gs       = GDT_OFF_DATA0;
    tss_idle.unused9  = 0x0000;
    tss_idle.ldt      = 0x0000;
    tss_idle.unused10 = 0x0000;
    tss_idle.dtrap    = 0x0000;
    tss_idle.iomap    = SIN_IOMAP;
}


void tss_inicializar_perro( perro_t *perro ) {
    tss *tarea;

    if( perro->jugador->index == JUGADOR_A ) {
        tarea = &tss_jugadorA[ perro->index ];
    } else {
        tarea = &tss_jugadorB[ perro->index ];
    }

    tarea->ptl      = 0x0000;
    tarea->unused0  = 0x0000;
    tarea->esp0     = (uint) mmu_proxima_pagina_fisica_libre();
    tarea->ss0      = GDT_OFF_DATA0;
    tarea->unused1  = 0x0000;
    tarea->esp1     = 0x00000000;
    tarea->ss1      = 0x0000;
    tarea->unused2  = 0x0000;
    tarea->esp2     = 0x00000000;
    tarea->ss2      = 0x0000;
    tarea->unused3  = 0x0000;
    tarea->cr3      = (uint) mmu_inicializar_memoria_perro( perro, perro->jugador->index, perro->tipo );
    tarea->eip      = DIR_CODE_TAREA;
    tarea->eflags   = EFLAGS_INT_HAB;
    tarea->eax      = 0x00000000;
    tarea->ecx      = 0x00000000;
    tarea->edx      = 0x00000000;
    tarea->ebx      = 0x00000000;
    tarea->esp      = DIR_PILA_TAREA;
    tarea->ebp      = DIR_PILA_TAREA;
    tarea->esi      = 0x00000000;
    tarea->edi      = 0x00000000;
    tarea->es       = GDT_OFF_DATA3;
    tarea->unused4  = 0x0000;
    tarea->cs       = GDT_OFF_CODE3;
    tarea->unused5  = 0x0000;
    tarea->ss       = GDT_OFF_DATA3;
    tarea->unused6  = 0x0000;
    tarea->ds       = GDT_OFF_DATA3;
    tarea->unused7  = 0x0000;
    tarea->fs       = GDT_OFF_DATA3;
    tarea->unused8  = 0x0000;
    tarea->gs       = GDT_OFF_DATA3;
    tarea->unused9  = 0x0000;
    tarea->ldt      = 0x0000;
    tarea->unused10 = 0x0000;
    tarea->dtrap    = 0x0000;
    tarea->iomap    = SIN_IOMAP;
}
