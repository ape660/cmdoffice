main: main.c cmdoffice.c cmdhandle.c msgtool.c generalcmd.c sockettool.c
	@gcc -g main.c cmdoffice.c cmdhandle.c msgtool.c generalcmd.c  sockettool.c -lpthread -o main
clean:
	@rm -f main