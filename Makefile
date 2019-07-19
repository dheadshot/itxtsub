itxtsub: main.o
	gcc -o itxtsub main.o
	chmod +x itxtsub

main.o: main.c main.h
	gcc ${CFLAGS} -c main.c
