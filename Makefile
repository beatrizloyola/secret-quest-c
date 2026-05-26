CC     = gcc
CFLAGS = -I/usr/local/include -Wall -Wextra
LIBS   = -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
SRC    = main.c mapa/mapa.c entidade/entidade.c
OUT    = game

$(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LIBS)

clean:
	rm -f $(OUT)
