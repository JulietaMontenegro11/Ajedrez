#pragma once
#include "tipos.hpp"
#include "helpers.hpp"
#include "jaque.hpp" // necesita ReglasFlags en la firma de movimientoLegal

// ---------------------- Movimiento / reglas ----------------------
bool lineaLibre(int f1,int c1,int f2,int c2);

// Versión ligera para atacar (sin enroque)
bool puedeAtacar(const vector<Pieza>& piezas, int attIdx, int f, int c);

bool estaCasillaAtacada(const vector<Pieza>& piezas, ColorPieza colorAtacante, int f, int c);

bool movimientoLegal(const vector<Pieza>& piezas, int idx, int dstF, int dstC, const ReglasFlags& flagsBlanco, const ReglasFlags& flagsNegro);
