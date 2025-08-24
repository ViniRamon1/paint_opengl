#include <GL/freeglut.h>
#include <stdio.h>
#include "render.h"
#include "eventos.h"
#include "transformacoes.h"

void init() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, 1000, 0, 800);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1000, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Paint");

    // Mostrando instrucoes no terminal (sem acentos)
    printf("[H] Mostrar/ocultar instruções na tela\n");

    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouseHandler);
    glutKeyboardFunc(tecladoHandler);
    glutSpecialFunc(tecladoEspecialHandler); // Setas
    glutPostRedisplay();
    glutMainLoop();
    return 0;
}
