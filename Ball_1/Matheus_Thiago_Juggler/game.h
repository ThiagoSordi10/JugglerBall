#ifndef __GAME__H__
#define __GAME__H__

struct bola {
	float raio;
	float x;
	float y;
	float dir_x, dir_y;
};

struct barra {
	float x, y;
	float largura, altura;
};

struct jogador {
	char nome[30];
	int pontuacao;
};

typedef struct bola bola_t;
typedef struct barra barra_t;
typedef struct jogador jogador_t;

void controla_jogo(bola_t *bolas, barra_t barra, int largura_tela, int altura_tela, int* vidas, int* pontuacao, int fase, float velocidade);

void inicializa_bolas(bola_t *bolas);

void gerencia_fase(int pontuacao, int* fase, float* velocidade, bola_t * bolas, barra_t* barra, int* modificado);

void modifica_ranking(jogador_t jogador, jogador_t *jogadores);

void abre_ranking(jogador_t *jogadores);

#endif
