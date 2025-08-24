#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"
#include "objetos.h"
#include <GL/freeglut.h>

// Funções auxiliares para limpar listas antes de carregar
void limparPontos() {
    printf(">>> limparPontos() iniciado\n");
    Ponto* p = listaPontos;
    while (p) {
        Ponto* tmp = p;
        p = p->prox;
        free(tmp);
    }
    listaPontos = NULL;
    printf(">>> limparPontos() finalizado\n");
}

void limparLinhas() {
    printf(">>> limparLinhas() iniciado\n");
    Linha* l = listaLinhas;
    while (l) {
        Linha* tmp = l;
        l = l->prox;
        free(tmp);
    }
    listaLinhas = NULL;
    printf(">>> limparLinhas() finalizado\n");
}

void limparPoligonos() {
    printf(">>> limparPoligonos() iniciado\n");
    Poligono* poli = listaPoligonos;
    while (poli) {
        Poligono* tmp = poli;

        // liberar lista circular de pontos
        if (poli->pontos) {
            Ponto* atual = poli->pontos;
            Ponto* primeiro = atual;
            printf("    Liberando lista circular de pontos de polígono...\n");

            if (atual->prox == atual) {
                // Apenas 1 ponto na lista
                printf("    Apenas 1 ponto na lista\n");
                free(atual);
            } else {
                // Múltiplos pontos - quebra a circularidade primeiro
                Ponto* ultimo = atual;
                while (ultimo->prox != primeiro) {
                    ultimo = ultimo->prox;
                }
                ultimo->prox = NULL; // quebra a circularidade

                // Agora libera normalmente
                while (atual) {
                    Ponto* temp = atual;
                    atual = atual->prox;
                    free(temp);
                }
            }
            printf("    Pontos liberados\n");
        }

        poli = poli->prox;
        free(tmp);
        printf("    Polígono liberado\n");
    }
    listaPoligonos = NULL;
    printf(">>> limparPoligonos() finalizado\n");
}

void salvarObjetos(const char* nome) {
    if (nome == NULL) return;

    printf(">>> salvarObjetos() iniciando\n");

    FILE* arquivo = fopen(nome, "w");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    // ======== PONTOS ========
    int cont = 0;
    Ponto* p = listaPontos;
    while (p) { cont++; p = p->prox; }
    fprintf(arquivo, "PONTOS: %d\n", cont);
    p = listaPontos;
    while (p) {
        fprintf(arquivo, "%.2f %.2f %d\n", p->x, p->y, p->selecionado);
        p = p->prox;
    }

    // ======== LINHAS ========
    cont = 0;
    Linha* l = listaLinhas;
    while (l) { cont++; l = l->prox; }
    fprintf(arquivo, "LINHAS: %d\n", cont);
    l = listaLinhas;
    while (l) {
        fprintf(arquivo, "%.2f %.2f %.2f %.2f %d\n",
                l->x1, l->y1, l->x2, l->y2, l->selecionado);
        l = l->prox;
    }

    // ======== POLÍGONOS ========
    cont = 0;
    Poligono* poli = listaPoligonos;
    while (poli) { cont++; poli = poli->prox; }
    fprintf(arquivo, "POLIGONOS: %d\n", cont);

    poli = listaPoligonos;
    while (poli) {
        int ptCount = 0;
        if (poli->pontos) {
            Ponto* aux = poli->pontos;
            Ponto* inicio = aux;

            // Conta os pontos com segurança
            do {
                ptCount++;
                aux = aux->prox;
                if (ptCount > 10000) { // proteção contra loop infinito
                    printf("Erro: possível loop infinito detectado ao contar pontos!\n");
                    break;
                }
            } while (aux && aux != inicio);
        }

        fprintf(arquivo, "%d %d\n", ptCount, poli->selecionado);

        // Salva os pontos
        if (poli->pontos && ptCount > 0) {
            Ponto* aux = poli->pontos;
            Ponto* inicio = aux;
            int contador = 0;

            do {
                fprintf(arquivo, "%.2f %.2f\n", aux->x, aux->y);
                aux = aux->prox;
                contador++;
                if (contador >= ptCount) break; // usa o contador para evitar loop
            } while (aux && aux != inicio && contador < 10000);
        }
        poli = poli->prox;
    }

    fclose(arquivo);
    printf(">>> salvarObjetos() finalizado\n");
}

void carregarObjetos(const char* nome) {
    printf(">>> carregarObjetos() iniciando\n");
    FILE* f = fopen(nome, "r");
    if (!f) {
        printf("Erro ao abrir arquivo %s\n", nome);
        return;
    }

    // limpa as listas existentes
    limparPontos();
    limparLinhas();
    limparPoligonos();

    int qtdPontos, qtdLinhas, qtdPoligonos;

    // Verifica se conseguiu ler o cabeçalho
    if (fscanf(f, "PONTOS: %d\n", &qtdPontos) != 1) {
        printf("Erro ao ler quantidade de pontos do arquivo\n");
        fclose(f);
        return;
    }

    // ======== PONTOS ========
    printf("Quantidade de pontos a carregar: %d\n", qtdPontos);
    if (qtdPontos < 0 || qtdPontos > 100000) { // validação
        printf("Quantidade de pontos inválida: %d\n", qtdPontos);
        fclose(f);
        return;
    }

    Ponto* ultimoPonto = NULL;
    for (int i = 0; i < qtdPontos; i++) {
        float x, y;
        int sel;
        if (fscanf(f, "%f %f %d\n", &x, &y, &sel) != 3) {
            printf("Erro ao ler ponto %d\n", i + 1);
            fclose(f);
            return;
        }

        Ponto* p = (Ponto*)malloc(sizeof(Ponto));
        if (!p) {
            printf("Erro ao alocar memória para ponto\n");
            fclose(f);
            return;
        }
        p->x = x;
        p->y = y;
        p->selecionado = sel;
        p->prox = NULL;

        if (!listaPontos) {
            listaPontos = p;
        } else {
            ultimoPonto->prox = p;
        }
        ultimoPonto = p;
        printf("  Ponto %d carregado: (%.2f, %.2f) selecionado=%d\n", i + 1, x, y, sel);
    }

    // ======== LINHAS ========
    if (fscanf(f, "LINHAS: %d\n", &qtdLinhas) != 1) {
        printf("Erro ao ler quantidade de linhas do arquivo\n");
        fclose(f);
        return;
    }

    printf("Quantidade de linhas a carregar: %d\n", qtdLinhas);
    if (qtdLinhas < 0 || qtdLinhas > 100000) {
        printf("Quantidade de linhas inválida: %d\n", qtdLinhas);
        fclose(f);
        return;
    }

    Linha* ultimaLinha = NULL;
    for (int i = 0; i < qtdLinhas; i++) {
        float x1, y1, x2, y2;
        int sel;
        if (fscanf(f, "%f %f %f %f %d\n", &x1, &y1, &x2, &y2, &sel) != 5) {
            printf("Erro ao ler linha %d\n", i + 1);
            fclose(f);
            return;
        }

        Linha* l = (Linha*)malloc(sizeof(Linha));
        if (!l) {
            printf("Erro ao alocar memória para linha\n");
            fclose(f);
            return;
        }
        l->x1 = x1; l->y1 = y1; l->x2 = x2; l->y2 = y2;
        l->selecionado = sel;
        l->prox = NULL;

        if (!listaLinhas) {
            listaLinhas = l;
        } else {
            ultimaLinha->prox = l;
        }
        ultimaLinha = l;
        printf("  Linha %d carregada: (%.2f, %.2f) a (%.2f, %.2f) selecionada=%d\n", i + 1, x1, y1, x2, y2, sel);
    }

    // ======== POLÍGONOS ========
    if (fscanf(f, "POLIGONOS: %d\n", &qtdPoligonos) != 1) {
        printf("Erro ao ler quantidade de polígonos do arquivo\n");
        fclose(f);
        return;
    }

    printf("Quantidade de polígonos a carregar: %d\n", qtdPoligonos);
    if (qtdPoligonos < 0 || qtdPoligonos > 100000) {
        printf("Quantidade de polígonos inválida: %d\n", qtdPoligonos);
        fclose(f);
        return;
    }

    Poligono* ultimoPoligono = NULL;
    for (int i = 0; i < qtdPoligonos; i++) {
        int qtdVertices, sel;
        if (fscanf(f, "%d %d\n", &qtdVertices, &sel) != 2) {
            printf("Erro ao ler dados do polígono %d\n", i + 1);
            fclose(f);
            return;
        }

        if (qtdVertices < 0 || qtdVertices > 10000) {
            printf("Quantidade de vértices inválida para polígono %d: %d\n", i + 1, qtdVertices);
            fclose(f);
            return;
        }

        Poligono* novo = (Poligono*)malloc(sizeof(Poligono));
        if (!novo) {
            printf("Erro ao alocar memória para polígono\n");
            fclose(f);
            return;
        }
        novo->pontos = NULL;
        novo->prox = NULL;
        novo->selecionado = sel;

        printf("  Polígono %d: %d vértices, selecionado=%d\n", i + 1, qtdVertices, sel);

        if (qtdVertices == 0) {
            // Polígono sem vértices
            if (!listaPoligonos) {
                listaPoligonos = novo;
            } else {
                ultimoPoligono->prox = novo;
            }
            ultimoPoligono = novo;
            continue;
        }

        Ponto* primeiro = NULL;
        Ponto* ultimo = NULL;

        // Carrega os vértices
        for (int j = 0; j < qtdVertices; j++) {
            float x, y;
            if (fscanf(f, "%f %f\n", &x, &y) != 2) {
                printf("Erro ao ler vértice %d do polígono %d\n", j + 1, i + 1);
                fclose(f);
                return;
            }

            Ponto* p = (Ponto*)malloc(sizeof(Ponto));
            if (!p) {
                printf("Erro ao alocar memória para ponto do polígono\n");
                fclose(f);
                return;
            }
            p->x = x;
            p->y = y;
            p->selecionado = 0;
            p->prox = NULL;

            if (!primeiro) {
                primeiro = p;
                ultimo = p;
            } else {
                ultimo->prox = p;
                ultimo = p;
            }
            printf("    Vértice %d do polígono %d carregado: (%.2f, %.2f)\n", j + 1, i + 1, x, y);
        }

        // Fecha a lista circular APENAS se há mais de um vértice
        if (primeiro && ultimo && qtdVertices > 1) {
            ultimo->prox = primeiro;
        } else if (qtdVertices == 1) {
            // Para um único vértice, aponta para si mesmo
            primeiro->prox = primeiro;
        }

        novo->pontos = primeiro;

        // Adiciona à lista de polígonos
        if (!listaPoligonos) {
            listaPoligonos = novo;
        } else {
            ultimoPoligono->prox = novo;
        }
        ultimoPoligono = novo;
    }

    fclose(f);

    // Força o redesenho da tela para mostrar os objetos carregados
    glutPostRedisplay();

    // Debug final - mostra o que foi carregado
    printf("=== RESUMO DO CARREGAMENTO ===\n");

    // Conta pontos carregados
    int totalPontos = 0;
    Ponto* p = listaPontos;
    while (p) { totalPontos++; p = p->prox; }
    printf("Total de pontos na memória: %d\n", totalPontos);

    // Conta linhas carregadas
    int totalLinhas = 0;
    Linha* l = listaLinhas;
    while (l) { totalLinhas++; l = l->prox; }
    printf("Total de linhas na memória: %d\n", totalLinhas);

    // Conta polígonos carregados
    int totalPoligonos = 0;
    Poligono* pol = listaPoligonos;
    while (pol) { totalPoligonos++; pol = pol->prox; }
    printf("Total de polígonos na memória: %d\n", totalPoligonos);

    printf("==============================\n");

    printf(">>> carregarObjetos() finalizado\n");
}
