/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de la tabla de descriptores globales
*/

#ifndef __GDT_H__
#define __GDT_H__

#include "defines.h"



typedef struct str_gdt_descriptor {
    unsigned short  gdt_length;
    unsigned int    gdt_addr;
} __attribute__((__packed__)) gdt_descriptor;

typedef struct str_gdt_entry {
    // Limite: máximo offset que puede tener un byte direccionable dentro del
    //         segmento (es el tamaño del segmento menos 1, ya que el rimer byte
    //         del segmento es el 0)
    unsigned short  limit_0_15; // Limite (bits  0 a 15)

    // Base: direccion a partir de la cual se despliega de forma continua el
    //       segmento
    unsigned short  base_0_15;  // Base (bits  0 a 15)
    unsigned char   base_23_16; // Base (bits 16 a 23)

    // Tipo: indica el tipo de segmento y esta fuertemente relacionado con el
    //       bit s
    // +----+----+----+----+---------------------------+-----------------------------------------------+
    // | s = 0                                                                                         |
    // +----+----+----+----+---------------------------+-----------------------------------------------+
    // | 11 | 10 | 9  | 8  | Modo 32 bits              | Modo IA-32e                                   |
    // +----+----+----+----+---------------------------+-----------------------------------------------+
    // | 0  | 0  | 0  | 0  | Reservado                 | 8 bytes superiores en un segmento de 16 bytes |
    // | 0  | 0  | 0  | 1  | TSS de 16 bits disponible | Reservado                                     |
    // | 0  | 0  | 1  | 0  | LDT                       | LDT                                           |
    // | 0  | 0  | 1  | 1  | TSS de 16 bits Busy       | Reservado                                     |
    // | 0  | 1  | 0  | 0  | Call Gate de 16 bits      | Reservado                                     |
    // | 0  | 1  | 0  | 1  | Task Gate                 | Reservado                                     |
    // | 0  | 1  | 1  | 0  | Interrupt Gate de 16 bits | Reservado                                     |
    // | 0  | 1  | 1  | 1  | Trap Gate de 16 bits      | Reservado                                     |
    // | 1  | 0  | 0  | 0  | Reservado                 | Reservado                                     |
    // | 1  | 0  | 0  | 1  | TSS de 32 bits disponible | TSS de 64 bits disponible                     |
    // | 1  | 0  | 1  | 0  | Reservado                 | Reservado                                     |
    // | 1  | 0  | 1  | 1  | TSS de 32 bits Busy       | TSS de 64 bits Busy                           |
    // | 1  | 1  | 0  | 0  | Call Gate de 32 bits      | Call Gate de 64 bits                          |
    // | 1  | 1  | 0  | 1  | Reservado                 | Reservado                                     |
    // | 1  | 1  | 1  | 0  | Interrupt Gate de 32 bits | Interrupt Gate de 64 bits                     |
    // | 1  | 1  | 1  | 1  | Trap Gate de 32 bits      | Trap Gate de 64 bits                          |
    // +----+----+----+----+---------------------------+-----------------------------------------------+
    //
    // +----+----+----+----+---------------------------+-----------------------------------------------+
    // | s = 1                                                                                         |
    // +----+----+----+----+---------------------------+-----------------------------------------------+
    // | 11 | 10 | 9  | 8  | Descriptor type           | Descripton                                    |
    // +----+----+----+----+---------------------------+-----------------------------------------------+
    // |    | E  | W  | A  |                           |                                               |
    // +----+----+----+----+---------------------------+-----------------------------------------------+
    // | 0  | 0  | 0  | 0  | Data                      | Read-Only                                     |
    // | 0  | 0  | 0  | 1  | Data                      | Read-Only, accessed                           |
    // | 0  | 0  | 1  | 0  | Data                      | Read/Write                                    |
    // | 0  | 0  | 1  | 1  | Data                      | Read/Write, accessed                          |
    // | 0  | 1  | 0  | 0  | Data                      | Read-Only, expand-down                        |
    // | 0  | 1  | 0  | 1  | Data                      | Read-Only, expand-down, accessed              |
    // | 0  | 1  | 1  | 0  | Data                      | Read/Write, expand-down                       |
    // | 0  | 1  | 1  | 1  | Data                      | Read/Write, expand-down, accessed             |
    // +----+----+----+----+---------------------------+-----------------------------------------------+
    // |    | C  | R  | A  |                           |                                               |
    // +----+----+----+----+---------------------------+-----------------------------------------------+
    // | 1  | 0  | 0  | 0  | Code                      | Execute-Only                                  |
    // | 1  | 0  | 0  | 1  | Code                      | Execute-Only, accessed                        |
    // | 1  | 0  | 1  | 0  | Code                      | Execute/Read                                  |
    // | 1  | 0  | 1  | 1  | Code                      | Execute/Read, accessed                        |
    // | 1  | 1  | 0  | 0  | Code                      | Execute-Only, conforming                      |
    // | 1  | 1  | 0  | 1  | Code                      | Execute-Only, conforming, accessed            |
    // | 1  | 1  | 1  | 0  | Code                      | Execute/Read, conforming                      |
    // | 1  | 1  | 1  | 1  | Code                      | Execute/Read, conforming, accessed            |
    // +----+----+----+----+---------------------------+-----------------------------------------------+
    unsigned char   type:4;

    // System: indica si el segmento es de sistema o no
    // s=0 : el segmento es de código o datos (depende de type)
    // s=1 : el segmento es de sistema (para tareas, iterrupciones, etc)
    //       (depende de type)
    unsigned char   s:1;

    // Descriptor Priviledge Level: nivel de privilegio del segmento
    // dpl=00 | dpl=01 | dpl=10 | dpl=11
    unsigned char   dpl:2;

    // Presente
    // p=0 : el segmento no esta presente en memoria RAM
    // p=1 : el segmento esta presente en memoria RAM
    unsigned char   p:1;

    // Limite
    unsigned char   limit_16_19:4; // Limite (bits 16 a 19)

    // Available: tiene ningun proposito especifico
    unsigned char   avl:1;

    // l=0 y db=0 : 16 bits de registros de segmento
    // l=0 y db=1 : 32 bits de registros de segmento
    // l=1 y db=0 : 64 bits de registros de segmento (db=0 siempre, sino rompemos todo)
    unsigned char   l:1;

    // Default/Big : determina el tamaño de los registros segmento (CS, DS, etc.)
    //               cuando l=0
    // db=0 : tamaño de registros de segmento de 16 bits
    // db=1 : tamaño de registros de segmento de 32 bits
    unsigned char   db:1;

    // Granularidad: indica factor que multiplica al limite del segmento
    //               indicado por el limit
    // g=0 : limite de segmento = limit * 0x1000
    // g=1 : limite de segmento = limit
    unsigned char   g:1;
    unsigned char   base_31_24;
} __attribute__((__packed__, aligned (8))) gdt_entry;

/* Tabla GDT */
extern gdt_entry gdt[];
extern gdt_descriptor GDT_DESC;


#endif  /* !__GDT_H__ */
