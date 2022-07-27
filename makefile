main: main.c cmdoffice.c cmdhandle.c msgtool.c generalcmd.c
	@gcc -g main.c cmdoffice.c cmdhandle.c msgtool.c generalcmd.c -lpthread -o main
clean:
	@rm -f main