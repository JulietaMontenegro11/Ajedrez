#include "graficos.hpp"

void centrarYescalar(sf::Sprite &s, int fila, int col){
    sf::FloatRect b = s.getLocalBounds();
    s.setOrigin(b.width/2.f, b.height/2.f);
    sf::Vector2f centro = centroCasilla(fila,col);
    s.setPosition(centro);
}
