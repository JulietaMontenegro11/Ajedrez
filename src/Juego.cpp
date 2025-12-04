// Juego.cpp
// Ajedrez SFML - Movimiento legal básico (peon, torre, alfil, caballo, dama, rey)
// Ajusta rutas de images en assets/images/ como ya tienes (PeonB.png, PeonR.png, TorreB.png, ...)

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <optional>

using namespace std;

// ----------------------
// Configuración (modifica si quieres)
// ----------------------
const int TAM_CASILLA = 70;
const int TABLERO_X = 381; // X real del tablero (sin márgenes)
const int TABLERO_Y = 67;  // Y real del tablero (sin márgenes)
const int FILAS = 8;
const int COLUMNAS = 8;

// ----------------------
// Utiles
// ----------------------
struct Pos { int fila, col; };
bool dentroTablero(int fila, int col){ return fila >= 0 && fila < FILAS && col >= 0 && col < COLUMNAS; }

// ----------------------
// Tipos y estructuras
// ----------------------
enum class TipoPieza { Pawn, Rook, Knight, Bishop, Queen, King };
enum class ColorPieza { White, Black };

struct Pieza {
    string id;             // id único, ej "PeonB_0"
    TipoPieza tipo;
    ColorPieza color;
    int fila, col;         // posición lógica en tablero
    sf::Sprite sprite;     // sprite asociado
};

// tablero lógico: almacena índice en vector piezas o -1 si vacío
int tableroLogico[FILAS][COLUMNAS];

// ----------------------
// Cargar textura helper
// ----------------------
bool cargarTxt(sf::Texture &t, const string &ruta){
    if(!t.loadFromFile(ruta)){
        cerr << "No se pudo cargar: " << ruta << "\n";
        return false;
    }
    return true;
}

// ----------------------
// Conversión coordenadas <-> casilla
// ----------------------
// Devuelve centro de casilla (en pixeles) para visualizar sprite correctamente
sf::Vector2f centroCasilla(int fila, int col){
    float x = TABLERO_X + col * TAM_CASILLA + TAM_CASILLA/2.0f;
    float y = TABLERO_Y + fila * TAM_CASILLA + TAM_CASILLA/2.0f;
    return {x, y};
}

// Dado un punto en pixeles, devuelve la casilla más cercana (por distancia al centro)
Pos casillaMasCercanaDesdePixel(float px, float py){
    // calcular candidato aproximado
    float fx = (px - TABLERO_X) / (float)TAM_CASILLA;
    float fy = (py - TABLERO_Y) / (float)TAM_CASILLA;
    int colC = (int)floor(fx + 0.5f);
    int filaC = (int)floor(fy + 0.5f);
    // clamp
    if (filaC < 0) filaC = 0;
    if (filaC > FILAS-1) filaC = FILAS-1;
    if (colC < 0) colC = 0;
    if (colC > COLUMNAS-1) colC = COLUMNAS-1;
    return {filaC, colC};
}

// ----------------------
// Validación de movimientos (considera bloqueos y captura)
// ----------------------
bool casillaOcupada(int fila, int col){ return tableroLogico[fila][col] != -1; }
ColorPieza colorDeIndice(int idx, const vector<Pieza>& piezas){ return piezas[idx].color; }
TipoPieza tipoDeIndice(int idx, const vector<Pieza>& piezas){ return piezas[idx].tipo; }

// Chequeo línea recta libre (excluye origen y destino)
bool lineaLibreRecta(int f1,int c1,int f2,int c2){
    int dx = (c2>c1)?1: (c2<c1)? -1: 0;
    int dy = (f2>f1)?1: (f2<f1)? -1: 0;
    int cx = c1 + dx;
    int cy = f1 + dy;
    while(cx != c2 || cy != f2){
        if (dentroTablero(cy,cx) && casillaOcupada(cy,cx)) return false;
        cx += dx; cy += dy;
    }
    return true;
}

bool movimientoLegal(const vector<Pieza>& piezas, int idx, int dstFila, int dstCol){
    if (!dentroTablero(dstFila,dstCol)) return false;
    const Pieza &p = piezas[idx];
    int srcF = p.fila, srcC = p.col;
    if (srcF == dstFila && srcC == dstCol) return false;
    // no poder capturar propia pieza
    if (casillaOcupada(dstFila,dstCol)){
        int occ = tableroLogico[dstFila][dstCol];
        if (occ >= 0 && piezas[occ].color == p.color) return false;
    }
    int dx = dstCol - srcC;
    int dy = dstFila - srcF;
    int adx = abs(dx), ady = abs(dy);

    switch(p.tipo){
        case TipoPieza::Pawn: {
            int dir = (p.color == ColorPieza::White) ? -1 : 1; // white up (fila decrece), black down
            // movimiento vertical simple
            if (dx == 0 && dy == dir && !casillaOcupada(dstFila,dstCol)) return true;
            // doble avance en primer movimiento
            if (dx == 0 && dy == 2*dir){
                bool inicio = (p.color == ColorPieza::White) ? (srcF == 6) : (srcF == 1);
                if (!inicio) return false;
                // casilla intermedia y destino deben estar vacías
                int midF = srcF + dir;
                if (!casillaOcupada(midF, srcC) && !casillaOcupada(dstFila,dstCol)) return true;
                return false;
            }
            // captura diagonal
            if (abs(dx) == 1 && dy == dir && casillaOcupada(dstFila,dstCol)){
                int occ = tableroLogico[dstFila][dstCol];
                if (occ >= 0 && piezas[occ].color != p.color) return true;
            }
            return false;
        }
        case TipoPieza::Rook: {
            if (dx != 0 && dy != 0) return false;
            if (!lineaLibreRecta(srcF,srcC,dstFila,dstCol)) return false;
            return true;
        }
        case TipoPieza::Bishop: {
            if (adx != ady) return false;
            if (!lineaLibreRecta(srcF,srcC,dstFila,dstCol)) return false;
            return true;
        }
        case TipoPieza::Queen: {
            if (!((dx == 0) || (dy == 0) || (adx == ady))) return false;
            if (!lineaLibreRecta(srcF,srcC,dstFila,dstCol)) return false;
            return true;
        }
        case TipoPieza::Knight: {
            if ( (adx==1 && ady==2) || (adx==2 && ady==1)) return true;
            return false;
        }
        case TipoPieza::King: {
            if (adx <= 1 && ady <= 1) return true;
            return false;
        }
    }
    return false;
}

// ----------------------
// Actualizar sprite visual en el centro de la casilla
// ----------------------
void centrarSpriteEnCasilla(sf::Sprite &s, int fila, int col){
    sf::Vector2f centro = centroCasilla(fila,col);
    // ajustar sprite para que su centro coincida
    sf::FloatRect bounds = s.getLocalBounds();
    s.setOrigin(bounds.width/2.0f, bounds.height/2.0f);
    s.setPosition(centro);
    // escalar para que quepa en TAM_CASILLA (si la textura no es 70x70)
    sf::Texture const* t = s.getTexture();
    if(t){
        float sx = (float)TAM_CASILLA / t->getSize().x;
        float sy = (float)TAM_CASILLA / t->getSize().y;
        s.setScale(sx, sy);
    }
}

// ----------------------
// Main
// ----------------------
int main(){
    // Ventana
    sf::RenderWindow window(sf::VideoMode(1000,700), "Ajedrez SFML - Reglas");
    window.setFramerateLimit(60);

    // Cargar texturas
    map<string,sf::Texture> tex;
    vector<pair<string,string>> archivos = {
        {"PeonB","assets/images/PeonB.png"},{"PeonR","assets/images/PeonR.png"},
        {"TorreB","assets/images/TorreB.png"},{"TorreR","assets/images/TorreR.png"},
        {"CaballoB","assets/images/CaballoB.png"},{"CaballoR","assets/images/CaballoR.png"},
        {"AlfilB","assets/images/AlfilB.png"},{"AlfilR","assets/images/AlfilR.png"},
        {"DamaB","assets/images/DamaB.png"},{"DamaR","assets/images/DamaR.png"},
        {"ReyB","assets/images/ReyB.png"},{"ReyR","assets/images/ReyR.png"},
        {"Fondo","assets/images/Fondo.png"},{"Tablero","assets/images/Tablero.png"}
    };
    for(auto &p:archivos){
        if(!cargarTxt(tex[p.first], p.second)){
            cerr << "Error cargando textura " << p.second << "\n";
            return -1;
        }
    }

    // Sprites fondo y tablero
    sf::Sprite fondo(tex["Fondo"]);
    sf::Sprite tablero(tex["Tablero"]);
    // posicionar tablero en coordenadas reales (TABLERO_X/TABLERO_Y). Si la imagen Tablero contiene margenes,
    // puedes escalarla para que la zona jugable coincida con 8*TAM_CASILLA.
    tablero.setPosition((float)TABLERO_X, (float)TABLERO_Y);
    // escala opcional: si la imagen Tablero mide 580x580 y quieres que area jugable sea 8*TAM_CASILLA = 560
    // calcula escala: 560/580 = 0.9655. Si tu imagen ya está preparada, puedes comentar la siguiente línea.
    float escalaTab = (8.0f * TAM_CASILLA) / (float)tex["Tablero"].getSize().x;
    tablero.setScale(escalaTab, escalaTab);

    // vector de piezas
    vector<Pieza> piezas;
    piezas.reserve(32);

    // inicializar tablero lógico a -1
    for(int r=0;r<FILAS;r++) for(int c=0;c<COLUMNAS;c++) tableroLogico[r][c] = -1;

    auto nuevaId = [&](const string &base, int n){
        return base + "_" + to_string(n);
    };

    // función auxiliar para añadir una pieza (crea sprite, la centra y actualiza tablero lógico)
    auto addPieza = [&](TipoPieza tipo, ColorPieza color, int fila, int col, const string &texKey, const string &idBase){
        Pieza p;
        static int contador = 0;
        p.id = nuevaId(idBase, contador++);
        p.tipo = tipo;
        p.color = color;
        p.fila = fila; p.col = col;
        p.sprite.setTexture(tex[texKey]);
        // centrar y escalar
        centrarSpriteEnCasilla(p.sprite, fila, col);
        tableroLogico[fila][col] = (int)piezas.size();
        piezas.push_back(move(p));
    };

    // Añadir todas las piezas (32) - nombres y mapeo a tipos
    // blancas en filas 6 (peones) y 7 (back rank)
    for(int c=0;c<8;c++) addPieza(TipoPieza::Pawn, ColorPieza::White, 6, c, "PeonB", "PeonB");
    addPieza(TipoPieza::Rook, ColorPieza::White, 7, 0, "TorreB", "TorreB");
    addPieza(TipoPieza::Knight, ColorPieza::White, 7, 1, "CaballoB", "CaballoB");
    addPieza(TipoPieza::Bishop, ColorPieza::White, 7, 2, "AlfilB", "AlfilB");
    addPieza(TipoPieza::Queen, ColorPieza::White, 7, 3, "DamaB", "DamaB");
    addPieza(TipoPieza::King, ColorPieza::White, 7, 4, "ReyB", "ReyB");
    addPieza(TipoPieza::Bishop, ColorPieza::White, 7, 5, "AlfilB", "AlfilB");
    addPieza(TipoPieza::Knight, ColorPieza::White, 7, 6, "CaballoB", "CaballoB");
    addPieza(TipoPieza::Rook, ColorPieza::White, 7, 7, "TorreB", "TorreB");

    // negras
    for(int c=0;c<8;c++) addPieza(TipoPieza::Pawn, ColorPieza::Black, 1, c, "PeonR", "PeonR");
    addPieza(TipoPieza::Rook, ColorPieza::Black, 0, 0, "TorreR", "TorreR");
    addPieza(TipoPieza::Knight, ColorPieza::Black, 0, 1, "CaballoR", "CaballoR");
    addPieza(TipoPieza::Bishop, ColorPieza::Black, 0, 2, "AlfilR", "AlfilR");
    addPieza(TipoPieza::Queen, ColorPieza::Black, 0, 3, "DamaR", "DamaR");
    addPieza(TipoPieza::King, ColorPieza::Black, 0, 4, "ReyR", "ReyR");
    addPieza(TipoPieza::Bishop, ColorPieza::Black, 0, 5, "AlfilR", "AlfilR");
    addPieza(TipoPieza::Knight, ColorPieza::Black, 0, 6, "CaballoR", "CaballoR");
    addPieza(TipoPieza::Rook, ColorPieza::Black, 0, 7, "TorreR", "TorreR");

    // Variables para arrastre
    bool arrastrando = false;
    int idxSeleccionado = -1;
    sf::Vector2f difMouse; // offset del mouse respecto al origin del sprite en el pick
    int origenFila= -1, origenCol = -1;

    // Bucle principal
    while(window.isOpen()){
        sf::Event ev;
        while(window.pollEvent(ev)){
            if(ev.type == sf::Event::Closed) window.close();

            // PICK: al presionar botón izquierdo, ver si clic sobre sprite (iterar de último a primero para topmost)
            if(ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button == sf::Mouse::Left){
                sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                // buscar pieza que contiene el punto, iterando en reversa para coger la superior
                idxSeleccionado = -1;
                for(int i = (int)piezas.size()-1; i>=0; --i){
                    if(piezas[i].sprite.getGlobalBounds().contains(mouse)){
                        idxSeleccionado = i;
                        break;
                    }
                }
                if(idxSeleccionado != -1){
                    arrastrando = true;
                    // calcular offset para arrastrar sin "saltar"
                    sf::Vector2f posSpr = piezas[idxSeleccionado].sprite.getPosition();
                    difMouse = mouse - posSpr;
                    origenFila = piezas[idxSeleccionado].fila;
                    origenCol  = piezas[idxSeleccionado].col;
                    // Mientras arrastramos, "liberamos" la casilla lógica temporalmente para permitir ver captura en ella
                    tableroLogico[origenFila][origenCol] = -1;
                }
            }

            // DROP: al soltar
            if(ev.type == sf::Event::MouseButtonReleased && ev.mouseButton.button == sf::Mouse::Left && arrastrando){
                arrastrando = false;
                sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                // encontrar casilla más cercana al punto del mouse
                Pos destino = casillaMasCercanaDesdePixel(mouse.x, mouse.y);
                int dstF = destino.fila;
                int dstC = destino.col;

                // seguridad: clamp
                if(!dentroTablero(dstF, dstC)){
                    dstF = max(0,min(FILAS-1,dstF));
                    dstC = max(0,min(COLUMNAS-1,dstC));
                }

                // validar movimiento
                bool valido = movimientoLegal(piezas, idxSeleccionado, dstF, dstC);

                if(valido){
                    // si hay pieza enemiga, eliminarla del vector y actualizar tablero
                    if(tableroLogico[dstF][dstC] != -1){
                        int idxVictima = tableroLogico[dstF][dstC];
                        // quitar víctima del mapa lógico y marcar su sprite fuera (o invisibilizar)
                        // Para mantener índices simples, marcaremos su sprite fuera de pantalla y tableroLogico en -1
                        piezas[idxVictima].sprite.setPosition(-1000.f, -1000.f);
                        piezas[idxVictima].fila = -1; piezas[idxVictima].col = -1;
                        tableroLogico[dstF][dstC] = -1;
                    }
                    // colocar pieza seleccionada en destino: actualizar datos lógicos y sprite centrado
                    piezas[idxSeleccionado].fila = dstF; piezas[idxSeleccionado].col = dstC;
                    centrarSpriteEnCasilla(piezas[idxSeleccionado].sprite, dstF, dstC);
                    tableroLogico[dstF][dstC] = idxSeleccionado;
                } else {
                    // regresar a origen
                    piezas[idxSeleccionado].fila = origenFila;
                    piezas[idxSeleccionado].col = origenCol;
                    centrarSpriteEnCasilla(piezas[idxSeleccionado].sprite, origenFila, origenCol);
                    tableroLogico[origenFila][origenCol] = idxSeleccionado;
                }
                idxSeleccionado = -1;
            }

            // (opcional) mover con teclado u otras interacciones puede ir aquí
        }

        // durante arrastre mover sprite con offset (fluido)
        if(arrastrando && idxSeleccionado != -1){
            sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            // la posición visual será mouse - difMouse (sprite origin está en centro por centrarSpriteEnCasilla)
            // Consideramos que sprite origin es centro; por tanto posicionar en mouse - difMouse:
            piezas[idxSeleccionado].sprite.setPosition(mouse - difMouse);
        }

        // dibujado
        window.clear();
        window.draw(fondo);
        window.draw(tablero);
        // dibujar todas piezas
        for(const auto &pz : piezas) window.draw(pz.sprite);
        window.display();
    }

    return 0;
}

