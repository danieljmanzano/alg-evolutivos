#include "Cardume.hpp"
#include <cmath> // para sqrt e pow
#include <cstdlib> // para rand()
#include <algorithm> // para std::max e std::min

// parâmetros do FSS
const float STEP_INDIVIDUAL = 20.0f; // tamanho do passo individual
const float STEP_VOLITIVE = 15.0f;   // tamanho do passo coletivo
const float W_SCALE = 10.0f;         // o quanto o peso muda por iteração
const float MIN_WEIGHT = 1.0f;       // peso mínimo de um peixe
const float MAX_WEIGHT = 50.0f;      // peso máximo

// função auxiliar para gerar float aleatório entre -1 e 1
float randClamped() {
    return ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
}

// função auxiliar: Distância Euclidiana (Fitness)
float calcularDistancia(float px, float py, float tx, float ty) {
    return sqrt(pow(px - tx, 2) + pow(py - ty, 2));
}

void Cardume::inicializar(int qtd_peixes, int largura, int altura) {
    // define um alvo inicial no centro
    target_x = largura / 2.0f;
    target_y = altura / 2.0f;

    peixes.clear();
    for (int i = 0; i < qtd_peixes; i++) {
        // posição aleatória na tela
        float x = (float)(rand() % largura);
        float y = (float)(rand() % altura);
        // peso inicial médio
        float w = MIN_WEIGHT + (MAX_WEIGHT - MIN_WEIGHT) / 2.0f;
        
        // cria o peixe com cor sortida
        int cores = 5;
        int cor = (rand() % cores) + 1; // gera número entre 1 e 5
        peixes.push_back(Peixe(x, y, w, cor));
    }
}

void Cardume::movimentoIndividual() {
    for (auto& p : peixes) {
        // guarda a posição anterior e a distância anterior
        float x_ant = p.x;
        float y_ant = p.y;
        float dist_ant = calcularDistancia(p.x, p.y, target_x, target_y);

        // tenta um movimento aleatório
        float angulo = randClamped() * 3.14159f; // ângulo aleatório
        p.delta_x = cos(angulo) * STEP_INDIVIDUAL;
        p.delta_y = sin(angulo) * STEP_INDIVIDUAL;

        p.x += p.delta_x;
        p.y += p.delta_y;

        // verifica se melhorou (se aproximou do alvo)
        float dist_nova = calcularDistancia(p.x, p.y, target_x, target_y);

        // de acordo com o FSS, se não melhorou, não vai para a nova posição
        if (dist_nova > dist_ant) {
            p.x = x_ant;
            p.y = y_ant;
            p.delta_x = 0;
            p.delta_y = 0;
        } 
        // se melhorou, ele fica na nova posição.
    }
}

void Cardume::alimentacao() {
    float max_delta_fitness = 0.0001f; // evitar divisão por zero

    // busca o maior delta fitness para normalizar o peso
    for (auto& p : peixes) {
        // reconstrói a posição anterior subtraindo o delta
        float x_ant = p.x - p.delta_x;
        float y_ant = p.y - p.delta_y;
        
        float dist_atual = calcularDistancia(p.x, p.y, target_x, target_y);
        float dist_ant = calcularDistancia(x_ant, y_ant, target_x, target_y);
        
        // delta fitness positivo significa que a distância diminuiu (melhorou)
        float delta = dist_ant - dist_atual;
        
        if (abs(delta) > max_delta_fitness)
            max_delta_fitness = abs(delta);
    }

    // atualiza os pesos
    for (auto& p : peixes) {
        float x_ant = p.x - p.delta_x;
        float y_ant = p.y - p.delta_y;
        
        float dist_atual = calcularDistancia(p.x, p.y, target_x, target_y);
        float dist_ant = calcularDistancia(x_ant, y_ant, target_x, target_y);
        
        float delta_fitness = dist_ant - dist_atual;

        // fórmula simplificada de alimentação: Peso = Peso + (Melhoria / Melhoria_Máxima) * Escala
        float ganho = (delta_fitness / max_delta_fitness) * W_SCALE;
        
        p.peso += ganho;

        // garante limites de peso
        if (p.peso > MAX_WEIGHT) p.peso = MAX_WEIGHT;
        if (p.peso < MIN_WEIGHT) p.peso = MIN_WEIGHT;
    }
}

void Cardume::movimentoColetivo() {
    // calcular o baricentro (centro de massa) do cardume
    float soma_pesos = 0;
    float soma_x_peso = 0;
    float soma_y_peso = 0;
    
    // variável para saber se o cardume como um todo ganhou peso
    float peso_total_atual = 0;
    
    for (const auto& p : peixes) {
        soma_pesos += p.peso;
        soma_x_peso += p.x * p.peso;
        soma_y_peso += p.y * p.peso;
    }

    float baricentro_x = soma_x_peso / soma_pesos;
    float baricentro_y = soma_y_peso / soma_pesos;

    static float peso_total_anterior = 0;
    
    bool peso_aumentou = soma_pesos > peso_total_anterior;
    peso_total_anterior = soma_pesos;

    float direcao = peso_aumentou ? -1.0f : 1.0f; 
    // -1 = contração (atrai para o baricentro)
    // +1 = dilatação (foge do baricentro)

    for (auto& p : peixes) {
        float dist_baricentro = calcularDistancia(p.x, p.y, baricentro_x, baricentro_y);
        
        if (dist_baricentro > 0.001f) { // evita divisão por zero
            // o passo é proporcional à distância do centro e ao peso
            float step = STEP_VOLITIVE * randClamped();
            
            p.x += step * (p.x - baricentro_x) / dist_baricentro * direcao;
            p.y += step * (p.y - baricentro_y) / dist_baricentro * direcao;
        }
    }
}