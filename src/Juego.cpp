#include "tipos.hpp"
#include "helpers.hpp"
#include "movimientos.hpp"
#include "jaque.hpp"
#include "graficos.hpp"

int main(){
    sf::RenderWindow window(sf::VideoMode(1000,700), "Ajedrez SFML - Jaque & Jaque Mate (con enroque + reglas especiales)");
    window.setFramerateLimit(60);

    // ---------------- Cargar texturas ----------------
    map<string,sf::Texture> tex;
    vector<pair<string,string>> lista = {
        {"PeonB","assets/images/PeonB.png"},{"PeonR","assets/images/PeonR.png"},
        {"TorreB","assets/images/TorreB.png"},{"TorreR","assets/images/TorreR.png"},
        {"CaballoB","assets/images/CaballoB.png"},{"CaballoR","assets/images/CaballoR.png"},
        {"AlfilB","assets/images/AlfilB.png"},{"AlfilR","assets/images/AlfilR.png"},
        {"DamaB","assets/images/DamaB.png"},{"DamaR","assets/images/DamaR.png"},
        {"ReyB","assets/images/ReyB.png"},{"ReyR","assets/images/ReyR.png"},
        {"Fondo","assets/images/Fondo.png"}, {"Tablero","assets/images/Tablero.png"},
        {"Escoge","assets/images/Escoge.png"}
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
    int minutosBlanco=5, segundosBlanco=0, minutosNegro=5, segundosNegro=0;
    bool tiempoAgotado=false;

    // Textura del reloj
    sf::Texture texReloj; 
    if(!cargarTxt(texReloj,"assets/reloj/Reloj 1.png")) return -1;
    sf::Sprite relojSpr(texReloj); 
    relojSpr.setPosition(800,50);

    // Texturas de números (0-9) oscuros
    map<int,sf::Texture> texNumerosOscuro;
    for(int i=0;i<=9;i++){
        sf::Texture t; 
        string fname = "assets/reloj/"+to_string(i)+" oscuro.png";
        if(!cargarTxt(t,fname)) return -1;
        texNumerosOscuro[i] = t;
    }

    // Textura "AGOTADO"
    sf::Texture texAgotado;
    if(!cargarTxt(texAgotado,"assets/imagenes/Agotado.png")) return -1;

    auto actualizarTemporizador = [&](ColorPieza turnoActual){
        if(tiempoAgotado) return;
        int* minPtr = (turnoActual==ColorPieza::White)? &minutosBlanco : &minutosNegro;
        int* segPtr = (turnoActual==ColorPieza::White)? &segundosBlanco : &segundosNegro;
        if(relojTurno.getElapsedTime().asSeconds()>=1.0f){
            if(*segPtr>0) (*segPtr)--;
            else{ if(*minPtr>0){ (*minPtr)--; *segPtr=59; } else tiempoAgotado=true; }
            relojTurno.restart();
        }
    };

    // ---------------- Promoción ----------------
    bool mostrandoPromocion=false; int idxPeonPromocion=-1;
    sf::Sprite recuadroPromocion(tex["Escoge"]);
    sf::Vector2u sz = tex["Escoge"].getSize();
    recuadroPromocion.setOrigin(sz.x/2.f, sz.y/2.f);
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

    // ---------------- Bucle principal ----------------
    while(window.isOpen()){
        sf::Event ev;
        while(window.pollEvent(ev)){
            if(ev.type==sf::Event::Closed) window.close();

            // Promoción
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
                    }
                } else {
                    centrarYescalar(piezas[idxSeleccionado].sprite, origenF, origenC);
                }
                idxSeleccionado=-1;
            }
        }

        actualizarTemporizador(turno);

        // ---------------- Dibujar ----------------
        window.clear();
        window.draw(fondo);
        window.draw(tablero);

        // Piezas
        for(auto &p:piezas){
            if(p.alive) window.draw(p.sprite);
        }

        // ---------------- Dibujar reloj y números ----------------
        window.draw(relojSpr);
        int min = (turno==ColorPieza::White)? minutosBlanco : minutosNegro;
        int seg = (turno==ColorPieza::White)? segundosBlanco : segundosNegro;

        // Referencias temporales a texturas
        sf::Texture &tMin1 = texNumerosOscuro[min/10];
        sf::Texture &tMin2 = texNumerosOscuro[min%10];
        sf::Texture &tSeg1 = texNumerosOscuro[seg/10];
        sf::Texture &tSeg2 = texNumerosOscuro[seg%10];

        sf::Sprite sMin1(tMin1); sMin1.setPosition(relojSpr.getPosition().x + 10, relojSpr.getPosition().y + 10);
        sf::Sprite sMin2(tMin2); sMin2.setPosition(relojSpr.getPosition().x + 35, relojSpr.getPosition().y + 10);
        sf::Sprite sSeg1(tSeg1); sSeg1.setPosition(relojSpr.getPosition().x + 80, relojSpr.getPosition().y + 10);
        sf::Sprite sSeg2(tSeg2); sSeg2.setPosition(relojSpr.getPosition().x + 105, relojSpr.getPosition().y + 10);

        window.draw(sMin1); window.draw(sMin2);
        window.draw(sSeg1); window.draw(sSeg2);

        if(tiempoAgotado){
            sf::Sprite agotadoSpr(texAgotado);
            sf::Vector2u sz = texAgotado.getSize();
            agotadoSpr.setOrigin(sz.x/2.f, sz.y/2.f);
            agotadoSpr.setPosition(1000/2.f,700/2.f);
            window.draw(agotadoSpr);
        }

        if(mostrandoPromocion){
            window.draw(recuadroPromocion);
            window.draw(btnRook); window.draw(btnKnight); window.draw(btnBishop); window.draw(btnQueen);
        }

        window.display();
    }

    return 0;
}
