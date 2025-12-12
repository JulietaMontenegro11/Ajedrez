#include "helpers.hpp"

// helpers implementations (exactas)
bool dentroTablero(int f,int c){ return f>=0 && f<FILAS && c>=0 && c<COLS; }

sf::Vector2f centroCasilla(int fila,int col){
    float x = TABLERO_X + col * TAM_CASILLA + TAM_CASILLA / 2.0f;
    float y = TABLERO_Y + fila * TAM_CASILLA + TAM_CASILLA / 2.0f;
    return {x,y};
}

pair<int,int> casillaMasCercana(float px, float py){
    float fx = (px - TABLERO_X) / (float)TAM_CASILLA;
    float fy = (py - TABLERO_Y) / (float)TAM_CASILLA;
    int c = (int)floor(fx + 0.5f);
    int r = (int)floor(fy + 0.5f);
    if (r < 0) r = 0;
    if (r > FILAS-1) r = FILAS-1;
    if (c < 0) c = 0;
    if (c > COLS-1) c = COLS-1;


    return {r,c};
}

bool cargarTxt(sf::Texture &t, const string &ruta){
    if(!t.loadFromFile(ruta)){
        cerr << "No se pudo cargar: " << ruta << "\n";
        return false;
    }
    return true;
}
