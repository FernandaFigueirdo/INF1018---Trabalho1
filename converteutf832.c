#include <stdio.h>
#include "converteutf832.h"

//Fernanda da Cal Figueiredo
//Mariana Costa Ribeiro

// Função que determina o número de bytes necessários para representar um caractere UTF-8 com base no primeiro byte.
int tamanhoBytesUTF8(unsigned char byte) {
    if ((byte & 0x80) == 0) {
        return 1;  // Se o bit mais significativo for 0, é um caractere ASCII (1 byte).
    }
    else if ((byte & 0xE0) == 0xC0) {
        return 2;  // Se os três primeiros bits forem 110, é um caractere UTF-8 de 2 bytes.
    }
    else if ((byte & 0xF0) == 0xE0) {
        return 3;  // Se os quatro primeiros bits forem 1110, é um caractere UTF-8 de 3 bytes.
    }
    else if ((byte & 0xF8) == 0xF0) {
        return 4;  // Se os cinco primeiros bits forem 11110, é um caractere UTF-8 de 4 bytes.
    }
    else {
        return -1;  // Se não corresponde a nenhuma dessas condições, é um byte inválido.
    }
}

// Função que converte uma sequência de bytes UTF-8 para seu valor Unicode.
unsigned int converterUtf8ParaUnicode(unsigned char* bytes, int tamanho) {
    unsigned int unicode = 0;
    if (tamanho == 1) {
        unicode = bytes[0];  // Caractere ASCII simples.
    }
    else if (tamanho == 2) {
        // Converte os dois bytes para o valor Unicode usando máscaras de bits.
        unicode = ((bytes[0] & 0x1F) << 6) | (bytes[1] & 0x3F);
    }
    else if (tamanho == 3) {
        // Converte três bytes UTF-8 em valor Unicode.
        unicode = ((bytes[0] & 0x0F) << 12) | ((bytes[1] & 0x3F) << 6) | (bytes[2] & 0x3F);
    }
    else if (tamanho == 4) {
        // Converte quatro bytes UTF-8 em valor Unicode.
        unicode = ((bytes[0] & 0x07) << 18) | ((bytes[1] & 0x3F) << 12) | ((bytes[2] & 0x3F) << 6) | (bytes[3] & 0x3F);
    }
    return unicode;  // Retorna o valor Unicode resultante.
}

// Função que converte de UTF-8 para UTF-32 (little-endian) e grava no arquivo de saída.
int convUtf8p32(FILE* arquivo_entrada, FILE* arquivo_saida) {
    unsigned int bom = 0x0000FEFF;  // BOM (Byte Order Mark) para arquivos UTF-32 little-endian.
    if (fwrite(&bom, 4, 1, arquivo_saida) != 1) {
        // Se falhar ao escrever o BOM, exibe uma mensagem de erro.
        fprintf(stderr, "Erro na escrita do BOM no arquivo de saída.\n");
        return -1;  // Retorna erro.
    }

    unsigned char bytes[4];  // Buffer para armazenar até 4 bytes de uma sequência UTF-8.
    while (fread(&bytes[0], 1, 1, arquivo_entrada) == 1) {
        // Lê o primeiro byte de uma sequência UTF-8.
        int tamanho = tamanhoBytesUTF8(bytes[0]);  // Determina quantos bytes o caractere UTF-8 usa.
        if (tamanho == -1) {
            // Se o tamanho for -1, o byte é inválido.
            fprintf(stderr, "Erro: byte inválido no arquivo UTF-8 de entrada.\n");
            return -1;  // Retorna erro.
        }

        for (int i = 1; i < tamanho; i++) {
            // Lê os bytes restantes do caractere UTF-8.
            fread(&bytes[i], 1, 1, arquivo_entrada);  // Não há necessidade de validação, pois o arquivo é corretamente codificado.
        }

        unsigned int unicode = converterUtf8ParaUnicode(bytes, tamanho);  // Converte os bytes lidos para o valor Unicode.
        // Converte o valor Unicode para UTF-32 little-endian (4 bytes).
        unsigned char utf32_le[4] = { unicode & 0xFF, (unicode >> 8) & 0xFF, (unicode >> 16) & 0xFF, (unicode >> 24) & 0xFF };

        // Escreve o valor UTF-32 no arquivo de saída.
        if (fwrite(utf32_le, 1, 4, arquivo_saida) != 4) {
            // Se ocorrer erro na escrita, exibe uma mensagem.
            fprintf(stderr, "Erro na escrita do caractere UTF-32 no arquivo de saída.\n");
            return -1;  // Retorna erro.
        }
    }

    return 0;  // Retorna sucesso após a conversão.
}

// Função que converte um valor Unicode de UTF-32 para um formato Unicode padrão.
unsigned int converterUtf32ParaUnicode(unsigned int unicode, int little_endian) {
    if (!little_endian) {
        // Se o arquivo for big-endian, inverte os bytes para obter o valor correto.
        unicode = ((unicode >> 24) & 0xFF) | ((unicode >> 8) & 0xFF00) |
            ((unicode << 8) & 0xFF0000) | ((unicode << 24) & 0xFF000000);
    }
    return unicode;  // Retorna o valor Unicode convertido.
}

// Função que converte um valor Unicode para a codificação UTF-8.
int converterUnicodeParaUtf8(unsigned int unicode, unsigned char* utf8) {
    int tamanho = 0;
    if (unicode <= 0x7F) {
        // Converte para um byte UTF-8 (caracteres ASCII).
        utf8[0] = unicode;
        tamanho = 1;
    }
    else if (unicode <= 0x7FF) {
        // Converte para dois bytes UTF-8.
        utf8[0] = 0xC0 | ((unicode >> 6) & 0x1F);
        utf8[1] = 0x80 | (unicode & 0x3F);
        tamanho = 2;
    }
    else if (unicode <= 0xFFFF) {
        // Converte para três bytes UTF-8.
        utf8[0] = 0xE0 | ((unicode >> 12) & 0x0F);
        utf8[1] = 0x80 | ((unicode >> 6) & 0x3F);
        utf8[2] = 0x80 | (unicode & 0x3F);
        tamanho = 3;
    }
    else if (unicode <= 0x10FFFF) {
        // Converte para quatro bytes UTF-8.
        utf8[0] = 0xF0 | ((unicode >> 18) & 0x07);
        utf8[1] = 0x80 | ((unicode >> 12) & 0x3F);
        utf8[2] = 0x80 | ((unicode >> 6) & 0x3F);
        utf8[3] = 0x80 | (unicode & 0x3F);
        tamanho = 4;
    }
    return tamanho;  // Retorna o número de bytes usados na codificação UTF-8.
}

// Função que converte de UTF-32 para UTF-8 e grava no arquivo de saída.
int convUtf32p8(FILE* arquivo_entrada, FILE* arquivo_saida) {
    unsigned int bom;
    // Lê o BOM do arquivo de entrada para determinar se é big-endian ou little-endian.
    if (fread(&bom, 4, 1, arquivo_entrada) != 1) {
        fprintf(stderr, "Erro na leitura do BOM no arquivo UTF-32 de entrada.\n");
        return -1;  // Retorna erro se falhar.
    }

    int little_endian = (bom == 0x0000FEFF);  // Verifica se o BOM indica little-endian.
    if (!little_endian && bom != 0xFFFE0000) {
        fprintf(stderr, "BOM inválido no arquivo UTF-32 de entrada.\n");
        return -1;  // Retorna erro se o BOM for inválido.
    }

    unsigned int unicode;
    unsigned char utf8[4];
    // Lê e converte cada valor UTF-32 para UTF-8.
    while (fread(&unicode, 4, 1, arquivo_entrada) == 1) {
        unicode = converterUtf32ParaUnicode(unicode, little_endian);  // Converte de big-endian se necessário.
        int tamanho = converterUnicodeParaUtf8(unicode, utf8);  // Converte o valor Unicode para UTF-8.

        // Escreve o valor UTF-8 no arquivo de saída.
        if (fwrite(utf8, 1, tamanho, arquivo_saida) != tamanho) {
            fprintf(stderr, "Erro na escrita do caractere UTF-8 no arquivo de saída.\n");
            return -1;  // Retorna erro se houver falha na escrita.
        }
    }

    return 0;  // Retorna sucesso após a conversão.
}
