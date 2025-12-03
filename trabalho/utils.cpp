#include "utils.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <iostream>

// auxiliar para random float entre -1 e 1
float rand_float_1_1() {
    return ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
}

// auxiliar para random float entre 0 e 1
float rand_float_0_1() {
    return (float)rand() / RAND_MAX;
}

// implementação do DNA ----
DNA::DNA(int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        // vetor aleatório unitário (ou quase)
        float angle = rand_float_1_1() * 3.14159f;
        genes.push_back({cos(angle) * MAX_FORCA, sin(angle) * MAX_FORCA});
    }
}

DNA::DNA(std::vector<std::pair<float, float>> novos_genes) {
    genes = novos_genes;
}

DNA DNA::crossover(DNA& parceiro) {
    std::vector<std::pair<float, float>> novos_genes;

    if (genes.empty() || parceiro.genes.empty())
        return DNA(TEMPO_VIDA); // se o DNA estiver vazio, retorna um DNA novo aleatório para evitar crash

    // ponto de corte aleatório (meio do DNA)
    int ponto_corte = rand() % genes.size();

    for (int i = 0; i < genes.size(); i++) {
        if (i < ponto_corte)
            novos_genes.push_back(this->genes[i]); // pega do pai A
        else
            novos_genes.push_back(parceiro.genes[i]); // pega do pai B
    }
    return DNA(novos_genes);
}

void DNA::mutacao() {
    for (auto& gene : genes) {
        if (rand_float_0_1() < TAXA_MUTACAO) {
            // recria este passo totalmente aleatório
            float angle = rand_float_1_1() * 3.14159f;
            gene = {cos(angle) * MAX_FORCA, sin(angle) * MAX_FORCA};
        }
    }
}

// implementação do peixe ----
Peixe::Peixe(float x_ini, float y_ini) : dna(TEMPO_VIDA), x(x_ini), y(y_ini), vel_x(0), vel_y(0), acc_x(0), acc_y(0), bateu(false), chegou(false) {}

Peixe::Peixe(float x_ini, float y_ini, DNA dna_pai) : dna(dna_pai), x(x_ini), y(y_ini), vel_x(0), vel_y(0), acc_x(0), acc_y(0), bateu(false), chegou(false) {}

void Peixe::aplicarForca(std::pair<float, float> forca) {
    acc_x += forca.first;
    acc_y += forca.second;
}

void Peixe::update(int frame, float tx, float ty, int w, int h, const std::vector<Obstaculo>& obstaculos) {
    if (bateu || chegou) return;

    float d = sqrt(pow(x - tx, 2) + pow(y - ty, 2));
    if (d < 15.0f) { 
        chegou = true;
        tempo_chegada = frame;
        return;
    }

    // colisão com paredes da tela
    if (x < 0 || x > w || y < 0 || y > h) {
        bateu = true;
        return;
    }

    // colisão com obstáculos (retângulos)
    for (const auto& obs : obstaculos) {
        // lógica simples de AABB (Axis-Aligned Bounding Box)
        if (x > obs.x && x < obs.x + obs.w &&
            y > obs.y && y < obs.y + obs.h) {
            bateu = true;
            return;
        }
    }

    if (frame < dna.genes.size()) {
        aplicarForca(dna.genes[frame]);
    }

    vel_x += acc_x;
    vel_y += acc_y;
    x += vel_x;
    y += vel_y;
    acc_x = 0; acc_y = 0;
}

void Peixe::calcularFitness(float tx, float ty) {
    float d = sqrt(pow(x - tx, 2) + pow(y - ty, 2));
    
    // o fitness é o inverso da distância (1 / d)
    // usa exponencial para premiar muito quem chega perto.
    fitness = 1.0f / (d + 1.0f); 
    
    // bônus enormes
    if (chegou) {
        fitness *= 10.0f; // multiplica por 10 se chegou
        // bônus de velocidade (quanto mais rápido, melhor)
        fitness *= (1.0f + (float)TEMPO_VIDA / (float)tempo_chegada);
    }
    
    if (bateu) {
        fitness /= 10.0f; // penalidade severa se bateu na parede
    }
}

// implementação da população ----
void Populacao::inicializar(int qtd, int largura, int altura) {
    peixes.clear();
    largura_mundo = largura;
    altura_mundo = altura;
    
    // Se spawn ainda não foi definido (primeira vez), define padrão
    if (spawn_x == 0 && spawn_y == 0) {
        spawn_x = largura / 2.0f;
        spawn_y = 50.0f;
        target_x = largura / 2.0f;
        target_y = altura - 50.0f;
    }
    
    frame_atual = 0;
    geracao_atual = 1;

    for (int i = 0; i < qtd; i++) {
        peixes.push_back(Peixe(spawn_x, spawn_y));
    }
}

void Populacao::setAlvo(float x, float y) {
    target_x = x;
    target_y = y;
    // se mudar o alvo, reinicia a geração (senão eles aprendem o caminho errado)
    frame_atual = 0;
    inicializar(peixes.size(), largura_mundo, altura_mundo);
}

void Populacao::setSpawn(float x, float y) {
    spawn_x = x;
    spawn_y = y;
    frame_atual = 0;
    inicializar(peixes.size(), largura_mundo, altura_mundo);
}

void Populacao::adicionarObstaculo(float x, float y, float w, float h) {
    obstaculos.push_back({x, y, w, h});
    // reinicia a geração quando coloca obstáculo
    frame_atual = 0;
    inicializar(peixes.size(), largura_mundo, altura_mundo);
}

void Populacao::limparObstaculos() {
    obstaculos.clear();
    frame_atual = 0;
    inicializar(peixes.size(), largura_mundo, altura_mundo);
}

void Populacao::executarPasso() {
    // move todo mundo
    for (auto& p : peixes)
        p.update(frame_atual, target_x, target_y, largura_mundo, altura_mundo, obstaculos);
    
    frame_atual++;

    // se acabou o tempo da geração
    if (frame_atual >= TEMPO_VIDA) {
        avaliar();
        selecaoNatural();
        frame_atual = 0;
        geracao_atual++;
        std::cout << "Geracao #" << geracao_atual << " iniciada." << std::endl;
    }
}

void Populacao::avaliar() {
    float max_fit = 0;
    for (auto& p : peixes) {
        p.calcularFitness(target_x, target_y);
        if (p.fitness > max_fit) max_fit = p.fitness;
    }

    if (max_fit == 0) max_fit = 1; // evita divisão por zero

    for (auto& p : peixes) 
        p.fitness /= max_fit;
}

void Populacao::selecaoNatural() {
    std::vector<Peixe> nova_populacao;
    
    if (peixes.empty()) return;

    for (int i = 0; i < peixes.size(); i++) {
        DNA* paiA = nullptr; 
        DNA* paiB = nullptr;
        
        // seleção do pai A
        int tentativas = 0;
        while(paiA == nullptr) {
            int r = rand() % peixes.size();
            // tenta selecionar baseada na probabilidade
            if (rand_float_0_1() < peixes[r].fitness) { 
                paiA = &peixes[r].dna;
                break; 
            }
            
            // segurança contra loop infinito (caso todos fitness sejam 0)
            tentativas++;
            if (tentativas > 1000) {
                paiA = &peixes[rand() % peixes.size()].dna; // pega qualquer um
                break;
            }
        }

        // seleção do pai B
        tentativas = 0;
        while(paiB == nullptr) {
            int r = rand() % peixes.size();
            if (rand_float_0_1() < peixes[r].fitness) { 
                paiB = &peixes[r].dna;
                break; 
            }
            
            tentativas++;
            if (tentativas > 1000) {
                paiB = &peixes[rand() % peixes.size()].dna;
                break;
            }
        }

        DNA filhoDNA = paiA->crossover(*paiB);
        filhoDNA.mutacao();
        
        Peixe filho(spawn_x, spawn_y, filhoDNA);
        nova_populacao.push_back(filho);
    }
    peixes = nova_populacao;
}