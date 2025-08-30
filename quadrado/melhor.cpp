#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>

using namespace std;


/* indivíduo da população */
typedef struct individuo {
    vector<int> genes; // o vetor de genes contém 5 números (que podem ser 0 ou 1)
                       // é usado para encontrar o valor representado pelo indivíduo
    double fitness;
} Individuo;


/* informações para geração de números aleatórios */
random_device rd; // obtém alguma informação aleatória do dispositivo. será usada para gerar um número verdadeiramente aleatório
mt19937 gen(rd()); // usando rd como semente, cria um gerador de números aleatórios
uniform_real_distribution<> dis(0.0, 1.0); // garante chances iguais de os números entre 0.0 e 1.0 serem escolhidos aleatoriamente
// tudo isso é usado apenas para a criação de um número aleatório quando necessário no código
// a vantagem de fazer tudo isso ao invés de usar o rand() é pelo fato de realmente gerar um número quase aleatório


/* constantes */
const int TAM_POPULACAO = 5;
const int NUM_GERACOES = 8;
const int TAM_CROMOSSOMO = 7;
const double TAXA_MUTACAO = 0.1;
const double TAXA_CROSSOVER = 0.7;
const int IDEAL = 16384; // o valor ideal que queremos alcançar (128²)


int binario_para_int(vector<int> num) {
    int resp = 0;
    for (int i = 0; i < num.size(); i++) 
        resp += num[i] * pow(2, num.size() - 1 - i);
    
    return resp;
}

// como estamos verificando o valor máximo de f(x) = x², o fitness é o valor representado nos genes ao quadrado
int calcula_fitness(vector<int> genes) {
    int num = binario_para_int(genes);
    return num * num;
}


vector<Individuo> inicializa_populacao() {
    vector<Individuo> pop(TAM_POPULACAO);
    uniform_int_distribution<> dis_int(0, 1); // aleatoriamente decide um valor de 0 ou 1

    for (int i = 0; i < TAM_POPULACAO; i++) {
        pop[i].genes.resize(TAM_CROMOSSOMO); // acerta o tamanho do vetor representando os genes
        for (int j = 0; j < TAM_CROMOSSOMO; j++) pop[i].genes[j] = dis_int(gen); // aleatoriza os cromossomos do dado indivíduo
        pop[i].fitness = calcula_fitness(pop[i].genes);
    }

    return pop;
}


void crossover(const Individuo& pai1, const Individuo& pai2, Individuo& filho) {
    uniform_int_distribution<> dis_ponto(1, TAM_CROMOSSOMO - 2); // ponto de crossover não pode ser o primeiro nem o último
    int ponto_crossover = dis_ponto(gen);
    filho = pai1;
    
    // troca os genes a partir do ponto de crossover
    for (int i = ponto_crossover; i < TAM_CROMOSSOMO; i++) 
        filho.genes[i] = pai2.genes[i];
}


void mutacao(Individuo& individuo) {
    for (int i = 0; i < TAM_CROMOSSOMO; i++) {
        if (dis(gen) < TAXA_MUTACAO) { // se o número aleatório for menor que a taxa de mutação, o gene é mutado
            individuo.genes[i] = 1 - individuo.genes[i]; // inverte o gene (0 vira 1 e 1 vira 0)
        }
    }
    individuo.fitness = calcula_fitness(individuo.genes); 
}


Individuo encontrar_melhor(const vector<Individuo>& populacao) {
    Individuo melhor = populacao[0];
    for (const auto& ind : populacao) 
        if (ind.fitness > melhor.fitness) 
            melhor = ind;
        
    return melhor;
}


int encontrar_idx_melhor(const vector<Individuo>& populacao) {
    int idx_melhor = 0;
    for (int i = 1; i < populacao.size(); i++) 
        if (populacao[i].fitness > populacao[idx_melhor].fitness) 
            idx_melhor = i;
    
    return idx_melhor;
}


void exibe_estatisticas(vector<Individuo> populacao, int geracao) {
    Individuo melhor = encontrar_melhor(populacao);
    double soma_fitness = 0.0;
    for (const auto& ind : populacao) soma_fitness += ind.fitness;
    
    double media_fitness = soma_fitness / populacao.size();

    cout << "Geração " << geracao << ": " << endl;
    cout << "  Melhor fitness: " << melhor.fitness << " (x = " << binario_para_int(melhor.genes) << ")" << endl;
    cout << "  Fitness médio: " << media_fitness << endl;
    cout << "  Diferença para o ideal: " << IDEAL - melhor.fitness << endl << endl;
}


int main(void) {
    vector<Individuo> populacao = inicializa_populacao();

    cout << "--------------------------" << endl;
    cout << "Tamanho da população: " << TAM_POPULACAO << endl;
    cout << "Número de gerações: " << NUM_GERACOES << endl;
    cout << "Tamanho do cromossomo: " << TAM_CROMOSSOMO << endl;
    cout << "Taxa de mutação: " << TAXA_MUTACAO << endl;
    cout << "Taxa de crossover: " << TAXA_CROSSOVER << endl;
    cout << "--------------------------" << endl << endl;

    // loop de evolução
    for (int i = 0; i < NUM_GERACOES; i++) {
        vector<Individuo> nova_populacao;

        Individuo melhor = encontrar_melhor(populacao); // elitismo. mantém o melhor
        nova_populacao.push_back(melhor);
        int idx_melhor = encontrar_idx_melhor(populacao);

        // loop para criação de novos indivíduos
        for (int j = 0; j <= TAM_POPULACAO - 1; j++) {
            if (j == idx_melhor) continue; // não clona o melhor denovo
            if (nova_populacao.size() >= TAM_POPULACAO) break; // se já encheu a nova população, para

            Individuo pai = populacao[j];
            Individuo filho = pai; // por padrão, o novo filho é uma cópia do pai. o crossover decide se ele herdará genes do melhor

            if (dis(gen) < TAXA_CROSSOVER)  // decide se haverá crossover
                crossover(melhor, pai, filho);

            mutacao(filho);
            nova_populacao.push_back(filho); 
        }

        populacao = nova_populacao;

        exibe_estatisticas(populacao, i); // exibe as estatísticas a cada 5 gerações

        if (melhor.fitness == IDEAL) {
            cout << "Ponto ideal encontrado na geração " << i << "!" << endl;
            break; // se encontrar o ideal, para a execução
        }
    }

    Individuo melhor_final = encontrar_melhor(populacao);
    cout << "\n=== RESULTADO FINAL ===" << endl;
    cout << "Melhor indivíduo: ";
    for (int gene : melhor_final.genes) {
        cout << gene;
    }
    cout << " = " << binario_para_int(melhor_final.genes);
    cout << " (fitness = " << melhor_final.fitness << ")" << endl;
    return 0;

}