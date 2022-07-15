section .text
	global main
main:
	push ebp
	mov ebp, esp
	mov dword [ebp - 4], 100
	mov dword [ebp - 8], 200
	mov dword [ebp - 12], 0
	mov dword [ebp - 12], 100
	mov eax, dword [ebp - 4]
	cmp eax, dword [ebp - 8]
	jle if_statement_0
	mov eax, dword [ebp - 4]
	mov dword [ebp - 12], eax
if_statement_0:
	mov eax, dword [ebp - 8]
	cmp eax, dword [ebp - 4]
	jle if_statement_1
	mov eax, dword [ebp - 8]
	mov dword [ebp - 12], eax
if_statement_1:
	mov eax, [ebp - 12]
	pop ebp
	ret