[bits 64]

; export the functions
global SyscallPrepare
global SyscallInvoke

section .data
    address dq 0
    ssn dq 0

section .text
    SyscallPrepare:
        mov [rel address], rcx
        mov [rel ssn], rdx
    ret

    ;; Invoke Syscall and pass given arguments
    SyscallInvoke:
        mov r10, rcx
        mov eax, [rel ssn]
        jmp QWORD [rel address]
    ret                         ; finished execution