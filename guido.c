#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
//definição das notas musicais em Hertz

#define C4 261.63
#define D4 293.66
#define E4 329.63
#define F4 349.23
#define G4 392.00
#define A4 440.00
#define B4 493.88


#define SAMPLE_RATE 44100
#define DURATION 0.3 
#define AMPLITUDE 12000



double mapear_vogal(char c){
    switch (c){
        case 'a': return C4;
        case 'e': return D4;
        case 'i': return E4;
        case 'o': return F4;
        case 'u': return G4; 
    }
}


double mapear_consoante(char c, double nota_atual) {
    
    double escala[7] = {C4, D4, E4, F4, G4, A4, B4};
    int idx = 0;
   
    for (int i = 0; i < 7; i++) {
        if (fabs(nota_atual - escala[i]) < 1.0) {
            idx = i;
            break;
        }
    }
    switch (c) {
        case 'b': case 'p':
            return escala[idx]; // mesma nota
        case 'c': case 'k': case 's': case 'q':
            return escala[(idx + 1) % 7]; 
        case 'd': case 't':
            return escala[(idx + 2) % 7]; 
        case 'f': case 'v':
            return escala[(idx + 3) % 7]; 
        case 'g': case 'j':
            return escala[(idx + 4) % 7]; 
        case 'l': case 'r':
            return escala[(idx + 5) % 7]; 
        case 'm': case 'n':
            return escala[(idx + 6) % 7]; 
        default:
            return nota_atual; 
    }
}


void gerar_onda_quadrada(int16_t* buffer, int num_samples, double freq) {
    if (freq == 0.0) {
        memset(buffer, 0, num_samples * sizeof(int16_t));
        return;
    }
    for (int i = 0; i < num_samples; i++) {
        double t = (double)i / SAMPLE_RATE;
        buffer[i] = (sin(2 * M_PI * freq * t) > 0) ? AMPLITUDE : -AMPLITUDE;
    }
}

void gerar_silencio(int16_t* buffer, int num_samples) {
    memset(buffer, 0, num_samples * sizeof(int16_t));
}

int eh_vogal(char c) {
    return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}


int eh_consoante(char c) {
    return (strchr("bpcksqdtfvgjlrmn", c) != NULL);
}

int main() {
    
    FILE* f = fopen("entrada.txt", "r");
    if (!f) {
        printf("Erro ao abrir entrada.txt\n");
        return 1;
    }
    char texto[4096];
    if (!fgets(texto, sizeof(texto), f)) {
        printf("Erro ao ler entrada.txt\n");
        fclose(f);
        return 1;
    }
    fclose(f);

    
    int samples_por_som = (int)(SAMPLE_RATE * DURATION);
    int total_samples = samples_por_som * strlen(texto);
    int16_t* canal_esquerdo = calloc(total_samples, sizeof(int16_t));
    int16_t* canal_direito  = calloc(total_samples, sizeof(int16_t));

    
    double nota_esq = C4; 
    double nota_dir = C4; 

    printf("Sequencia de notas:\n");

    
    int pos = 0;
    for (size_t i = 0; i < strlen(texto); i++) {
        char c = texto[i];
        if (c >= 'A' && c <= 'Z') c = c + 32; 

        if (c == ' ') {
            printf("[pausa]\n");
            gerar_silencio(&canal_esquerdo[pos], samples_por_som);
            gerar_silencio(&canal_direito[pos], samples_por_som);
        } else if (eh_vogal(c)) {
            nota_esq = mapear_vogal(c);
            printf("%c: L = %.2f Hz\n", c, nota_esq);
            gerar_onda_quadrada(&canal_esquerdo[pos], samples_por_som, nota_esq);
            gerar_onda_quadrada(&canal_direito[pos], samples_por_som, nota_dir);
        } else if (eh_consoante(c)) {
            nota_dir = mapear_consoante(c, nota_dir);
            printf("%c: R = %.2f Hz\n", c, nota_dir);
            gerar_onda_quadrada(&canal_esquerdo[pos], samples_por_som, nota_esq);
            gerar_onda_quadrada(&canal_direito[pos], samples_por_som, nota_dir);
        } else {
            
            printf("%c: [Ignorado]\n", c);
            gerar_onda_quadrada(&canal_esquerdo[pos], samples_por_som, nota_esq);
            gerar_onda_quadrada(&canal_direito[pos], samples_por_som, nota_dir);
        }
        pos += samples_por_som;
    }

   
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_PCM;
    format.channels = 2;
    format.sampleRate = SAMPLE_RATE;
    format.bitsPerSample = 16;

    drwav wav;
    if (!drwav_init_file_write(&wav, "saida.wav", &format, NULL)) {
        printf("Erro ao criar saida.wav\n");
        free(canal_esquerdo);
        free(canal_direito);
        return 1;
    }

    
    for (int i = 0; i < total_samples; i++) {
        int16_t frame[2] = {canal_esquerdo[i], canal_direito[i]};
        drwav_write_pcm_frames(&wav, 1, frame);
    }
    drwav_uninit(&wav);

   
    free(canal_esquerdo);
    free(canal_direito);

    printf("Arquivo saida.wav gerado!\n");
    return 0;
}