#include <GL/glut.h> // biblioteca OpenGL
#include <iostream>
#include <random> // biblioteca para números aleatórios
                  // usada para gerar posições iniciais e cores aleatórias dos peixes
#include <vector>
#include <chrono>
#include <thread> // biblioteca usada para delay na execução
#include "Cardume.hpp"

using namespace std;

// gerador de números aleatórios
default_random_engine gerador(chrono::system_clock::now().time_since_epoch().count());

Cardume cardume;
int LARGURA_TELA = 800;
int ALTURA_TELA = 600;

// função que desenha na tela
void display() {
    glClear(GL_COLOR_BUFFER_BIT); // limpa a tela
    
    // desenhar a comida (alvo)
    glColor3f(1.0, 0.0, 0.0); // vermelho
    glPointSize(10.0);
    glBegin(GL_POINTS);
        glVertex2f(cardume.target_x, cardume.target_y);
    glEnd();

    // desenhar os peixes
    glPointSize(4.0);
    glBegin(GL_POINTS);
    for (const auto& peixe : cardume.peixes) {
        switch (peixe.cor) {
            case BRANCO:
                glColor3f(1.0f, 1.0f, 1.0f);
                break;
            case VERDE:
                glColor3f(0.0f, 1.0f, 0.0f); 
                break;
            case LARANJA:
                glColor3f(1.0f, 0.65f, 0.0f); // RGB aproximado do laranja
                break;
            case AMARELO:
                glColor3f(1.0f, 1.0f, 0.0f);
                break;
            default:
                glColor3f(0.2f, 0.2f, 0.2f); // preto (cor padrão caso falhe)
                break;
        }
        glVertex2f(peixe.x, peixe.y);
    }

    glEnd();

    // troca os buffers (animação suave)
    glutSwapBuffers(); 
}

// função chamada repetidamente
void idle() {
    this_thread::sleep_for(chrono::milliseconds(30)); // controla a velocidade da simulação
    cardume.executarPasso(); // roda um ciclo do algoritmo FSS
    glutPostRedisplay(); // manda redesenhar a tela
}

// função que trata interatividade do usuário (por meio do mouse)
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        cardume.setAlvo(x, ALTURA_TELA - y);
}

void setup() {
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f); // cor de fundo azul escuro
    // configura a tela para usar pixels (0 a 800), não coordenadas normalizadas (-1 a 1)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, LARGURA_TELA, 0, ALTURA_TELA);
}

int main(int argc, char** argv) {
    cout << "Quantos peixes no cardume? ";
    int qtd_peixes;
    cin >> qtd_peixes;

    cardume.inicializar(qtd_peixes, LARGURA_TELA, ALTURA_TELA);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(LARGURA_TELA, ALTURA_TELA);
    glutCreateWindow("FSS - Busca em Cardume Interativa");
    
    setup();
    
    glutDisplayFunc(display); // desenha
    glutIdleFunc(idle); // calcula iterações
    glutMouseFunc(mouse); // escuta o mouse

    glutMainLoop(); // roda o programa
    return 0;
}