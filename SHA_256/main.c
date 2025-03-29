#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define BLOCK_SIZE 512
#define WORD_SIZE 32
#define WORD_PER_LINE 16
#define SCHEDULE_SIZE 64
#define TOTAL_LINES (chunks * 16)

void convert_input_to_binary(char *input, uint8_t chunks, uint8_t binary[][WORD_SIZE]){

    int length = strlen(input);
    int bit_index = 0, word_index = 0;

    for(int i = 0; i < length; i++){

        int character = input[i];
        for(int bit = 7; bit >= 0; bit--){
            binary[word_index][bit_index++] = (character >> bit) & 1;
            if(bit_index == WORD_SIZE){
                word_index++;
                bit_index = 0;
            }
            if (word_index >= TOTAL_LINES)  return;
            
        }

        if(i == length - 1){
            binary[word_index][bit_index] = 1;
        }
    }
}

void append_message_length(uint64_t message_length, uint8_t chunks, uint8_t binary[][WORD_SIZE]){
    int bit_index = 0, word_index = TOTAL_LINES - 2;
    for(int bit = 63; bit >= 0; bit--){

        binary[word_index][bit_index++] = (message_length >> bit) & 1;
        if(bit_index == WORD_SIZE){
            word_index++;
            bit_index = 0;
        }
        if (word_index >= TOTAL_LINES) return;
    }
}

void print_message_block(uint8_t chunks, uint8_t bin[][WORD_SIZE]){
    printf("\nMessage block\n");
    for(int j = 0; j < TOTAL_LINES; j++){
        for(int i = 0; i < WORD_SIZE; i++){
            if(i % 8 == 0 && i != 0) 
                printf(" ");
                
            printf("%d", bin[j][i]);
        }
        printf("\n");
    }
}

void copy_message_block(uint8_t bin[][WORD_SIZE], uint8_t message_schedule[][WORD_SIZE]){
    for(int j = 0; j < WORD_PER_LINE; j++){
        for(int i = 0; i < WORD_SIZE; i++){
            message_schedule[j][i] = bin[j][i];
        }
    }
}

void right_rotate(uint8_t word[], int n, uint8_t result[]) {
    for(int i = 0; i < WORD_SIZE; i++){
        result[i] = word[(i - n + WORD_SIZE) % WORD_SIZE];
    }
}

void right_shift(uint8_t word[], int n, uint8_t result[]) {
    for(int i = 0; i < WORD_SIZE; i++){
        if(i < n)
            result[i] = 0;
        else
            result[i] = word[i - n];
    }
}

void xor_words(uint8_t w1[], uint8_t w2[], uint8_t w3[], uint8_t result[]) {
    for (int i = 0; i < WORD_SIZE; i++) {
        result[i] = w1[i] ^ w2[i] ^ w3[i];
    }
}

void compute_sigma_and_theta(uint8_t schedule[][WORD_SIZE], uint8_t sigma[], uint8_t theta[], int index) {
    uint8_t rotated7[WORD_SIZE], rotated18[WORD_SIZE], shifted3[WORD_SIZE];
    uint8_t rotated17[WORD_SIZE], rotated19[WORD_SIZE], shifted10[WORD_SIZE];

    right_rotate(schedule[index + 1], 7, rotated7);
    right_rotate(schedule[index + 1], 18, rotated18);
    right_shift(schedule[index + 1], 3, shifted3);
    xor_words(rotated7, rotated18, shifted3, sigma);

    right_rotate(schedule[index + 14], 17, rotated17);
    right_rotate(schedule[index + 14], 19, rotated19);
    right_shift(schedule[index + 14], 10, shifted10);
    xor_words(rotated17, rotated19, shifted10, theta);
}

uint32_t convert_array_to_decimal(uint8_t word[]) {
    uint32_t result = 0;
    for (int i = 0; i < WORD_SIZE; i++) {
        result = (result << 1) | word[i];
    }
    return result;
}

void convert_decimal_to_array(uint32_t value, uint8_t word[]) {
    for (int i = 0; i < WORD_SIZE; i++) {
        word[i] = (value >> (31 - i)) & 1;
    }
}

void compute_w_values(uint8_t schedule[][WORD_SIZE], uint8_t sigma[], uint8_t theta[], int index, uint32_t *Wn_decimal) {
    uint32_t w0 = convert_array_to_decimal(schedule[index]);
    uint32_t w9 = convert_array_to_decimal(schedule[index + 9]);
    uint32_t sigma_value = convert_array_to_decimal(sigma);
    uint32_t theta_value = convert_array_to_decimal(theta);
    *Wn_decimal = w0 + sigma_value + w9 + theta_value;
}

int main() {
    char input[] = "H7s5ddhud7t8sJa ne7ed89Doedf8Lo verfdsa87h";
    uint64_t message_length = strlen(input) * 8;
    uint8_t chunks = ((message_length + 1 + 64) + 511) / BLOCK_SIZE;

    printf("%d chunks\n", chunks);
    printf("%d total lines\n", TOTAL_LINES);
    printf("%llu input length in bits\n", message_length);

    uint8_t message_block[TOTAL_LINES][WORD_SIZE];
    memset(message_block, 0, sizeof(message_block));

    convert_input_to_binary(input, chunks, message_block);
    append_message_length(message_length, chunks, message_block);
    // print_message_block(chunks, message_block);

    uint8_t message_schedule[SCHEDULE_SIZE][WORD_SIZE] = {0};
    copy_message_block(message_block, message_schedule);

    uint8_t sigma[WORD_SIZE], theta[WORD_SIZE];

    uint32_t *Wn_decimal = malloc(sizeof(uint32_t));
    if (Wn_decimal == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    
    for (int j = 0; j < 48; j++) {
        compute_sigma_and_theta(message_schedule, sigma, theta, j);
        compute_w_values(message_schedule, sigma, theta, j, Wn_decimal);
        convert_decimal_to_array(*Wn_decimal, message_schedule[j + 16]);
    }

    printf("\nMessage Schedule\n");
    for(int j = 0; j < 64; j++){
        for(int i = 0; i < 32; i++){
            printf("%d", message_schedule[j][i]);
        }
        printf("\n");
    }

    free(Wn_decimal);
    return 0;

}