#include "jaque.hpp"

// Implementaciones exactas (copiadas)

int encontrarIndiceRey(const vector<Pieza>& piezas, ColorPieza color){
    for(int i=0;i<(int)piezas.size();++i){
        if (piezas[i].alive && piezas[i].tipo==TipoPieza::King && piezas[i].color==color)
            return i;
    }
    return -1;
}

bool estaEnJaque(const vector<Pieza>& piezas, ColorPieza color){
    int reyIdx = encontrarIndiceRey(piezas, color);
    if (reyIdx == -1) return false;
    int reyF = piezas[reyIdx].fila, reyC = piezas[reyIdx].col;
    if (!dentroTablero(reyF, reyC)) return false;

    for(int i=0;i<(int)piezas.size();++i){
        if (!piezas[i].alive) continue;
        if (piezas[i].color == color) continue;
        if (movimientoLegal(piezas, i, reyF, reyC, ReglasFlags{}, ReglasFlags{})) return true; // flags vacíos para ataque
    }
    return false;
}

// Simulación (respeta protección y enroque extendido)
bool dejaReyEnJaqueSimulado(vector<Pieza>& piezas, int moverIdx, int dstF, int dstC, const ReglasFlags& flagsBlanco, const ReglasFlags& flagsNegro){
    int backupTab[FILAS][COLS];
    for(int r=0;r<FILAS;r++) for(int c=0;c<COLS;c++) backupTab[r][c] = tableroLogico[r][c];
    vector<Pieza> backupPiezas = piezas;

    int srcF = piezas[moverIdx].fila;
    int srcC = piezas[moverIdx].col;

    // manejar captura si existe (bloqueada si protegido)
    int victIdx = -1;
    if (dentroTablero(dstF,dstC)) {
        victIdx = tableroLogico[dstF][dstC];
        if (victIdx != -1) {
            const ReglasFlags& flagsOponente = (piezas[moverIdx].color == ColorPieza::White) ? flagsNegro : flagsBlanco;
            if (flagsOponente.proteccionActiva && flagsOponente.guardiaIdx == victIdx) {
                // captura bloqueada -> el movimiento a esa casilla no es válido realmente
                piezas = backupPiezas;
                for(int r=0;r<FILAS;r++) for(int c=0;c<COLS;c++) tableroLogico[r][c] = backupTab[r][c];
                return true; // fuerza "en jaque" como consecuencia de movimiento inválido
            }
        }
    }

    if (dentroTablero(srcF,srcC)) tableroLogico[srcF][srcC] = -1;

    if (victIdx != -1){
        piezas[victIdx].alive = false;
        piezas[victIdx].fila = piezas[victIdx].col = -1;
    }
    piezas[moverIdx].fila = dstF;
    piezas[moverIdx].col = dstC;
    tableroLogico[dstF][dstC] = moverIdx;

    // enroque normal y extendido en simulación
    if (backupPiezas[moverIdx].tipo == TipoPieza::King && abs(dstC - srcC) >= 2){
        int dir = (dstC - srcC) > 0 ? 1 : -1;
        int rookCol = (dir>0)? 7 : 0;
        int rookIdx = backupTab[srcF][rookCol];
        if (rookIdx != -1){
            int newRookCol = (abs(dstC - srcC) == 2) ? (srcC + dir) : (srcC + 2*dir); // extendido: torre cruza 2 casillas
            if (dentroTablero(srcF, rookCol)) tableroLogico[srcF][rookCol] = -1;
            tableroLogico[srcF][newRookCol] = rookIdx;
            piezas[rookIdx].fila = srcF;
            piezas[rookIdx].col = newRookCol;
        }
    }

    ColorPieza colorMover = piezas[moverIdx].color;
    bool enJaque = estaEnJaque(piezas, colorMover);

    piezas = backupPiezas;
    for(int r=0;r<FILAS;r++) for(int c=0;c<COLS;c++) tableroLogico[r][c] = backupTab[r][c];

    return enJaque;
}

bool esJaqueMate(vector<Pieza>& piezas, ColorPieza color, const ReglasFlags& flagsBlanco, const ReglasFlags& flagsNegro){
    if (!estaEnJaque(piezas, color)) return false;

    for(int i=0;i<(int)piezas.size();++i){
        if (!piezas[i].alive) continue;
        if (piezas[i].color != color) continue;
        for(int rf=0; rf<FILAS; ++rf){
            for(int rc=0; rc<COLS; ++rc){
                if (!movimientoLegal(piezas, i, rf, rc, flagsBlanco, flagsNegro)) continue;
                if (!dejaReyEnJaqueSimulado(piezas, i, rf, rc, flagsBlanco, flagsNegro)) return false;
            }
        }
    }
    return true;
}
