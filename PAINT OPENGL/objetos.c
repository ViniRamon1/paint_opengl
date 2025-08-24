#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "objetos.h"

// Listas globais
Ponto* listaPontos = NULL;
Linha* listaLinhas = NULL;
Poligono* listaPoligonos = NULL;

// Vari�veis para cria��o de pol�gono (AGORA GLOBAIS para acesso do render)
Poligono* poligonoAtual = NULL;
Ponto* ultimoPontoPoligono = NULL;

// ==================== FUN��ES DE CRIA��O ====================

void adicionaPonto(float x, float y) {
    Ponto* novo = (Ponto*)malloc(sizeof(Ponto));
    if (!novo) {
        printf("ERRO: Falha ao alocar mem�ria para ponto\n");
        return;
    }

    novo->x = x;
    novo->y = y;
    novo->selecionado = 0;
    novo->prox = listaPontos;
    listaPontos = novo;

    printf("Ponto criado em (%.2f, %.2f)\n", x, y);
}

void adicionaLinha(float x1, float y1, float x2, float y2) {
    Linha* nova = (Linha*)malloc(sizeof(Linha));
    if (!nova) {
        printf("ERRO: Falha ao alocar mem�ria para linha\n");
        return;
    }

    nova->x1 = x1;
    nova->y1 = y1;
    nova->x2 = x2;
    nova->y2 = y2;
    nova->selecionado = 0;
    nova->prox = listaLinhas;
    listaLinhas = nova;

    printf("Linha criada de (%.2f, %.2f) a (%.2f, %.2f)\n", x1, y1, x2, y2);
}

void adicionaPontoPoligono(float x, float y, float tolerancia) {
    printf(">>> adicionaPontoPoligono(%.2f, %.2f) chamada\n", x, y);

    // Se n�o h� pol�gono atual, cria um novo
    if (!poligonoAtual) {
        printf("Criando novo pol�gono\n");
        poligonoAtual = (Poligono*)malloc(sizeof(Poligono));
        if (!poligonoAtual) {
            printf("ERRO: Falha ao alocar mem�ria para pol�gono\n");
            return;
        }
        poligonoAtual->pontos = NULL;
        poligonoAtual->selecionado = 0;
        poligonoAtual->prox = NULL;
        ultimoPontoPoligono = NULL;
    }

    // Verifica se clicou pr�ximo ao primeiro ponto (para fechar automaticamente)
    if (ultimoPontoPoligono && poligonoAtual->pontos) {
        float dx = x - poligonoAtual->pontos->x;
        float dy = y - poligonoAtual->pontos->y;
        float distancia = sqrtf(dx * dx + dy * dy);

        if (distancia <= tolerancia) {
            printf("Clique pr�ximo ao primeiro ponto detectado - fechando pol�gono\n");
            fechaPoligono();
            return;
        }
    }

    // Cria novo ponto para o pol�gono
    Ponto* novoPonto = (Ponto*)malloc(sizeof(Ponto));
    if (!novoPonto) {
        printf("ERRO: Falha ao alocar mem�ria para ponto do pol�gono\n");
        return;
    }

    novoPonto->x = x;
    novoPonto->y = y;
    novoPonto->selecionado = 0;
    novoPonto->prox = NULL;

    // Adiciona o ponto � lista do pol�gono
    if (!poligonoAtual->pontos) {
        // Primeiro ponto
        poligonoAtual->pontos = novoPonto;
        printf("Primeiro ponto do pol�gono adicionado\n");
    } else {
        // Pontos subsequentes - adiciona no final
        ultimoPontoPoligono->prox = novoPonto;
        printf("Ponto %p adicionado ao pol�gono\n", (void*)novoPonto);
    }

    ultimoPontoPoligono = novoPonto;

    // Conta pontos para debug
    int contPontos = 0;
    Ponto* aux = poligonoAtual->pontos;
    while (aux) {
        contPontos++;
        aux = aux->prox;
        if (contPontos > 1000) break; // prote��o
    }

    printf("Pol�gono atual tem %d pontos\n", contPontos);
}

void fechaPoligono() {
    printf(">>> fechaPoligono() chamada\n");

    if (!poligonoAtual) {
        printf("Nenhum pol�gono para fechar\n");
        return;
    }

    if (!poligonoAtual->pontos) {
        printf("Pol�gono sem pontos - cancelando\n");
        free(poligonoAtual);
        poligonoAtual = NULL;
        ultimoPontoPoligono = NULL;
        return;
    }

    // Conta pontos
    int contPontos = 0;
    Ponto* aux = poligonoAtual->pontos;
    while (aux) {
        contPontos++;
        aux = aux->prox;
        if (contPontos > 1000) break; // prote��o
    }

    if (contPontos < 3) {
        printf("Pol�gono precisa de pelo menos 3 pontos (tem %d)\n", contPontos);
        return;
    }

    // Fecha a lista circular
    if (ultimoPontoPoligono) {
        ultimoPontoPoligono->prox = poligonoAtual->pontos;
        printf("Lista circular fechada - �ltimo ponto aponta para o primeiro\n");
    }

    // Adiciona o pol�gono � lista global
    poligonoAtual->prox = listaPoligonos;
    listaPoligonos = poligonoAtual;

    printf("Pol�gono fechado com %d pontos e adicionado � lista\n", contPontos);

    // Reset para pr�ximo pol�gono
    poligonoAtual = NULL;
    ultimoPontoPoligono = NULL;
}

// ==================== FUN��ES DE EXCLUS�O ====================

void excluirSelecionado() {
    printf(">>> excluirSelecionado() chamada\n");

    // Procura ponto selecionado
    Ponto* pontoAnt = NULL;
    Ponto* pontoAtual = listaPontos;

    while (pontoAtual) {
        if (pontoAtual->selecionado) {
            if (pontoAnt) {
                pontoAnt->prox = pontoAtual->prox;
            } else {
                listaPontos = pontoAtual->prox;
            }
            free(pontoAtual);
            printf("Ponto deletado\n");
            return;
        }
        pontoAnt = pontoAtual;
        pontoAtual = pontoAtual->prox;
    }

    // Procura linha selecionada
    Linha* linhaAnt = NULL;
    Linha* linhaAtual = listaLinhas;

    while (linhaAtual) {
        if (linhaAtual->selecionado) {
            if (linhaAnt) {
                linhaAnt->prox = linhaAtual->prox;
            } else {
                listaLinhas = linhaAtual->prox;
            }
            free(linhaAtual);
            printf("Linha deletada\n");
            return;
        }
        linhaAnt = linhaAtual;
        linhaAtual = linhaAtual->prox;
    }

    // Procura pol�gono selecionado
    Poligono* poligAnt = NULL;
    Poligono* poligAtual = listaPoligonos;

    while (poligAtual) {
        if (poligAtual->selecionado) {
            // Libera pontos do pol�gono
            if (poligAtual->pontos) {
                Ponto* primeiro = poligAtual->pontos;
                Ponto* atual = primeiro;

                // Quebra a circularidade primeiro
                Ponto* ultimo = primeiro;
                while (ultimo->prox && ultimo->prox != primeiro) {
                    ultimo = ultimo->prox;
                }
                if (ultimo->prox == primeiro) {
                    ultimo->prox = NULL; // quebra o c�rculo
                }

                // Agora libera todos os pontos
                while (atual) {
                    Ponto* temp = atual;
                    atual = atual->prox;
                    free(temp);
                }
            }

            // Remove da lista
            if (poligAnt) {
                poligAnt->prox = poligAtual->prox;
            } else {
                listaPoligonos = poligAtual->prox;
            }
            free(poligAtual);
            printf("Pol�gono deletado\n");
            return;
        }
        poligAnt = poligAtual;
        poligAtual = poligAtual->prox;
    }

    printf("Nenhum objeto selecionado para deletar\n");
}

// ==================== FUN��ES DE DEBUG ====================

void imprimirEstadoListas() {
    printf("=== ESTADO DAS LISTAS ===\n");

    int contPontos = 0;
    for (Ponto* p = listaPontos; p; p = p->prox) contPontos++;
    printf("Pontos: %d\n", contPontos);

    int contLinhas = 0;
    for (Linha* l = listaLinhas; l; l = l->prox) contLinhas++;
    printf("Linhas: %d\n", contLinhas);

    int contPoligonos = 0;
    for (Poligono* pol = listaPoligonos; pol; pol = pol->prox) contPoligonos++;
    printf("Pol�gonos: %d\n", contPoligonos);

    if (poligonoAtual) {
        int contPontosAtual = 0;
        for (Ponto* p = poligonoAtual->pontos; p; p = p->prox) {
            contPontosAtual++;
            if (contPontosAtual > 100) break; // prote��o
        }
        printf("Pol�gono em constru��o: %d pontos\n", contPontosAtual);
    } else {
        printf("Nenhum pol�gono em constru��o\n");
    }

    printf("========================\n");
}
