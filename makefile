main: main.c cmdoffice.c cmdhandle.c msgtool.c generalcmd.c sockettool.c officeio.c
	@gcc -g main.c cmdoffice.c cmdhandle.c msgtool.c generalcmd.c  sockettool.c officeio.c -lpthread -o main
clean:
	@rm -f main