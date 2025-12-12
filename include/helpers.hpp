#pragma once
#include "tipos.hpp"

// ---------------------- Helpers ----------------------
bool dentroTablero(int f,int c);

sf::Vector2f centroCasilla(int fila,int col);

pair<int,int> casillaMasCercana(float px, float py);

bool cargarTxt(sf::Texture &t, const string &ruta);

// centrar y escalar sprite (declaración para uso en main/graficos)
void centrarYescalar(sf::Sprite &s, int fila, int col);
