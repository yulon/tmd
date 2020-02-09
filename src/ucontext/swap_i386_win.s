use32

mov [esp-4], eax
mov eax, [esp+4]

mov [eax+12], edx
mov edx, [esp-4]
mov [eax], edx
mov [eax+4], ebx
mov [eax+8], ecx
mov [eax+16], esi
mov [eax+20], edi
mov [eax+24], esp
mov [eax+28], ebp

; ip
mov edx, [esp]
mov [eax+32], edx

; flags
; pushfd
; pop edx
; mov [eax+36], edx

stmxcsr [eax+40]
fnstcw [eax+44]

; load NT_TIB
mov edx, 018h
mov esi, [fs:edx]

add esi, 4
lea edi, [eax+48]
mov ecx, 2
rep movsd

;;;;;

mov eax, [esp+8]

mov ebx, [eax+4]
mov esp, [eax+24]
mov ebp, [eax+28]

; ip
mov edx, [eax+32]
mov [esp], edx

; flags
; mov edx, [eax+36]
; push edx
; popfd

ldmxcsr [eax+40]
fldcw [eax+44]

; NT_TIB
mov edx, 018h
mov edi, [fs:edx]

add edi, 4
lea esi, [eax+48]
mov ecx, 2
rep movsd

mov esi, [eax+16]
mov edi, [eax+20]
mov eax, 1
ret
