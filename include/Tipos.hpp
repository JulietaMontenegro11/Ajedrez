#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>
using namespace std;

// ---------------------- Configuración ----------------------
const int TAM_CASILLA = 70;
const int TABLERO_X   = 381; // coordenada X del tablero (sin márgenes)
const int TABLERO_Y   = 50;  // coordenada Y del tablero (sin márgenes)
const int FILAS = 8;
const int COLS  = 8;

// tolerancia (px) para aceptar un "drop" en la casilla más cercana
const float RADIO_ACEPTACION = 40.0f;

// animación captura
const float DURACION_ANIMACION_CAPTURA = 0.30f; // segundos

// dot color
const sf::Color DOT_COLOR(200, 200, 255, 220);

// ---------------------- Tipos ----------------------
enum class TipoPieza { Pawn, Rook, Knight, Bishop, Queen, King };
enum class ColorPieza { White, Black };

struct Pieza {
    string id;           // id único
    TipoPieza tipo;
    ColorPieza color;
    int fila, col;       // posición lógica (-1,-1 si fuera)
    sf::Sprite sprite;

    // para animaciones / restaurar escala
    float baseSx = 1.0f;
    float baseSy = 1.0f;

    bool alive = true;
    bool animandoCaptura = false;
    sf::Clock animClock;

    // si la pieza ya se movió (importante para enroque)
    bool hasMoved = false;

    // protección "guardia": si true, no puede ser capturada durante el turno de protección activo
    bool protegido = false;
};

// tablero lógico: índice de vector piezas, o -1 si vacío
extern int tableroLogico[FILAS][COLS];

// Flags de reglas especiales por jugador
struct ReglasFlags {
    bool guardiaUsado = false;           // 1 vez por juego
    int  guardiaIdx = -1;                // índice pieza protegida
    bool proteccionActiva = false;       // protección corre durante el turno completo del rival
    ColorPieza proteccionTurnoDe = ColorPieza::White; // quién está protegido este turno
    bool enroque3Usado = false;          // 1 vez por juego (enroque extendido)
};
