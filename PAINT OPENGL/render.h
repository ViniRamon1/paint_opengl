#ifndef RENDER_H
#define RENDER_H

// ==================== VARI햂EIS GLOBAIS ====================
extern int mostrarInstrucoes;

// ==================== FUN합ES PRINCIPAIS ====================
void display();
void alternarInstrucoes();

// ==================== FUN합ES DE RENDERIZA플O ====================
void renderizarPontos();
void renderizarLinhas();
void renderizarPoligonos();
void renderizarInstrucoes();
void renderizarGrid();
void renderizarCursor();

// ==================== FUN합ES DE TEXTO ====================
void desenharTexto(float x, float y, const char* texto);
void desenharTextoGrande(float x, float y, const char* texto);

// ==================== FUN합ES AUXILIARES ====================
void configurarCores();
void redimensionarJanela(int largura, int altura);
void imprimirEstatisticas();

#endif
