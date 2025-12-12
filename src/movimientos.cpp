#include "movimientos.hpp"

// Implementaciones exactas de las funciones de movimiento (copiadas tal cual)

bool lineaLibre(int f1,int c1,int f2,int c2){
    int dx = (c2>c1)?1: (c2<c1)? -1: 0;
    int dy = (f2>f1)?1: (f2<f1)? -1: 0;
    int x = c1 + dx;
    int y = f1 + dy;
    while(x != c2 || y != f2){
        if (!dentroTablero(y,x)) return false;
        if (tableroLogico[y][x] != -1) return false;
        x += dx; y += dy;
    }
    return true;
}

// Versión ligera para atacar (sin enroque)
bool puedeAtacar(const vector<Pieza>& piezas, int attIdx, int f, int c){
    if (attIdx < 0 || attIdx >= (int)piezas.size()) return false;
    const Pieza &p = piezas[attIdx];
    if (!p.alive) return false;
    int sF = p.fila, sC = p.col;
    if (!dentroTablero(sF,sC)) return false;
    int dx = c - sC;
    int dy = f - sF;
    int adx = abs(dx), ady = abs(dy);

    switch(p.tipo){
        case TipoPieza::Pawn: {
            int dir = (p.color == ColorPieza::White) ? -1 : 1;
            if (ady==1 && adx==1 && dy==dir) return true;
            return false;
        }
        case TipoPieza::Rook: {
            if (dx!=0 && dy!=0) return false;
            return lineaLibre(sF,sC,f,c);
        }
        case TipoPieza::Bishop: {
            if (adx!=ady) return false;
            return lineaLibre(sF,sC,f,c);
        }
        case TipoPieza::Queen: {
            if (!((dx==0) || (dy==0) || (adx==ady))) return false;
            return lineaLibre(sF,sC,f,c);
        }
        case TipoPieza::Knight: {
            if ((adx==1 && ady==2) || (adx==2 && ady==1)) return true;
            return false;
        }
        case TipoPieza::King: {
            if (adx<=1 && ady<=1) return true;
            return false;
        }
    }
    return false;
}

bool estaCasillaAtacada(const vector<Pieza>& piezas, ColorPieza colorAtacante, int f, int c){
    for (int i=0;i<(int)piezas.size();++i){
        if (!piezas[i].alive) continue;
        if (piezas[i].color != colorAtacante) continue;
        if (puedeAtacar(piezas, i, f, c)) return true;
    }
    return false;
}

// Core: movimiento legal (incluye enroque normal y extendido, y bloquea captura de pieza protegida)
bool movimientoLegal(const vector<Pieza>& piezas, int idx, int dstF, int dstC, const ReglasFlags& flagsBlanco, const ReglasFlags& flagsNegro){
    if (!dentroTablero(dstF,dstC)) return false;
    const Pieza &p = piezas[idx];
    if (!p.alive) return false;
    int sF = p.fila, sC = p.col;
    if (sF==dstF && sC==dstC) return false;

    // no capturar propia pieza
    if (tableroLogico[dstF][dstC] != -1){
        int occ = tableroLogico[dstF][dstC];
        if (occ >= 0 && piezas[occ].color == p.color) return false;

        // Regla 1: pieza protegida no puede ser capturada durante el turno de protección
        const ReglasFlags& flagsOponente = (p.color == ColorPieza::White) ? flagsNegro : flagsBlanco;
        if (flagsOponente.proteccionActiva && flagsOponente.guardiaIdx == occ){
            // La captura se ignora este turno: el movimiento a casilla ocupada por protegido no es legal
            return false;
        }
    }

    int dx = dstC - sC;
    int dy = dstF - sF;
    int adx = abs(dx), ady = abs(dy);

    switch(p.tipo){
        case TipoPieza::Pawn: {
            int dir = (p.color == ColorPieza::White) ? -1 : 1; // white sube (fila decrece)
            if (dx==0 && dy==dir && tableroLogico[dstF][dstC]==-1) return true;
            if (dx==0 && dy==2*dir){
                bool inicio = (p.color==ColorPieza::White)? (sF==6) : (sF==1);
                if (!inicio) return false;
                int midF = sF + dir;
                if (tableroLogico[midF][sC]==-1 && tableroLogico[dstF][dstC]==-1) return true;
                return false;
            }
            if (abs(dx)==1 && dy==dir && tableroLogico[dstF][dstC]!=-1){
                int occ = tableroLogico[dstF][dstC];
                if (occ>=0 && piezas[occ].color != p.color){
                    // también respeta protección del oponente (ya validado arriba)
                    return true;
                }
            }
            return false;
        }
        case TipoPieza::Rook: {
            if (dx!=0 && dy!=0) return false;
            if (!lineaLibre(sF,sC,dstF,dstC)) return false;
            return true;
        }
        case TipoPieza::Bishop: {
            if (adx!=ady) return false;
            if (!lineaLibre(sF,sC,dstF,dstC)) return false;
            return true;
        }
        case TipoPieza::Queen: {
            if (!((dx==0) || (dy==0) || (adx==ady))) return false;
            if (!lineaLibre(sF,sC,dstF,dstC)) return false;
            return true;
        }
        case TipoPieza::Knight: {
            if ((adx==1 && ady==2) || (adx==2 && ady==1)) return true;
            return false;
        }
        case TipoPieza::King: {
            // movimiento normal de 1 casilla
            if (adx<=1 && ady<=1) return true;

            // --- enroque normal (2 casillas) ---
            if (ady==0 && (adx==2)){
                if (p.hasMoved) return false;
                int dir = (dx>0)? 1 : -1;
                int rookCol = (dir>0) ? 7 : 0;
                if (!dentroTablero(sF, rookCol)) return false;
                int rookIdx = tableroLogico[sF][rookCol];
                if (rookIdx == -1) return false;
                const Pieza &r = piezas[rookIdx];
                if (!r.alive || r.tipo != TipoPieza::Rook || r.color != p.color || r.hasMoved) return false;

                // camino libre
                for (int cc = (dir>0? sC+1 : rookCol+1); cc < (dir>0? rookCol : sC); ++cc){
                    if (tableroLogico[sF][cc] != -1) return false;
                }

                // casillas del rey no deben estar atacadas
                ColorPieza enemigo = (p.color==ColorPieza::White)? ColorPieza::Black : ColorPieza::White;
                if (estaCasillaAtacada(piezas, enemigo, sF, sC)) return false;
                int passC = sC + dir;
                int finalC = sC + 2*dir;
                if (estaCasillaAtacada(piezas, enemigo, sF, passC)) return false;
                if (estaCasillaAtacada(piezas, enemigo, sF, finalC)) return false;
                return true;
            }

            // --- Regla 2: enroque extendido (3 casillas) una vez por jugador ---
            if (ady==0 && (adx==3)){
                // Nota: validación de "una vez por partida" se hace antes de aplicar movimiento (no aquí),
                // pero aquí comprobamos condiciones posicionales.
                if (p.hasMoved) return false;
                int dir = (dx>0)? 1 : -1;
                // Se puede enrocar con CUALQUIER torre del lado escogido siempre que no se haya movido.
                int rookCol = (dir>0) ? 7 : 0;
                if (!dentroTablero(sF, rookCol)) return false;
                int rookIdx = tableroLogico[sF][rookCol];
                if (rookIdx == -1) return false;
                const Pieza &r = piezas[rookIdx];
                if (!r.alive || r.tipo != TipoPieza::Rook || r.color != p.color || r.hasMoved) return false;

                // camino libre completo entre rey y torre
                for (int cc = (dir>0? sC+1 : rookCol+1); cc < (dir>0? rookCol : sC); ++cc){
                    if (tableroLogico[sF][cc] != -1) return false;
                }

                // casillas del rey no deben estar atacadas durante el paso
                ColorPieza enemigo = (p.color==ColorPieza::White)? ColorPieza::Black : ColorPieza::White;
                if (estaCasillaAtacada(piezas, enemigo, sF, sC)) return false;
                int passC1 = sC + dir;
                int passC2 = sC + 2*dir;
                int finalC = sC + 3*dir;
                if (estaCasillaAtacada(piezas, enemigo, sF, passC1)) return false;
                if (estaCasillaAtacada(piezas, enemigo, sF, passC2)) return false;
                if (estaCasillaAtacada(piezas, enemigo, sF, finalC)) return false;
                return true;
            }

            return false;
        }
    }
    return false;
}
