CFLAGFS:=-I include/  -Wall -Wextra -g
SANFLAGS:=-fsanitize=address -fsanitize=undefined -fsanitize=leak

crop: obj/main.o obj/bmp.o | obj
	gcc obj/main.o obj/bmp.o -o crop

run: ./crop
	@LSAN_OPTIONS=detect_leaks=1 ./crop $(ARGS)

obj:
	mkdir obj

obj/main.o: src/main.c include/bmp.h | obj
	gcc -c $(CFLAGFS) src/main.c -o obj/main.o 	

obj/bmp.o: src/bmp.c include/bmp.h | obj
	gcc -c $(CFLAGFS) src/bmp.c -o obj/bmp.o

.PHONY: clean run
clean: 
	rm -rf crop obj *.bmp *.txt