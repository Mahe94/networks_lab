section .text
	global main
	extern scanf
	extern printf
	
print:
	push ebp
	mov ebp, esp
	sub esp, 8
	fstp qword[ebp-8]
	push format2
	call printf
	mov esp, ebp
	pop ebp
	ret
	
read:
	push ebp
	mov ebp, esp
	sub esp, 8
	lea eax, [esp]
	push eax
	push format1
	call scanf
	fld qword[ebp-8]
	mov esp, ebp
	pop ebp
	ret
	
main:
	mov eax, 4
	mov ebx, 1
	mov ecx, msg1
	mov edx, len1
	int 80h
	
	mov eax, 3
	mov ebx, 0
	mov ecx, size
	mov edx, 1
	int 80h
	
	sub byte[size], '0'
	
	mov byte[temp], 0
	mov ebx, array
	
reading:
	mov al, byte[temp]
	cmp al, byte[size]
	jnb adding
	
	call read
	fstp qword[ebx]
	
	inc byte[temp]
	add ebx, 8
	jmp reading
	
adding:
	mov ebx, array
	mov al, byte[size]
	mov byte[temp],al
	fldz
	fstp qword[sum]
adding2:
	fld qword[sum]
	fadd qword[ebx]
	fstp qword[sum]
	
	add ebx, 8
	inc byte[temp]
	
	mov al, byte[temp]
	cmp al, byte[size]
	je final
	jmp adding2
	
printing:
	mov byte[temp], 0
	mov ebx, array
	
printing2:
	mov al, byte[temp]
	cmp al, byte[size]
	jnb exit
	
	fld qword[ebx]
	call print
	
	inc byte[temp]
	add ebx, 8
	jmp printing2
	
final: 
	fld qword[sum]
	call print
exit:
	mov eax, 1
	mov ebx, 0
	int 80h
	
section .data
	msg1: db "Enter the size:"
	len1: equ $-msg1
	format1: db "%lf", 0
	format2: db "The number is %lf", 10, 0
	
section .bss
	array: resq 50
	size: resb 1
	temp: resb 1
	sum: resq 1
