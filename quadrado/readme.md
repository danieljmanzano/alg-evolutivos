Algoritmos evolutivos aplicados para encontrar o ponto máximo da função f(x) = x², com x  variando entre 0 e 127

"torneio.cpp" usa a ideia de repopulação por meio de decidir dois pais aleatórios por meio de torneios e criar os filhos

"melhor.cpp" aplica a reprodução usando o melhor indivíduo sobre a população toda


Os códigos em questão são quase idênticos, as diferenças são sobre essas estratégias de reprodução diferentes para analisar os diferentes resultados (não me dei o trabalho de centralizar as operações comuns em um arquivo separado, seria muito trabalho a mais...)

Conclusão: como o problema e a função são simples, ambas as estratégias chegaram a resultados semelhantes (quase sempre chegam ao resultado ideal nas últimas gerações). Talvez mudar a função ou aumentar a complexidade dos parâmetros principais seja uma boa ideia para um outro problema futuro