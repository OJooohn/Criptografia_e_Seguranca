#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define BLOCK_SIZE 512
#define WORD_SIZE 32
#define WORD_PER_LINE 16
#define SCHEDULE_SIZE 64
#define TOTAL_LINES(chunks) ((chunks) * 16)

void convert_input_to_binary(char *input, uint8_t chunks, uint8_t binary[][WORD_SIZE]) {
    int total_lines = TOTAL_LINES(chunks);
    int length = strlen(input);
    int bit_index = 0, word_index = 0;

    for (int i = 0; i < length; i++) {
        int character = input[i];
        for (int bit = 7; bit >= 0; bit--) {
            binary[word_index][bit_index++] = (character >> bit) & 1;
            if (bit_index == WORD_SIZE) {
                word_index++;
                bit_index = 0;
            }
            if (word_index >= total_lines) return;
        }

        if (i == length - 1) {
            binary[word_index][bit_index] = 1;
        }
    }
}

void append_message_length(uint64_t message_length, uint8_t chunks, uint8_t binary[][WORD_SIZE]) {
    int total_lines = TOTAL_LINES(chunks);
    int bit_index = 0, word_index = total_lines - 2;
    for (int bit = 63; bit >= 0; bit--) {
        binary[word_index][bit_index++] = (message_length >> bit) & 1;
        if (bit_index == WORD_SIZE) {
            word_index++;
            bit_index = 0;
        }
        if (word_index >= total_lines) return;
    }
}

void print_message_block(uint8_t chunks, uint8_t bin[][WORD_SIZE]) {
    int total_lines = TOTAL_LINES(chunks);
    printf("\nMessage block\n");
    for (int j = 0; j < total_lines; j++) {
        for (int i = 0; i < WORD_SIZE; i++) {
            if (i % 8 == 0 && i != 0)
                printf(" ");
            printf("%d", bin[j][i]);
        }
        printf("\n");
    }
}

void copy_message_block(uint8_t bin[][WORD_SIZE], uint8_t message_schedule[][WORD_SIZE], int chunk) {
    for (int j = 0; j < WORD_PER_LINE; j++) {
        for (int i = 0; i < WORD_SIZE; i++) {
            message_schedule[j][i] = bin[j + (chunk - 1) * WORD_PER_LINE][i];
        }
    }
}

void right_rotate(uint8_t word[], int n, uint8_t result[]) {
    for (int i = 0; i < WORD_SIZE; i++) {
        result[i] = word[(i - n + WORD_SIZE) % WORD_SIZE];
    }
}

void right_shift(uint8_t word[], int n, uint8_t result[]) {
    for (int i = 0; i < WORD_SIZE; i++) {
        if (i < n)
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

void compute_sigma_and_theta(uint8_t schedule[][WORD_SIZE], uint8_t theta0[], uint8_t theta1[], int index) {
    uint8_t rotated7[WORD_SIZE], rotated18[WORD_SIZE], shifted3[WORD_SIZE];
    uint8_t rotated17[WORD_SIZE], rotated19[WORD_SIZE], shifted10[WORD_SIZE];

    right_rotate(schedule[index + 1], 7, rotated7);
    right_rotate(schedule[index + 1], 18, rotated18);
    right_shift(schedule[index + 1], 3, shifted3);
    xor_words(rotated7, rotated18, shifted3, theta0);

    right_rotate(schedule[index + 14], 17, rotated17);
    right_rotate(schedule[index + 14], 19, rotated19);
    right_shift(schedule[index + 14], 10, shifted10);
    xor_words(rotated17, rotated19, shifted10, theta1);
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

void compute_w_values(uint8_t schedule[][WORD_SIZE], uint8_t theta0[], uint8_t theta1[], int index, uint32_t *Wn_decimal) {
    uint32_t w0 = convert_array_to_decimal(schedule[index]);
    uint32_t w9 = convert_array_to_decimal(schedule[index + 9]);
    uint32_t sigma_value = convert_array_to_decimal(theta0);
    uint32_t theta_value = convert_array_to_decimal(theta1);
    *Wn_decimal = w0 + sigma_value + w9 + theta_value;
}

void compute_sigma1(uint32_t e, uint32_t *sigma1) {
    uint8_t e_array[WORD_SIZE];
    convert_decimal_to_array(e, e_array);

    uint8_t rotated6[WORD_SIZE], rotated11[WORD_SIZE], rotated25[WORD_SIZE];
    right_rotate(e_array, 6, rotated6);
    right_rotate(e_array, 11, rotated11);
    right_rotate(e_array, 25, rotated25);

    uint8_t sigma1_array[WORD_SIZE] = {0};
    xor_words(rotated6, rotated11, rotated25, sigma1_array);
    *sigma1 = convert_array_to_decimal(sigma1_array);
}

void compute_choice(uint32_t e, uint32_t f, uint32_t g, uint32_t *choice) {
    *choice = (e & f) ^ (~e & g);
}

void compute_temp1(uint32_t h, uint32_t sigma1, uint32_t choice, uint32_t K, uint32_t Wn_decimal, uint32_t *temp1) {
    *temp1 = h + sigma1 + choice + K + Wn_decimal;
}

void compute_sigma0(uint32_t a, uint32_t *sigma0) {
    uint8_t a_array[WORD_SIZE];
    convert_decimal_to_array(a, a_array);

    uint8_t rotated2[WORD_SIZE], rotated13[WORD_SIZE], rotated22[WORD_SIZE];
    right_rotate(a_array, 2, rotated2);
    right_rotate(a_array, 13, rotated13);
    right_rotate(a_array, 22, rotated22);

    uint8_t sigma0_array[WORD_SIZE] = {0};
    xor_words(rotated2, rotated13, rotated22, sigma0_array);
    *sigma0 = convert_array_to_decimal(sigma0_array);
}

void compute_majority(uint32_t a, uint32_t b, uint32_t c, uint32_t *majority) {
    *majority = (a & b) ^ (a & c) ^ (b & c);
}

void compute_temp2(uint32_t sigma0, uint32_t majority, uint32_t *temp2) {
    *temp2 = sigma0 + majority;
}

int main() {
    char input[] = "Vila Bodega, um lugar de alegria, onde a diversão nunca termina, é todo dia!";
    uint64_t message_length = strlen(input) * 8;
    uint8_t chunks = ((message_length + 1 + 64) + 511) / BLOCK_SIZE;
    int total_lines = TOTAL_LINES(chunks);

    // printf("%d chunks\n", chunks);
    // printf("%d total lines\n", total_lines);
    // printf("%llu input length in bits\n", message_length);

    uint8_t message_block[total_lines][WORD_SIZE];
    memset(message_block, 0, sizeof(message_block));

    convert_input_to_binary(input, chunks, message_block);
    append_message_length(message_length, chunks, message_block);

    uint8_t theta0[WORD_SIZE], theta1[WORD_SIZE];

    uint32_t *Wn_decimal = malloc(sizeof(uint32_t));
    if (Wn_decimal == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    uint32_t hash[8] = {
        0x6a09e667,
        0xbb67ae85,
        0x3c6ef372,
        0xa54ff53a,
        0x510e527f,
        0x9b05688c,
        0x1f83d9ab,
        0x5be0cd19
    };

    uint32_t K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    uint32_t a, b, c, d, e, f, g, h;

    uint32_t *temp1, *sigma1, *choice, *temp2, *sigma0, *majority;
    temp1 = sigma1 = choice = temp2 = sigma0 = majority = NULL;

    temp1 = malloc(sizeof(uint32_t));
    sigma1 = malloc(sizeof(uint32_t));
    choice = malloc(sizeof(uint32_t));
    temp2 = malloc(sizeof(uint32_t));
    sigma0 = malloc(sizeof(uint32_t));
    majority = malloc(sizeof(uint32_t));

    if (!temp1 || !sigma1 || !choice || !temp2 || !sigma0 || !majority) {
        printf("Memory allocation failed\n");
        free(temp1); free(sigma1); free(choice);
        free(temp2); free(sigma0); free(majority);
        return 1;
    }

    for (int chunk = 1; chunk <= chunks; chunk++) {
        a = hash[0];
        b = hash[1];
        c = hash[2];
        d = hash[3];
        e = hash[4];
        f = hash[5];
        g = hash[6];
        h = hash[7];

        uint8_t message_schedule[SCHEDULE_SIZE][WORD_SIZE] = {0};
        copy_message_block(message_block, message_schedule, chunk);

        for (int j = 0; j < 48; j++) {
            compute_sigma_and_theta(message_schedule, theta0, theta1, j);
            compute_w_values(message_schedule, theta0, theta1, j, Wn_decimal);
            convert_decimal_to_array(*Wn_decimal, message_schedule[j + 16]);
        }

        for (int i = 0; i < 64; i++) {
            compute_sigma1(e, sigma1);
            compute_choice(e, f, g, choice);
            compute_temp1(h, *sigma1, *choice, K[i], convert_array_to_decimal(message_schedule[i]), temp1);
            compute_sigma0(a, sigma0);
            compute_majority(a, b, c, majority);
            compute_temp2(*sigma0, *majority, temp2);

            h = g;
            g = f;
            f = e;
            e = d + *temp1;
            d = c;
            c = b;
            b = a;
            a = *temp1 + *temp2;
        }

        hash[0] += a;
        hash[1] += b;
        hash[2] += c;
        hash[3] += d;
        hash[4] += e;
        hash[5] += f;
        hash[6] += g;
        hash[7] += h;
    }

    printf("\nInput: %s", input);

    char output[64];
    for (int i = 0; i < 8; i++) {
        sprintf(output + i * 8, "%08x", hash[i]);
    }

    printf("\nHash value: ");
    for (int i = 0; i < 64; i++) {
        printf("%c", output[i]);
    }
    printf("\n");

    free(Wn_decimal);
    free(temp1);
    free(sigma1);
    free(choice);
    free(temp2);
    free(sigma0);
    free(majority);
    return 0;
}