#include <GL/freeglut.h>
#include <cmath>

void desenharSol() {
    // Cor amarela/laranja do sol do Abaporu
    glColor3f(1.0f, 0.85f, 0.0f);
    
    // Posição no canto superior direito e tamanho
    float raio = 0.3f;
    float centroX = 0.5f;
    float centroY = 0.5f;

    // Desenho do círculo preenchido
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(centroX, centroY); // Centro
        for (float u = 0.0f; u <= 6.283f + 0.1f; u += 0.1f) {
            float x = centroX + raio * cos(u);
            float y = centroY + raio * sin(u);
            glVertex2f(x, y);
        }
    glEnd();
}

void desenharChao() {
    // Tom de verde característico do chão na obra Abaporu
    glColor3f(0.2f, 0.8f, 0.2f); 

    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f); // Canto inferior esquerdo
        glVertex2f( 1.0f, -1.0f); // Canto inferior direito
        glVertex2f( 1.0f, -0.4f); // Canto superior direito (altura do chão)
        glVertex2f(-1.0f, -0.4f); // Canto superior esquerdo
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    desenharSol();
    desenharChao();
    
    glFlush();
}

void init() {
    // Fundo azul claro (Céu)
    glClearColor(0.4f, 0.7f, 1.0f, 1.0f);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Sistema de coordenadas simples: -1 a 1 em ambos os eixos
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    
    glutCreateWindow("Releitura Geometrica - Abaporu (Teste Inicial)");
    
    init();
    
    // Regista a função de desenho
    glutDisplayFunc(display);
    
    // Inicia o ciclo de eventos do FreeGLUT
    glutMainLoop();
    
    return 0;
}