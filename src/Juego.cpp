#include "tipos.hpp"
#include "helpers.hpp"
#include "movimientos.hpp"
#include "jaque.hpp"
#include "graficos.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>
using namespace std;
// Devuelve true si el jugador no tiene ningún movimiento legal
// Verifica si un jugador no tiene movimientos legales
// Para contar piezas, solo lee el vector
int contarPiezas(const std::vector<Pieza>& piezas, ColorPieza color) {
    int count = 0;
    for (const auto& p : piezas) {
        if (p.alive && p.color == color) count++;
    }
    return count;
}

// Para revisar si no hay movimientos legales, solo lee el vector
// Recibe el vector por referencia normal (no const)
bool sinMovimientosLegales(std::vector<Pieza>& piezas, ColorPieza color, ReglasFlags& flagsBlanco, ReglasFlags& flagsNegro) {
    for (size_t i = 0; i < piezas.size(); i++) {
        if (piezas[i].alive && piezas[i].color == color) {
            for (int f = 0; f < FILAS; f++) {
                for (int c = 0; c < COLS; c++) {
                    if (movimientoLegal(piezas, (int)i, f, c, flagsBlanco, flagsNegro)) {
                        if (!dejaReyEnJaqueSimulado(piezas, (int)i, f, c, flagsBlanco, flagsNegro)) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}
vector<Pieza> piezasComidasBlanco;
vector<Pieza> piezasComidasNegro;

void agregarPiezaCapturada(const Pieza& p){
    if(p.color == ColorPieza::White)
        piezasComidasBlanco.push_back(p);
    else
        piezasComidasNegro.push_back(p);
}



int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 700), "Ajedrez SFML - Jaque & Jaque Mate");
    window.setFramerateLimit(60);

    sf::SoundBuffer bufferPieza;
    if(!bufferPieza.loadFromFile("sounds/sonidopieza.wav")){
    std::cout << "Error al cargar sonido!" << std::endl;
    }
    sf::Sound sonidopieza;
    sonidopieza.setBuffer(bufferPieza);
    sonidopieza.setVolume(100); // 0 a 100

    


    // ---------------- Pantalla de inicio ----------------
sf::Texture texInicio, texBtnJugar;
if(!cargarTxt(texInicio, "assets/images/Inicio2.png")) return -1;
if(!cargarTxt(texBtnJugar, "assets/images/Jugar.png")) return -1;

sf::Sprite spriteInicio(texInicio);
sf::Sprite spriteJugar(texBtnJugar);
sf::Vector2u szBtn = texBtnJugar.getSize();
spriteJugar.setOrigin(szBtn.x/2.f, szBtn.y/2.f);
spriteJugar.setPosition(1000/2.f, 600.f);

// Bucle de pantalla de inicio
bool inicio = true;
while(inicio && window.isOpen()){
    sf::Event ev;
    while(window.pollEvent(ev)){
        if(ev.type == sf::Event::Closed) window.close();
        if(ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button == sf::Mouse::Left){
            sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if(spriteJugar.getGlobalBounds().contains(mouse)){
                inicio = false; // Click en jugar → salir del bucle de inicio
            }
        }
    }

    window.clear();
    window.draw(spriteInicio);
    window.draw(spriteJugar);
    window.display();
}

bool startGame = false;
while(window.isOpen() && !startGame){
    sf::Event ev;
    while(window.pollEvent(ev)){
        if(ev.type == sf::Event::Closed) return 0;

        if(ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button==sf::Mouse::Left){
            sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if(spriteJugar.getGlobalBounds().contains(mouse)) startGame = true;
        }
    }
    window.clear();
    window.draw(spriteInicio);
    window.draw(spriteJugar);
    window.display();
}

    // ---------------- Cargar texturas ----------------
    map<string, sf::Texture> tex;
    vector<pair<string, string>> lista = {
        {"PeonB","assets/images/PeonB.png"},{"PeonR","assets/images/PeonR.png"},
        {"TorreB","assets/images/TorreB.png"},{"TorreR","assets/images/TorreR.png"},
        {"CaballoB","assets/images/CaballoB.png"},{"CaballoR","assets/images/CaballoR.png"},
        {"AlfilB","assets/images/AlfilB.png"},{"AlfilR","assets/images/AlfilR.png"},
        {"DamaB","assets/images/DamaB.png"},{"DamaR","assets/images/DamaR.png"},
        {"ReyB","assets/images/ReyB.png"},{"ReyR","assets/images/ReyR.png"},
        {"Fondo","assets/images/Fondo.png"}, {"Tablero","assets/images/Tablero.png"},
        {"Escoge","assets/images/Escoge.png"},
        {"GanadorJ1","assets/images/GanadorJ1.png"},{"GanadorJ2","assets/images/GanadorJ2.png"},
        {"Empate","assets/images/Empate.png"},
        
    };
    for(auto &p: lista){
        if(!cargarTxt(tex[p.first], p.second)) return -1;
    }

    sf::Sprite fondo(tex["Fondo"]);
    sf::Sprite tablero(tex["Tablero"]);
    tablero.setPosition((float)TABLERO_X, (float)TABLERO_Y);
    float escalaTab = (8.0f * TAM_CASILLA) / (float)tex["Tablero"].getSize().x;
    tablero.setScale(escalaTab, escalaTab);

    // ---------------- Piezas ----------------
    vector<Pieza> piezas; piezas.reserve(32);
    for(int r=0;r<FILAS;r++) for(int c=0;c<COLS;c++) tableroLogico[r][c] = -1;

    auto idMaker = [&](const string &base, int n){ return base + "_" + to_string(n); };
    int contadorId = 0;
    auto addPieza = [&](TipoPieza tipo, ColorPieza color, int fila, int col, const string &texKey, const string &baseKey){
        Pieza p; p.id = idMaker(baseKey, contadorId++);
        p.tipo = tipo; p.color = color; p.fila = fila; p.col = col;
        p.sprite.setTexture(tex[texKey]);
        const sf::Texture* t = p.sprite.getTexture();
        if(t){ p.baseSx = (float)TAM_CASILLA/(float)t->getSize().x; p.baseSy = (float)TAM_CASILLA/(float)t->getSize().y; p.sprite.setScale(p.baseSx, p.baseSy);}
        centrarYescalar(p.sprite,fila,col);
        tableroLogico[fila][col] = (int)piezas.size();
        piezas.push_back(move(p));
    };

    // Blancas abajo
    for(int c=0;c<8;c++) addPieza(TipoPieza::Pawn, ColorPieza::White,6,c,"PeonB","PeonB");
    addPieza(TipoPieza::Rook, ColorPieza::White,7,0,"TorreB","TorreB");
    addPieza(TipoPieza::Knight, ColorPieza::White,7,1,"CaballoB","CaballoB");
    addPieza(TipoPieza::Bishop, ColorPieza::White,7,2,"AlfilB","AlfilB");
    addPieza(TipoPieza::Queen, ColorPieza::White,7,3,"DamaB","DamaB");
    addPieza(TipoPieza::King, ColorPieza::White,7,4,"ReyB","ReyB");
    addPieza(TipoPieza::Bishop, ColorPieza::White,7,5,"AlfilB","AlfilB");
    addPieza(TipoPieza::Knight, ColorPieza::White,7,6,"CaballoB","CaballoB");
    addPieza(TipoPieza::Rook, ColorPieza::White,7,7,"TorreB","TorreB");

    // Negras arriba
    for(int c=0;c<8;c++) addPieza(TipoPieza::Pawn, ColorPieza::Black,1,c,"PeonR","PeonR");
    addPieza(TipoPieza::Rook, ColorPieza::Black,0,0,"TorreR","TorreR");
    addPieza(TipoPieza::Knight, ColorPieza::Black,0,1,"CaballoR","CaballoR");
    addPieza(TipoPieza::Bishop, ColorPieza::Black,0,2,"AlfilR","AlfilR");
    addPieza(TipoPieza::Queen, ColorPieza::Black,0,3,"DamaR","DamaR");
    addPieza(TipoPieza::King, ColorPieza::Black,0,4,"ReyR","ReyR");
    addPieza(TipoPieza::Bishop, ColorPieza::Black,0,5,"AlfilR","AlfilR");
    addPieza(TipoPieza::Knight, ColorPieza::Black,0,6,"CaballoR","CaballoR");
    addPieza(TipoPieza::Rook, ColorPieza::Black,0,7,"TorreR","TorreR");

    // Flags por jugador
    ReglasFlags flagsBlanco, flagsNegro;

    // Interacción
    bool arrastrando=false; int idxSeleccionado=-1;
    sf::Vector2f difMouse; int origenF=-1, origenC=-1;
    ColorPieza turno = ColorPieza::White;
    vector<pair<int,int>> movimientosValidos;

    // ---------------- Temporizador ----------------
    sf::Clock relojTurno;
    int minutosBlanco = 10, segundosBlanco = 0;
    int minutosNegro   = 10, segundosNegro = 0;
    bool tiempoAgotado=false;


// ---------------- Texturas de números ----------------
map<int,sf::Texture> texNumerosClaro, texNumerosOscuro;

for(int i=0; i<=9; i++){
    sf::Texture tClaro, tOscuro;
    if(!cargarTxt(tClaro, "assets/images/" + to_string(i) + "_claro.png"))
        cerr << "Error cargando " << i << "_claro.png" << endl;
    if(!cargarTxt(tOscuro, "assets/images/" + to_string(i) + "_oscuro.png"))
        cerr << "Error cargando " << i << "_oscuro.png" << endl;
    texNumerosClaro[i] = tClaro;
    texNumerosOscuro[i] = tOscuro;
}

    sf::Texture texPuntoOscuro, texPuntoClaro, texMoscuro, texMclaro;
    if(!cargarTxt(texPuntoOscuro,"assets/images/puntos_oscuro.png")) return -1;
    if(!cargarTxt(texPuntoClaro,"assets/images/puntos_claro.png")) return -1;
    if(!cargarTxt(texMoscuro,"assets/images/m_oscuro.png")) return -1;
    if(!cargarTxt(texMclaro,"assets/images/m_claro.png")) return -1;

    // ---------------- Promoción ----------------
    bool mostrandoPromocion=false; int idxPeonPromocion=-1;
    sf::Sprite recuadroPromocion(tex["Escoge"]);
    sf::Vector2u szRec = tex["Escoge"].getSize();
    recuadroPromocion.setOrigin(szRec.x/2.f, szRec.y/2.f);
    recuadroPromocion.setPosition(1000/2.f,700/2.f);
    sf::Sprite btnRook, btnKnight, btnBishop, btnQueen;
    auto configurarBotonesPromocion = [&](ColorPieza color){
        btnRook.setTexture(tex[(color==ColorPieza::White)?"TorreB":"TorreR"]);
        btnKnight.setTexture(tex[(color==ColorPieza::White)?"CaballoB":"CaballoR"]);
        btnBishop.setTexture(tex[(color==ColorPieza::White)?"AlfilB":"AlfilR"]);
        btnQueen.setTexture(tex[(color==ColorPieza::White)?"DamaB":"DamaR"]);
        auto setScaleFor=[&](sf::Sprite& s){const sf::Texture* t=s.getTexture(); if(t){ s.setScale((float)TAM_CASILLA/t->getSize().x,(float)TAM_CASILLA/t->getSize().y); }};
        setScaleFor(btnRook); setScaleFor(btnKnight); setScaleFor(btnBishop); setScaleFor(btnQueen);
        sf::Vector2f center = recuadroPromocion.getPosition();
        float sep = 20.f, totalWidth = 4.f*TAM_CASILLA+3.f*sep;
        float startX = center.x-totalWidth/2.f, yCenter=center.y;
        btnRook.setPosition(startX+0*(TAM_CASILLA+sep),yCenter);
        btnKnight.setPosition(startX+1*(TAM_CASILLA+sep),yCenter);
        btnBishop.setPosition(startX+2*(TAM_CASILLA+sep),yCenter);
        btnQueen.setPosition(startX+3*(TAM_CASILLA+sep),yCenter);
    };
    auto aplicarPromocion=[&](TipoPieza nuevoTipo){
        if(idxPeonPromocion<0||idxPeonPromocion>=(int)piezas.size()) return;
        Pieza &peon = piezas[idxPeonPromocion];
        if(!peon.alive||peon.tipo!=TipoPieza::Pawn) return;
        peon.tipo = nuevoTipo;
        string texKey;
        if(nuevoTipo==TipoPieza::Rook) texKey=(peon.color==ColorPieza::White)?"TorreB":"TorreR";
        if(nuevoTipo==TipoPieza::Knight) texKey=(peon.color==ColorPieza::White)?"CaballoB":"CaballoR";
        if(nuevoTipo==TipoPieza::Bishop) texKey=(peon.color==ColorPieza::White)?"AlfilB":"AlfilR";
        if(nuevoTipo==TipoPieza::Queen) texKey=(peon.color==ColorPieza::White)?"DamaB":"DamaR";
        peon.sprite.setTexture(tex[texKey]);
        const sf::Texture* t=peon.sprite.getTexture();
        if(t){ peon.baseSx=(float)TAM_CASILLA/t->getSize().x; peon.baseSy=(float)TAM_CASILLA/t->getSize().y; peon.sprite.setScale(peon.baseSx,peon.baseSy);}
        centrarYescalar(peon.sprite,peon.fila,peon.col);
        mostrandoPromocion=false; idxPeonPromocion=-1;
    };

    // ---------------- Fin de juego ----------------
    bool finJuego=false;
    sf::Sprite spriteFin;

    // ---------------- Bucle principal ----------------
    while(window.isOpen()){
        sf::Event ev;
        while(window.pollEvent(ev)){
            if(ev.type==sf::Event::Closed) window.close();

            if(finJuego) continue;

// ---------------- Interacción ----------------
if(mostrandoPromocion){
    if(ev.type==sf::Event::MouseButtonPressed && ev.mouseButton.button==sf::Mouse::Left){
        sf::Vector2f mpos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if(btnRook.getGlobalBounds().contains(mpos)) aplicarPromocion(TipoPieza::Rook);
        else if(btnKnight.getGlobalBounds().contains(mpos)) aplicarPromocion(TipoPieza::Knight);
        else if(btnBishop.getGlobalBounds().contains(mpos)) aplicarPromocion(TipoPieza::Bishop);
        else if(btnQueen.getGlobalBounds().contains(mpos)) aplicarPromocion(TipoPieza::Queen);
    }
    continue;
}

// PRESIONAR
if(ev.type==sf::Event::MouseButtonPressed && ev.mouseButton.button==sf::Mouse::Left){
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    idxSeleccionado=-1;
    for(int i=(int)piezas.size()-1;i>=0;i--){
        if(!piezas[i].alive) continue;
        if(piezas[i].sprite.getGlobalBounds().contains(mouse)){
            if((turno==ColorPieza::White&&piezas[i].color!=ColorPieza::White)||(turno==ColorPieza::Black&&piezas[i].color!=ColorPieza::Black))
                idxSeleccionado=-1;
            else idxSeleccionado=i;
            break;
        }
    }
    if(idxSeleccionado!=-1){
        arrastrando=true;
        difMouse = mouse - piezas[idxSeleccionado].sprite.getPosition();
        origenF = piezas[idxSeleccionado].fila; origenC = piezas[idxSeleccionado].col;
        movimientosValidos.clear();
        for(int rf=0;rf<FILAS;rf++) for(int rc=0;rc<COLS;rc++)
            if(movimientoLegal(piezas,idxSeleccionado,rf,rc,flagsBlanco,flagsNegro))
                movimientosValidos.emplace_back(rf,rc);
        piezas[idxSeleccionado].sprite.setScale(piezas[idxSeleccionado].baseSx*1.15f,piezas[idxSeleccionado].baseSy*1.15f);
    }
}

// Regla 1: guardia
if(ev.type==sf::Event::KeyPressed && ev.key.code==sf::Keyboard::G && idxSeleccionado!=-1){
    ReglasFlags &flags=(turno==ColorPieza::White)?flagsBlanco:flagsNegro;
    if(!flags.guardiaUsado){ flags.guardiaUsado=true; flags.guardiaIdx=idxSeleccionado; piezas[idxSeleccionado].protegido=true; flags.proteccionActiva=true; flags.proteccionTurnoDe=turno;}
}

// SOLTAR
if(ev.type==sf::Event::MouseButtonReleased && ev.mouseButton.button==sf::Mouse::Left && arrastrando && idxSeleccionado!=-1){
    arrastrando=false;
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    auto [dstF,dstC]=casillaMasCercana(mouse.x,mouse.y);
    sf::Vector2f centro=centroCasilla(dstF,dstC);
    float dist = hypotf(mouse.x-centro.x, mouse.y-centro.y);
    bool dentroRadio=(dist<=RADIO_ACEPTACION);
    bool legal=movimientoLegal(piezas,idxSeleccionado,dstF,dstC,flagsBlanco,flagsNegro);
    piezas[idxSeleccionado].sprite.setScale(piezas[idxSeleccionado].baseSx,piezas[idxSeleccionado].baseSy);

    if(dentroRadio && legal){
        if(!dejaReyEnJaqueSimulado(piezas,idxSeleccionado,dstF,dstC,flagsBlanco,flagsNegro)){
            if(tableroLogico[dstF][dstC]!=-1){
                int victim=tableroLogico[dstF][dstC];
                const ReglasFlags& flagsOponente = (piezas[idxSeleccionado].color==ColorPieza::White)? flagsNegro:flagsBlanco;
                if(!(flagsOponente.proteccionActiva && flagsOponente.guardiaIdx==victim)){
                    if(victim>=0 && piezas[victim].alive && piezas[victim].color!=piezas[idxSeleccionado].color){
                        piezas[victim].animandoCaptura=true;
                        piezas[victim].animClock.restart();
                    }
                }
            }
            int victim = tableroLogico[dstF][dstC];
           if(victim>=0 && piezas[victim].alive && piezas[victim].color!=piezas[idxSeleccionado].color){
                piezas[victim].alive=false;
                agregarPiezaCapturada(piezas[victim]);
                sonidopieza.play();  
                } else {
                 sonidopieza.play();  
            }


            bool fueEnroque=false;
            int moveCols = abs(dstC-origenC);

            if(piezas[idxSeleccionado].tipo==TipoPieza::King && (moveCols==2 || moveCols==3)){
                int dir=(dstC-origenC)>0?1:-1;
                int rookCol=(dir>0)?7:0;
                int rookIdx=tableroLogico[origenF][rookCol];
                if(rookIdx!=-1){
                    if(dentroTablero(origenF,origenC)) tableroLogico[origenF][origenC]=-1;
                    piezas[idxSeleccionado].fila=dstF; piezas[idxSeleccionado].col=dstC;
                    centrarYescalar(piezas[idxSeleccionado].sprite,dstF,dstC);
                    tableroLogico[dstF][dstC]=idxSeleccionado;
                    piezas[idxSeleccionado].hasMoved=true;
                    int newRookCol=(moveCols==2)?(origenC+dir):(origenC+2*dir);
                    if(dentroTablero(origenF,rookCol)) tableroLogico[origenF][rookCol]=-1;
                    piezas[rookIdx].fila=origenF; piezas[rookIdx].col=newRookCol;
                    centrarYescalar(piezas[rookIdx].sprite,piezas[rookIdx].fila,piezas[rookIdx].col);
                    tableroLogico[origenF][newRookCol]=rookIdx;
                    piezas[rookIdx].hasMoved=true;
                    fueEnroque=true;
                    if(moveCols==3){ ReglasFlags &f=(turno==ColorPieza::White)?flagsBlanco:flagsNegro; f.enroque3Usado=true; }
                }
            }

            if(!fueEnroque){
                if(dentroTablero(origenF,origenC)) tableroLogico[origenF][origenC]=-1;
                piezas[idxSeleccionado].fila=dstF; piezas[idxSeleccionado].col=dstC;
                centrarYescalar(piezas[idxSeleccionado].sprite,dstF,dstC);
                tableroLogico[dstF][dstC]=idxSeleccionado;
                piezas[idxSeleccionado].hasMoved=true;
            }

            if(piezas[idxSeleccionado].tipo==TipoPieza::Pawn){
                if((piezas[idxSeleccionado].color==ColorPieza::White && dstF==0) ||
                   (piezas[idxSeleccionado].color==ColorPieza::Black && dstF==7)){
                    mostrandoPromocion=true; idxPeonPromocion=idxSeleccionado;
                    configurarBotonesPromocion(turno);
                }
            }

            turno=(turno==ColorPieza::White)?ColorPieza::Black:ColorPieza::White;

            if(!finJuego){
    if(esJaqueMate(piezas, ColorPieza::White, flagsBlanco, flagsNegro)) {
        spriteFin.setTexture(tex["GanadorJ2"]); finJuego=true;
    }
    else if(esJaqueMate(piezas, ColorPieza::Black, flagsBlanco, flagsNegro)) {
        spriteFin.setTexture(tex["GanadorJ1"]); finJuego=true;
    }
    else if(sinMovimientosLegales(piezas, turno, flagsBlanco, flagsNegro)) {
        spriteFin.setTexture(tex["Empate"]); finJuego=true;
    }
    // Aquí puedes agregar tu lógica de tiempo agotado y fichas capturadas
}

        }
    } else {
        centrarYescalar(piezas[idxSeleccionado].sprite, origenF, origenC);
      }
    idxSeleccionado=-1;
}

        }
// ---------------- Actualizar temporizador ----------------
if(!finJuego){
    int* minPtr = (turno==ColorPieza::White)? &minutosBlanco : &minutosNegro;
    int* segPtr = (turno==ColorPieza::White)? &segundosBlanco : &segundosNegro;
    if(relojTurno.getElapsedTime().asSeconds()>=1.0f){
        if(*segPtr>0) (*segPtr)--;
        else{ if(*minPtr>0){ (*minPtr)--; *segPtr=59; } else tiempoAgotado=true; }
        relojTurno.restart();
    }
}

// ---------------- Comprobar fin de juego ----------------
bool jaqueMateBlanco = esJaqueMate(piezas, ColorPieza::White, flagsBlanco, flagsNegro);
bool jaqueMateNegro = esJaqueMate(piezas, ColorPieza::Black, flagsBlanco, flagsNegro);
bool noMovBlanco = sinMovimientosLegales(piezas, ColorPieza::White, flagsBlanco, flagsNegro);
bool noMovNegro = sinMovimientosLegales(piezas, ColorPieza::Black, flagsBlanco, flagsNegro);

if(!finJuego){
    if(jaqueMateBlanco) { finJuego=true; spriteFin.setTexture(tex["GanadorJ2"]); }
    else if(jaqueMateNegro) { finJuego=true; spriteFin.setTexture(tex["GanadorJ1"]); }
    else if(noMovBlanco && turno==ColorPieza::White) { finJuego=true; spriteFin.setTexture(tex["Empate"]); }
    else if(noMovNegro && turno==ColorPieza::Black) { finJuego=true; spriteFin.setTexture(tex["Empate"]); }
    else if(tiempoAgotado){
        int piezasBlancas=contarPiezas(piezas,ColorPieza::White);
        int piezasNegras=contarPiezas(piezas,ColorPieza::Black);
        if(piezasBlancas>piezasNegras) spriteFin.setTexture(tex["GanadorJ1"]);
        else if(piezasNegras>piezasBlancas) spriteFin.setTexture(tex["GanadorJ2"]);
        else spriteFin.setTexture(tex["Empate"]);
        finJuego=true;
    }

    if(finJuego){
        sf::Vector2u sz = spriteFin.getTexture()->getSize();
        spriteFin.setOrigin(sz.x/2.f, sz.y/2.f);
        spriteFin.setPosition(1000/2.f,700/2.f);
    }
}

window.clear();
window.draw(fondo);
window.draw(tablero);

// Dibujar piezas vivas
for(auto &p:piezas) 
    if(p.alive) window.draw(p.sprite);

// ---------------- NUEVO: Dibujar piezas capturadas ----------------
for(auto &s: piezasComidasBlanco) window.draw(s.sprite);
for(auto &s: piezasComidasNegro) window.draw(s.sprite);

// ---------------- Dibujar tiempo con coordenadas fijas ----------------
int min = (turno==ColorPieza::White) ? minutosBlanco : minutosNegro;
int seg = (turno==ColorPieza::White) ? segundosBlanco : segundosNegro;

auto &texNum = (turno==ColorPieza::White) ? texNumerosClaro : texNumerosOscuro;
auto &texPt  = (turno==ColorPieza::White) ? texPuntoClaro : texPuntoOscuro;
auto &texM   = (turno==ColorPieza::White) ? texMclaro : texMoscuro;

if(turno == ColorPieza::White){
    // Coordenadas CLARO
    sf::Sprite m1(texNum[min/10]); m1.setPosition(212.f, 16.f); m1.setScale(0.5f,0.5f); window.draw(m1);
    sf::Sprite m2(texNum[min%10]); m2.setPosition(246.f, 16.f); m2.setScale(0.5f,0.5f); window.draw(m2);
    sf::Sprite m(texM);            m.setPosition(285.f, 56.f);  m.setScale(0.5f,0.5f);  window.draw(m);
    sf::Sprite pt(texPt);          pt.setPosition(202.f, 85.f); pt.setScale(0.5f,0.5f); window.draw(pt);
    sf::Sprite s1(texNum[seg/10]); s1.setPosition(226.f, 91.f); s1.setScale(0.5f,0.5f); window.draw(s1);
    sf::Sprite s2(texNum[seg%10]); s2.setPosition(264.f, 91.f); s2.setScale(0.5f,0.5f); window.draw(s2);
} else {
    // Coordenadas OSCURO
    sf::Sprite m1(texNum[min/10]); m1.setPosition(77.f, 16.f);  m1.setScale(0.5f,0.5f); window.draw(m1);
    sf::Sprite m2(texNum[min%10]); m2.setPosition(114.f, 16.f); m2.setScale(0.5f,0.5f); window.draw(m2);
    sf::Sprite m(texM);            m.setPosition(158.f, 56.f);  m.setScale(0.5f,0.5f);  window.draw(m);
    sf::Sprite pt(texPt);          pt.setPosition(72.f, 85.f);  pt.setScale(0.5f,0.5f); window.draw(pt);
    sf::Sprite s1(texNum[seg/10]); s1.setPosition(97.f, 91.f);  s1.setScale(0.5f,0.5f); window.draw(s1);
    sf::Sprite s2(texNum[seg%10]); s2.setPosition(138.f, 91.f); s2.setScale(0.5f,0.5f); window.draw(s2);
}

if(finJuego) window.draw(spriteFin);

if(mostrandoPromocion){
    window.draw(recuadroPromocion);
    window.draw(btnRook); window.draw(btnKnight); window.draw(btnBishop); window.draw(btnQueen);
}

window.display();
    }

    return 0;
}

