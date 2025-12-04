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
// Constantes de tablero
// ----------------------
const int TAM_CASILLA = 70;     // tamaño de cada casilla
const int OFFSET_X = 375;       // distancia del borde izquierdo
const int OFFSET_Y = 60;        // distancia del borde superior
const int DESPLAZAMIENTO_X = 5; // ajuste fino horizontal
const int DESPLAZAMIENTO_Y = 5; // ajuste fino vertical

int main() {
    sf::RenderWindow ventana(sf::VideoMode(1000, 700), "Ajedrez SFML");

    // ----- Cargar imágenes -----
    sf::Texture fondoTXT, tableroTXT;
    cargarImagen(fondoTXT, "assets/images/Fondo.png");
    cargarImagen(tableroTXT, "assets/images/Tablero.png");

    sf::Sprite fondo(fondoTXT);
    sf::Sprite tablero(tableroTXT);
    tablero.setPosition(OFFSET_X, OFFSET_Y);

    // ----- Cargar piezas -----
    map<string, sf::Texture> texturaPieza;
    string nombres[] = {"Peon","Torre","Caballo","Alfil","Dama","Rey"};
    string colores[] = {"B","R"}; // B = blancas, R = negras

    for (string base : nombres) {
        for (string c : colores) {
            cargarImagen(texturaPieza[base + c], "assets/images/" + base + c + ".png");
        }
    }

    // ----- Crear sprites -----
    map<string, sf::Sprite> pieza;
    for (auto &t : texturaPieza)
        pieza[t.first].setTexture(t.second);

    // ----------- Función para colocar una pieza en el tablero -----------
    auto colocarPieza = [&](string nombre, int columna, int fila) {
        pieza[nombre].setPosition(
            OFFSET_X + columna * TAM_CASILLA + DESPLAZAMIENTO_X,
            OFFSET_Y + fila * TAM_CASILLA + DESPLAZAMIENTO_Y
        );
        pieza[nombre].setScale(
            (float)TAM_CASILLA / pieza[nombre].getTexture()->getSize().x,
            (float)TAM_CASILLA / pieza[nombre].getTexture()->getSize().y
        );
    };

    // ----------- Colocación inicial de piezas -----------

    // Peones
    for (int i = 0; i < 8; i++) {
        pieza["PeonB" + to_string(i)] = pieza["PeonB"];
        colocarPieza("PeonB" + to_string(i), i, 6);

        pieza["PeonR" + to_string(i)] = pieza["PeonR"];
        colocarPieza("PeonR" + to_string(i), i, 1);
    }

    // Torres
    pieza["TorreB0"] = pieza["TorreB"]; colocarPieza("TorreB0", 0, 7);
    pieza["TorreB1"] = pieza["TorreB"]; colocarPieza("TorreB1", 7, 7);

    pieza["TorreR0"] = pieza["TorreR"]; colocarPieza("TorreR0", 0, 0);
    pieza["TorreR1"] = pieza["TorreR"]; colocarPieza("TorreR1", 7, 0);

    // Caballos
    pieza["CaballoB0"] = pieza["CaballoB"]; colocarPieza("CaballoB0", 1, 7);
    pieza["CaballoB1"] = pieza["CaballoB"]; colocarPieza("CaballoB1", 6, 7);

    pieza["CaballoR0"] = pieza["CaballoR"]; colocarPieza("CaballoR0", 1, 0);
    pieza["CaballoR1"] = pieza["CaballoR"]; colocarPieza("CaballoR1", 6, 0);

    // Alfiles
    pieza["AlfilB0"] = pieza["AlfilB"]; colocarPieza("AlfilB0", 2, 7);
    pieza["AlfilB1"] = pieza["AlfilB"]; colocarPieza("AlfilB1", 5, 7);

    pieza["AlfilR0"] = pieza["AlfilR"]; colocarPieza("AlfilR0", 2, 0);
    pieza["AlfilR1"] = pieza["AlfilR"]; colocarPieza("AlfilR1", 5, 0);

    // Damas
    colocarPieza("DamaB", 3, 7);
    colocarPieza("DamaR", 3, 0);

    // Reyes
    colocarPieza("ReyB", 4, 7);
    colocarPieza("ReyR", 4, 0);

    // --------------- Movimiento del mouse (arrastrar) ---------------
    bool moviendo = false;
    string piezaSeleccionada = "";
    sf::Vector2f diferencia;

    while (ventana.isOpen()) {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed)
                ventana.close();

            // Clic para seleccionar pieza
            if (evento.type == sf::Event::MouseButtonPressed && evento.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mouse = ventana.mapPixelToCoords(sf::Mouse::getPosition(ventana));
                for (auto &p : pieza) {
                    if (p.second.getGlobalBounds().contains(mouse)) {
                        moviendo = true;
                        piezaSeleccionada = p.first;
                        diferencia = mouse - p.second.getPosition();
                    }
                }
            }

            // Soltar pieza
            if (evento.type == sf::Event::MouseButtonReleased && evento.mouseButton.button == sf::Mouse::Left) {
                moviendo = false;
                piezaSeleccionada = "";
            }
        }

        if (moviendo && piezaSeleccionada != "") {
            sf::Vector2f mouse = ventana.mapPixelToCoords(sf::Mouse::getPosition(ventana));
            pieza[piezaSeleccionada].setPosition(mouse - diferencia);
        }

        ventana.clear();
        ventana.draw(fondo);
        ventana.draw(tablero);

        for (auto &p : pieza)
            ventana.draw(p.second);

        ventana.display();
    }

    return 0;
}
