//Definindo bibliotecas a serem utilizadas
#include <stdio.h>
#include <stdlib.h> // Biblioteca para manipular alocações de memória
#include <stdint.h> // Biblioteca para manipular dados com tamanho específicos (16 e 8 bits)
#include <string.h> // Biblioteca para manipulação de strings
#include <math.h>
//Definindo um valor para pi (biblioteca math.h)
#ifndef M_PI
#define M_PI 3.14159265358979323846 
#endif
#define DR_WAV_IMPLEMENTATION //Definindo o método que será utilizado (proveniente da biblioteca dr_wav.h)
#include "dr_wav.h"

//Definindo as frequênciass das notas
#define C4 261.63
#define D4 293.66
#define E4 329.63
#define F4 349.23
#define G4 392.00
#define A4 440.00
#define B4 493.88

#define SAMPLE_RATE 44100 //Define a taxa de amostragem de áudio (44.1kHz), ou seja, a qualidade do som
#define DURATION 0.3 //Duração em segundos de cada pausa ou nota
#define AMPLITUDE 12000 //Define a altura máximo de cada onda sonora (altura do som)

//Função mapear vogais para notas (canal esquerdo)
double mapear_vogal(char c) {
    switch (c) {
        case 'a': return C4;
        case 'e': return D4;
        case 'i': return E4;
        case 'o': return F4;
        case 'u': return G4;
        default: return 0.0;
    }
}

//Função mapear consoantes para intervalos (canal direito)
double mapear_consoante(char c, double nota_atual) {
//Escala: C4, D4, E4, F4, G4, A4, B4
    double escala[7] = {C4, D4, E4, F4, G4, A4, B4}; //criando um vetor chamado escala com as 7 notas da escala musical já definida 
    int idx = 0;
//Descobrir o índice da nota atual
    for (int i = 0; i < 7; i++) { //define uma variável i com valor inicial = 0; o laço/repetição continua enquanto i for menor que 7; ...
        // ... a cada repetição do laço, o valor de i aumenta 1
        if (fabs(nota_atual - escala[i]) < 1.0) { //se o valor absoluto (função fabs) da difereça for menor que 1 (ou seja, se as notas são praticamente iguais)
            idx = i; //salva o índice i (posição) onde encontrou a nota igual
            break;
        }
    }
    switch (c) {
        case 'b': case 'p':
            return escala[idx]; //retorna a mesma nota que já estava tocando na posição (encontrada) idx da escala
        case 'c': case 'k': case 's': case 'q':
            return escala[(idx + 1) % 7]; //pega o índice atual, soma 1, tira o módulo disso e se o resto da divisão for 0, siginifica que foi uma divisão de 7 por 7, não existe escala 7, volta para escala 0 
        case 'd': case 't':
            return escala[(idx + 2) % 7]; //terceira nota
        case 'f': case 'v':
            return escala[(idx + 3) % 7]; //quarta nota
        case 'g': case 'j':
            return escala[(idx + 4) % 7]; //quinta nota
        case 'l': case 'r':
            return escala[(idx + 5) % 7]; //sexta nota
        case 'm': case 'n':
            return escala[(idx + 6) % 7]; //sétima nota
        default:
            return nota_atual; //mantém a nota 
    }
}

//Função geradora de onda quadrada para uma frequência
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

//Função geradora de silêncio (pausas)
void gerar_silencio(int16_t* buffer, int num_samples) {
    memset(buffer, 0, num_samples * sizeof(int16_t));
}

//Verificando se é vogal
int eh_vogal(char c) {
    return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}

//Verificando se é consoante mapeada
int eh_consoante(char c) {
    return (strchr("bpcksqdtfvgjlrmn", c) != NULL); //Colocando condição de consoantes diferentes de um valor nulo
}

//Configurando e estruturando main
int main() {
//Lendo arquivo de texto (.txt)
    FILE* f = fopen("entrada.txt", "r"); //FILE* f é um ponteiro; Definir abertura do arquivo (modo leitura "r") e assumindo o nome "entrada.txt" para ele
    if (!f) { //se não conseguiu abrir o arquivo, imprime mensagem de erro
        printf("Erro ao abrir entrada.txt\n");
        return 1;
    }
    char texto[4096]; //criação de vetor de caracteres para armazenar o conteúdo do texto do arquivo txt
    if (!fgets(texto, sizeof(texto), f)) { 
        printf("Erro ao ler entrada.txt\n");
        fclose(f); //fecha o arquivo se não conseguir ler 
        return 1;
    }
    fclose(f);

//Inicializando buffers de áudio
    int samples_por_som = (int)(SAMPLE_RATE * DURATION); //calcula quantas amostras tem cada som (nota ou pausa)
    int total_samples = samples_por_som * strlen(texto); //calcula quantas amostras tem o texto inteiro 
    int16_t* canal_esquerdo = calloc(total_samples, sizeof(int16_t)); //cria dois vetores para o áudio de cada canal preenchidos com zeros (sizeoff)
    int16_t* canal_direito  = calloc(total_samples, sizeof(int16_t)); 

//Defininindo notas iniciais
    double nota_esq = C4; //Começa em C4
    double nota_dir = C4; //Começa em C4

    printf("Sequencia de notas:\n"); //printando a sequencia de notas como requerido 

//Processar cada caractere
    int pos = 0; //indica o índice atual nos vetores de audio canal_esquerdo/direito; *cada caractere do texto ocupa samples_por_som amostras no audio 
    for (size_t i = 0; i < strlen(texto); i++) {
        char c = texto[i];
        if (c >= 'A' && c <= 'Z') c = c + 32; //Se for maiúscula, transforma em minúscula somando 32 ao valor do caractere (isso funciona porque no código ASCII, a diferença entre 'A' e 'a' é 32).
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
            // Ignorar outros caracteres
            printf("%c: [Ignorado]\n", c);
            gerar_onda_quadrada(&canal_esquerdo[pos], samples_por_som, nota_esq);
            gerar_onda_quadrada(&canal_direito[pos], samples_por_som, nota_dir);
        }
        pos += samples_por_som;
    }

//Gerando/Escrevendo arquivo WAV estéreo
    drwav_data_format format;
    format.container = drwav_container_riff; //container RIFF
    format.format = DR_WAVE_FORMAT_PCM; //formato PCM 
    format.channels = 2; //dois canais (esquerdo e direiro)
    format.sampleRate = SAMPLE_RATE; //taxa de amostragem 
    format.bitsPerSample = 16; //bits por amostra

    drwav wav; //inicializa a escrita do arquivo wav com o nome de "saida.wav"
    if (!drwav_init_file_write(&wav, "saida.wav", &format, NULL)) { 
        printf("Erro ao criar saida.wav\n"); //imprime mensagem de erro
        free(canal_esquerdo);
        free(canal_direito); //libera memória e encerra o programa em caso de erro
        return 1;
    }

// Utilizando um for para intercalar entre os canais (L, R, L, R, ...) os quais as letras R e L representam Right e Left, respectivamente
    for (int i = 0; i < total_samples; i++) {  // Inicia o loop para processar todas as amostras de áudio, uma por uma
        int16_t frame[2] = {canal_esquerdo[i], canal_direito[i]}; // Cria um "frame" estéreo combinando a amostra atual da esquerda e da direita.
        drwav_write_pcm_frames(&wav, 1, frame); // Escreve exatamente 1 frame (o par L+R) no arquivo WAV aberto.
    }                                                           
    drwav_uninit(&wav); //Finaliza o arquivo WAV, grava o tamanho total no cabeçalho e fecha o arquivo 

//Liberando memórias dos canais
    free(canal_esquerdo);
    free(canal_direito);

    printf("Arquivo saida.wav gerado!\n"); 
    return 0;
}