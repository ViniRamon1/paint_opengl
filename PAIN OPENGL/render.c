#include <GL/freeglut.h>
#include <stdio.h>
#include <string.h>
#include "render.h"
#include "objetos.h"
#include "eventos.h"

// Declarações externas das variáveis de eventos.c
extern int opcao;
extern float toleranciaPx;
extern void* objetoSelecionado;
extern int tipoSelecionado;

// Variável global para controlar exibição das instruções
int mostrarInstrucoes = 1;

// ==================== FUNÇÕES DE RENDERIZAÇÃO ====================

void desenharTexto(float x, float y, const char* texto) {
    glRasterPos2f(x, y);
    for (const char* c = texto; *c; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

void desenharTextoGrande(float x, float y, const char* texto) {
    glRasterPos2f(x, y);
    for (const char* c = texto; *c; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void renderizarPontos() {
    glColor3f(1.0f, 1.0f, 1.0f); // Branco para pontos normais
    glPointSize(6.0f);
    glBegin(GL_POINTS);

    for (Ponto* p = listaPontos; p; p = p->prox) {
        if (p->selecionado) {
            glEnd();
            glColor3f(1.0f, 1.0f, 0.0f); // Amarelo para selecionado
            glPointSize(10.0f);
            glBegin(GL_POINTS);
            glVertex2f(p->x, p->y);
            glEnd();
            glColor3f(1.0f, 1.0f, 1.0f); // Volta ao branco
            glPointSize(6.0f);
            glBegin(GL_POINTS);
        } else {
            glVertex2f(p->x, p->y);
        }
    }

    glEnd();
}

void renderizarLinhas() {
    glLineWidth(2.0f);

    for (Linha* l = listaLinhas; l; l = l->prox) {
        if (l->selecionado) {
            glColor3f(1.0f, 1.0f, 0.0f); // Amarelo para selecionado
            glLineWidth(4.0f);
        } else {
            glColor3f(0.0f, 1.0f, 0.0f); // Verde para linhas normais
            glLineWidth(2.0f);
        }

        glBegin(GL_LINES);
        glVertex2f(l->x1, l->y1);
        glVertex2f(l->x2, l->y2);
        glEnd();
    }
}

void renderizarPoligonos() {
    // ===== RENDERIZA POLÍGONOS FINALIZADOS =====
    for (Poligono* pol = listaPoligonos; pol; pol = pol->prox) {
        if (!pol->pontos) continue;

        if (pol->selecionado) {
            glColor3f(1.0f, 1.0f, 0.0f); // Amarelo para selecionado
            glLineWidth(4.0f);
        } else {
            glColor3f(0.0f, 0.0f, 1.0f); // Azul para polígonos normais
            glLineWidth(2.0f);
        }

        glBegin(GL_LINE_LOOP);

        Ponto* primeiro = pol->pontos;
        Ponto* atual = primeiro;
        int contador = 0;
        const int MAX_VERTICES = 1000;

        do {
            glVertex2f(atual->x, atual->y);
            atual = atual->prox;
            contador++;

            if (contador > MAX_VERTICES) {
                printf("AVISO: Muitos vértices no polígono, limitando renderização\n");
                break;
            }

        } while (atual && atual != primeiro && contador < MAX_VERTICES);

        glEnd();
    }

    // ===== RENDERIZA POLÍGONO EM CONSTRUÇÃO =====
    if (poligonoAtual && poligonoAtual->pontos) {
        // Desenha pontos do polígono em construção
        glColor3f(1.0f, 0.5f, 0.0f); // Laranja
        glPointSize(8.0f);
        glBegin(GL_POINTS);

        Ponto* atual = poligonoAtual->pontos;
        int contador = 0;

        while (atual && contador < 100) {  // Proteção contra loop infinito
            glVertex2f(atual->x, atual->y);
            atual = atual->prox;
            contador++;
        }
        glEnd();

        // Desenha linhas conectando os pontos (se houver 2 ou mais pontos)
        if (contador >= 2) {
            glColor3f(1.0f, 0.7f, 0.0f); // Laranja claro para linhas
            glLineWidth(2.0f);
            glBegin(GL_LINE_STRIP);  // Linha aberta (em construção)

            atual = poligonoAtual->pontos;
            int cont2 = 0;

            while (atual && cont2 < contador) {
                glVertex2f(atual->x, atual->y);
                atual = atual->prox;
                cont2++;
            }
            glEnd();
        }

        // Destaca o primeiro ponto em verde (onde clicar para fechar)
        if (contador >= 3) {
            glColor3f(0.0f, 1.0f, 0.0f); // Verde brilhante
            glPointSize(12.0f);
            glBegin(GL_POINTS);
            glVertex2f(poligonoAtual->pontos->x, poligonoAtual->pontos->y);
            glEnd();
        }
    }
}

void renderizarInstrucoes() {
    if (!mostrarInstrucoes) return;

    // Fundo semi-transparente para as instruções (canto esquerdo, 1/4 da tela)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(5, 5);
    glVertex2f(240, 5);  // 240px = menos de 1/4 da tela (1000px)
    glVertex2f(240, 795);
    glVertex2f(5, 795);
    glEnd();
    glDisable(GL_BLEND);

    // Título das instruções
    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTexto(10, 775, "=== PAINT 2D ===");

    // Status atual no topo
    glColor3f(1.0f, 1.0f, 0.0f);
    desenharTexto(10, 755, "STATUS:");
    glColor3f(1.0f, 1.0f, 1.0f);

    // Mostra o modo atual
    char modoAtual[50];
    if (opcao == 0) strcpy(modoAtual, "Modo: PONTO");
    else if (opcao == 1) strcpy(modoAtual, "Modo: LINHA");
    else if (opcao == 2) strcpy(modoAtual, "Modo: POLIGONO");
    else if (opcao == 3) strcpy(modoAtual, "Modo: SELECAO");
    else strcpy(modoAtual, "Modo: INDEFINIDO");

    desenharTexto(15, 740, modoAtual);

    // Mostra objeto selecionado
    if (objetoSelecionado) {
        char objetoTexto[50];
        if (tipoSelecionado == 0) strcpy(objetoTexto, "Sel: PONTO");
        else if (tipoSelecionado == 1) strcpy(objetoTexto, "Sel: LINHA");
        else if (tipoSelecionado == 2) strcpy(objetoTexto, "Sel: POLIGONO");
        else strcpy(objetoTexto, "Sel: INDEFINIDO");

        glColor3f(1.0f, 1.0f, 0.0f);
        desenharTexto(15, 725, objetoTexto);
    } else {
        glColor3f(0.7f, 0.7f, 0.7f);
        desenharTexto(15, 725, "Sel: NENHUM");
    }

    // Mostra tolerância atual
    char toleranciaTexto[30];
    snprintf(toleranciaTexto, sizeof(toleranciaTexto), "Tol: %.1f px", toleranciaPx);
    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTexto(15, 710, toleranciaTexto);

    // Instruções de criação
    glColor3f(0.8f, 1.0f, 0.8f); // Verde claro
    desenharTexto(10, 685, "CRIACAO:");
    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTexto(15, 670, "[P] Criar ponto");
    desenharTexto(15, 655, "[L] Criar linha");
    desenharTexto(15, 640, "[G] Criar poligono");
    desenharTexto(15, 625, "[Enter] Fechar poligono");

    // Mostra status do polígono em construção
    if (poligonoAtual && poligonoAtual->pontos) {
        int pontosAtual = 0;
        Ponto* aux = poligonoAtual->pontos;
        while (aux && pontosAtual < 100) {
            pontosAtual++;
            aux = aux->prox;
        }

        if (pontosAtual > 0) {
            glColor3f(1.0f, 0.7f, 0.0f); // Laranja
            char statusPoli[30];
            snprintf(statusPoli, sizeof(statusPoli), "Construindo: %d pts", pontosAtual);
            desenharTexto(15, 610, statusPoli);
        }
    }

    // Instruções de seleção
    glColor3f(1.0f, 1.0f, 0.8f); // Amarelo claro
    desenharTexto(10, 600, "SELECAO:");
    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTexto(15, 585, "[S] Modo selecao");
    desenharTexto(15, 570, "[D] Deletar selecionado");
    desenharTexto(15, 555, "Scroll - Escalar");

    // Instruções de transformações
    glColor3f(1.0f, 0.8f, 1.0f); // Rosa claro
    desenharTexto(10, 530, "TRANSFORMACOES:");
    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTexto(15, 515, "[Setas] Mover");
    desenharTexto(15, 500, "[T] Transladar 20px");
    desenharTexto(15, 485, "[R] Rotacao +15°");
    desenharTexto(15, 470, "[r] Rotacao -15°");
    desenharTexto(15, 455, "[E] Escalar 1.2x");
    desenharTexto(15, 440, "[F] Refletir eixo X");
    desenharTexto(15, 425, "[C] Cisalhar");

    // Seção de undo
    glColor3f(1.0f, 0.8f, 0.8f); // Vermelho claro
    desenharTexto(10, 400, "UNDO:");
    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTexto(15, 385, "[U] Desfazer");

    // Instruções de arquivo
    glColor3f(0.8f, 0.8f, 1.0f); // Azul claro
    desenharTexto(10, 360, "ARQUIVO:");
    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTexto(15, 345, "[V] Salvar");
    desenharTexto(15, 330, "[W] Carregar");

    // Animação
    glColor3f(0.8f, 1.0f, 1.0f); // Ciano claro
    desenharTexto(10, 305, "ANIMACAO:");
    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTexto(15, 290, "[I] Iniciar");
    desenharTexto(15, 275, "[A] Parar");

    // Interface
    glColor3f(1.0f, 0.8f, 0.6f); // Laranja claro
    desenharTexto(10, 250, "INTERFACE:");
    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTexto(15, 235, "[H] Ocultar/mostrar");
    desenharTexto(15, 220, "[+/-] Ajustar tol.");

    // Contador de objetos
    int contPontos = 0, contLinhas = 0, contPoligonos = 0;

    for (Ponto* p = listaPontos; p; p = p->prox) contPontos++;
    for (Linha* l = listaLinhas; l; l = l->prox) contLinhas++;
    for (Poligono* pol = listaPoligonos; pol; pol = pol->prox) contPoligonos++;

    glColor3f(0.9f, 0.9f, 0.9f);
    desenharTexto(10, 195, "OBJETOS:");

    char contadorTexto1[30];
    char contadorTexto2[30];
    char contadorTexto3[30];
    snprintf(contadorTexto1, sizeof(contadorTexto1), "Pontos: %d", contPontos);
    snprintf(contadorTexto2, sizeof(contadorTexto2), "Linhas: %d", contLinhas);
    snprintf(contadorTexto3, sizeof(contadorTexto3), "Poligonos: %d", contPoligonos);

    desenharTexto(15, 180, contadorTexto1);
    desenharTexto(15, 165, contadorTexto2);
    desenharTexto(15, 150, contadorTexto3);

    // Sistema avançado
    glColor3f(0.8f, 1.0f, 0.8f);
    desenharTexto(10, 125, "SISTEMA:");
    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTexto(15, 110, "* Matriz composta");
    desenharTexto(15, 95, "* Precisao alta");
    desenharTexto(15, 80, "* Performance +");

    // Dica
    glColor3f(1.0f, 0.8f, 0.6f);
    desenharTexto(10, 55, "DICA:");
    glColor3f(1.0f, 1.0f, 1.0f);
    desenharTexto(15, 40, "Scroll = escalar");
    desenharTexto(15, 25, "objeto selecionado");

    // Versão compacta
    glColor3f(0.5f, 0.5f, 0.5f);
    desenharTexto(10, 10, "v2.0 Matrix Paint");
}

void renderizarGrid() {
    // Grid sutil para ajudar no posicionamento
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(1.0f);

    // Linhas verticais
    glBegin(GL_LINES);
    for (int x = 0; x <= 1000; x += 50) {
        glVertex2f(x, 0);
        glVertex2f(x, 800);
    }
    glEnd();

    // Linhas horizontais
    glBegin(GL_LINES);
    for (int y = 0; y <= 800; y += 50) {
        glVertex2f(0, y);
        glVertex2f(1000, y);
    }
    glEnd();
}

void renderizarCursor() {
    // Pequena cruz no cursor para melhor precisão (implementação futura)
    // Requer captura da posição do mouse
}

// ==================== FUNÇÃO PRINCIPAL DE DISPLAY ====================

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Renderiza grid de fundo (sutil)
    renderizarGrid();

    // Renderiza todos os objetos
    renderizarPontos();
    renderizarLinhas();
    renderizarPoligonos();

    // Renderiza instruções se ativadas
    renderizarInstrucoes();

    // Força a renderização
    glFlush();
}

// ==================== FUNÇÕES AUXILIARES ====================

void alternarInstrucoes() {
    mostrarInstrucoes = !mostrarInstrucoes;
}

void configurarCores() {
    // Configurações de cor podem ser expandidas aqui
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f); // Fundo quase preto
}

void redimensionarJanela(int largura, int altura) {
    // Mantém proporção quando redimensiona
    glViewport(0, 0, largura, altura);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1000, 0, 800);
    glMatrixMode(GL_MODELVIEW);
}

// ==================== FUNÇÕES DE STATUS ====================

void imprimirEstatisticas() {
    int contPontos = 0, contLinhas = 0, contPoligonos = 0;

    for (Ponto* p = listaPontos; p; p = p->prox) contPontos++;
    for (Linha* l = listaLinhas; l; l = l->prox) contLinhas++;
    for (Poligono* pol = listaPoligonos; pol; pol = pol->prox) contPoligonos++;

    printf("=== ESTATÍSTICAS ===\n");
    printf("Pontos: %d\n", contPontos);
    printf("Linhas: %d\n", contLinhas);
    printf("Polígonos: %d\n", contPoligonos);
    printf("Total: %d objetos\n", contPontos + contLinhas + contPoligonos);
    printf("==================\n");
}
