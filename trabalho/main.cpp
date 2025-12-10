#include <GL/glut.h> // biblioteca OpenGL
#include <iostream>
#include <random> // biblioteca para números aleatórios
#include <vector>
#include <chrono>
#include <thread> // biblioteca usada para delay na execução
#include "utils.hpp"

using namespace std;

// gerador de números aleatórios
default_random_engine gerador(chrono::system_clock::now().time_since_epoch().count());

// máquina de estados que avalia o que o mouse está fazendo agora
enum Modo { MODO_ALVO, MODO_SPAWN, MODO_OBSTACULO };
Modo modoAtual = MODO_ALVO; // começa movendo a comida

bool simulacaoRodando = false;
Populacao populacao;
int LARGURA_TELA = 800;
int ALTURA_TELA = 600;

void desenharTexto(float x, float y, string texto) {
    glRasterPos2f(x, y);
    for (char c : texto) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

// função que desenha a tela
void display() {
    glClear(GL_COLOR_BUFFER_BIT); 

    // desenha spawn (círculo azul claro)
    glColor3f(0.0, 0.8, 1.0);
    glPointSize(15.0);
    glBegin(GL_POINTS);
        glVertex2f(populacao.spawn_x, populacao.spawn_y);
    glEnd();

    // desenha alvo (círculo vermelho)
    glColor3f(1.0, 0.0, 0.0);
    glPointSize(15.0);
    glBegin(GL_POINTS);
        glVertex2f(populacao.target_x, populacao.target_y);
    glEnd();

    // desenha obstáculos (retângulos brancos)
    glColor3f(0.8, 0.8, 0.8);
    for (const auto& obs : populacao.obstaculos) {
        glRectf(obs.x, obs.y, obs.x + obs.w, obs.y + obs.h);
    }

    // desenha peixes
    glPointSize(4.0);
    glBegin(GL_POINTS);
    for (const auto& p : populacao.peixes) {
        if (p.chegou) glColor3f(0.0, 1.0, 0.0); // verde (Sucesso)
        else if (p.bateu) glColor3f(0.3, 0.3, 0.3); // cinza (Morto)
        else glColor3f(0.0, 0.0, 1.0); // azul (vivo)
        
        glVertex2f(p.x, p.y);
    }
    glEnd();
    
    // desenha texto informativo
    glColor3f(1.0, 1.0, 1.0); // texto branco
    
    if (!simulacaoRodando) {
        desenharTexto(10, ALTURA_TELA - 30, "MODO CONFIGURACAO (Pausado)");
        desenharTexto(10, ALTURA_TELA - 50, "Configure o mapa e aperte [ENTER] para iniciar");
    } else {
        string genInfo = "Geracao: " + to_string(populacao.geracao_atual);
        desenharTexto(10, ALTURA_TELA - 30, "SIMULANDO...");
        desenharTexto(10, ALTURA_TELA - 50, genInfo);
    }

    // mostrar modo atual do mouse
    string modoStr = "";
    if (modoAtual == MODO_ALVO) modoStr = "[1] Mover Alvo";
    else if (modoAtual == MODO_SPAWN) modoStr = "[2] Mover Ninho";
    else if (modoAtual == MODO_OBSTACULO) modoStr = "[3] Criar Muro";
    
    desenharTexto(10, 20, "Modo Mouse: " + modoStr);

    glutSwapBuffers(); 
}

// função chamada repetidamente
void idle() {
    if (simulacaoRodando) {
        if (!global_chegou) {
            if (!acelera_simulacao)
                populacao.executarPasso();
            else
                for (int i = 0; i < 100; i++) populacao.executarPasso(); // simulação rápida
        } else if (global_chegou) {
            if (!acelera_simulacao)
                populacao.executarPasso();
            else
                for (int i = 0; i < 100; i++) populacao.executarPasso(); // simulação rápida
        }
    }
    glutPostRedisplay(); 
}

// função que trata interatividade do usuário (por meio do mouse)
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float mouseX = x;
        float mouseY = ALTURA_TELA - y; // inverter Y do OpenGL

        if (modoAtual == MODO_ALVO) {
            populacao.setAlvo(mouseX, mouseY);
            cout << "Novo Alvo definido!" << endl;
        } 
        else if (modoAtual == MODO_SPAWN) {
            populacao.setSpawn(mouseX, mouseY);
            cout << "Novo Ninho definido!" << endl;
        } 
        else if (modoAtual == MODO_OBSTACULO) {
            // cria um bloco de 40x40 pixels onde clicou
            populacao.adicionarObstaculo(mouseX - 20, mouseY - 20, 40, 40);
            cout << "Obstaculo criado!" << endl;
        }
    }
    // botão direito para limpar obstáculos
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        populacao.limparObstaculos();
        cout << "Obstaculos removidos." << endl;
    }
}

// função que trata entradas necessárias para a simulação
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 13: // tecla enter
            simulacaoRodando = !simulacaoRodando; // alterna entre rodando e pausado
            if (simulacaoRodando) cout << "Simulacao INICIADA!" << endl;
            else cout << "Simulacao PAUSADA!" << endl;
            break;
        case '1':
            modoAtual = MODO_ALVO;
            break;
        case '2':
            modoAtual = MODO_SPAWN;
            break;
        case '3':
            modoAtual = MODO_OBSTACULO;
            break;
        case 'r': 
        case 'R':
            populacao.inicializar(populacao.peixes.size(), LARGURA_TELA, ALTURA_TELA);
            simulacaoRodando = false; // reseta e pausa para configurar de novo
            break;
        case 27: // ESC
            exit(0);
            break;

        /* controle da simulação especificamente após algum peixe ter encontrado a comida */
        // adição especificamente para a apresentação do trabalho
        case 'a': 
        case 'A':
            acelera_simulacao = true;
            break;
        case 'd':
        case 'D':
            acelera_simulacao = false;
            break;
    }
}

void setup() {
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f); // fundo azul
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, LARGURA_TELA, 0, ALTURA_TELA);
}

int main(int argc, char** argv) {
    populacao.inicializar(100, LARGURA_TELA, ALTURA_TELA); // 100 peixes

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(LARGURA_TELA, ALTURA_TELA);
    glutCreateWindow("Evolucao Pathfinding - Use 1, 2, 3");
    
    setup();
    
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard); // registra o teclado

    cout << "--- CONTROLES ---" << endl;
    cout << "[1] Modo Alvo (Clique para mover a comida)" << endl;
    cout << "[2] Modo Ninho (Clique para mover o nascimento)" << endl;
    cout << "[3] Modo Obstaculo (Clique para criar paredes)" << endl;
    cout << "[Botao Direito] Limpar Obstaculos" << endl;

    glutMainLoop();
    return 0;
}