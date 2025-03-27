#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void strToBin(char input[100], int bin[16][32]){

    int inputLen = strlen(input);
    int i = 0, j = 0;

    for(int k = 0; k < inputLen; k++){

        int c = input[k];

        for(int x = 7; x >= 0; x--){
            bin[j][i++] = (c >> x) & 1;
            if(i == 32){
                j++;
                i = 0;
            }
            if (j >= 16) { 
                return;
            }
        }

        if(k == inputLen - 1){
            bin[j][i] = 1;
        }

    }

}

void printBinMatrix(int bin[16][32]){

    for(int j = 0; j < 16; j++){
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

void addMessageLength(int inputLen, int bin[16][32]){

    uint64_t messasgeLength = inputLen * 8;

    int i = 0, j = 14;
    for(int x = 63; x >= 0; x--){

        bin[j][i++] = (messasgeLength >> x) & 1;
        if(i == 32){
            j++;
            i = 0;
        }
        if (j >= 16) { 
            return;
        }

    }

}

int main(){

    char input[100];
    printf("Informe o input: ");
    fgets(input, 100, stdin);

    // Remover \n do input
    input[strcspn(input, "\n")] = '\0';

    int bin[16][32] = {0};

    strToBin(input, bin);
    addMessageLength(strlen(input), bin);
    
    printBinMatrix(bin);

    return 0;
}