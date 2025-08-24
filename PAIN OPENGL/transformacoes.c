#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "transformacoes.h"
#include "objetos.h"

#define EPSILON 1e-6
#define M_PI 3.14159265358979323846

// ==================== FUNÇÕES BÁSICAS DE MATRIZ ====================

void matrizIdentidade(float M[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            M[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

void copiarMatriz(float origem[3][3], float destino[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            destino[i][j] = origem[i][j];
        }
    }
}

void multiplicarMatrizes(float A[3][3], float B[3][3], float R[3][3]) {
    float temp[3][3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            temp[i][j] = 0.0f;
            for (int k = 0; k < 3; k++) {
                temp[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    copiarMatriz(temp, R);
}

int matrizesIguais(float A[3][3], float B[3][3], float tolerancia) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (fabsf(A[i][j] - B[i][j]) > tolerancia) {
                return 0;
            }
        }
    }
    return 1;
}

void imprimirMatriz(float M[3][3]) {
    printf("Matriz 3x3:\n");
    for (int i = 0; i < 3; i++) {
        printf("  [%7.3f %7.3f %7.3f]\n", M[i][0], M[i][1], M[i][2]);
    }
}

// ==================== MATRIZES DE TRANSFORMAÇÃO ====================

void matrizTranslacao(float M[3][3], float dx, float dy) {
    matrizIdentidade(M);
    M[0][2] = dx;
    M[1][2] = dy;
}

void matrizRotacao(float M[3][3], float angulo) {
    matrizIdentidade(M);
    float rad = angulo * M_PI / 180.0f;
    float cosA = cosf(rad);
    float sinA = sinf(rad);

    M[0][0] = cosA;  M[0][1] = -sinA;
    M[1][0] = sinA;  M[1][1] = cosA;
}

void matrizEscala(float M[3][3], float sx, float sy) {
    matrizIdentidade(M);
    M[0][0] = sx;
    M[1][1] = sy;
}

void matrizReflexaoX(float M[3][3]) {
    matrizIdentidade(M);
    M[1][1] = -1.0f;
}

void matrizReflexaoY(float M[3][3]) {
    matrizIdentidade(M);
    M[0][0] = -1.0f;
}

void matrizCisalhamento(float M[3][3], float shx, float shy) {
    matrizIdentidade(M);
    M[0][1] = shx;
    M[1][0] = shy;
}

void matrizRotacaoEmTorno(float M[3][3], float angulo, float cx, float cy) {
    float T1[3][3], R[3][3], T2[3][3], temp[3][3];

    // T(-cx, -cy) * R(angulo) * T(cx, cy)
    matrizTranslacao(T1, -cx, -cy);
    matrizRotacao(R, angulo);
    matrizTranslacao(T2, cx, cy);

    multiplicarMatrizes(R, T1, temp);
    multiplicarMatrizes(T2, temp, M);
}

void matrizEscalaEmTorno(float M[3][3], float sx, float sy, float cx, float cy) {
    float T1[3][3], S[3][3], T2[3][3], temp[3][3];

    // T(-cx, -cy) * S(sx, sy) * T(cx, cy)
    matrizTranslacao(T1, -cx, -cy);
    matrizEscala(S, sx, sy);
    matrizTranslacao(T2, cx, cy);

    multiplicarMatrizes(S, T1, temp);
    multiplicarMatrizes(T2, temp, M);
}

// ==================== GERENCIAMENTO DO MATRIX STACK ====================

void inicializarMatrixStack(MatrixStack* stack) {
    if (!stack) return;
    stack->topo = 0;
    matrizIdentidade(stack->matrizes[0]);
}

void pushMatrix(MatrixStack* stack) {
    if (!stack || stack->topo >= MAX_MATRIX_STACK - 1) {
        printf("ERRO: Matrix stack overflow!\n");
        return;
    }

    stack->topo++;
    copiarMatriz(stack->matrizes[stack->topo - 1], stack->matrizes[stack->topo]);
}

void popMatrix(MatrixStack* stack) {
    if (!stack || stack->topo <= 0) {
        printf("ERRO: Matrix stack underflow!\n");
        return;
    }

    stack->topo--;
}

void loadIdentity(MatrixStack* stack) {
    if (!stack) return;
    matrizIdentidade(stack->matrizes[stack->topo]);
}

float (*getMatrixAtual(MatrixStack* stack))[3] {
    if (!stack) return NULL;
    return stack->matrizes[stack->topo];
}

void composeMatrix(MatrixStack* stack, float M[3][3]) {
    if (!stack) return;

    float atual[3][3];
    copiarMatriz(stack->matrizes[stack->topo], atual);
    multiplicarMatrizes(M, atual, stack->matrizes[stack->topo]);
}

void composeTranslacao(MatrixStack* stack, float dx, float dy) {
    float T[3][3];
    matrizTranslacao(T, dx, dy);
    composeMatrix(stack, T);
}

void composeRotacao(MatrixStack* stack, float angulo) {
    float R[3][3];
    matrizRotacao(R, angulo);
    composeMatrix(stack, R);
}

void composeEscala(MatrixStack* stack, float sx, float sy) {
    float S[3][3];
    matrizEscala(S, sx, sy);
    composeMatrix(stack, S);
}

void composeReflexaoX(MatrixStack* stack) {
    float Rx[3][3];
    matrizReflexaoX(Rx);
    composeMatrix(stack, Rx);
}

void composeReflexaoY(MatrixStack* stack) {
    float Ry[3][3];
    matrizReflexaoY(Ry);
    composeMatrix(stack, Ry);
}

void composeCisalhamento(MatrixStack* stack, float shx, float shy) {
    float Sh[3][3];
    matrizCisalhamento(Sh, shx, shy);
    composeMatrix(stack, Sh);
}

void composeRotacaoEmTorno(MatrixStack* stack, float angulo, float cx, float cy) {
    float R[3][3];
    matrizRotacaoEmTorno(R, angulo, cx, cy);
    composeMatrix(stack, R);
}

void composeEscalaEmTorno(MatrixStack* stack, float sx, float sy, float cx, float cy) {
    float S[3][3];
    matrizEscalaEmTorno(S, sx, sy, cx, cy);
    composeMatrix(stack, S);
}

// ==================== APLICAÇÃO DE TRANSFORMAÇÕES ====================

void aplicarMatrizPonto(Ponto* p, float M[3][3]) {
    if (!p) return;

    float x = p->x;
    float y = p->y;

    p->x = M[0][0] * x + M[0][1] * y + M[0][2];
    p->y = M[1][0] * x + M[1][1] * y + M[1][2];
}

void aplicarMatrizListaPontos(Ponto* lista, float M[3][3]) {
    if (!lista) return;

    Ponto* primeiro = lista;
    Ponto* atual = lista;
    int contador = 0;
    const int MAX_PONTOS = 10000;

    // Detecta se é lista circular ou linear
    if (lista->prox == lista) {
        // Apenas um ponto
        aplicarMatrizPonto(lista, M);
        return;
    }

    do {
        aplicarMatrizPonto(atual, M);
        atual = atual->prox;
        contador++;

        if (contador > MAX_PONTOS) {
            printf("AVISO: Loop muito longo na lista de pontos, interrompendo...\n");
            break;
        }

    } while (atual && atual != primeiro);
}

void aplicarMatrizObjeto(void* obj, int tipo, float M[3][3]) {
    if (!obj) return;

    switch (tipo) {
        case TIPO_PONTO: {
            Ponto* p = (Ponto*)obj;
            aplicarMatrizPonto(p, M);
            break;
        }

        case TIPO_LINHA: {
            Linha* l = (Linha*)obj;
            Ponto p1 = {l->x1, l->y1, 0, NULL};
            Ponto p2 = {l->x2, l->y2, 0, NULL};

            aplicarMatrizPonto(&p1, M);
            aplicarMatrizPonto(&p2, M);

            l->x1 = p1.x; l->y1 = p1.y;
            l->x2 = p2.x; l->y2 = p2.y;
            break;
        }

        case TIPO_POLIGONO: {
            Poligono* pol = (Poligono*)obj;
            aplicarMatrizListaPontos(pol->pontos, M);
            break;
        }

        default:
            printf("ERRO: Tipo de objeto desconhecido: %d\n", tipo);
            break;
    }
}

void aplicarMatrixStack(MatrixStack* stack, void* obj, int tipo) {
    if (!stack) return;
    aplicarMatrizObjeto(obj, tipo, stack->matrizes[stack->topo]);
}

// ==================== OPERAÇÕES EM OBJETOS (REFORMULADAS) ====================

void transladarObjeto(void* obj, int tipo, float dx, float dy) {
    float T[3][3];
    matrizTranslacao(T, dx, dy);
    aplicarMatrizObjeto(obj, tipo, T);
}

void rotacionarObjeto(void* obj, int tipo, float angulo) {
    if (!obj) return;

    float cx, cy;
    calcularCentro(obj, tipo, &cx, &cy);

    float R[3][3];
    matrizRotacaoEmTorno(R, angulo, cx, cy);
    aplicarMatrizObjeto(obj, tipo, R);

    printf("Rotacionando %.1f° em torno do centro (%.2f, %.2f)\n", angulo, cx, cy);
}

void escalarObjeto(void* obj, int tipo, float sx, float sy) {
    if (!obj) return;

    float cx, cy;
    calcularCentro(obj, tipo, &cx, &cy);

    float S[3][3];
    matrizEscalaEmTorno(S, sx, sy, cx, cy);
    aplicarMatrizObjeto(obj, tipo, S);
}

void refletirObjeto(void* obj, int tipo, int eixo) {
    float R[3][3];

    if (eixo == EIXO_X) {
        matrizReflexaoX(R);
    } else {
        matrizReflexaoY(R);
    }

    aplicarMatrizObjeto(obj, tipo, R);
}

void cisalharObjeto(void* obj, int tipo, float shx, float shy) {
    float Sh[3][3];
    matrizCisalhamento(Sh, shx, shy);
    aplicarMatrizObjeto(obj, tipo, Sh);
}

void rotacionarObjetoEmTorno(void* obj, int tipo, float angulo, float cx, float cy) {
    float R[3][3];
    matrizRotacaoEmTorno(R, angulo, cx, cy);
    aplicarMatrizObjeto(obj, tipo, R);
}

void escalarObjetoEmTorno(void* obj, int tipo, float sx, float sy, float cx, float cy) {
    float S[3][3];
    matrizEscalaEmTorno(S, sx, sy, cx, cy);
    aplicarMatrizObjeto(obj, tipo, S);
}

void transformarObjetoCompleto(void* obj, int tipo, float dx, float dy, float angulo, float sx, float sy) {
    if (!obj) return;

    // Cria matriz composta: T * R * S
    MatrixStack stack;
    inicializarMatrixStack(&stack);

    SCALE(&stack, sx, sy);
    ROTATE(&stack, angulo);
    TRANSLATE(&stack, dx, dy);

    aplicarMatrixStack(&stack, obj, tipo);
}

// ==================== UTILITÁRIOS ====================

void calcularCentro(void* objeto, int tipo, float* cx, float* cy) {
    if (!cx || !cy) return;

    *cx = 0.0f;
    *cy = 0.0f;

    if (!objeto) return;

    switch (tipo) {
        case TIPO_PONTO: {
            Ponto* p = (Ponto*)objeto;
            *cx = p->x;
            *cy = p->y;
            break;
        }

        case TIPO_LINHA: {
            Linha* l = (Linha*)objeto;
            *cx = (l->x1 + l->x2) / 2.0f;
            *cy = (l->y1 + l->y2) / 2.0f;
            break;
        }

        case TIPO_POLIGONO: {
            Poligono* pol = (Poligono*)objeto;
            if (!pol->pontos) break;

            Ponto* primeiro = pol->pontos;
            Ponto* atual = pol->pontos;
            int n = 0;
            int contador = 0;
            const int MAX_PONTOS = 10000;

            do {
                *cx += atual->x;
                *cy += atual->y;
                n++;
                atual = atual->prox;
                contador++;

                if (contador > MAX_PONTOS) {
                    printf("AVISO: Loop muito longo no cálculo do centro\n");
                    break;
                }

            } while (atual && atual != primeiro);

            if (n > 0) {
                *cx /= n;
                *cy /= n;
            }
            break;
        }

        default:
            printf("ERRO: Tipo desconhecido no cálculo do centro: %d\n", tipo);
            break;
    }
}

int calcularInversa(float M[3][3], float inversa[3][3]) {
    // Calcula determinante
    float det = M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]) -
                M[0][1] * (M[1][0] * M[2][2] - M[1][2] * M[2][0]) +
                M[0][2] * (M[1][0] * M[2][1] - M[1][1] * M[2][0]);

    if (fabsf(det) < EPSILON) {
        printf("ERRO: Matriz singular, não possui inversa\n");
        return 0;
    }

    // Calcula matriz de cofatores
    float cofatores[3][3];
    cofatores[0][0] = M[1][1] * M[2][2] - M[1][2] * M[2][1];
    cofatores[0][1] = -(M[1][0] * M[2][2] - M[1][2] * M[2][0]);
    cofatores[0][2] = M[1][0] * M[2][1] - M[1][1] * M[2][0];

    cofatores[1][0] = -(M[0][1] * M[2][2] - M[0][2] * M[2][1]);
    cofatores[1][1] = M[0][0] * M[2][2] - M[0][2] * M[2][0];
    cofatores[1][2] = -(M[0][0] * M[2][1] - M[0][1] * M[2][0]);

    cofatores[2][0] = M[0][1] * M[1][2] - M[0][2] * M[1][1];
    cofatores[2][1] = -(M[0][0] * M[1][2] - M[0][2] * M[1][0]);
    cofatores[2][2] = M[0][0] * M[1][1] - M[0][1] * M[1][0];

    // Transpõe e divide pelo determinante
    float invDet = 1.0f / det;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            inversa[i][j] = cofatores[j][i] * invDet; // Transposta
        }
    }

    return 1;
}

void decomporMatriz(float M[3][3], float* tx, float* ty, float* angulo, float* sx, float* sy) {
    if (!tx || !ty || !angulo || !sx || !sy) return;

    // Extrai translação
    *tx = M[0][2];
    *ty = M[1][2];

    // Extrai escala e rotação
    float a = M[0][0];
    float b = M[0][1];
    float c = M[1][0];
    float d = M[1][1];

    // Escala
    *sx = sqrtf(a * a + c * c);
    *sy = sqrtf(b * b + d * d);

    // Ajusta sinal da escala
    if ((a * d - b * c) < 0) {
        *sy = -*sy;
    }

    // Ângulo de rotação
    *angulo = atan2f(c, a) * 180.0f / M_PI;

    printf("Decomposição: T(%.2f,%.2f) R(%.2f°) S(%.2f,%.2f)\n",
           *tx, *ty, *angulo, *sx, *sy);
}
