# INF1018---Trabalho1
Relatório Trabalho 1
Inf1018 - Software básico
Alunas: Fernanda da Cal Figueiredo e Mariana Costa Ribeiro

Objetivo
Este trabalho tem como objetivo implementar duas funções para realizar a conversão entre os formatos de codificação de texto UTF-8 e UTF-32. As funções implementadas são:
convUtf8p32: Converte um arquivo codificado em UTF-8 para UTF-32 (little-endian). 
convUtf32p8: Converte um arquivo codificado em UTF-32 (little-endian ou big-endian) para UTF-8.


O que está funcionando:
As conversões de UTF-8 para UTF-32 e de UTF-32 para UTF-8 estão funcionando corretamente, com a preservação dos valores Unicode e o tratamento de diferentes comprimentos de caracteres.                                                   
A função convUtf32p8 trata corretamente arquivos big-endian e little-endian, com a inversão dos bytes conforme necessário.                                                            Mensagens de erro são emitidas corretamente em casos de falha na leitura, escrita ou formatação incorreta.


O que não está funcionando:
O código não trata casos de arquivos de entrada que estejam corrompidos ou que contenham caracteres UTF-8 malformados. No entanto, isso é permitido pelo enunciado, que indica que os arquivos de entrada sempre estarão corretamente codificados.
A função convUtf8p32 não valida os bytes de continuação do UTF-8, já que essa validação não é necessária no contexto deste trabalho.


Conclusão
O código implementa com sucesso as funções de conversão de arquivos entre UTF-8 e UTF-32, atendendo às exigências do enunciado. As funções conseguem lidar com os cenários especificados e emitem mensagens de erro adequadas quando necessário.
