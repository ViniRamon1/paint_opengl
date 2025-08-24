#ifndef SELECAO_H
#define SELECAO_H

#include "objetos.h"

// =================== FUN��ES DE SELE��O ORIGINAIS ===================
Ponto* selecionarPonto(float x, float y, float tolerancia);
Linha* selecionarLinha(float x, float y, float tolerancia);
Poligono* selecionarPoligono(float x, float y);

// =================== FUN��ES DE SELE��O ALTERNATIVAS ===================
Poligono* selecionarPoligonoSimples(float x, float y, float tolerancia);
Poligono* selecionarPoligonoSeguro(float x, float y, float tolerancia);

#endif
