#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "selecao.h"

// =================== Sele��o de ponto ===================
Ponto* selecionarPonto(float x, float y, float tolerancia) {
    Ponto* aux = listaPontos;
    while(aux) {
        float dx = aux->x - x;
        float dy = aux->y - y;
        if ((dx*dx + dy*dy) <= (tolerancia * tolerancia))
            return aux;
        aux = aux->prox;
    }
    return NULL;
}

// =================== Sele��o de linha ===================
Linha* selecionarLinha(float x, float y, float tolerancia) {
    Linha* aux = listaLinhas;
    while(aux) {
        float A = x - aux->x1;
        float B = y - aux->y1;
        float C = aux->x2 - aux->x1;
        float D = aux->y2 - aux->y1;

        float dot = A*C + B*D;
        float len_sq = C*C + D*D;
        float param = (len_sq != 0) ? dot / len_sq : -1;

        float xx, yy;
        if (param < 0) { xx = aux->x1; yy = aux->y1; }
        else if (param > 1) { xx = aux->x2; yy = aux->y2; }
        else { xx = aux->x1 + param*C; yy = aux->y1 + param*D; }

        float dx = x - xx;
        float dy = y - yy;
        if ((dx*dx + dy*dy) <= (tolerancia * tolerancia))
            return aux;

        aux = aux->prox;
    }
    return NULL;
}

// =================== Sele��o de pol�gono (Ray Casting CORRIGIDO) ===================
Poligono* selecionarPoligono(float x, float y) {
    printf(">>> selecionarPoligono(%.2f, %.2f) iniciado\n", x, y);

    Poligono* aux = listaPoligonos;
    int contadorPoligonos = 0;

    while(aux && contadorPoligonos < 1000) {
        printf("Testando pol�gono %d\n", contadorPoligonos);
        contadorPoligonos++;

        if (!aux->pontos) {
            printf("  Pol�gono sem pontos, pulando\n");
            aux = aux->prox;
            continue;
        }

        int intersec = 0;
        Ponto* primeiro = aux->pontos;
        Ponto* atual = primeiro;
        Ponto* proximo = atual->prox;

        int contadorPontos = 0;
        const int MAX_PONTOS = 1000;

        // NOVA L�GICA: Percorre a lista de forma mais segura
        do {
            contadorPontos++;

            if (contadorPontos > MAX_PONTOS) {
                printf("ERRO: Loop infinito detectado no pol�gono, interrompendo\n");
                break;
            }

            if (!proximo) {
                printf("  Lista linear detectada, conectando �ltimo ao primeiro\n");
                proximo = primeiro; // Conecta ao primeiro ponto
            }

            // Ray casting: verifica intersec��o com a aresta atual->proximo
            if (((atual->y > y) != (proximo->y > y)) &&
                (x < (proximo->x - atual->x) * (y - atual->y) / (proximo->y - atual->y) + atual->x)) {
                intersec++;
            }

            atual = atual->prox;

            // Se � lista circular e voltou ao primeiro, para
            if (atual == primeiro) {
                printf("  Lista circular completa, %d pontos processados\n", contadorPontos);
                break;
            }

            // Se � lista linear e chegou ao fim, para
            if (!atual) {
                printf("  Lista linear completa, %d pontos processados\n", contadorPontos);
                break;
            }

            proximo = atual->prox;

        } while (contadorPontos < MAX_PONTOS);

        printf("  Intersec��es encontradas: %d\n", intersec);

        if (intersec % 2 == 1) {
            printf("Pol�gono selecionado (ponto dentro)\n");
            return aux; // Ponto est� dentro do pol�gono
        }

        aux = aux->prox;
    }

    printf("Nenhum pol�gono selecionado\n");
    return NULL;
}

// =================== VERS�O ALTERNATIVA MAIS SIMPLES ===================
Poligono* selecionarPoligonoSimples(float x, float y, float tolerancia) {
    printf(">>> selecionarPoligonoSimples(%.2f, %.2f) iniciado\n", x, y);

    // Esta vers�o verifica se o clique est� pr�ximo a qualquer v�rtice do pol�gono
    // � mais simples e n�o pode travar

    Poligono* aux = listaPoligonos;

    while(aux) {
        if (!aux->pontos) {
            aux = aux->prox;
            continue;
        }

        Ponto* primeiro = aux->pontos;
        Ponto* atual = primeiro;
        int contador = 0;

        do {
            contador++;
            if (contador > 1000) break; // Prote��o

            // Verifica se clicou pr�ximo a este v�rtice
            float dx = atual->x - x;
            float dy = atual->y - y;
            if ((dx*dx + dy*dy) <= (tolerancia * tolerancia)) {
                printf("Pol�gono selecionado por proximidade ao v�rtice\n");
                return aux;
            }

            atual = atual->prox;

        } while (atual && atual != primeiro && contador < 1000);

        aux = aux->prox;
    }

    printf("Nenhum pol�gono pr�ximo encontrado\n");
    return NULL;
}

// =================== Fun��o de sele��o com fallback ===================
Poligono* selecionarPoligonoSeguro(float x, float y, float tolerancia) {
    // Tenta primeiro o m�todo de proximidade (mais seguro)
    Poligono* resultado = selecionarPoligonoSimples(x, y, tolerancia);

    if (resultado) {
        return resultado;
    }

    // Se n�o encontrou por proximidade, tenta ray casting (com timeout)
    printf("Tentando ray casting...\n");
    return selecionarPoligono(x, y);
}
