#include "utils.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <iostream>

extern bool bateu = false;

// auxiliar para random float entre -1 e 1
float rand_float_1_1() {
    return ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
}

// auxiliar para random float entre 0 e 1
float rand_float_0_1() {
    return (float)rand() / RAND_MAX;
}


// implementação do DNA ------
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

    return DNA(this->genes); 
}

void DNA::mutacao(float taxa_variavel) {
    for (auto& gene : genes) {
        // usa a taxa passada por parâmetro
        if (rand_float_0_1() < taxa_variavel) { 
            
            float anguloAtual = atan2(gene.second, gene.first);
            float desvio = rand_float_1_1() * 0.5f; 
            float novoAngulo = anguloAtual + desvio;
            
            gene.first = cos(novoAngulo) * MAX_FORCA;
            gene.second = sin(novoAngulo) * MAX_FORCA;
        }
    }
}


// implementação do peixe ------
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
    fitness = 1.0f / (d + 1.0f); 
    if (chegou) fitness *= 20.0f;
    if (bateu) fitness /= 20.0f;
}


// implementação da população ------
void Populacao::inicializar(int qtd, int largura, int altura) {
    peixes.clear();
    largura_mundo = largura;
    altura_mundo = altura;

    // configura grid A*
    cols = largura / TAM_GRID;
    rows = altura / TAM_GRID;
    
    // se spawn ainda não foi definido (primeira vez), define padrão
    if (spawn_x == 0 && spawn_y == 0) {
        spawn_x = largura / 2.0f;
        spawn_y = 50.0f;
        target_x = largura / 2.0f;
        target_y = altura - 50.0f;
    }

    recalcularMapaDistancias();
    
    frame_atual = 0;
    geracao_atual = 1;

    for (int i = 0; i < qtd; i++) {
        peixes.push_back(Peixe(spawn_x, spawn_y));
    }
}

void Populacao::recalcularMapaDistancias() {
    // inicializa com valor infinito
    mapaDistancia.assign(cols, std::vector<int>(rows, 999999));

    // marca paredes como -1
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            float cx = i * TAM_GRID + TAM_GRID/2;
            float cy = j * TAM_GRID + TAM_GRID/2;
            
            for (const auto& obs : obstaculos) {
                if (cx > obs.x && cx < obs.x + obs.w &&
                    cy > obs.y && cy < obs.y + obs.h) {
                    mapaDistancia[i][j] = -1; // bloqueado
                    break;
                }
            }
        }
    }

    // BFS a partir do Alvo
    int targetI = (int)target_x / TAM_GRID;
    int targetJ = (int)target_y / TAM_GRID;

    if (targetI < 0) targetI = 0; if (targetI >= cols) targetI = cols - 1;
    if (targetJ < 0) targetJ = 0; if (targetJ >= rows) targetJ = rows - 1;

    std::queue<std::pair<int, int>> fila;
    
    if (mapaDistancia[targetI][targetJ] != -1) {
        mapaDistancia[targetI][targetJ] = 0;
        fila.push({targetI, targetJ});
    }

    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    while (!fila.empty()) {
        auto [cx, cy] = fila.front();
        fila.pop();
        int distAtual = mapaDistancia[cx][cy];

        for (int k = 0; k < 4; k++) {
            int nx = cx + dx[k];
            int ny = cy + dy[k];

            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows) {
                if (mapaDistancia[nx][ny] > distAtual + 1) { // se achou caminho mais curto
                    mapaDistancia[nx][ny] = distAtual + 1;
                    fila.push({nx, ny});
                }
            }
        }
    }
}

int Populacao::getDistanciaDoMapa(float x, float y) {
    int i = (int)x / TAM_GRID;
    int j = (int)y / TAM_GRID;
    if (i < 0 || i >= cols || j < 0 || j >= rows) return 9999;
    int d = mapaDistancia[i][j];
    return (d == -1) ? 9999 : d;
}

void Populacao::setAlvo(float x, float y) {
    target_x = x;
    target_y = y;
    
    // se mudou o alvo, o mapa de distâncias muda completamente
    recalcularMapaDistancias();
    
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
    
    // se adicionou parede, o caminho muda
    recalcularMapaDistancias();
    
    frame_atual = 0;
    inicializar(peixes.size(), largura_mundo, altura_mundo);
}

void Populacao::limparObstaculos() {
    obstaculos.clear();
    
    recalcularMapaDistancias();
    
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
        // distância pelo labirinto (do mapa A*)
        int distGrid = getDistanciaDoMapa(p.x, p.y);
        
        // distância euclidiana direta (ajuste fino)
        float distEuclidiana = sqrt(pow(p.x - target_x, 2) + pow(p.y - target_y, 2));
        
        // score combinado: grid vale muito mais (TAM_GRID), euclidiana é o ajuste
        float distTotal = (float)distGrid * TAM_GRID + (distEuclidiana * 0.1f);
        
        p.fitness = 1.0f / (distTotal + 1.0f);
        
        if (p.chegou) {
            p.fitness *= 20.0f; // aplica bônus por chegar
            p.fitness *= (1.0f + (float)TEMPO_VIDA / (float)p.tempo_chegada);
        }
        if (p.bateu) {
            p.fitness /= 20.0f; // penaliza por bater
        }

        if (p.fitness > max_fit) max_fit = p.fitness;
    }

    if (max_fit == 0) max_fit = 1; // evita divisão por zero

    for (auto& p : peixes) 
        p.fitness /= max_fit;
}

DNA* torneio(const std::vector<Peixe>& pop) {
    int k = 5;
    int melhorIndex = -1;
    float melhorFit = -1.0f;

    for (int i = 0; i < k; i++) {
        int r = rand() % pop.size();
        if (pop[r].fitness > melhorFit) {
            melhorFit = pop[r].fitness;
            melhorIndex = r;
        }
    }
    return const_cast<DNA*>(&pop[melhorIndex].dna);
}

void Populacao::selecaoNatural() {
    std::vector<Peixe> nova_populacao;
    if (peixes.empty()) return;

    // aplica o elitismo
    int indexMelhor = 0;
    float maxFit = -1.0f;
    for (int i = 0; i < peixes.size(); i++) {
        if (peixes[i].fitness > maxFit) {
            maxFit = peixes[i].fitness;
            indexMelhor = i;
        }
    }
    
    // define uma taxa de mutação relativa a se o melhor peixe chegou ou não
    float taxa_atual; 
    if (peixes[indexMelhor].chegou) {
        taxa_atual = 0.01f; // taxa de 1% na fase de exploração fina
        bateu = true; // marca para a main conseguir diminuir o tempo de simulação
    }
    else taxa_atual = 0.1f; // taxa de 10% na fase de busca

    // o campeão passa sem sofrer mutação
    Peixe campeao(spawn_x, spawn_y, peixes[indexMelhor].dna);
    nova_populacao.push_back(campeao);

    // reprodução assexuada (clonagem + mutação)
    for (int i = 1; i < peixes.size(); i++) { // começa de 1 porque o 0 é o melhor
        DNA* pai = torneio(peixes); // torneio para selecionar o pai
        DNA filhoDNA = DNA(pai->genes); // clona o dna do pai
        filhoDNA.mutacao(taxa_atual); // muta
        
        Peixe filho(spawn_x, spawn_y, filhoDNA);
        nova_populacao.push_back(filho);
    }
    
    peixes = nova_populacao;
}