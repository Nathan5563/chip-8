all: 
	gcc -o CHIP-8 driver.c -g3 -Wall -pedantic -std=c99

display:
	cd Components/Display && gcc -o display display.c display.h -g3 -Wall -pedantic -std=c99 $(shell pkg-config --cflags --libs sdl2)

clean:
	rm CHIP-8 Components/Display/display
