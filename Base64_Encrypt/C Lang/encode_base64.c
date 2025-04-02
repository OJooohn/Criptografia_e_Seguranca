#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tabela Base64 padrão
const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define BUFFER 100

void encode_base64(char *input, char *output) {
    int input_length = strlen(input) - 1; // Remove o '\n' do fgets
    int bit_stream[input_length * 8]; // Array para armazenar bits (8 bits por caractere)
    int bit_count = 0;

    // Converter cada caractere da string em binário (8 bits cada)
    for (int i = 0; i < input_length; i++) {
        unsigned char c = input[i];
        for (int j = 7; j >= 0; j--) {
            // & 1 -> pega o bit mais a direita
            bit_stream[bit_count++] = (c >> j) & 1; // Extrai os bits um por um
        }
    }

    // Agrupar bits em blocos de 6 e converter para Base64
    int output_index = 0;
    for (int i = 0; i < bit_count; i += 6) {
        // value em binario = 000000
        int value = 0;

        // Formar um número de 6 bits
        for (int j = 0; j < 6; j++) {
            if (i + j < bit_count) {
                // | -> porta logia OR
                value = (value << 1) | bit_stream[i + j];
            } else {
                value = (value << 1); // Adicionar zeros à direita se faltar bits
            }
        }

        // Pegar o caractere correspondente na tabela Base64
        output[output_index++] = base64_table[value];
    }

    // Padding (caso o número de bits não seja múltiplo de 3 bytes)
    while (output_index % 4 != 0) {
        output[output_index++] = '=';
    }

    output[output_index] = '\0'; // Finalizar a string Base64
}

int main() {
    char string[BUFFER];
    char output[BUFFER];

    printf("Digite uma string: ");
    fgets(string, BUFFER, stdin);

    encode_base64(string, output);

    printf("Base64: %s\n", output);

    return 0;
}
