#ifndef UNDO_SYSTEM_H
#define UNDO_SYSTEM_H

#include "transformacoes.h"

#define MAX_UNDO_STACK 50

typedef struct {
    float matriz[3][3];
    void* objeto;
    int tipo;
    long objetoId;  // ID �nico baseado no endere�o do objeto
    char descricao[64];
} TransformacaoUndo;

// Fun��es principais do sistema de undo
void salvarParaUndo(float matriz[3][3], void* obj, int tipo, const char* desc);
int desfazerTransformacao();
int refazerTransformacao();
void limparHistoricoUndo();

// Fun��es wrapper com undo autom�tico
void transladarComUndo(void* obj, int tipo, float dx, float dy);
void rotacionarComUndo(void* obj, int tipo, float angulo);
void escalarComUndo(void* obj, int tipo, float sx, float sy);
void refletirComUndo(void* obj, int tipo, int eixo);
void cisalharComUndo(void* obj, int tipo, float shx, float shy);

// Fun��o para validar se objeto ainda existe
int objetoValido(void* obj, long objetoId);

// Status do sistema
int quantidadeUndoDisponiveis();
int quantidadeRedoDisponiveis();
void imprimirHistoricoUndo(); // Para debug

#endif
