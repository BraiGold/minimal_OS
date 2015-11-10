; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================

%include "imprimir.mac"

global start

;; GDT
extern GDT_DESC

;; SCREEN
extern screen_test
extern screen_inicializar
extern screen_imprimir_nombre_grupo

;; IDT
extern idt_inicializar
extern IDT_DESC

;; MMU
extern mmu_inicializar
extern mmu_inicializar_memoria_perro
extern mmu_inicializar_dir_kernel
extern mmu_unmapear_pagina
extern mmu_test_ejercicio4

;; TSS
extern tss_inicializar
extern tss_inicializar_idle
extern tss_test_ejercicio6

;; PIC
extern resetear_pic
extern habilitar_pic
extern deshabilitar_pic

;; GAME
extern game_inicializar

;; SCHED
extern sched_inicializar

;; Saltear seccion de datos
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
iniciando_mr_msg db     'Iniciando kernel (Modo Real)...'
iniciando_mr_len equ    $ - iniciando_mr_msg

iniciando_mp_msg db     'Iniciando kernel (Modo Protegido)...'
iniciando_mp_len equ    $ - iniciando_mp_msg

space db         ' '
len_space equ    $ - space

tarea_offset:     dd 0x00
tarea_selector:   dw 0x00

;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
    ; Deshabilitar interrupciones
    cli
    
    ; Cambiar modo de video a 80 X 50
    mov ax, 0003h
    int 10h ; set mode 03h
    xor bx, bx
    mov ax, 1112h
    int 10h ; load 8x8 font

    ; Imprimir mensaje de bienvenida
    imprimir_texto_mr iniciando_mr_msg, iniciando_mr_len, 0x07, 0, 0

    ; Habilitar A20
    call habilitar_A20

    ; Cargar la GDT
    lgdt [GDT_DESC]

    ; Setear el bit PE del registro CR0
    mov eax, cr0
    or  eax, 1
    mov cr0, eax

    ; Saltar a modo protegido
    jmp 0x40:modo_protegido

BITS 32
modo_protegido:

    ; Establecer selectores de segmentos
    mov ax, 0x50
    mov ds, ax   ; datos
    mov es, ax   ; datos
    mov gs, ax   ; datos
    mov ss, ax   ; pila
    mov fs, ax   ; video

        ;; Ejercicio 1 punto c)
        ;mov ax, 0x60
        ;mov fs, ax   ; video
        ;call screen_test
        

    ; Establecer la base de la pila
    mov ebp, 0x27000
    mov esp, ebp
    
    ; Imprimir mensaje de bienvenida
    imprimir_texto_mp iniciando_mp_msg, iniciando_mp_len, 0x07, 2, 0

    ; Inicializar el juego
    call game_inicializar

    ; Inicializar pantalla
    call screen_inicializar

    ;Inicializar el directorio de paginas
    call mmu_inicializar_dir_kernel

    ;Cargar directorio de paginas
    mov cr3, eax

    ;Habilitar paginacion
    mov eax, cr0
    or  eax, 0x80000000
    mov cr0, eax

        ;; Ejercicio 3 punto c
        ;call screen_imprimir_nombre_grupo

        ;; Ejercicio 3 punto f
        ;mov  eax , cr3
        ;push eax
        ;push 0x3FF000
        ;call mmu_unmapear_pagina
        ;add  esp , 8

    ;Inicializar el manejador de memoria
    call mmu_inicializar

        ;; Ejercicio 4 punto c
        ;call mmu_test_ejercicio4
        ;mov cr3, eax
        ;imprimir_texto_mp space, len_space, 0x07, 1, 0

    ; Inicializar tss
    call tss_inicializar

    ; Inicializar tss de la tarea Idle
    call tss_inicializar_idle

    ; Inicializar el scheduler
    call sched_inicializar

    ; Inicializar la IDT
    call idt_inicializar

    ; Cargar IDT
    lidt [IDT_DESC]

        ;; Ejercicio 2 punto b
        ;xor ax , ax
        ;div ax

    ; Configurar controlador de interrupciones
    call deshabilitar_pic
    call resetear_pic
    call habilitar_pic

    ; Cargar tarea inicial
    mov ax, 0x60
    ltr ax

    ; Habilitar interrupciones
    sti

        ;;Ejercicio 6
        ;call tss_test_ejercicio6
        ;xchg bx, bx
        ;mov [tarea_selector], ax
        ;jmp far [tarea_offset]

    ; Saltar a la primera tarea: Idle
    jmp 0x68:0

    ; Ciclar infinitamente (por si algo sale mal...)
    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $
    jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
