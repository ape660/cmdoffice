SOURCES = $(wildcard *.c ../*.c)
OBJS := $(patsubst %.c,%.o, $(SOURCES))

CC = gcc
TARGET = main
 
all: main
 
%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
 
include $(SOURCES:.c=.d)
 
$(TARGET):$(OBJS) $(SOURCES)
	@$(CC) -o  $(TARGET) $(OBJS) -lpthread
	@rm -f *.o  *.d ../*.o ../*.d

.PHONY : clean
clean:
	@rm -f *.o $(TARGET) *.d ../*.o ../*.d


