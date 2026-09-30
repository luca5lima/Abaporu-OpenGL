#include <GL/freeglut.h>
#include <cmath>

// ============================================================
//  Estruturas e variáveis globais
// ============================================================
struct Ponto { float x, y; };

// Cor do sol (troca com a tecla 'e')
float solR = 1.0f, solG = 0.85f, solB = 0.0f;
int corAtual = 0;
const float paleta[][3] = {
    {1.0f, 0.85f, 0.0f}, // amarelo
    {1.0f, 0.5f,  0.0f}, // laranja
    {1.0f, 0.2f,  0.2f}, // vermelho
    {1.0f, 1.0f,  1.0f}, // branco
    {0.8f, 0.4f,  1.0f}  // roxo
};
const int TOTAL_CORES = sizeof(paleta) / sizeof(paleta[0]);

// Transformação GLOBAL do homem (mexa aqui para posicionar a figura inteira)
float homemX = 0.0f, homemY = 0.0f, homemEsc = 1.0f;

// ============================================================
//  Cena original
// ============================================================
void desenharSol() {
    glColor3f(solR, solG, solB);
    float raio = 0.3f, centroX = 0.2f, centroY = 0.5f;

    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(centroX, centroY);
        for (float u = 0.0f; u <= 6.283f + 0.1f; u += 0.1f)
            glVertex2f(centroX + raio * cos(u), centroY + raio * sin(u));
    glEnd();
}

void desenharChao() {
    glColor3f(0.2f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f( 1.0f, -1.0f);
        glVertex2f( 1.0f, -0.4f);
        glVertex2f(-1.0f, -0.4f);
    glEnd();
}

// Elipse paramétrica: x = cx + rx*cos(u), y = cy + ry*sin(u)
void desenharElipse(float centroX, float centroY, float raioX, float raioY,
                    float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(centroX, centroY);
        for (float u = 0.0f; u <= 6.28318f + 0.1f; u += 0.05f)
            glVertex2f(centroX + raioX * cos(u), centroY + raioY * sin(u));
    glEnd();
}

void desenharCacto() {
    float r = 0.0f, g = 0.45f, b = 0.15f;
    desenharElipse(0.72f, -0.20f, 0.08f, 0.04f, r, g, b);
    desenharElipse(0.78f,  0.06f, 0.04f, 0.30f, r, g, b);
    desenharElipse(0.52f, -0.05f, 0.08f, 0.04f, r, g, b);
    desenharElipse(0.46f,  0.14f, 0.04f, 0.24f, r, g, b);
    desenharElipse(0.62f, -0.05f, 0.07f, 0.40f, r, g, b);
}

// ============================================================
//  Curva paramétrica de Bézier cúbica
//  B(t) = (1-t)^3 P0 + 3(1-t)^2 t P1 + 3(1-t) t^2 P2 + t^3 P3
// ============================================================
Ponto bezier(const Ponto p[4], float t) {
    float u = 1.0f - t;
    float b0 = u * u * u;
    float b1 = 3.0f * u * u * t;
    float b2 = 3.0f * u * t * t;
    float b3 = t * t * t;
    Ponto c;
    c.x = b0 * p[0].x + b1 * p[1].x + b2 * p[2].x + b3 * p[3].x;
    c.y = b0 * p[0].y + b1 * p[1].y + b2 * p[2].y + b3 * p[3].y;
    return c;
}

// Derivada B'(t): dá a direção da curva (usada para achar a "normal")
Ponto bezierDerivada(const Ponto p[4], float t) {
    float u = 1.0f - t;
    Ponto d;
    d.x = 3*u*u*(p[1].x-p[0].x) + 6*u*t*(p[2].x-p[1].x) + 3*t*t*(p[3].x-p[2].x);
    d.y = 3*u*u*(p[1].y-p[0].y) + 6*u*t*(p[2].y-p[1].y) + 3*t*t*(p[3].y-p[2].y);
    return d;
}

// Desenha um membro "gordinho": a Bézier é o esqueleto (eixo central) e a
// espessura w(t) varia linearmente de w0 até w1. Nas pontas entram círculos.
void desenharMembro(const Ponto p[4], float w0, float w1,
                    float r, float g, float b) {
    const int N = 40;
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= N; i++) {
        float t = (float)i / N;
        Ponto c = bezier(p, t);
        Ponto d = bezierDerivada(p, t);
        float len = sqrt(d.x * d.x + d.y * d.y);
        float nx = -d.y / len;  // normal = tangente girada 90°
        float ny =  d.x / len;
        float w = w0 + (w1 - w0) * t;
        glVertex2f(c.x + nx * w, c.y + ny * w);
        glVertex2f(c.x - nx * w, c.y - ny * w);
    }
    glEnd();
    desenharElipse(p[0].x, p[0].y, w0, w0, r, g, b); // ponta inicial arredondada
    desenharElipse(p[3].x, p[3].y, w1, w1, r, g, b); // ponta final arredondada
}

// ============================================================
//  Partes do corpo (cada uma no seu sistema de coordenadas local)
// ============================================================
const float PELE[3]    = {0.93f, 0.65f, 0.36f};
const float SOMBRA[3]  = {0.86f, 0.56f, 0.29f};
const float CLARA[3]   = {0.97f, 0.76f, 0.50f};

// Pé: origem no tornozelo, apontando para +x
void desenharPe() {
    // sola/dorso do pé
    desenharElipse(0.22f, 0.0f, 0.27f, 0.10f, PELE[0], PELE[1], PELE[2]);

    // dedos: cada um é filho do pé (herdam a rotação do pé)
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
            glTranslatef(0.47f - i * 0.03f, 0.035f - i * 0.045f, 0.0f);
            glRotatef(-10.0f * i, 0, 0, 1);
            glScalef(1.0f - 0.12f * i, 1.0f - 0.10f * i, 1.0f); // dedos menores
            desenharElipse(0, 0, 0.05f, 0.032f, SOMBRA[0], SOMBRA[1], SOMBRA[2]);
        glPopMatrix();
    }
    // unha do dedão
    glPushMatrix();
        glTranslatef(0.49f, 0.04f, 0.0f);
        desenharElipse(0, 0, 0.025f, 0.018f, CLARA[0], CLARA[1], CLARA[2]);
    glPopMatrix();
}

// Mão: origem no pulso
void desenharMao() {
    desenharElipse(0.05f, 0.0f, 0.09f, 0.05f, PELE[0], PELE[1], PELE[2]); // palma
    for (int i = 0; i < 3; i++) {                                         // dedos
        glPushMatrix();
            glTranslatef(0.13f, 0.03f - i * 0.03f, 0.0f);
            glRotatef(-15.0f * (i - 1), 0, 0, 1);
            desenharElipse(0, 0, 0.055f, 0.02f, PELE[0], PELE[1], PELE[2]);
        glPopMatrix();
    }
}

// Cabeça: origem no pescoço; elipse pequena + "cabelo" (meia elipse)
void desenharCabeca() {
    desenharElipse(0.0f, 0.0f, 0.06f, 0.035f, PELE[0], PELE[1], PELE[2]);

    glColor3f(0.10f, 0.07f, 0.05f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.0f);
        for (float u = 0.0f; u <= 3.1416f + 0.05f; u += 0.1f)
            glVertex2f(0.065f * cos(u), 0.038f * sin(u));
    glEnd();
}

// ============================================================
//  O homem completo: modelagem hierárquica com pilha de matrizes
// ============================================================
void desenharHomem() {
    glPushMatrix();
    // Transformação global da figura: escala + translação
    glTranslatef(homemX, homemY, 0.0f);
    glScalef(homemEsc, homemEsc, 1.0f);

    // Morro verde onde o homem está sentado (círculo achatado por escala)
    glPushMatrix();
        glTranslatef(-0.35f, -0.85f, 0.0f);
        glScalef(1.0f, 0.5f, 1.0f);
        desenharElipse(0, 0, 0.6f, 0.6f, 0.05f, 0.50f, 0.15f);
    glPopMatrix();

    // 1) Coxa de trás (mais escura, fica atrás de tudo)
    glPushMatrix();
        glTranslatef(-0.72f, -0.72f, 0.0f);
        const Ponto coxa[4] = {{0,0}, {-0.05f,0.35f}, {0.10f,0.60f}, {0.25f,0.90f}};
        desenharMembro(coxa, 0.13f, 0.10f, SOMBRA[0], SOMBRA[1], SOMBRA[2]);
    glPopMatrix();

    // 2) Pescoço + cabeça (a cabeça é "filha" do pescoço)
    glPushMatrix();
        glTranslatef(-0.24f, 0.20f, 0.0f);
        const Ponto pescoco[4] = {{0,0}, {-0.02f,0.10f}, {-0.05f,0.20f}, {-0.03f,0.30f}};
        desenharMembro(pescoco, 0.045f, 0.030f, PELE[0], PELE[1], PELE[2]);

        glTranslatef(pescoco[3].x, pescoco[3].y, 0.0f); // vai até o fim do pescoço
        glRotatef(30.0f, 0, 0, 1);                      // inclina a cabeça
        desenharCabeca();
    glPopMatrix();

    // 3) Perna grande (joelho no alto, canela descendo até o pé gigante)
    glPushMatrix();
        glTranslatef(-0.22f, 0.22f, 0.0f);
        const Ponto perna[4] = {{0,0}, {0.0f,-0.30f}, {0.12f,-0.50f}, {0.32f,-0.72f}};
        desenharMembro(perna, 0.15f, 0.10f, PELE[0], PELE[1], PELE[2]);

        // pé: filho da perna, posicionado no tornozelo (fim da curva)
        glPushMatrix();
            glTranslatef(perna[3].x, perna[3].y, 0.0f);
            glRotatef(-15.0f, 0, 0, 1);
            desenharPe();
        glPopMatrix();
    glPopMatrix();

    // 4) Perna fina da frente (do joelho até a mão apoiada no chão)
    glPushMatrix();
        glTranslatef(-0.40f, 0.30f, 0.0f);
        const Ponto fina[4] = {{0,0}, {-0.12f,-0.25f}, {-0.10f,-0.55f}, {0.06f,-1.0f}};
        desenharMembro(fina, 0.07f, 0.05f, CLARA[0], CLARA[1], CLARA[2]);

        // mão: filha da perna fina
        glPushMatrix();
            glTranslatef(fina[3].x, fina[3].y, 0.0f);
            glRotatef(-30.0f, 0, 0, 1);
            desenharMao();
        glPopMatrix();
    glPopMatrix();

    glPopMatrix(); // fim da transformação global
}

// ============================================================
//  Callbacks
// ============================================================
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    desenharSol();
    desenharChao();
    desenharCacto();
    desenharHomem();   //homem fica na frente do cacto

    glFlush();
}

void teclado(unsigned char tecla, int x, int y) {
    if (tecla == 'e' || tecla == 'E') {
        corAtual = (corAtual + 1) % TOTAL_CORES;
        solR = paleta[corAtual][0];
        solG = paleta[corAtual][1];
        solB = paleta[corAtual][2];
        glutPostRedisplay();
    }
}

void init() {
    glClearColor(0.4f, 0.7f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
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

    glutDisplayFunc(display);
    glutKeyboardFunc(teclado);

    glutMainLoop();
    return 0;
}