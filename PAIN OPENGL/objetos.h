#ifndef OBJETOS_H
#define OBJETOS_H

// ==================== ESTRUTURAS ====================

typedef struct Ponto {
    float x, y;
    int selecionado;
    struct Ponto* prox;
} Ponto;

typedef struct Linha {
    float x1, y1, x2, y2;
    int selecionado;
    struct Linha* prox;
} Linha;

typedef struct Poligono {
    Ponto* pontos;          // Lista circular de pontos
    int selecionado;
    struct Poligono* prox;
} Poligono;

// ==================== LISTAS GLOBAIS ====================
extern Ponto* listaPontos;
extern Linha* listaLinhas;
extern Poligono* listaPoligonos;

// ==================== VARIÁVEIS PARA POLÍGONO EM CONSTRUÇÃO ====================
extern Poligono* poligonoAtual;  // Agora pode ser acessado pelo render.c

// ==================== FUNÇÕES DE CRIAÇÃO ====================
void adicionaPonto(float x, float y);
void adicionaLinha(float x1, float y1, float x2, float y2);
void adicionaPontoPoligono(float x, float y, float tolerancia);
void fechaPoligono();

// ==================== FUNÇÕES DE EXCLUSÃO ====================
void excluirSelecionado();

// ==================== FUNÇÕES DE EXCLUSÃO ====================
void excluirSelecionado();

// ==================== FUNÇÕES DE DEBUG ====================
void imprimirEstadoListas();
void imprimirEstadoListas();

#endif
