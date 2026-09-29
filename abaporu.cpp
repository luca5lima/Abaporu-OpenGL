#include <GL/freeglut.h>
#include <cmath>

void desenharSol() {
    // Cor amarela/laranja do sol do Abaporu
    glColor3f(1.0f, 0.85f, 0.0f);
    
    // Posição no canto superior direito e tamanho
    float raio = 0.3f;
    float centroX = 0.2f;
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

// Função auxiliar para desenhar elipses preenchidas
void desenharElipse(float centroX, float centroY, float raioX, float raioY, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(centroX, centroY); // Centro da elipse
        for (float u = 0.0f; u <= 6.28318f + 0.1f; u += 0.05f) { // 2 * PI
            float x = centroX + raioX * cos(u);
            float y = centroY + raioY * sin(u);
            glVertex2f(x, y);
        }
    glEnd();
}

void desenharCacto() {
    // Cor do Cacto (Verde Escuro)
    float r = 0.0f, g = 0.45f, b = 0.15f;

    // 1. Haste/Braço Esquerdo (menor, na parte inferior)
    // Conector horizontal + ponta vertical
    desenharElipse(0.72f, -0.20f, 0.08f, 0.04f, r, g, b); // Conexão horizontal
    desenharElipse(0.78f,  0.06f, 0.04f, 0.30f, r, g, b); // Ponta vertical direita

    // 2. Haste/Braço Direito (médio, na parte intermediária)
    // Conector horizontal + ponta vertical
    desenharElipse(0.52f, -0.05f, 0.08f, 0.04f, r, g, b); // Conexão horizontal
    desenharElipse(0.46f,  0.14f, 0.04f, 0.24f, r, g, b); // Ponta vertical esquerda

    // 3. Tronco Principal (Elipse bem esticada no eixo Y)
    desenharElipse(0.62f, -0.05f, 0.07f, 0.40f, r, g, b);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    desenharSol();
    desenharChao();
    desenharCacto();
    
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
    
    glutCreateWindow("Releitura Geometrica - Abaporu");
    
    init();
    
    // Regista a função de desenho
    glutDisplayFunc(display);
    
    // Inicia o ciclo de eventos do FreeGLUT
    glutMainLoop();
    
    return 0;
}