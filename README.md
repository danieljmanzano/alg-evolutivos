### Trabalho desenvolvido para a disciplina SSC0713 - Sistemas Evolutivos Aplicados à Robótica

Alunos:
Artur Kenzo Obara Kawazoe - 15652663 \
Daniel Jorge Manzano - 15446861 \
Larissa Pires Moreira Rocha Duarte - 15522358

## Introdução

O projeto desenvolvido foi um algoritmo genético que simula um cardume de peixes em busca de comida. Quando uma nova população é criada, os peixes seguem um caminho aleatório até que um deles
 chegue à comida. Esse peixe, então, passa seus genes para as próximas gerações por meio de reprodução assexuada (clonagem+mutação), certificando-se que não ocorre mutação no peixe campeão. Assim, nas gerações seguintes, cada vez mais peixes seguem
 o caminho correto. 

 A simulação é interativa, ou seja, antes de iniciar uma nova população, é possível escolher quais serão as posições iniciais do cardume e das comidas, bem como adicionar obstáculos.

## Instalação

Para rodar o programa, é preciso instalar a biblioteca OpenGL. No Linux, rode o seguinte comando no terminal:
```
    sudo apt-get install libglew-dev libglu1-mesa-dev freeglut3-dev
```
Em seguida, clone o repositório localmente e navegue até ele para ter acesso aos códigos em questão rodando os seguintes comandos no terminal:
```
    git clone https://github.com/danieljmanzano/alg-evolutivos
    cd alg-evolutivos/trabalho
```

## Execução

Para executar os códigos, use os seguintes comandos:
```
    make
    make run
```
Após executá-los, será gerada uma tela do simulador com o OpenGL. Use o mouse para posicionar o elemento desejado, alterando o elemento em questão selecionando as opções com as teclas 1 (posicionar alvo/comida), 2 (posicionar ninho, de onde saem os peixes) e 3 (posicionar obstáculos). Por fim, pressione Enter para iniciar a simulação. Caso deseje pará-la em algum momento, pressione ESC sobre a tela do simulador ou CTRL+C no terminal.
Para limpar o executável gerado pelos códigos, no terminal, digite:
```
    make clean
```
## Funcionamento

O funcionamento dos códigos, visualizado na simulação, itera repetidamente sobre uma população de 100 peixes que busca o melhor fitness no caminho pela comida. Por padrão, enquanto nenhum peixe chega ao alvo, a simulação é visualizada de forma acelerada; quando pelo menos um o encontra, a simulação desacelera a um tempo "normal". Para alterar a velocidade da simulação a qualquer momento, digite sobre a tela **a** para acelerar e **d** para desacelerar.

Pode também ser criados obstáculos por meio da modificação do arquivo "obstaculos.txt". Nele, cada linha representa um obstáculo, com as informações coordenada x, coordenada y, largura e altura, em ordem.

Além da simulação visual com o OpenGL, o código printa no terminal, a cada iteração: o número da geração, o fitness do melhor indivíduo e o fitness médio da população.

## Explicação conceitual

Na modelagem desse problema, cada indivíduo possui um genoma que é uma coleção de genes, cada gene é um par de movimentos para os eixos x e y, assim cada genoma é uma sequência de movimentos.

O fitness de cada indivíduo é medido combinando a distância Euclidiana (ajuste fino) com a distância de caminho mínimo no grid (calculada via busca em largura/BFS), que considera o desvio real dos obstáculos e uma discretização do espaço. O fitness é dado pelo inverso da soma desses valores, quando o indivíduo colide com um obstáculo ele recebe uma penalidade, quando ele acerta o objetivo ele recebe um bônus por acertar e um bônus inversamente proporcional à quantidade de passos para chegar, recompensando soluções mais rápidas.

A forma de reprodução escolhida foi a clonagem com mutação, pois no contexto desse algoritmo, a reprodução assexuada funciona melhor que o crossover, considerando que, se dois indivíduos com caminhos ótimos distintos se reproduzem, o filho pode seguir um caminho diferente que sai da rota ideal, enquanto a clonagem preserva a trajetória do pai. O melhor indivíduo de cada geração sempre se reproduz (elitismo), os outros são selecionados por meio de torneios com indivíduos aleatórios. Enquanto o programa não encontra uma solução possível, a mutação tem uma taxa elevada, após achar a primeira solução possível a mutação é reduzida para realizar ajustes finos, caso o programa estagne em uma solução, a mutação aumentará para buscar mais soluções.

Atualmente no código, a taxa de mutação quando a solução para de melhorar é de 12%, por alcançar a melhor fitness dentre as taxas testadas. A seguir está o teste realizado:

<figure>
 <img width="1920" height="1020" alt="imagemreadme" src="https://github.com/user-attachments/assets/f03951f2-4d7a-4dc8-a56c-2ed990a94d21" /> 
 <figcaption>Imagem do teste</figcaption> 
</figure>


Nos gráficos a seguir, a linha vermelha representa o melhor fitnesss por geração e, a azul, o finess médio
<figure>
 <img width="600" height="371" alt="chart" src="https://github.com/user-attachments/assets/073887dd-4d5a-4797-aedd-8be1f2af650e" />
 <figcaption>Gráfico com mutação de 10%</figcaption>
</figure>

<figure> 
 <img width="600" height="371" alt="chart(1)" src="https://github.com/user-attachments/assets/ef5e18e6-d19f-4ede-9712-d9af3a41116f" />
 <figcaption>Gráfico com mutação de 12%</figcaption>
</figure>

<figure>
 <img width="600" height="371" alt="chart(2)" src="https://github.com/user-attachments/assets/ff86d305-4817-4483-807f-e75c2af36125" />
 <figcaption>Gráfico com mutação de 14%</figcaption>
</figure>

Todos os gráficos apresentam um comportamento similar: uma solução nova é encontrada; o fitness médio decola; a solução estagna; a mutação aumenta; o fitness médio despenca, devido à criação de indivíduos muito diferentes do máximo local; uma nova solução ótima é encontrada e o ciclo se repete. Pelos resultados observados, a mutação de 12% apresentou o melhor desempenho. Dessa forma, foi a escolhida para integrar o código final.

Foi testado também o uso de uma taxa de mutação que incrementa gradualmente conforme mais tempo passa estagnado.
<img width="1920" height="1020" alt="image" src="https://github.com/user-attachments/assets/7efdcc7f-aa40-4709-81d3-18ccc9b8ff54" /> Teste aplicado


<img width="600" height="371" alt="chart(3)" src="https://github.com/user-attachments/assets/f714e3ee-1801-4b10-a71c-458533d85b6a" /> Mutação de 5% nas primeiras 10 gerações após detectar estagnação, 12% em diante.

<img width="600" height="371" alt="chart(4)" src="https://github.com/user-attachments/assets/3a302654-8300-4ac1-97ff-71e2a4b5ec36" /> Mutação de 12% após detectar estagnação.

Como o teste demonstra o aumento gradual apresentou pouquíssima diferença do aumento brusco, levando à decisão de ter somente duas etapas de mutação após encontrar o alvo, 1% e 12%.


# Estrutura e Implementação

A modularização do código foi feita de modo a separar as lógicas evolutivas da interface gráfica, de modo que a organização ficou da seguinte forma:

- ```main.cpp```: Arquivo que controla a lógica e fluxo de informações gerais do usuário em relação à interface, além de usar, mas não implementar, os "métodos evolutivos" do projeto (chamadas às funções da classe ```Populacao```).

- ```utils.cpp / utils.hpp```: Códigos com os algoritmos que aplicam as lógicas evolutivas em si. Como fatores auxiliares, implementam estruturas de dados (o vetor de forças ```DNA```, o agente ```Peixe``` e o ```Obstaculo```), lógicas físicas do movimento, cálculos relacionados aos algoritmos de caminhos (como a implementação da distância calculada com o algoritmo de BFS), entre outros métodos. Por fim, eles aplicam, principalmente, todos os métodos necessários para a aplicação da lógica evolutiva em si: ```selecaoNatural```, ```torneio```, ```avaliar``` e ```mutacao```.

O fluxo do algoritmo se baseia na repetição de chamadas, em ```idle``` (dentro de ```main.cpp```), à função ```executarPasso```. O pipeline de execução segue a seguinte lógica:

1. Pré-Processamento (Pathfinding): O método ```recalcularMapaDistancias``` executa uma busca em largura (BFS) no grid, que gera um campo de fluxo onde cada célula tem a distância real até o objetivo, levando em consideração a presença de obstáculos.

2. Ciclo de Vida: Durante os frames da geração, cada peixe usa o gene correspondente ao instante atual para aplicar a força resultante em sua aceleração, atualizando sua posição e verificando colisões via AABB (Axis-Aligned Bounding Box).

3. Avaliação (Fitness): Ao fim do tempo de vida (fim da iteração/geração atual), o fitness combina a distância do grid (calculada na etapa 1) com a distância euclidiana para um ajuste fino.

4. Evolução: O sistema aplica o elitismo (preservando o melhor) e gera o restante da população através de clonagem com mutação adaptativa, reiniciando o ciclo. Assim como mencionado anteriormente, a taxa de mutação é alterada com base no cenário em questão (caso algum indivíduo tenha alcançado o alvo, diminui a mutação; caso não tenha ou as populações estejam estagnadas, aumenta).


