#include <random>
#include <vector>

using namespace std;

#define BRANCO 1
#define VERDE 2
#define LARANJA 3
#define AMARELO 4

struct Peixe {
    float x, y;
    float peso;
    float delta_x, delta_y; // deslocamento
    int cor; // representação da cor do peixe feita por meio de números específicos

    // construtor
    Peixe(float x_inicial, float y_inicial, float peso_inicial, int cor): 
        x(x_inicial), y(y_inicial), peso(peso_inicial), delta_x(0), delta_y(0), cor(cor) {}
};

class Cardume {
public:
    vector<Peixe> peixes;
    float target_x, target_y;

    // inicializa o cardume com a quantidade de peixes e dimensões do ambiente
    void inicializar(int qtd_peixes, int largura, int altura);
    
    // executa um passo da simulação
    void executarPasso() {
        alimentacao();
        movimentoIndividual();
        movimentoColetivo();
    }
    
    // atualiza o alvo quando o usuário clica
    void setAlvo(float x, float y) {
        target_x = x;
        target_y = y;
    }

private:
    void alimentacao(); 
    void movimentoIndividual();
    void movimentoColetivo();
};

