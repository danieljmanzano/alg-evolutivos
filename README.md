### Trabalho desenvolvido para a disciplina SSC0713 - Sistemas Evolutivos Aplicados à Robótica
Alunos:
Artur Kenzo Obara Kawazoe - \
Daniel Jorge Manzano - 15446861 \
Larissa Pires Moreira Rocha Duarte - 15522358
## Introdução
O projeto desenvolvido foi um algoritmo genético que simula um cardume de peixes em busca de comida. Quando uma nova população é criada, os peixes seguem um caminho aleatório até que um deles
 chegue à comida. Esse peixe, então, passa seus genes para as próximas gerações por meio de reprodução assexuada (clonagem+mutação), certificando-se que não ocorre mutação no peixe campeão. Assim, nas gerações seguintes, cada vez mais peixes seguem
 o caminho correto. \
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
Após executá-los, será gerada uma tela do simulador com o OpenGL. Use o mouse para posicionar o elemento desejado, alterando o elemento em questão selecionando as opções com as teclas 1 (posicionar alvo/comida), 2 (posicionar ninho, de onde saem os peixes) e 3 (posicionar obstáculos). Por fim, pressione Enter para iniciar a simulação. Caso deseje pará-la em algum momento, pressione ESC sobre a tela do simulador ou CTRL+C no terminal. \
Para limpar o executável gerado pelos códigos, no terminal, digite:
```
    make clean
```
## Funcionamento
O funcionamento dos códigos, visualizado na simulação, itera repetidamente sobre uma população de 100 peixes que busca o melhor fitness no caminho pela comida. Por padrão, enquanto nenhum peixe chega ao alvo, a simulação é visualizada de forma acelerada; quando pelo menos um o encontra, a simulação desacelera a um tempo "normal". Para alterar a velocidade da simulação a qualquer momento, digite, sobre a tela, 'a' para acelerar e 'd' para desacelerar. \
Além da simulação visual com o OpenGL, o código printa no terminal, a cada iteração: o número da geração, o fitness do melhor indivíduo e o fitness médio da população.

## Especificidades Técnicas
aqui nois explica melhor coisas particulares do codigo

