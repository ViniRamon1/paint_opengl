#ifndef TRANSFORMACOES_H
#define TRANSFORMACOES_H
#include "objetos.h"

// Tipos de objetos
#define TIPO_PONTO    0
#define TIPO_LINHA    1
#define TIPO_POLIGONO 2

// Tipos de eixo para reflexão
#define EIXO_X 0
#define EIXO_Y 1

// ==================== SISTEMA DE MATRIZ COMPOSTA ====================

// Stack de matrizes para transformações hierárquicas
#define MAX_MATRIX_STACK 32

typedef struct {
    float matrizes[MAX_MATRIX_STACK][3][3];
    int topo;
} MatrixStack;

// ==================== FUNÇÕES BÁSICAS DE MATRIZ ====================
void matrizIdentidade(float M[3][3]);
void copiarMatriz(float origem[3][3], float destino[3][3]);
void multiplicarMatrizes(float A[3][3], float B[3][3], float R[3][3]);
int matrizesIguais(float A[3][3], float B[3][3], float tolerancia);
void imprimirMatriz(float M[3][3]);

// ==================== MATRIZES DE TRANSFORMAÇÃO ====================
void matrizTranslacao(float M[3][3], float dx, float dy);
void matrizRotacao(float M[3][3], float angulo);
void matrizEscala(float M[3][3], float sx, float sy);
void matrizReflexaoX(float M[3][3]);
void matrizReflexaoY(float M[3][3]);
void matrizCisalhamento(float M[3][3], float shx, float shy);

// Transformações compostas específicas
void matrizRotacaoEmTorno(float M[3][3], float angulo, float cx, float cy);
void matrizEscalaEmTorno(float M[3][3], float sx, float sy, float cx, float cy);

// ==================== GERENCIAMENTO DO MATRIX STACK ====================
void inicializarMatrixStack(MatrixStack* stack);
void pushMatrix(MatrixStack* stack);
void popMatrix(MatrixStack* stack);
void loadIdentity(MatrixStack* stack);
float (*getMatrixAtual(MatrixStack* stack))[3];

// Composição de transformações no stack
void composeMatrix(MatrixStack* stack, float M[3][3]);
void composeTranslacao(MatrixStack* stack, float dx, float dy);
void composeRotacao(MatrixStack* stack, float angulo);
void composeEscala(MatrixStack* stack, float sx, float sy);
void composeReflexaoX(MatrixStack* stack);
void composeReflexaoY(MatrixStack* stack);
void composeCisalhamento(MatrixStack* stack, float shx, float shy);
void composeRotacaoEmTorno(MatrixStack* stack, float angulo, float cx, float cy);
void composeEscalaEmTorno(MatrixStack* stack, float sx, float sy, float cx, float cy);

// ==================== APLICAÇÃO DE TRANSFORMAÇÕES ====================
void aplicarMatrizPonto(Ponto* p, float M[3][3]);
void aplicarMatrizListaPontos(Ponto* lista, float M[3][3]);
void aplicarMatrizObjeto(void* obj, int tipo, float M[3][3]);
void aplicarMatrixStack(MatrixStack* stack, void* obj, int tipo);

// ==================== OPERAÇÕES EM OBJETOS (REFORMULADAS) ====================
// Todas agora usam matriz composta internamente
void transladarObjeto(void* obj, int tipo, float dx, float dy);
void rotacionarObjeto(void* obj, int tipo, float angulo);
void escalarObjeto(void* obj, int tipo, float sx, float sy);
void refletirObjeto(void* obj, int tipo, int eixo);
void cisalharObjeto(void* obj, int tipo, float shx, float shy);

// Novas operações avançadas
void rotacionarObjetoEmTorno(void* obj, int tipo, float angulo, float cx, float cy);
void escalarObjetoEmTorno(void* obj, int tipo, float sx, float sy, float cx, float cy);
void transformarObjetoCompleto(void* obj, int tipo, float dx, float dy, float angulo, float sx, float sy);

// ==================== UTILITÁRIOS ====================
void calcularCentro(void* objeto, int tipo, float* cx, float* cy);
int calcularInversa(float M[3][3], float inversa[3][3]);
void decomporMatriz(float M[3][3], float* tx, float* ty, float* angulo, float* sx, float* sy);

// ==================== MACROS DE CONVENIÊNCIA ====================
#define MATRIX_PUSH(stack) pushMatrix(stack)
#define MATRIX_POP(stack) popMatrix(stack)
#define MATRIX_LOAD_IDENTITY(stack) loadIdentity(stack)
#define TRANSLATE(stack, dx, dy) composeTranslacao(stack, dx, dy)
#define ROTATE(stack, angle) composeRotacao(stack, angle)
#define SCALE(stack, sx, sy) composeEscala(stack, sx, sy)
#define ROTATE_AROUND(stack, angle, cx, cy) composeRotacaoEmTorno(stack, angle, cx, cy)
#define SCALE_AROUND(stack, sx, sy, cx, cy) composeEscalaEmTorno(stack, sx, sy, cx, cy)
#define REFLECT_X(stack) composeReflexaoX(stack)
#define REFLECT_Y(stack) composeReflexaoY(stack)
#define SHEAR(stack, shx, shy) composeCisalhamento(stack, shx, shy)

#endif
