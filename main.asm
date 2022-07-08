section .text
	global main
main:
	push ebp
	mov ebp, esp
	mov dword [ebp - 4], 2123
	mov eax, dword [ebp - 4]
	mov dword [ebp - 8], eax
	mov eax, dword [ebp - 4]
	mov byte [ebp - 9], al
	movzx eax, byte [ebp - 9]
	mov byte [ebp - 10], al
	mov eax, dword [ebp - 8]
	mov word [ebp - 12], ax
	movzx eax, word [ebp - 12]
	mov byte [ebp - 13], al
	mov eax, 0
	pop ebp
	ret