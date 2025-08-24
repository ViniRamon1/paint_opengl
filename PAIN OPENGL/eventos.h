#ifndef EVENTOS_H
#define EVENTOS_H

// ==== Vari�veis globais ====
extern float toleranciaPx;

// Objeto selecionado e tipo
extern void* objetoSelecionado;
extern int tipoSelecionado;
// -1 = nenhum, 0 = ponto, 1 = linha, 2 = pol�gono

// ==== Fun��es de eventos ====
void mouseHandler(int button, int state, int x, int y);
void tecladoHandler(unsigned char tecla, int x, int y);
void tecladoEspecialHandler(int tecla, int x, int y); // <-- NOVO

#endif
