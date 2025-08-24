// undo_system.c - VERSÃO CORRIGIDA
#include "undo_system.h"
#include "objetos.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

static TransformacaoUndo undoStack[MAX_UNDO_STACK];
static TransformacaoUndo redoStack[MAX_UNDO_STACK];
static int undoTopo = -1;
static int redoTopo = -1;

// Função para gerar ID único baseado no endereço
long gerarObjetoId(void* obj) {
    return (long)obj;
}

void salvarParaUndo(float matriz[3][3], void* obj, int tipo, const char* desc) {
    if (!obj) {
        printf("ERRO: Objeto nulo não pode ser salvo para undo\n");
        return;
    }

    // Limpa redo stack quando nova ação é feita
    redoTopo = -1;

    // Gerencia overflow do stack
    if (undoTopo >= MAX_UNDO_STACK - 1) {
        // Move todos os elementos uma posição para baixo
        for (int i = 0; i < MAX_UNDO_STACK - 1; i++) {
            undoStack[i] = undoStack[i + 1];
        }
        undoTopo = MAX_UNDO_STACK - 2;
    }

    undoTopo++;

    // Calcula e salva a inversa
    float inversa[3][3];
    if (calcularInversa(matriz, inversa)) {
        copiarMatriz(inversa, undoStack[undoTopo].matriz);
        undoStack[undoTopo].objeto = obj;
        undoStack[undoTopo].tipo = tipo;
        undoStack[undoTopo].objetoId = gerarObjetoId(obj);

        // Copia descrição com segurança
        if (desc) {
            strncpy(undoStack[undoTopo].descricao, desc, 63);
            undoStack[undoTopo].descricao[63] = '\0';
        } else {
            strcpy(undoStack[undoTopo].descricao, "Transformação");
        }

        printf("Undo salvo: %s (stack: %d)\n", undoStack[undoTopo].descricao, undoTopo + 1);
    } else {
        undoTopo--;
        printf("ERRO: Não foi possível calcular matriz inversa\n");
    }
}

int objetoValido(void* obj, long objetoId) {
    if (!obj) return 0;
    if (gerarObjetoId(obj) != objetoId) return 0;

    // Verifica se o objeto ainda existe nas listas globais
    // Pontos
    for (Ponto* p = listaPontos; p; p = p->prox) {
        if ((void*)p == obj) return 1;
    }

    // Linhas
    for (Linha* l = listaLinhas; l; l = l->prox) {
        if ((void*)l == obj) return 1;
    }

    // Polígonos
    for (Poligono* pol = listaPoligonos; pol; pol = pol->prox) {
        if ((void*)pol == obj) return 1;
    }

    return 0; // Objeto não encontrado
}

int desfazerTransformacao() {
    if (undoTopo < 0) {
        printf("Nada para desfazer\n");
        return 0;
    }

    TransformacaoUndo* undo = &undoStack[undoTopo];

    // Valida se objeto ainda existe
    if (!objetoValido(undo->objeto, undo->objetoId)) {
        printf("AVISO: Objeto não existe mais, removendo do histórico: %s\n", undo->descricao);
        undoTopo--;
        return desfazerTransformacao(); // Tenta o próximo
    }

    // Salva a transformação atual para redo ANTES de aplicar undo
    float matrizRedo[3][3];
    if (calcularInversa(undo->matriz, matrizRedo)) {
        // Move para redo stack
        if (redoTopo < MAX_UNDO_STACK - 1) {
            redoTopo++;
            redoStack[redoTopo] = *undo; // Copia toda a estrutura
            copiarMatriz(matrizRedo, redoStack[redoTopo].matriz);
        }
    }

    // Aplica o undo
    aplicarMatrizObjeto(undo->objeto, undo->tipo, undo->matriz);
    printf("Desfeito: %s\n", undo->descricao);

    undoTopo--;
    return 1;
}

int refazerTransformacao() {
    if (redoTopo < 0) {
        printf("Nada para refazer\n");
        return 0;
    }

    TransformacaoUndo* redo = &redoStack[redoTopo];

    if (!objetoValido(redo->objeto, redo->objetoId)) {
        printf("AVISO: Objeto não existe mais para refazer: %s\n", redo->descricao);
        redoTopo--;
        return refazerTransformacao(); // Tenta o próximo
    }

    aplicarMatrizObjeto(redo->objeto, redo->tipo, redo->matriz);
    printf("Refeito: %s\n", redo->descricao);

    // Move de volta para undo stack
    if (undoTopo < MAX_UNDO_STACK - 1) {
        undoTopo++;
        undoStack[undoTopo] = *redo;
    }

    redoTopo--;
    return 1;
}

void limparHistoricoUndo() {
    undoTopo = -1;
    redoTopo = -1;
    printf("Histórico de undo/redo limpo\n");
}

// ========== FUNÇÕES WRAPPER COM UNDO AUTOMÁTICO ==========

void transladarComUndo(void* obj, int tipo, float dx, float dy) {
    if (!obj) return;

    float T[3][3];
    matrizTranslacao(T, dx, dy);

    char desc[64];
    snprintf(desc, sizeof(desc), "Translação (%.1f, %.1f)", dx, dy);

    salvarParaUndo(T, obj, tipo, desc);
    aplicarMatrizObjeto(obj, tipo, T);
}

void rotacionarComUndo(void* obj, int tipo, float angulo) {
    if (!obj) return;

    float cx, cy;
    calcularCentro(obj, tipo, &cx, &cy);

    float R[3][3];
    matrizRotacaoEmTorno(R, angulo, cx, cy);

    char desc[64];
    snprintf(desc, sizeof(desc), "Rotação %.1f°", angulo);

    salvarParaUndo(R, obj, tipo, desc);
    aplicarMatrizObjeto(obj, tipo, R);
}

void escalarComUndo(void* obj, int tipo, float sx, float sy) {
    if (!obj) return;

    float cx, cy;
    calcularCentro(obj, tipo, &cx, &cy);

    float S[3][3];
    matrizEscalaEmTorno(S, sx, sy, cx, cy);

    char desc[64];
    snprintf(desc, sizeof(desc), "Escala (%.2fx, %.2fx)", sx, sy);

    salvarParaUndo(S, obj, tipo, desc);
    aplicarMatrizObjeto(obj, tipo, S);
}

void refletirComUndo(void* obj, int tipo, int eixo) {
    if (!obj) return;

    char desc[64];
    snprintf(desc, sizeof(desc), "Reflexão eixo %s", eixo == 0 ? "X" : "Y");

    float cx, cy;
    calcularCentro(obj, tipo, &cx, &cy);

    // USANDO O SISTEMA AVANÇADO DO SEU transformacoes.h
    float T1[3][3], R[3][3], T2[3][3], temp[3][3], resultado[3][3];

    // T(-cx, -cy)
    matrizTranslacao(T1, -cx, -cy);

    // Reflexão
    if (eixo == 0) {
        matrizReflexaoX(R);
    } else {
        matrizReflexaoY(R);
    }

    // T(cx, cy)
    matrizTranslacao(T2, cx, cy);

    // Compõe: T2 * R * T1
    multiplicarMatrizes(R, T1, temp);
    multiplicarMatrizes(T2, temp, resultado);

    salvarParaUndo(resultado, obj, tipo, desc);
    aplicarMatrizObjeto(obj, tipo, resultado);
}

void cisalharComUndo(void* obj, int tipo, float shx, float shy) {
    if (!obj) return;

    float cx, cy;
    calcularCentro(obj, tipo, &cx, &cy);

    char desc[64];
    snprintf(desc, sizeof(desc), "Cisalhamento (%.2f, %.2f)", shx, shy);

    float T1[3][3], Sh[3][3], T2[3][3], temp[3][3], resultado[3][3];

    // T(-cx, -cy)
    matrizTranslacao(T1, -cx, -cy);

    // Cisalhamento
    matrizCisalhamento(Sh, shx, shy);

    // T(cx, cy)
    matrizTranslacao(T2, cx, cy);

    // Compõe: T2 * Sh * T1
    multiplicarMatrizes(Sh, T1, temp);
    multiplicarMatrizes(T2, temp, resultado);

    salvarParaUndo(resultado, obj, tipo, desc);
    aplicarMatrizObjeto(obj, tipo, resultado);
}

// ========== FUNÇÕES DE STATUS E DEBUG ==========

int quantidadeUndoDisponiveis() {
    return undoTopo + 1;
}

int quantidadeRedoDisponiveis() {
    return redoTopo + 1;
}

void imprimirHistoricoUndo() {
    printf("=== HISTÓRICO UNDO/REDO ===\n");
    printf("Undo disponíveis: %d\n", quantidadeUndoDisponiveis());
    printf("Redo disponíveis: %d\n", quantidadeRedoDisponiveis());

    if (undoTopo >= 0) {
        printf("Últimas transformações:\n");
        for (int i = undoTopo; i >= 0 && i > undoTopo - 5; i--) {
            printf("  [%d] %s\n", i + 1, undoStack[i].descricao);
        }
    }
    printf("===========================\n");
}
