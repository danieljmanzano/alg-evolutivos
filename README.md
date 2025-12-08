### Trabalho desenvolvido para a disciplina SSC0713 - Sistemas Evolutivos Aplicados à Robótica
Alunos:
Artur Kenzo Obara Kawazoe - \
Daniel Jorge Manzano - \
Larissa Pires Moreira Rocha Duarte - 15522358 \
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
## Funcionamento

