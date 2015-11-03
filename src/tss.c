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

void tss_inicializar() {
    int i, id_tarea;

    // Se cargan tareas inicial e idle
    gdt_cargar_tarea( GDT_IDX_TSS_INICIAL, &tss_inicial );
    gdt_cargar_tarea( GDT_IDX_TSS_IDLE, &tss_idle );

    // Se carga tareas del jugador A
    pos = GDT_IDX_TSS_A1;

    for( id_tarea = 0; id_tarea < MAX_CANT_PERROS_VIVOS; id_tarea++ ) {
        gdt_cargar_tarea( pos, &tss_jugadorA[id_tarea] );
        pos++;
    }

    // Se carga tareas del jugador B
    pos = GDT_IDX_TSS_B1;

    for( id_tarea = 0; id_tarea < MAX_CANT_PERROS_VIVOS; id_tarea++ ) {
        gdt_cargar_tarea( pos, &tss_jugadorB[id_tarea] );
        pos++;
    }

    // Se inicializa la tarea idle
    void tss_inicializar_idle();
}

void tss_inicializar_idle() {
    tss_idle.ptl      = 0x00;
    tss_idle.unused0  = 0x00;
    tss_idle.esp0     = 0x00;
    tss_idle.ss0      = 0x00;
    tss_idle.unused1  = 0x00;
    tss_idle.esp1     = 0x00;
    tss_idle.ss1      = 0x00;
    tss_idle.unused2  = 0x00;
    tss_idle.esp2     = 0x00;
    tss_idle.ss2      = 0x00;
    tss_idle.unused3  = 0x00;
    tss_idle.cr3      = PDE_DESC;
    tss_idle.eip      = DIR_BASE_TAREA_IDLE;
    tss_idle.eflags   = 0x202;
    tss_idle.eax      = 0x00;
    tss_idle.ecx      = 0x00;
    tss_idle.edx      = 0x00;
    tss_idle.ebx      = 0x00;
    tss_idle.esp      = DIR_PILA_KERNEL;
    tss_idle.ebp      = DIR_PILA_KERNEL;
    tss_idle.esi      = 0x00;
    tss_idle.edi      = 0x00;
    tss_idle.es       = GDT_OFF_DATA0;
    tss_idle.unused4  = 0x00;
    tss_idle.cs       = GDT_OFF_CODE0;
    tss_idle.unused5  = 0x00;
    tss_idle.ss       = GDT_OFF_DATA0;
    tss_idle.unused6  = 0x00;
    tss_idle.ds       = GDT_OFF_DATA0;
    tss_idle.unused7  = 0x00;
    tss_idle.fs       = GDT_OFF_DATA0;
    tss_idle.unused8  = 0x00;
    tss_idle.gs       = GDT_OFF_DATA0;
    tss_idle.unused9  = 0x00;
    tss_idle.ldt      = 0x00;
    tss_idle.unused10 = 0x00;
    tss_idle.dtrap    = 0x00;
    tss_idle.iomap    = 0xFFFF;
}
/*
void tss_inicializar_perro( tss *tarea, perro_t *perro ) {
    tarea->ptl      = 0x00;
    tarea->unused0  = 0x00;
    tarea->esp0     = 0x00;
    tarea->ss0      = 0x00;
    tarea->unused1  = 0x00;
    tarea->esp1     = 0x00;
    tarea->ss1      = 0x00;
    tarea->unused2  = 0x00;
    tarea->esp2     = 0x00;
    tarea->ss2      = 0x00;
    tarea->unused3  = 0x00;
    tarea->cr3      = mmu_inicializar_memoria_perro( perro );
    tarea->eip      = 0x401000;
    tarea->eflags   = 0x202;
    tarea->eax      = 0x00;
    tarea->ecx      = 0x00;
    tarea->edx      = 0x00;
    tarea->ebx      = 0x00;
    tarea->esp      = 0x00;
    tarea->ebp      = 0x00;
    tarea->esi      = 0x00;
    tarea->edi      = 0x00;
    tarea->es       = 0x00;
    tarea->unused4  = 0x00;
    tarea->cs       = GDT_OFF_CODE3;
    tarea->unused5  = 0x00;
    tarea->ss       = GDT_OFF_DATA3;
    tarea->unused6  = 0x00;
    tarea->ds       = GDT_OFF_DATA3;
    tarea->unused7  = 0x00;
    tarea->fs       = GDT_OFF_DATA3;
    tarea->unused8  = 0x00;
    tarea->gs       = GDT_OFF_DATA3;
    tarea->unused9  = 0x00;
    tarea->ldt      = 0x00;
    tarea->unused10 = 0x00;
    tarea->dtrap    = 0x00;
    tarea->iomap    = 0xFFFF;
}*/
