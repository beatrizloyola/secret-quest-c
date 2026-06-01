CC  = gcc
SRC = main.c mapa/mapa.c entidade/entidade.c sistema/sistema.c

# ---- Linux ou WSL ----
ifeq ($(OS),Windows_NT)
CFLAGS = -I/mingw64/include -Wall -Wextra
LIBS   = -L/mingw64/lib -lraylib -lopengl32 -lgdi32 -lwinmm
OUT    = game.exe
CLEAN  = $(RM) $(OUT)
else
CFLAGS = -I/usr/local/include -Wall -Wextra
LIBS   = -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
OUT    = game
CLEAN  = rm -f $(OUT)
endif

$(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LIBS)

clean:
	$(CLEAN)
