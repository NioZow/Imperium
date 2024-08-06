[bits 64]

; export the function
global SyscallInvoke

; import the global Instance
extern __Instance

[section .text$B]

    ; Invoke Syscall and pass given arguments
    SyscallInvoke:
        ; store the adress of the SYSCALL struct
        mov r11, [rel __Instance]
        mov r12, [r11]

        ; invoke the syscall
        mov r10, rcx
        mov eax, [r12 + 0x08]
        jmp QWORD [r12]
    ret