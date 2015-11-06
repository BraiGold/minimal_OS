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

;; Sched
extern sched_atender_tick
extern sched_tarea_actual

;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
%define nr0     0x30, 0x30
%define nr1     0x30, 0x31
%define nr2     0x30, 0x32
%define nr3     0x30, 0x33
%define nr4     0x30, 0x34
%define nr5     0x30, 0x35
%define nr6     0x30, 0x36
%define nr7     0x30, 0x37
%define nr8     0x30, 0x38
%define nr9     0x30, 0x39
%define nr10    0x31, 0x30
%define nr11    0x31, 0x31
%define nr12    0x31, 0x32
%define nr13    0x31, 0x33
%define nr14    0x31, 0x34
%define nr15    0x31, 0x35
%define nr16    0x31, 0x36
%define nr17    0x31, 0x37
%define nr18    0x31, 0x38
%define nr19    0x31, 0x39

;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

%macro ISR 1
global _isr%1

msg%1 db     'Interrupcion ', nr%1
len%1 equ    $ - msg%1

_isr%1:
    mov eax, %1
    imprimir_texto_mp msg%1, len%1, 0x0f, 20, 20
    jmp $

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
extern sched

_isr32:
    pushad
    
    call fin_intr_pic1

    call sched_atender_tick

    str cx 
    cmp ax , cx 
    je .fin 
        mov [sched_tarea_selector] , ax
        jmp far [sched_tarea_offset]
    .fin: 


    popad
    
    iret

;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
global _isr33
extern game_atender_teclado

_isr33:
    pushad
    
    call fin_intr_pic1
    
    in   al, 0x60
    test al, 0x80
    push eax
    call game_atender_teclado
    add  esp, 4
    
    popad
    
    iret

;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;
global _isr70
;extern game_tick

_isr70:
    pushad
    
    call fin_intr_pic1
    ;call game_tick

    mov eax, 0x42

    popad

    iret

