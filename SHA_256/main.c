#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define TOTAL_LINES (chunks * 16)

void input_to_binary(char *input, uint8_t chunks, uint8_t bin[][32]){

    int len = strlen(input);
    int i = 0, j = 0;

    for(int k = 0; k < len; k++){

        int c = input[k];

        for(int x = 7; x >= 0; x--){
            bin[j][i++] = (c >> x) & 1;
            if(i == 32){
                j++;
                i = 0;
            }
            if (j >= TOTAL_LINES)  return;
            
        }

        if(k == len - 1){
            bin[j][i] = 1;
        }

    }

}

void add_message_length(uint64_t messageLength, uint8_t chunks, uint8_t bin[][32]){

    int i = 0, j = TOTAL_LINES - 2;
    for(int x = 63; x >= 0; x--){

        bin[j][i++] = (messageLength >> x) & 1;
        if(i == 32){
            j++;
            i = 0;
        }
        if (j >= TOTAL_LINES) { 
            return;
        }
    }

}

void print_message_block(uint8_t chunks, uint8_t bin[][32]){

    for(int j = 0; j < TOTAL_LINES; j++){
        for(int i = 0; i < 32; i++){
            if(i % 8 == 0 && i != 0) 
                printf(" ");
                
            printf("%d", bin[j][i]);
        }
        printf("\n");
    }

}

void copy_message_block(uint8_t bin[][32], uint8_t message_schedule[][32]){

    for(int j = 0; j < 16; j++){
        for(int i = 0; i < 32; i++){
            message_schedule[j][i] = bin[j][i];
            printf("%d", message_schedule[j][i]);
        }
        printf("\n");
    }

}

void right_rotate(uint8_t word[], int n, uint8_t result[]){

    // printf("\n");
    // printf("\nRight rotate %d\n", n);
    for(int i = 0; i < 32; i++){
        // printf("word[i] %d | (i - n + 32) %% 32 %d\n", word[i], (i - n + 32) % 32);
        result[i] = word[(i - n + 32) % 32];
        // printf("%d", result[i]);
    }

}

void right_shift(uint8_t word[], int n, uint8_t result[]){
    
    // printf("\n");
    // printf("\nRight shift %d\n", n);
    for(int i = 0; i < 32; i++){
        if(i < n)
            result[i] = 0;
        else
            result[i] = word[i - 3];
        // printf("%d", result[i]);
    }
}

void xor(uint8_t word1[], uint8_t word2[], uint8_t word3[], uint8_t result[]){

    // printf("\n-------------------------------\n");
    for(int i = 0; i < 32; i++){
        result[i] = word1[i] ^ word2[i] ^ word3[i];
        // printf("%d", result[i]);
    }

}

void calculate_sigmas(uint8_t message_schedule[][32], uint8_t sigma0[], uint8_t sigma1[], int j){

    // Sigma 0 = right rotate 7 + right rotate 18 + shift right 3 of W1
    uint8_t RR7[32];  // right rotate 7
    uint8_t RR18[32]; // right rotate 18
    uint8_t RS8[32];  // right shift 3

    // Sigma 1 = right rotate 17 + right rotate 19 + shift right 10 of W14
    uint8_t RR17[32]; // right rotate 17
    uint8_t RR19[32]; // right rotate 19
    uint8_t RS10[32]; // right shift 10
        
    // printf("\nSigma0");
    right_rotate(message_schedule[j + 1], 7, RR7);
    right_rotate(message_schedule[j + 1], 18, RR18);
    right_shift(message_schedule[j + 1], 3, RS8);
    xor(RR7, RR18, RS8, sigma0);

    // printf("\nSigma1\n");
    right_rotate(message_schedule[j + 14], 17, RR17);
    right_rotate(message_schedule[j + 14], 19, RR19);
    right_shift(message_schedule[j + 14], 10, RS10);
    xor(RR17, RR19, RS10, sigma1);

}

int main(){

    char input[] = "H7s5ddhud7t8sJa ne7ed89Doedf8Lo verfdsa87h";

    uint64_t message_length = strlen(input) * 8;

    // Calcula quantos chunks de 512 bits são necessários
    // +1 bit para o 1 no final
    // +64 bits para o tamanho da mensagem
    // +511 para arredondar para cima 
    uint8_t chunks = ((message_length + 1 + 64) + 511) / 512;
    
    printf("%d chunks\n", chunks);
    printf("%d total lines\n", chunks * 16);
    printf("%llu input len in bits\n", message_length);

    uint8_t message_block[TOTAL_LINES][32];
    memset(message_block, 0, sizeof(message_block)); 

    input_to_binary(input, chunks, message_block);
    add_message_length(message_length, chunks, message_block);
    
    print_message_block(chunks, message_block);

    uint8_t message_schedule[64][32];
    memset(message_schedule, 0, sizeof(message_schedule));

    // Copy the first 16 words to the message schedule
    copy_message_block(message_block, message_schedule);

    uint8_t sigma0[32], sigma1[32];
    for(int j = 0; j < 42; j++){
        // Calculate sigma 0 and sigma 1
        calculate_sigmas(message_schedule, sigma0, sigma1, j);
    }

    return 0;
}