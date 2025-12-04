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
// Constantes del tablero
// ----------------------
const int TAM_CASILLA = 70;       // cada casilla 70x70
const int TABLERO_REAL = 560;     // tablero sin margenes
const int MARGEN = 10;            // margenes de la imagen del tablero
const int OFFSET_X = 375;         // distancia del tablero al borde izquierdo del fondo
const int OFFSET_Y = 60;          // distancia del tablero al borde superior del fondo

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
    string nombres[] = {"Peon", "Torre", "Caballo", "Alfil", "Dama", "Rey"};
    string colores[] = {"B", "R"}; // B = blancas, R = negras

    for (string base : nombres) {
        for (string c : colores) {
            if(!cargarImagen(texturaPieza[base + c], "assets/images/" + base + c + ".png")) return -1;
        }
    }

    // ----- Crear sprites para cada pieza (únicos) -----
    map<string, sf::Sprite> pieza;

    auto crearYColocar = [&](string nombreBase, string color, int col, int fila, int indice=0) {
        string nombre = nombreBase + color;
        if(indice >= 0) nombre += "_" + to_string(indice);
        sf::Sprite sprite(texturaPieza[nombreBase + color]);
        sprite.setPosition(OFFSET_X + col * TAM_CASILLA, OFFSET_Y + fila * TAM_CASILLA);
        pieza[nombre] = sprite;
    };

    // -------- Colocación inicial de piezas --------
    // Peones
    for (int i = 0; i < 8; i++) {
        crearYColocar("Peon", "B", i, 6, i);
        crearYColocar("Peon", "R", i, 1, i);
    }

    // Torres
    crearYColocar("Torre", "B", 0, 7, 0);
    crearYColocar("Torre", "B", 7, 7, 1);
    crearYColocar("Torre", "R", 0, 0, 0);
    crearYColocar("Torre", "R", 7, 0, 1);

    // Caballos
    crearYColocar("Caballo", "B", 1, 7, 0);
    crearYColocar("Caballo", "B", 6, 7, 1);
    crearYColocar("Caballo", "R", 1, 0, 0);
    crearYColocar("Caballo", "R", 6, 0, 1);

    // Alfiles
    crearYColocar("Alfil", "B", 2, 7, 0);
    crearYColocar("Alfil", "B", 5, 7, 1);
    crearYColocar("Alfil", "R", 2, 0, 0);
    crearYColocar("Alfil", "R", 5, 0, 1);

    // Damas
    crearYColocar("Dama", "B", 3, 7);
    crearYColocar("Dama", "R", 3, 0);

    // Reyes
    crearYColocar("Rey", "B", 4, 7);
    crearYColocar("Rey", "R", 4, 0);

    // ----- Movimiento del mouse para arrastrar piezas -----
    bool moviendo = false;
    string piezaSeleccionada = "";
    sf::Vector2f diferencia;

    while (ventana.isOpen()) {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed)
                ventana.close();

            // Seleccionar pieza con clic izquierdo
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

        // Arrastrar pieza
        if (moviendo && piezaSeleccionada != "") {
            sf::Vector2f mouse = ventana.mapPixelToCoords(sf::Mouse::getPosition(ventana));
            pieza[piezaSeleccionada].setPosition(mouse - diferencia);
        }

        // ----- Dibujado -----
        ventana.clear();
        ventana.draw(fondo);
        ventana.draw(tablero);

        for (auto &p : pieza)
            ventana.draw(p.second);

        ventana.display();
    }

    return 0;
}


