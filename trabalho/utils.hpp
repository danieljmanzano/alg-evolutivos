#ifndef CARDUME_HPP
#define CARDUME_HPP

#include <vector>
#include <utility> // para std::pair
#include <queue>   // NOVA: Necessário para o BFS (algoritmo A*)

// parâmetros
#define TEMPO_VIDA 300 // quantos frames dura uma geração
#define MAX_FORCA 0.5f // aceleração máxima por frame
#define TAM_GRID 20    // tamanho da célula do grid para o pathfinding

extern bool bateu; // variável global para indicar se o peixe bateu

struct Obstaculo {
    float x, y, w, h; // w == width, h == height
};

struct DNA {
    // o genótipo é um vetor de vetores (forças x, y)
    std::vector<std::pair<float, float>> genes;

    DNA(int tamanho); // construtor aleatório
    DNA(std::vector<std::pair<float, float>> novos_genes); // construtor herdado

    void mutacao(float taxa_mutacao);
    DNA crossover(DNA& parceiro);
};

struct Peixe {
    float x, y;
    float vel_x, vel_y;
    float acc_x, acc_y;
    
    DNA dna;
    float fitness;
    
    bool bateu; // bateu na parede (morreu)
    bool chegou; // chegou na comida
    int tempo_chegada; // para dar mais valor a quem chega mais rápido

    Peixe(float x_ini, float y_ini); // construtor aleatório
    Peixe(float x_ini, float y_ini, DNA dna_pai); // construtor herdado

    void aplicarForca(std::pair<float, float> forca); // aplica uma força ao peixe
    void update(int frame, float target_x, float target_y, int largura, int altura, const std::vector<Obstaculo>& obstaculos); // atualiza a física do peixe
    void calcularFitness(float target_x, float target_y); // calcula o fitness do peixe (mantido, mas vamos usar uma lógica superior no avaliar)
};

class Populacao {
public:
    std::vector<Peixe> peixes; // peixes na população
    std::vector<Obstaculo> obstaculos; // obstáculos no ambiente
    
    // variáveis para o mapa de distância (usado no A*)
    std::vector<std::vector<int>> mapaDistancia; 
    int cols, rows;

    float target_x, target_y; // posição do alvo (comida)
    float spawn_x, spawn_y; // posição de spawn dos peixes
    int frame_atual;
    int geracao_atual;
    int largura_mundo, altura_mundo;

    void inicializar(int qtd, int largura, int altura);
    void executarPasso();
    
    // funções de interação
    void setAlvo(float x, float y);
    void setSpawn(float x, float y);
    void adicionarObstaculo(float x, float y, float w, float h);
    void limparObstaculos();

    // --- NOVO: Funções auxiliares do A* ---
    void recalcularMapaDistancias(); 
    int getDistanciaDoMapa(float x, float y);
    // --------------------------------------

private:
    void avaliar();
    void selecaoNatural();
};

#endif