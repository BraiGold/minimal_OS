; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "imprimir.mac"

BITS 32

sched_tarea_offset:     dd 0x00
sched_tarea_selector:   dw 0x00

;; PIC
extern fin_intr_pic1

;; SCREEN
extern datos
extern segmentos
extern ejecucion
extern control
extern stack

;; SCHED
extern sched_atender_tick
extern sched_tarea_actual

;; GAME
extern game_perro_termino
extern game_activar_ventana_debug

;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
;%define nr0     0x30, 0x30
;%define nr1     0x30, 0x31
;%define nr2     0x30, 0x32
;%define nr3     0x30, 0x33
;%define nr4     0x30, 0x34
;%define nr5     0x30, 0x35
;%define nr6     0x30, 0x36
;%define nr7     0x30, 0x37
;%define nr8     0x30, 0x38
;%define nr9     0x30, 0x39
;%define nr10    0x31, 0x30
;%define nr11    0x31, 0x31
;%define nr12    0x31, 0x32
;%define nr13    0x31, 0x33
;%define nr14    0x31, 0x34
;%define nr15    0x31, 0x35
;%define nr16    0x31, 0x36
;%define nr17    0x31, 0x37
;%define nr18    0x31, 0x38
;%define nr19    0x31, 0x39

;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

%macro ISR 1
global _isr%1

;msg%1 db     'Interrupcion ', nr%1
;len%1 equ    $ - msg%1

_isr%1:
    ;mov eax, %1
    ;imprimir_texto_mp msg%1, len%1, 0x0f, 4, 25
    ;jmp $

    ; STACK USAGE WITH PRIVILEGE-LEVEL CHANGE
    ;
    ;    Handler's Stack
    ;    +------------+
    ; 20 |     SS     |
    ;    +------------+
    ; 16 |     ESP    |
    ;    +------------+
    ; 12 |   EFLAGS   |
    ;    +------------+
    ; 08 |     CS     |
    ;    +------------+
    ; 04 |     EIP    |
    ;    +------------+
    ; 00 | Error Code | <--- ESP after transfer to handler
    ;    +------------+

    ; se obtienen los registros de datos
    mov dword [datos + 00], eax
    mov dword [datos + 04], ebx
    mov dword [datos + 08], ecx
    mov dword [datos + 12], edx
    mov dword [datos + 16], esi
    mov dword [datos + 20], edi
    mov dword [datos + 24], ebp

    mov eax, [esp + 20]
    mov dword [datos + 28], eax     ; esp anterior

    ; se obtienen los registros de segmento
    mov ax, [esp + 08]
    mov word [segmentos + 00], ax   ; cs anterior

    mov ax, ds
    mov word [segmentos + 02], ax   ; ds

    mov ax, es
    mov word [segmentos + 04], ax   ; es

    mov ax, fs
    mov word [segmentos + 06], ax   ; fs

    mov ax, gs
    mov word [segmentos + 08], ax   ; gs

    mov ax, [esp + 20]
    mov word [segmentos + 10], ax   ; ds

    ; se obtienen los registros de ejecucion
    mov eax, [esp + 04]
    mov dword [ejecucion + 00], eax ; eip anterior

    mov eax, [esp + 12]
    mov dword [ejecucion + 04], eax ; eflags anterior

    ; se obtiene los registros de control
    mov eax, cr0
    mov dword [control + 00], eax

    mov eax, cr2
    mov dword [control + 04], eax

    mov eax, cr3
    mov dword [control + 08], eax

    mov eax, cr4
    mov dword [control + 12], eax

    ; se obtiene el contenido de la stack anterior
    mov eax, [esp + 16]
    
    mov ebx, [eax + 00]
    mov dword [stack + 00], ebx

    mov ebx, [eax + 04]
    mov dword [stack + 04], ebx

    mov ebx, [eax + 08]
    mov dword [stack + 08], ebx

    mov ebx, [eax + 12]
    mov dword [stack + 12], ebx

    sti

    ; se desaloja la tarea actual que es la que provoco esta interrupcion
    call sched_tarea_actual

    push eax
    call game_perro_termino
    add  esp, 4

    ; se indica que algo exploto
    call game_activar_ventana_debug

    ; se salta al a tarea IDLE
    jmp 0x68:0

%endmacro

;;
;; Datos
;; -------------------------------------------------------------------------- ;;
; Scheduler

;;
;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8
ISR 9
ISR 10
ISR 11
ISR 12
ISR 13
ISR 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19

;;
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr32
extern game_atender_tick
extern sched_atender_tick

_isr32:
    pushad
    pushfd
    
    call fin_intr_pic1

    call game_atender_tick
    call sched_atender_tick

    str cx 
    cmp ax, cx

    je .fin
        mov [sched_tarea_selector] , ax
        jmp far [sched_tarea_offset]
    .fin: 

    popfd
    popad
    
    iret

;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
global _isr33
extern game_atender_teclado

_isr33:
    pushad
    pushfd
    
    call fin_intr_pic1
    
    in   al, 0x60
    test al, 0x80
    push eax
    call game_atender_teclado
    add  esp, 4
    
    popfd
    popad
    
    iret

;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;
global _isr70
extern game_syscall_manejar

_isr70:
    push ebp
    push ebx
    push ecx
    push edx
    push edi
    push esi
    pushfd

    push ecx
    push eax
    
    call fin_intr_pic1

    ; se atiende
    call game_syscall_manejar
    add esp, 8

    ; se salta al a tarea IDLE
    jmp 0x68:0

    popfd
    pop esi
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop ebp

    iret

