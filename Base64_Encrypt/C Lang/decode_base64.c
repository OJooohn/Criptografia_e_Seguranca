#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER 800

const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void decode_base64(char *input, char *output){
    int input_length = strlen(input) - 1; // Remove o '\n' do fgets
    int bit_stream[input_length * 8]; // Array para armazenar bits (8 bits por caractere)
    int bit_count = 0;

    // Transformar a string de 6 bits para 8 bits
    for(int i = 0; i < input_length; i++){
        char c = input[i];

        if(c == '=') break;

        for(int j = 0; j < 64; j++){
            if(base64_table[j] == c){
                for(int k = 5; k >= 0; k--){
                    bit_stream[bit_count++] = (j >> k) & 1;
                }
            }
        }
    }

    // Converter os bits para caracteres
    int output_index = 0;
    for(int i = 0; i < bit_count; i += 8){
        int value = 0;
        for(int j = 0; j < 8; j++){
            if(i + j < bit_count){
                value = (value << 1) | bit_stream[i + j];
            } else {
                break;
            }
        }
        output[output_index++] = value;
    }

    output[output_index] = '\0';

}

int main(){

    char string[BUFFER];
    char output[BUFFER];

    printf("Digite a string em base64: ");
    fgets(string, BUFFER, stdin);

    decode_base64(string, output);

    printf("Decodificado: %s\n", output);

    return 0;
}
