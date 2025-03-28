#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define TOTAL_LINES (chunks * 16)

void strToBin(char *input, uint8_t chunks, int bin[][32]){

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
            if (j >= TOTAL_LINES) { 
                return;
            }
        }

        if(k == len - 1){
            bin[j][i] = 1;
        }

    }

}

void printBinMatrix(uint8_t chunks, int bin[][32]){

    for(int j = 0; j < TOTAL_LINES; j++){
        for(int i = 0; i < 32; i++){
            if(i % 8 == 0 && i != 0){
                printf(" ");
            }   
            printf("%d", bin[j][i]);
        }
        printf("\n");
    }

}

void printBinary(uint64_t num) {
    for (int i = 63; i >= 0; i--) {  // Percorre os 64 bits
        printf("%d", (num >> i) & 1);
        if (i % 8 == 0) printf(" "); // Espaço para melhor visualização
    }
    printf("\n");
}

void addMessageLength( uint8_t chunks, uint64_t messageLength, int bin[16][32]){

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

int main(){

    char input[] = "Testando o codigo para ver se funciona corretamente os chunks e se tiver mais de 2 chunks o codigo vai saber fazer o que fazer";

    uint64_t messageLength = strlen(input) * 8;

    // Calcula quantos chunks de 512 bits são necessários
    uint8_t chunks = 1;
    if (messageLength > 448) {
        chunks = ((messageLength + 1 + 64) + 511) / 512;
    }

    printf("%d chunks\n", chunks);
    printf("%d total lines\n", chunks * 16);
    printf("%llu input len in bits\n", messageLength);

    int bin[chunks * 16][32];
    memset(bin, 0, sizeof(bin)); 

    strToBin(input, chunks, bin);
    addMessageLength(chunks, messageLength, bin);
    
    printBinMatrix(chunks, bin);

    return 0;
}