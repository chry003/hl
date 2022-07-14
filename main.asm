section .text
	global main
main:
	push ebp
	mov ebp, esp
	mov dword [ebp - 4], 20
	mov dword [ebp - 8], 10
	mov eax, dword [ebp - 4]
	cmp eax, dword [ebp - 8]
	setg al
	movzx eax, al
	mov dword [ebp - 8], eax
	cmp dword [ebp - 8], 0
	sete al
	movzx eax, al
	mov byte [ebp - 9], al
	mov eax, dword [ebp - 8]
	cmp al, byte [ebp - 9]
	setg al
	movzx eax, al
	mov byte [ebp - 9], al
	mov eax, [ebp - 9]
	pop ebp
	ret
