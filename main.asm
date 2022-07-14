section .text
	global main
main:
	push ebp
	mov ebp, esp
	mov dword [ebp - 4], 1030
	mov eax, dword [ebp - 4]
	mov dword [ebp - 8], eax
	mov eax, dword [ebp - 4]
	cmp eax, dword [ebp - 8]
	setne al
	movzx eax, al
	mov dword [ebp - 8], eax
	mov word [ebp - 10], 843
	mov word [ebp - 12], 200
	mov ax, word [ebp - 12]
	cmp eax, dword [ebp - 4]
	setle al
	movzx eax, al
	mov word [ebp - 12], ax
	cmp word [ebp - 12], 1
	setne al
	movzx eax, al
	mov word [ebp - 10], ax
	movzx eax, word [ebp - 10]
	mov byte [ebp - 13], al
	mov eax, [ebp - 8]
	pop ebp
	ret
