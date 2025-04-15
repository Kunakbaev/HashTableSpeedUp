global myStrcmp

section .text

myStrcmp:
    ; loop never reaches ends, because return from function happens earlier
    ; and before that return, rax is always set
    ;mov rax, 0

    ; we know that we are dealing with long strings (len >= 12),
    ; so we can first compare 8 characters at once, and then continue loop
    ; mov rdx, [rdi]
    ; mov rcx, [rsi]
    ; cmp rcx, rdx
    ; je first8charsEqual
    ;     mov rax, 0
    ;     ret
    ; first8charsEqual:
    ; add rdi, 8
    ; add rsi, 8

    xor rcx, rcx
    xor rdx, rdx

    charsLoop:
        mov dl, byte [rdi]
        mov cl, byte [rsi]

        cmp rcx, rdx
        je sameChars
            ; different chars
            mov rax, 0
            ret
        sameChars:

        cmp rcx, 0
        je completelyEqual
            ; strings still are equal, continue loop
            inc rdi
            inc rsi
            jmp charsLoop
        completelyEqual:

        ; same chars, but both are equal to \0
        ; strings are completely equal
        mov rax, 1
        ret

    ; this will never be reached, so this is useless command
    ; ret

