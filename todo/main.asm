section .text
	global main
main:
	push ebp
	mov ebp, esp
	mov dword [ebp - 4], 10
	mov dword [ebp - 8], 20
	mov eax, dword [ebp - 4]
	cmp eax, dword [ebp - 8]
	jg if_statement_0
	mov dword [ebp - 4], 20
	mov dword [ebp - 8], 20
if_statement_0:
	mov eax, [ebp - 8]
	pop ebp
	ret