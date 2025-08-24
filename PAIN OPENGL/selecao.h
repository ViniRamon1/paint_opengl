#ifndef SELECAO_H
#define SELECAO_H

#include "objetos.h"

// =================== FUN합ES DE SELE플O ORIGINAIS ===================
Ponto* selecionarPonto(float x, float y, float tolerancia);
Linha* selecionarLinha(float x, float y, float tolerancia);
Poligono* selecionarPoligono(float x, float y);

// =================== FUN합ES DE SELE플O ALTERNATIVAS ===================
Poligono* selecionarPoligonoSimples(float x, float y, float tolerancia);
Poligono* selecionarPoligonoSeguro(float x, float y, float tolerancia);

#endif
