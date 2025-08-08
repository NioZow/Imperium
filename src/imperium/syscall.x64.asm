[bits 64]

; export the functions
global SyscallConfig
global SyscallInvoke

section .text
    SyscallConfig:
        mov r11, rcx
    ret

    ;; invoke Syscall and pass given arguments
    SyscallInvoke:
        mov r10, rcx
        mov eax, [r11 + 0x8]    ; set the syscall service number into eax
        jmp QWORD [r11]         ; jump to the following syscall
    ret                         ; finished execution
