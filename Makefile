# Nombre del ejecutable
OBJ = Juego.exe

# Compilador
CXX = g++

# Flags para SFML
FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Archivo fuente
SRC = src/Juego.cpp

# Regla principal
all: $(OBJ)

$(OBJ): $(SRC)
	$(CXX) $(SRC) -o $(OBJ) $(FLAGS)

# Limpiar
clean:
	del $(OBJ)



