#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>  // <-- ADICIONAR para sinf()
#include "objetos.h"
#include "animacao.h"
#include "transformacoes.h"  // <-- ADICIONAR esta inclus�o

int animando = 0;
float deslocamento = 0;
int direcao = 1;

void animacao(int valor) {
    if (animando) {
        deslocamento += 5.0f * direcao;

        if (deslocamento >= 200) {
            direcao = -1;
        }
        else if (deslocamento <= 0) {
            direcao = 1;
        }

        float dx = 5.0f * direcao;

        // ===== VERS�O OTIMIZADA: Usa sistema de transforma��es =====

        // Translada todos os pontos
        for (Ponto* p = listaPontos; p != NULL; p = p->prox) {
            transladarObjeto(p, TIPO_PONTO, dx, 0.0f);
        }

        // Translada todas as linhas
        for (Linha* l = listaLinhas; l != NULL; l = l->prox) {
            transladarObjeto(l, TIPO_LINHA, dx, 0.0f);
        }

        // Translada todos os pol�gonos
        for (Poligono* pol = listaPoligonos; pol != NULL; pol = pol->prox) {
            transladarObjeto(pol, TIPO_POLIGONO, dx, 0.0f);
        }

        glutPostRedisplay();
        glutTimerFunc(100, animacao, 0);
    }
}

void iniciarAnimacao() {
    if (!animando) {
        animando = 1;
        direcao = 1;
        printf(" Anima��o iniciada\n");
        glutTimerFunc(100, animacao, 0);
    }
}

void pararAnimacao() {
    if (animando) {
        animando = 0;
        printf(" Anima��o parada\n");
    }
}

// ===== NOVA FUN��O: Anima��o com m�ltiplas transforma��es =====
void animacaoCompleta(int valor) {
    if (animando) {
        static float tempo = 0.0f;
        tempo += 0.1f;

        // Calcula transforma��es baseadas no tempo
        float dx = 5.0f * direcao;
        float angulo = 1.0f; // Rota��o suave
        float escala = 1.0f + 0.05f * sinf(tempo); // Pulsa��o suave

        // Aplica transforma��es compostas para cada tipo de objeto
        for (Ponto* p = listaPontos; p != NULL; p = p->prox) {
            transformarObjetoCompleto(p, TIPO_PONTO, dx, 0.0f, angulo, escala, escala);
        }

        for (Linha* l = listaLinhas; l != NULL; l = l->prox) {
            transformarObjetoCompleto(l, TIPO_LINHA, dx, 0.0f, angulo, escala, escala);
        }

        for (Poligono* pol = listaPoligonos; pol != NULL; pol = pol->prox) {
            transformarObjetoCompleto(pol, TIPO_POLIGONO, dx, 0.0f, angulo, escala, escala);
        }

        // Controle de dire��o
        deslocamento += 5.0f * direcao;
        if (deslocamento >= 200) {
            direcao = -1;
        }
        else if (deslocamento <= 0) {
            direcao = 1;
        }

        glutPostRedisplay();
        glutTimerFunc(100, animacaoCompleta, 0);
    }
}
