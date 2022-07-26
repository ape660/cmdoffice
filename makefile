hello: main.c cmdoffice.c cmdpipeline.c officeprint.c
	@gcc main.c cmdoffice.c cmdpipeline.c officeprint.c -lpthread -o main
clean:
	rm -f main