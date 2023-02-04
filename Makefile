build:
	gcc -Wall -std=c99 ./src/*.c -lSDL2 -lSDL2_mixer -o game

run:
	./game

clean:
	rm game


