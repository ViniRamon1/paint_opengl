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

// ==================== VARI�VEIS PARA POL�GONO EM CONSTRU��O ====================
extern Poligono* poligonoAtual;  // Agora pode ser acessado pelo render.c

// ==================== FUN��ES DE CRIA��O ====================
void adicionaPonto(float x, float y);
void adicionaLinha(float x1, float y1, float x2, float y2);
void adicionaPontoPoligono(float x, float y, float tolerancia);
void fechaPoligono();

// ==================== FUN��ES DE EXCLUS�O ====================
void excluirSelecionado();

// ==================== FUN��ES DE EXCLUS�O ====================
void excluirSelecionado();

// ==================== FUN��ES DE DEBUG ====================
void imprimirEstadoListas();
void imprimirEstadoListas();

#endif
