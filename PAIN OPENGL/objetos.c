#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "objetos.h"

// Listas globais
Ponto* listaPontos = NULL;
Linha* listaLinhas = NULL;
Poligono* listaPoligonos = NULL;

// Variáveis para criação de polígono (AGORA GLOBAIS para acesso do render)
Poligono* poligonoAtual = NULL;
Ponto* ultimoPontoPoligono = NULL;

// ==================== FUNÇÕES DE CRIAÇÃO ====================

void adicionaPonto(float x, float y) {
    Ponto* novo = (Ponto*)malloc(sizeof(Ponto));
    if (!novo) {
        printf("ERRO: Falha ao alocar memória para ponto\n");
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
        printf("ERRO: Falha ao alocar memória para linha\n");
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

    // Se não há polígono atual, cria um novo
    if (!poligonoAtual) {
        printf("Criando novo polígono\n");
        poligonoAtual = (Poligono*)malloc(sizeof(Poligono));
        if (!poligonoAtual) {
            printf("ERRO: Falha ao alocar memória para polígono\n");
            return;
        }
        poligonoAtual->pontos = NULL;
        poligonoAtual->selecionado = 0;
        poligonoAtual->prox = NULL;
        ultimoPontoPoligono = NULL;
    }

    // Verifica se clicou próximo ao primeiro ponto (para fechar automaticamente)
    if (ultimoPontoPoligono && poligonoAtual->pontos) {
        float dx = x - poligonoAtual->pontos->x;
        float dy = y - poligonoAtual->pontos->y;
        float distancia = sqrtf(dx * dx + dy * dy);

        if (distancia <= tolerancia) {
            printf("Clique próximo ao primeiro ponto detectado - fechando polígono\n");
            fechaPoligono();
            return;
        }
    }

    // Cria novo ponto para o polígono
    Ponto* novoPonto = (Ponto*)malloc(sizeof(Ponto));
    if (!novoPonto) {
        printf("ERRO: Falha ao alocar memória para ponto do polígono\n");
        return;
    }

    novoPonto->x = x;
    novoPonto->y = y;
    novoPonto->selecionado = 0;
    novoPonto->prox = NULL;

    // Adiciona o ponto à lista do polígono
    if (!poligonoAtual->pontos) {
        // Primeiro ponto
        poligonoAtual->pontos = novoPonto;
        printf("Primeiro ponto do polígono adicionado\n");
    } else {
        // Pontos subsequentes - adiciona no final
        ultimoPontoPoligono->prox = novoPonto;
        printf("Ponto %p adicionado ao polígono\n", (void*)novoPonto);
    }

    ultimoPontoPoligono = novoPonto;

    // Conta pontos para debug
    int contPontos = 0;
    Ponto* aux = poligonoAtual->pontos;
    while (aux) {
        contPontos++;
        aux = aux->prox;
        if (contPontos > 1000) break; // proteção
    }

    printf("Polígono atual tem %d pontos\n", contPontos);
}

void fechaPoligono() {
    printf(">>> fechaPoligono() chamada\n");

    if (!poligonoAtual) {
        printf("Nenhum polígono para fechar\n");
        return;
    }

    if (!poligonoAtual->pontos) {
        printf("Polígono sem pontos - cancelando\n");
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
        if (contPontos > 1000) break; // proteção
    }

    if (contPontos < 3) {
        printf("Polígono precisa de pelo menos 3 pontos (tem %d)\n", contPontos);
        return;
    }

    // Fecha a lista circular
    if (ultimoPontoPoligono) {
        ultimoPontoPoligono->prox = poligonoAtual->pontos;
        printf("Lista circular fechada - último ponto aponta para o primeiro\n");
    }

    // Adiciona o polígono à lista global
    poligonoAtual->prox = listaPoligonos;
    listaPoligonos = poligonoAtual;

    printf("Polígono fechado com %d pontos e adicionado à lista\n", contPontos);

    // Reset para próximo polígono
    poligonoAtual = NULL;
    ultimoPontoPoligono = NULL;
}

// ==================== FUNÇÕES DE EXCLUSÃO ====================

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

    // Procura polígono selecionado
    Poligono* poligAnt = NULL;
    Poligono* poligAtual = listaPoligonos;

    while (poligAtual) {
        if (poligAtual->selecionado) {
            // Libera pontos do polígono
            if (poligAtual->pontos) {
                Ponto* primeiro = poligAtual->pontos;
                Ponto* atual = primeiro;

                // Quebra a circularidade primeiro
                Ponto* ultimo = primeiro;
                while (ultimo->prox && ultimo->prox != primeiro) {
                    ultimo = ultimo->prox;
                }
                if (ultimo->prox == primeiro) {
                    ultimo->prox = NULL; // quebra o círculo
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
            printf("Polígono deletado\n");
            return;
        }
        poligAnt = poligAtual;
        poligAtual = poligAtual->prox;
    }

    printf("Nenhum objeto selecionado para deletar\n");
}

// ==================== FUNÇÕES DE DEBUG ====================

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
    printf("Polígonos: %d\n", contPoligonos);

    if (poligonoAtual) {
        int contPontosAtual = 0;
        for (Ponto* p = poligonoAtual->pontos; p; p = p->prox) {
            contPontosAtual++;
            if (contPontosAtual > 100) break; // proteção
        }
        printf("Polígono em construção: %d pontos\n", contPontosAtual);
    } else {
        printf("Nenhum polígono em construção\n");
    }

    printf("========================\n");
}
