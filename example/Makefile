TARGET = main
CC = gcc

$(TARGET): main.c ../cmdoffice.c ../cmdhandle.c ../msgtool.c ../generalcmd.c ../sockettool.c ../officeio.c
	@$(CC) -g $^ -lpthread -o $@

.PHONY : clean
clean:
	@rm -f $(TARGET)