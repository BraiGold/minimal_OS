
/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

    Definiciones globales del sistema.
*/

#ifndef __DEFINES_H__
#define __DEFINES_H__

/* Tipos basicos */
/* -------------------------------------------------------------------------- */
#define NULL                    0
#define TRUE                    0x00000001
#define FALSE                   0x00000000
#define ERROR                   1

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;

/* Constantes basicas */
/* -------------------------------------------------------------------------- */
#define PAGE_SIZE               0x00001000
#define TASK_SIZE               4096

#define BOOTSECTOR              0x00001000 /* direccion fisica de comienzo del bootsector (copiado) */
#define KERNEL                  0x00001200 /* direccion fisica de comienzo del kernel */

#define DIR_PILA_KERNEL         0x00027000
#define DIR_PDIR_KERNEL         0x00027000

#define DIR_TAREA_IDLE          0x00016000
#define DIR_TAREA_A1            0x00010000
#define DIR_TAREA_A2            0x00011000
#define DIR_TAREA_B1            0x00012000
#define DIR_TAREA_B2            0x00013000

#define DIR_VIRTUAL_AREA_COMP   0x00400000
#define DIR_VIRTUAL_TAREA       0x00401000
#define DIR_VIRTUAL_AUX         0x00402000

#define MAPA_BASE_FISICA_LIBRE  0x00100000
#define MAPA_BASE_FISICA        0x00500000
#define MAPA_BASE_VIRTUAL       0x00800000

/* Indices en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_COUNT 30

#define GDT_IDX_NULL_DESC           0
#define GDT_IDX_CODE0               8
#define GDT_IDX_CODE3               9
#define GDT_IDX_DATA0               10
#define GDT_IDX_DATA3               11
//#define GDT_IDX_VIDEO               12

#define GDT_IDX_TSS_INICIAL         12
#define GDT_IDX_TSS_IDLE            13

#define GDT_IDX_TSS_A1              14
#define GDT_IDX_TSS_A2              15
#define GDT_IDX_TSS_A3              16
#define GDT_IDX_TSS_A4              17
#define GDT_IDX_TSS_A5              18
#define GDT_IDX_TSS_A6              19
#define GDT_IDX_TSS_A7              20
#define GDT_IDX_TSS_A8              21

#define GDT_IDX_TSS_B1              22
#define GDT_IDX_TSS_B2              23
#define GDT_IDX_TSS_B3              24
#define GDT_IDX_TSS_B4              25
#define GDT_IDX_TSS_B5              26
#define GDT_IDX_TSS_B6              27
#define GDT_IDX_TSS_B7              28
#define GDT_IDX_TSS_B8              29


/* Offsets en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_OFF_NULL_DESC           (GDT_IDX_NULL_DESC      << 3)
#define GDT_OFF_CODE0               (GDT_IDX_CODE0          << 3)
#define GDT_OFF_CODE3               (GDT_IDX_CODE3          << 3)
#define GDT_OFF_DATA0               (GDT_IDX_DATA0          << 3)
#define GDT_OFF_DATA3               (GDT_IDX_DATA3          << 3)
//#define GDT_OFF_VIDEO               (GDT_IDX_VIDEO          << 3)

#define GDT_OFF_TSS_INICIAL         (GDT_IDX_TSS_INICIAL << 3)
#define GDT_OFF_TSS_IDLE            (GDT_IDX_TSS_IDLE    << 3)

#define GDT_OFF_TSS_A1              (GDT_IDX_TSS_A1      << 3)
#define GDT_OFF_TSS_A2              (GDT_IDX_TSS_A2      << 3)
#define GDT_OFF_TSS_A3              (GDT_IDX_TSS_A3      << 3)
#define GDT_OFF_TSS_A4              (GDT_IDX_TSS_A4      << 3)
#define GDT_OFF_TSS_A5              (GDT_IDX_TSS_A5      << 3)
#define GDT_OFF_TSS_A6              (GDT_IDX_TSS_A6      << 3)
#define GDT_OFF_TSS_A7              (GDT_IDX_TSS_A7      << 3)
#define GDT_OFF_TSS_A8              (GDT_IDX_TSS_A8      << 3)

#define GDT_OFF_TSS_B1              (GDT_IDX_TSS_B1      << 3)
#define GDT_OFF_TSS_B2              (GDT_IDX_TSS_B2      << 3)
#define GDT_OFF_TSS_B3              (GDT_IDX_TSS_B3      << 3)
#define GDT_OFF_TSS_B4              (GDT_IDX_TSS_B4      << 3)
#define GDT_OFF_TSS_B5              (GDT_IDX_TSS_B5      << 3)
#define GDT_OFF_TSS_B6              (GDT_IDX_TSS_B6      << 3)
#define GDT_OFF_TSS_B7              (GDT_IDX_TSS_B7      << 3)
#define GDT_OFF_TSS_B8              (GDT_IDX_TSS_B8      << 3)

/* Selectores de segmentos */
/* -------------------------------------------------------------------------- */
#define IT_GDT      0
#define IT_LDT      1

#define RPL_0       0
#define RPL_1       1
#define RPL_2       2
#define RPL_3       3

#define SELECTOR_SEGMENTO(index, it, rpl) index << 3 | it << 2 | rpl


#endif  /* !__DEFINES_H__ */
