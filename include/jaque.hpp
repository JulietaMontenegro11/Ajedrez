#pragma once
#include "tipos.hpp"
#include "movimientos.hpp"


// Declaraciones para jaque/jaque mate
int encontrarIndiceRey(const vector<Pieza>& piezas, ColorPieza color);

bool estaEnJaque(const vector<Pieza>& piezas, ColorPieza color);

// Simulación (respeta protección y enroque extendido)
bool dejaReyEnJaqueSimulado(vector<Pieza>& piezas, int moverIdx, int dstF, int dstC, const ReglasFlags& flagsBlanco, const ReglasFlags& flagsNegro);

bool esJaqueMate(vector<Pieza>& piezas, ColorPieza color, const ReglasFlags& flagsBlanco, const ReglasFlags& flagsNegro);
