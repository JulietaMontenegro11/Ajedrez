# AlMate

Almate, uno de los clasicos juegos de mesa, pero en especial de estrategia: el Ajedrez, pero con nuevas reglas y funciones que aumentaran el desafío y las dificultades para ganar. Los jugadores mejorarán su lógica y capacidad de análisis mediante un combate entre piezas.

### 🎯 Objetivo del Juego

Dar Jaque Mate al Rey del oponente, es decir, colocar al rey bajo ataque de manera que no pueda escapar en ningún movimiento legal.

### Intrucciones de juego

Cada jugador comienza con 16 piezas:
    1 Rey
    1 Dama
    2 Torres
    2 Alfiles
    2 Caballos
    8 Peones

#### Movimiento de piezas:

    Rey. Se mueve 1 casilla en cualquier dirección (horizontal, vertical o diagonal). No puede moverse a una casilla atacada por una pieza enemiga.

    Dama. Se mueve cualquier número de casillas en línea recta: horizontal, vertical o diagonal.

    Torre. Se mueve cualquier número de casillas horizontal o verticalmente.

    Alfil. Se mueve cualquier número de casillas diagonalmente.

    Caballo. Se mueve en forma de “L”: dos casillas en una dirección (horizontal o vertical) y luego una en perpendicular. Puede saltar sobre piezas.

    Peón. Se mueve 1 casilla hacia adelante. En su primer movimiento puede avanzar 2 casillas. Captura en diagonal 1 casilla hacia adelante.
    Promoción: al llegar a la última fila, puede convertirse en Dama, Torre, Alfil o Caballo.

#### Jaque y jaque mate

    Jaque: El rey está bajo amenaza de captura.

    Jaque mate: El rey está en jaque y no hay movimientos legales para escapar, terminando el juego.

    Ahogado (Stalemate): Cuando el jugador no está en jaque pero no puede mover ninguna pieza legalmente, el juego termina en empate.

#### Enroque

    Movimiento simultáneo de rey y torre.

    Condiciones:
    Ninguna de las piezas se ha movido antes.
    No hay piezas entre rey y torre.
    Rey no está en jaque, ni pasa por casillas atacadas.
    Tipos:
    Enroque corto: rey se mueve 2 casillas hacia la torre más cercana (g1 o g8).
    Enroque largo: rey se mueve 2 casillas hacia la torre más lejana (c1 o c8).

#### Nuevas funciones

Peón protegido

Enroque extremo


### Instrucciones para compilar y ejecutar

Clonar repositorio:
```bash
git clone https://github.com/JulietaMontenegro11/Ajedrez
```
Ingresa en la terminal para compilar:
```bash
g++ src/Juego.cpp -Iinclude -o bin/Juego.exe -lsfml-graphics -lsfml-window -lsfml-system
```
Ingresar en la terminal para ejecutar:
```bash
>C:\Users\camil\.vscode\Ajedrez\bin\Juego.exe

```
### 🎮 Controles

Lista los controles:

Botón de touchpad o mouse: seleccionar pieza y arrastrarla a la casilla destinada.

### ⚙️ Mecánicas

Movimientos de piezas: rey, dama, torre, alfil, caballo y peón, con todas sus reglas.
Jaque y jaque mate: el objetivo sigue siendo dar jaque mate al rey enemigo.
Enroque: corto o largo, según las reglas.
Promoción de peón.

### 🏆 Características

- Ajedrez estilo clásico

### 👥 Equipo

- **Líder**: Julieta Montenegro Espinosa (@JulietaMontenegro11)
- **Integrante 2**: Dailin Nava Portillo (@usuario-github)

### 🛠️ Tecnologías

- Lenguaje: C++
- Gráficos: Creados en Canva
- Plataforma: Windows
- IDE: Visual Studio Code
- Librerias: 

### 📜 Créditos

---

