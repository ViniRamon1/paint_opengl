#ifndef RENDER_H
#define RENDER_H

// ==================== VARI�VEIS GLOBAIS ====================
extern int mostrarInstrucoes;

// ==================== FUN��ES PRINCIPAIS ====================
void display();
void alternarInstrucoes();

// ==================== FUN��ES DE RENDERIZA��O ====================
void renderizarPontos();
void renderizarLinhas();
void renderizarPoligonos();
void renderizarInstrucoes();
void renderizarGrid();
void renderizarCursor();

// ==================== FUN��ES DE TEXTO ====================
void desenharTexto(float x, float y, const char* texto);
void desenharTextoGrande(float x, float y, const char* texto);

// ==================== FUN��ES AUXILIARES ====================
void configurarCores();
void redimensionarJanela(int largura, int altura);
void imprimirEstatisticas();

#endif
