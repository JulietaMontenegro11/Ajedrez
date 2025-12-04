#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include <string>

using namespace std;

// ----------------------
// Función para cargar una imagen
// ----------------------
bool cargarImagen(sf::Texture &imagen, const string &ruta) {
    if (!imagen.loadFromFile(ruta)) {
        cout << "No se pudo cargar: " << ruta << endl;
        return false;
    }
    return true;
}

// ----------------------
// Tamaños y offsets
// ----------------------
const int TAM_CASILLA = 70;           // tamaño de cada casilla jugable
const int TABLERO_TOTAL = 580;        // tablero incluyendo márgenes 10px
const int OFFSET_X = 375;             // distancia del borde izquierdo
const int OFFSET_Y = 60;              // distancia del borde superior
const int MARGEN = 10;                // margen dentro de la imagen del tablero

// ----------------------
// Desplazamiento adicional para ajuste fino
// ----------------------
const int DESPLAZ_X_BLANCA = 2;
const int DESPLAZ_Y_BLANCA = 2;
const int DESPLAZ_X_NEGRA = 0;
const int DESPLAZ_Y_NEGRA = 0;

int main() {
    sf::RenderWindow ventana(sf::VideoMode(1000, 700), "Ajedrez SFML");

    // ----- Cargar imágenes -----
    sf::Texture fondoTXT, tableroTXT;
    if (!cargarImagen(fondoTXT, "assets/images/Fondo.png")) return -1;
    if (!cargarImagen(tableroTXT, "assets/images/Tablero.png")) return -1;

    sf::Sprite fondo(fondoTXT);
    sf::Sprite tablero(tableroTXT);
    tablero.setPosition(OFFSET_X, OFFSET_Y);

    // ----- Cargar texturas de piezas -----
    map<string, sf::Texture> texturaPieza;
    string tipos[] = {"Peon", "Torre", "Caballo", "Alfil", "Dama", "Rey"};
    string colores[] = {"B", "R"}; // B = blancas, R = negras

    for (string tipo : tipos) {
        for (string c : colores) {
            cargarImagen(texturaPieza[tipo + c], "assets/images/" + tipo + c + ".png");
        }
    }

    // ----- Crear sprites de piezas (solo los que se van a usar) -----
    map<string, sf::Sprite> pieza;

    auto colocarPieza = [&](sf::Sprite &s, int columna, int fila, bool blanca) {
        int dx = blanca ? DESPLAZ_X_BLANCA : DESPLAZ_X_NEGRA;
        int dy = blanca ? DESPLAZ_Y_BLANCA : DESPLAZ_Y_NEGRA;

        s.setPosition(OFFSET_X + MARGEN + columna * TAM_CASILLA + dx,
                      OFFSET_Y + MARGEN + fila * TAM_CASILLA + dy);

        s.setScale((float)TAM_CASILLA / s.getTexture()->getSize().x,
                   (float)TAM_CASILLA / s.getTexture()->getSize().y);
    };

    // ----- Colocación inicial de piezas -----

    // Peones
    for (int i = 0; i < 8; i++) {
        sf::Sprite pb(texturaPieza["PeonB"]);
        colocarPieza(pb, i, 6, true);
        pieza["PeonB" + to_string(i)] = pb;

        sf::Sprite pr(texturaPieza["PeonR"]);
        colocarPieza(pr, i, 1, false);
        pieza["PeonR" + to_string(i)] = pr;
    }

    // Torres
    {
        sf::Sprite tb0(texturaPieza["TorreB"]); colocarPieza(tb0, 0, 7, true); pieza["TorreB0"] = tb0;
        sf::Sprite tb1(texturaPieza["TorreB"]); colocarPieza(tb1, 7, 7, true); pieza["TorreB1"] = tb1;

        sf::Sprite tr0(texturaPieza["TorreR"]); colocarPieza(tr0, 0, 0, false); pieza["TorreR0"] = tr0;
        sf::Sprite tr1(texturaPieza["TorreR"]); colocarPieza(tr1, 7, 0, false); pieza["TorreR1"] = tr1;
    }

    // Caballos
    {
        sf::Sprite cb0(texturaPieza["CaballoB"]); colocarPieza(cb0, 1, 7, true); pieza["CaballoB0"] = cb0;
        sf::Sprite cb1(texturaPieza["CaballoB"]); colocarPieza(cb1, 6, 7, true); pieza["CaballoB1"] = cb1;

        sf::Sprite cr0(texturaPieza["CaballoR"]); colocarPieza(cr0, 1, 0, false); pieza["CaballoR0"] = cr0;
        sf::Sprite cr1(texturaPieza["CaballoR"]); colocarPieza(cr1, 6, 0, false); pieza["CaballoR1"] = cr1;
    }

    // Alfiles
    {
        sf::Sprite ab0(texturaPieza["AlfilB"]); colocarPieza(ab0, 2, 7, true); pieza["AlfilB0"] = ab0;
        sf::Sprite ab1(texturaPieza["AlfilB"]); colocarPieza(ab1, 5, 7, true); pieza["AlfilB1"] = ab1;

        sf::Sprite ar0(texturaPieza["AlfilR"]); colocarPieza(ar0, 2, 0, false); pieza["AlfilR0"] = ar0;
        sf::Sprite ar1(texturaPieza["AlfilR"]); colocarPieza(ar1, 5, 0, false); pieza["AlfilR1"] = ar1;
    }

    // Damas
    {
        sf::Sprite db(texturaPieza["DamaB"]); colocarPieza(db, 3, 7, true); pieza["DamaB"] = db;
        sf::Sprite dr(texturaPieza["DamaR"]); colocarPieza(dr, 3, 0, false); pieza["DamaR"] = dr;
    }

    // Reyes
    {
        sf::Sprite rb(texturaPieza["ReyB"]); colocarPieza(rb, 4, 7, true); pieza["ReyB"] = rb;
        sf::Sprite rr(texturaPieza["ReyR"]); colocarPieza(rr, 4, 0, false); pieza["ReyR"] = rr;
    }

    // ----- Movimiento del mouse (arrastrar piezas) -----
    bool moviendo = false;
    string piezaSeleccionada = "";
    sf::Vector2f diferencia;

    while (ventana.isOpen()) {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed)
                ventana.close();

            // Selección de pieza
            if (evento.type == sf::Event::MouseButtonPressed && evento.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mouse = ventana.mapPixelToCoords(sf::Mouse::getPosition(ventana));
                for (auto &p : pieza) {
                    if (p.second.getGlobalBounds().contains(mouse)) {
                        moviendo = true;
                        piezaSeleccionada = p.first;
                        diferencia = mouse - p.second.getPosition();
                        break;
                    }
                }
            }

            // Soltar pieza
            if (evento.type == sf::Event::MouseButtonReleased && evento.mouseButton.button == sf::Mouse::Left) {
                moviendo = false;
                piezaSeleccionada = "";
            }
        }

        // Movimiento en tiempo real
        if (moviendo && piezaSeleccionada != "") {
            sf::Vector2f mouse = ventana.mapPixelToCoords(sf::Mouse::getPosition(ventana));
            pieza[piezaSeleccionada].setPosition(mouse - diferencia);
        }

        // Dibujado
        ventana.clear();
        ventana.draw(fondo);
        ventana.draw(tablero);
        for (auto &p : pieza)
            ventana.draw(p.second);
        ventana.display();
    }

    return 0;
}


