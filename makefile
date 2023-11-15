compile:
	gcc memorysystem.c

build:
	gcc memorysystem.c -o memorysystem.exe -Wall -lm

run:
	gcc memorysystem.c -o memorysystem.exe -Wall -lm
	./memorysystem.exe

clean:
	rm memorysystem.exe

rebuild:
	rm memorysystem.exe
	gcc memorysystem.c -o memorysystem.exe -Wall -lm
