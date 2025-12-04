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

// Tamaño de cada casilla jugable
const int TAM_CASILLA = 70;

// Offset donde comienza el tablero dentro del fondo
const int OFFSET_X = 375; // distancia del borde izquierdo
const int OFFSET_Y = 60;  // distancia del borde superior

int main() {
    sf::RenderWindow ventana(sf::VideoMode(1000, 700), "Ajedrez SFML");

    // ----- Cargar imágenes -----
    sf::Texture fondoTXT, tableroTXT;
    if (!cargarImagen(fondoTXT, "assets/images/Fondo.png")) return -1;
    if (!cargarImagen(tableroTXT, "assets/images/Tablero.png")) return -1;

    sf::Sprite fondo(fondoTXT);
    sf::Sprite tablero(tableroTXT);
    tablero.setPosition(OFFSET_X, OFFSET_Y);

    // ----- Cargar piezas -----
    map<string, sf::Texture> texturaPieza;
    string nombres[] = {"Peon","Torre","Caballo","Alfil","Dama","Rey"};
    string colores[] = {"B","R"}; // B = blancas, R = negras

    for (string base : nombres) {
        for (string c : colores) {
            string clave = base + c;
            if (!cargarImagen(texturaPieza[clave], "assets/images/" + clave + ".png")) return -1;
        }
    }

    // ----- Crear sprites de piezas -----
    map<string, sf::Sprite> pieza;

    auto crearYColocar = [&](string nombre, int col, int fila) {
        sf::Sprite nuevoSprite(texturaPieza[nombre]);
        nuevoSprite.setScale((float)TAM_CASILLA / texturaPieza[nombre].getSize().x,
                             (float)TAM_CASILLA / texturaPieza[nombre].getSize().y);
        nuevoSprite.setPosition(OFFSET_X + col * TAM_CASILLA,
                                OFFSET_Y + fila * TAM_CASILLA);
        pieza[nombre] = nuevoSprite;
    };

    // ----------- Colocación inicial de piezas -----------

    // Peones
    for (int i = 0; i < 8; i++) {
        crearYColocar("PeonB", i, 6);
        crearYColocar("PeonR", i, 1);
    }

    // Torres
    crearYColocar("TorreB", 0, 7);
    crearYColocar("TorreB", 7, 7);
    crearYColocar("TorreR", 0, 0);
    crearYColocar("TorreR", 7, 0);

    // Caballos
    crearYColocar("CaballoB", 1, 7);
    crearYColocar("CaballoB", 6, 7);
    crearYColocar("CaballoR", 1, 0);
    crearYColocar("CaballoR", 6, 0);

    // Alfiles
    crearYColocar("AlfilB", 2, 7);
    crearYColocar("AlfilB", 5, 7);
    crearYColocar("AlfilR", 2, 0);
    crearYColocar("AlfilR", 5, 0);

    // Damas
    crearYColocar("DamaB", 3, 7);
    crearYColocar("DamaR", 3, 0);

    // Reyes
    crearYColocar("ReyB", 4, 7);
    crearYColocar("ReyR", 4, 0);

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


