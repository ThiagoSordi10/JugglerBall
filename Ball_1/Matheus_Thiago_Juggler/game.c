#include "game.h"
#include <stdio.h>
#define RAIO 15.0
#define TAM_RANK 10

void colisao_paredes(bola_t* bola, int largura_tela) {
	if ((*bola).x >= largura_tela - (*bola).raio)
	{
		(*bola).dir_x = -2;
		(*bola).x = largura_tela - (*bola).raio;
	}
	else if ((*bola).x <= (*bola).raio) {
		(*bola).dir_x = 2;
		(*bola).x = (*bola).raio;
	}
}

int colisao_terra(bola_t* bola, int altura_tela) {
	if ((*bola).y >= altura_tela - (*bola).raio - 5)
	{
		(*bola).dir_y = -80;
		return 1;
	}
	return 0;
}

void trajeto_bola(bola_t* bola, float velocidade) {
	if ((*bola).dir_y != 0 || (*bola).dir_x != 0) {
		(*bola).dir_y = (*bola).dir_y + velocidade; 
	}

	(*bola).x += velocidade * (*bola).dir_x;
	(*bola).y += ((*bola).dir_y * 0.01);
}

void controla_jogo(bola_t *bolas, barra_t barra, int largura_tela, int altura_tela, int* vidas, int* pontuacao, int fase, float velocidade) {
	for (int i = 0; i < 3; i++) {

		trajeto_bola(&bolas[i], velocidade);

		colisao_paredes(&bolas[i], largura_tela);

		if (bolas[i].y >= barra.y - 15 && bolas[i].y <= barra.y - 14 && (bolas[i].x <= (barra.x + barra.largura+15) && bolas[i].x >= barra.x-15)) {
			if (bolas[i].x >= barra.x - 50 && bolas[i].x < barra.x) {
				bolas[i].dir_x = -2;
			}
			else if (bolas[i].x <= barra.x + 50 && bolas[i].x >= barra.x) {
				bolas[i].dir_x = 2;
			}
			*pontuacao += 20;
			if (bolas[1].dir_x == 0 && bolas[1].dir_y == 0) {
				bolas[1].dir_x = 1;
				bolas[1].dir_y = 1;
			}
			bolas[i].dir_y = -80;
		}

		if (colisao_terra(&bolas[i], altura_tela) == 1) {
			*vidas -= 1;
		}
	}
}

void inicializa_bolas(bola_t *bolas) {
	int posicaoX = 200;
	for (int i = 0; i < 3; i++) {
		bolas[i].raio = RAIO;
		bolas[i].x = posicaoX;
		bolas[i].y = 15;
		posicaoX += 50;
	}
	bolas[0].dir_x = 1;
	bolas[0].dir_y = 1;
	bolas[1].dir_x = 0;
	bolas[1].dir_y = 0;
	bolas[2].dir_x = 0;
	bolas[2].dir_y = 0;
}

void gerencia_fase(int pontuacao, int* fase, float* velocidade, bola_t *bolas, barra_t* barra, int *modificado) {
	if (pontuacao >= 200 && pontuacao < 500) {
		*fase = 2;
		if (bolas[0].dir_y < -30 && bolas[0].dir_y > -50 && bolas[2].dir_x == 0 && bolas[2].dir_y == 0) {
			bolas[2].dir_x = 1;
			bolas[2].dir_y = 1;
		}
	}
	else if (pontuacao >= 500 && pontuacao < 700 ) {
		*velocidade = 0.12;
		*fase = 3;
	}
	else if (pontuacao >= 700 && *fase != 4) {
		*fase = 4;
		(*barra).largura = (*barra).largura / 2;
		*modificado = 1;
	}
}

void move_jogadores_ranking(jogador_t jogador_novo, jogador_t *jogadores, int pos) {
	jogador_t jogador_aux;
	for (int i = pos; i < TAM_RANK; i++) {
		jogador_aux = jogadores[i];
		jogadores[i] = jogador_novo;
		jogador_novo = jogador_aux;
	}
}

void modifica_ranking(jogador_t jogador, jogador_t *jogadores) {
	FILE *arq;

	for (int i = 0; i < TAM_RANK; i++) {
		if (jogador.pontuacao >= jogadores[i].pontuacao) {
			move_jogadores_ranking(jogador, jogadores, i);
			break;
		}
	}

	arq = fopen("ranking.txt", "w");
	printf("\n\n");
	for (int i = 0; i < TAM_RANK; i++) {
		if (i + 1 == TAM_RANK) {
			fprintf(arq, "%s,%d", jogadores[i].nome, jogadores[i].pontuacao);
		}
		else {
			fprintf(arq, "%s,%d\n", jogadores[i].nome, jogadores[i].pontuacao);
		}
		printf("%s\n", jogadores[i].nome);
	}

	fclose(arq);
	
}

void abre_ranking(jogador_t *jogadores) {
	FILE *arq;
	char c, numero_str[15];
	int i = 0;
	int j = 0;
	int antes_virgula = 1;

	arq = fopen("ranking.txt", "r+");

	if (arq == NULL)
	{
		printf("ERRO! O arquivo não foi aberto!\n");
		return;
	}

	do
	{
		c = getc(arq);
		if (c == ',') {
			jogadores[i].nome[j] = '\0';
			j = 0;
			antes_virgula = 0;
			continue;
		}

		if (c == '\n') {
			jogadores[i].pontuacao = atoi(numero_str);
			printf("%s\n", jogadores[i].nome);
			numero_str[j-1] = '\0';
			i++;
			j = 0;
			antes_virgula = 1;
			continue;
		}

		if (antes_virgula == 1) {
			jogadores[i].nome[j] = c;
			j++;
		}
		else {
			numero_str[j] = c;
			j++;
		}


	} while (c != EOF);

	jogadores[i].pontuacao = atoi(numero_str);

	fclose(arq);

}
