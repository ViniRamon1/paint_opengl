#include <GL/freeglut.h>
#include <stdio.h>
#include "objetos.h"
#include "selecao.h"
#include "transformacoes.h"
#include "undo_system.h"  // <-- ADICIONADO
#include <math.h>
#include "animacao.h"
#include "arquivo.h"
#include "render.h"

float toleranciaPx = 8.0f;

// Variáveis globais
int opcao = 0; // 0 = ponto, 1 = linha, 2 = polígono, 3 = seleção
int capturaL = 0;
float linha_vetor[2];

// Último objeto selecionado
void* objetoSelecionado = NULL;
int tipoSelecionado = -1; // -1 = nenhum, 0 = ponto, 1 = linha, 2 = polígono

void mouseHandler(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
        int windowWidth  = glutGet(GLUT_WINDOW_WIDTH);

        float x_open = (float)x * (1000.0f / (float)windowWidth);
        float y_open = (float)(windowHeight - y) * (800.0f / (float)windowHeight);

        // px -> mundo
        float sx = 1000.0f / (float)windowWidth;
        float sy = 800.0f  / (float)windowHeight;
        float tolWorld = fmaxf(sx, sy) * toleranciaPx;

        // ======== MODO SELEÇÃO ========
        if (opcao == 3) {
            // limpa seleções anteriores
            for (Ponto* pAux = listaPontos; pAux; pAux = pAux->prox) pAux->selecionado = 0;
            for (Linha* lAux = listaLinhas; lAux; lAux = lAux->prox) lAux->selecionado = 0;
            for (Poligono* polAux = listaPoligonos; polAux; polAux = polAux->prox) polAux->selecionado = 0;

            objetoSelecionado = NULL;
            tipoSelecionado = -1;

            // Tenta selecionar ponto primeiro
            Ponto* p = selecionarPonto(x_open, y_open, 5.0f);
            if (p) {
                p->selecionado = 1;
                objetoSelecionado = p;
                tipoSelecionado = 0;
                printf("Ponto selecionado em (%.2f, %.2f)\n", p->x, p->y);
                glutPostRedisplay();
                return;
            }

            // Tenta selecionar linha
            Linha* l = selecionarLinha(x_open, y_open, 5.0f);
            if (l) {
                l->selecionado = 1;
                objetoSelecionado = l;
                tipoSelecionado = 1;
                printf("Linha selecionada\n");
                glutPostRedisplay();
                return;
            }

            // Tenta selecionar polígono
            printf("Tentando selecionar polígono...\n");
            Poligono* pol = selecionarPoligonoSeguro(x_open, y_open, tolWorld);
            if (pol) {
                pol->selecionado = 1;
                objetoSelecionado = pol;
                tipoSelecionado = 2;
                printf("Polígono selecionado\n");
                glutPostRedisplay();
                return;
            }

            printf("Nenhum objeto encontrado no clique.\n");
            return;
        }

        // ======== MODO CRIAÇÃO ========
        if (opcao == 0) {
            adicionaPonto(x_open, y_open);
        } else if (opcao == 1) {
            if (capturaL == 0) {
                linha_vetor[0] = x_open;
                linha_vetor[1] = y_open;
                capturaL = 1;
            } else {
                adicionaLinha(linha_vetor[0], linha_vetor[1], x_open, y_open);
                capturaL = 0;
            }
        } else if (opcao == 2) {
            adicionaPontoPoligono(x_open, y_open, tolWorld);
        }

        glutPostRedisplay();
    }

    // ======== SCROLL DO MOUSE (escala COM UNDO) ========
    if (button == 3 && state == GLUT_DOWN) {  // Scroll Up
        if (objetoSelecionado) {
            escalarComUndo(objetoSelecionado, tipoSelecionado, 1.1f, 1.1f);
            printf("Escalado + (scroll up) com undo\n");
            glutPostRedisplay();
        }
    }
    else if (button == 4 && state == GLUT_DOWN) {  // Scroll Down
        if (objetoSelecionado) {
            escalarComUndo(objetoSelecionado, tipoSelecionado, 0.9f, 0.9f);
            printf("Escalado - (scroll down) com undo\n");
            glutPostRedisplay();
        }
    }
}

void tecladoHandler(unsigned char tecla, int x, int y) {

    if (tecla == 'p' || tecla == 'P') {
        opcao = 0;
        printf("Criando ponto\n");
    }
    else if (tecla == 'h' || tecla == 'H') {
        alternarInstrucoes();
        printf("Instruções %s\n", mostrarInstrucoes ? "mostradas" : "ocultadas");
        glutPostRedisplay();
    }
    else if (tecla == 'l' || tecla == 'L') {
        opcao = 1;
        capturaL = 0;
        printf("Criando linha\n");
    }
    else if (tecla == 'g' || tecla == 'G') {
        opcao = 2;
        printf("Criando polígono\n");
    }
    else if ((tecla == 13 || tecla == '\r') && opcao == 2) {
        fechaPoligono();
    }
    // ======== SELEÇÃO + SALVAR ========
    else if (tecla == 's' || tecla == 'S') {
        opcao = 3;
        printf("Modo seleção ativado\n");
    }
    else if (tecla == 'v' || tecla == 'V') {
        salvarObjetos("objetos.txt");
        printf("Objetos salvos\n");
    }
    else if (tecla == 'w' || tecla == 'W') {
        carregarObjetos("objetos.txt");
        printf("Objetos carregados\n");
    }
    else if (tecla == 'i' || tecla == 'I') {
        iniciarAnimacao();
        printf("Animação iniciada\n");
    }
    else if (tecla == 'a' || tecla == 'A') {
        pararAnimacao();
        printf("Animação parada\n");
    }
    // ======== DELETAR OBJETO SELECIONADO ========
    else if (tecla == 'd' || tecla == 'D') {
        if (objetoSelecionado) {
            excluirSelecionado();
            objetoSelecionado = NULL;
            tipoSelecionado = -1;
            printf("Objeto deletado\n");
            glutPostRedisplay();
        }
    }

    // ======== UNDO/REDO SYSTEM ========
    else if (tecla == 'u' || tecla == 'U') {
        if (desfazerTransformacao()) {
            glutPostRedisplay();
        }
    }
    else if (tecla == 'y' || tecla == 'Y') { // Redo
        if (refazerTransformacao()) {
            glutPostRedisplay();
        }
    }
    else if (tecla == 'z' || tecla == 'Z') { // Limpar histórico
        limparHistoricoUndo();
    }
    else if (tecla == 'q' || tecla == 'Q') { // Status do undo (debug)
        imprimirHistoricoUndo();
    }

    // ======== TRANSFORMAÇÕES COM UNDO ========
    else if (tecla == 'R') { // R maiúsculo - rotação horária
        if (objetoSelecionado) {
            rotacionarComUndo(objetoSelecionado, tipoSelecionado, 15.0f);
            printf("Rotacionado 15° (horário) com undo\n");
            glutPostRedisplay();
        } else {
            printf("Nenhum objeto selecionado para rotacionar\n");
        }
    }
    else if (tecla == 'r') { // r minúsculo - rotação anti-horária
        if (objetoSelecionado) {
            rotacionarComUndo(objetoSelecionado, tipoSelecionado, -15.0f);
            printf("Rotacionado 15° (anti-horário) com undo\n");
            glutPostRedisplay();
        } else {
            printf("Nenhum objeto selecionado para rotacionar\n");
        }
    }
    else if (tecla == 't' || tecla == 'T') {
        if (objetoSelecionado) {
            transladarComUndo(objetoSelecionado, tipoSelecionado, 20.0f, 0.0f);
            printf("Transladado com undo\n");
            glutPostRedisplay();
        }
    }
    else if (tecla == 'e' || tecla == 'E') {
        if (objetoSelecionado) {
            escalarComUndo(objetoSelecionado, tipoSelecionado, 1.2f, 1.2f);
            printf("Escalado com undo\n");
            glutPostRedisplay();
        }
    }
    else if (tecla == 'f' || tecla == 'F') {
        if (objetoSelecionado) {
            refletirComUndo(objetoSelecionado, tipoSelecionado, 0);
            printf("Refletido no eixo X com undo\n");
            glutPostRedisplay();
        }
    }
    else if (tecla == 'c' || tecla == 'C') {
        if (objetoSelecionado) {
            cisalharComUndo(objetoSelecionado, tipoSelecionado, 0.3f, 0.0f);
            printf("Cisalhamento aplicado com undo\n");
            glutPostRedisplay();
        }
    }

    // ======== AJUSTE DA TOLERÂNCIA ========
    else if (tecla == '+' || tecla == '=') {
        toleranciaPx += 2.0f;
        printf("Tol: %.1f px\n", toleranciaPx);
    }
    else if (tecla == '-' || tecla == '_') {
        toleranciaPx = (toleranciaPx > 2) ? toleranciaPx - 2 : 2;
        printf("Tol: %.1f px\n", toleranciaPx);
    }

    glutPostRedisplay();
}

void tecladoEspecialHandler(int tecla, int x, int y) {
    if (!objetoSelecionado) return;

    switch (tecla) {
        case GLUT_KEY_LEFT:
            transladarComUndo(objetoSelecionado, tipoSelecionado, -10.0f, 0.0f);
            printf("Transladado para esquerda com undo\n");
            break;
        case GLUT_KEY_RIGHT:
            transladarComUndo(objetoSelecionado, tipoSelecionado, 10.0f, 0.0f);
            printf("Transladado para direita com undo\n");
            break;
        case GLUT_KEY_UP:
            transladarComUndo(objetoSelecionado, tipoSelecionado, 0.0f, 10.0f);
            printf("Transladado para cima com undo\n");
            break;
        case GLUT_KEY_DOWN:
            transladarComUndo(objetoSelecionado, tipoSelecionado, 0.0f, -10.0f);
            printf("Transladado para baixo com undo\n");
            break;
    }

    glutPostRedisplay();
}
