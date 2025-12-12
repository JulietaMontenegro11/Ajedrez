# Nombre del ejecutable
OBJ = Juego.exe

# Compilador
CXX = g++

# Flags de compilación
CXXFLAGS = -std=c++17 -Wall -Iinclude

# Flags para SFML
FLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

# Archivos fuente
SRC = src/Juego.cpp src/tipos.cpp src/graficos.cpp src/jaque.cpp src/helpers.cpp src/movimientos.cpp 

# Regla principal
all: $(OBJ)

$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OBJ) $(FLAGS)

# Limpiar
clean:
	rm -f $(OBJ)




