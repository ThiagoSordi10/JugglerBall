#include "game.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define TAM_RANK 10

const int LARGURA_TELA = 500;
const int ALTURA_TELA = 480;
ALLEGRO_BITMAP *area_central;
ALLEGRO_FONT *fonte30 = NULL;
ALLEGRO_FONT *fonte20 = NULL;
ALLEGRO_FONT *fonte10 = NULL;
ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL, *fila_contador = NULL;
ALLEGRO_TIMER *contador;

bool inicializar();
void tela_jogo();
void pause();
void game_over();
void modela_barra();
void help();
void ranking();

int main(void)
{
	bola_t bolas[3];
	barra_t barra;
	int opcao;

	barra.largura = 100;
	barra.altura = 30;


	if (!inicializar(barra))
	{
		return -1;
	}

	while (1 == 1) {
		opcao = tela_inicio();
		switch (opcao) {
		case 1:
			inicializa_bolas(bolas);
			tela_jogo(barra, bolas);
			opcao = 0;
			break;
		case 2:
			ranking();
			break;
		case 3:
			help();
			break;
		}
	}
	
	al_destroy_bitmap(area_central);
	al_destroy_font(fonte30);
	al_destroy_font(fonte20);
	al_destroy_font(fonte10);
	al_destroy_event_queue(fila_eventos);
	al_destroy_event_queue(fila_contador);
	al_destroy_display(janela);

	return 0;
}

int tela_inicio() {
	int opcao = 0;
	while (opcao == 0) {
		al_draw_textf(fonte30, al_map_rgb(255, 100, 100), LARGURA_TELA/2, ALTURA_TELA/2-100, ALLEGRO_ALIGN_CENTRE, "Juggler Ball");
		al_draw_textf(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2, ALLEGRO_ALIGN_CENTRE, "Inicia jogo");
		al_draw_textf(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + 50, ALLEGRO_ALIGN_CENTRE, "Recordes");
		al_draw_textf(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + 100, ALLEGRO_ALIGN_CENTRE, "Help");
		al_flip_display();
		al_clear_to_color(al_map_rgb(0, 0, 0));

		if (!al_is_event_queue_empty(fila_eventos))
		{
			ALLEGRO_EVENT evento;
			al_wait_for_event(fila_eventos, &evento);

			if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
			{
				if (evento.mouse.x >= LARGURA_TELA / 2 - 100 && evento.mouse.x <= LARGURA_TELA / 2 + 100 && 
					evento.mouse.y >= ALTURA_TELA / 2 - 10 && evento.mouse.y <= ALTURA_TELA / 2 + 10) {
					opcao = 1;
				}
				else if (evento.mouse.x >= LARGURA_TELA / 2 - 100 && evento.mouse.x <= LARGURA_TELA / 2 + 100 &&
					evento.mouse.y >= ALTURA_TELA / 2 + 40 && evento.mouse.y <= ALTURA_TELA / 2 + 60) {
					opcao = 2;
				}
				else if(evento.mouse.x >= LARGURA_TELA / 2 - 100 && evento.mouse.x <= LARGURA_TELA / 2 + 100 &&
					evento.mouse.y >= ALTURA_TELA / 2 + 90 && evento.mouse.y <= ALTURA_TELA / 2 + 110){
					opcao = 3;
				}
			}

			if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				exit(0);
			}
		}

	}
	return opcao;
}

void tela_game_over(int pontuacao, char *nome) {
	al_draw_textf(fonte30, al_map_rgb(255, 0, 0), LARGURA_TELA / 2, 90, ALLEGRO_ALIGN_CENTRE, "%s", "Game over");

	al_draw_textf(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 140, ALLEGRO_ALIGN_CENTRE, "Pontuacao: %d", pontuacao);
		al_draw_textf(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 160, ALLEGRO_ALIGN_CENTRE, "Nome> %s", nome);
	al_draw_textf(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 240, ALLEGRO_ALIGN_CENTRE, "%s", "Clique com o mouse para continuar");
	al_flip_display();
	al_clear_to_color(al_map_rgb(0, 0, 0));
}

void tela_pause() {
	al_draw_textf(fonte30, al_map_rgb(255, 0, 0), LARGURA_TELA / 2, 90, ALLEGRO_ALIGN_CENTRE, "%s", "Pausado");
	al_draw_textf(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 120, ALLEGRO_ALIGN_CENTRE, "%s", "Clique com o mouse para retornar ao jogo");
	al_draw_textf(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 140, ALLEGRO_ALIGN_CENTRE, "%s", "ou R para reinicar");
	al_flip_display();
}

void tela_ranking(jogador_t *jogadores) {
	al_draw_textf(fonte30, al_map_rgb(255, 100, 100), LARGURA_TELA / 2, ALTURA_TELA / 2 - 200, ALLEGRO_ALIGN_CENTRE, "Juggler Ball");
	al_draw_text(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 - 150, ALLEGRO_ALIGN_CENTRE, "Ranking");
	al_draw_text(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 - 100, ALLEGRO_ALIGN_CENTRE, "Posicao - Nome - Pontuacao");
	int posicao_y = -60;
	for (int i = 0; i < TAM_RANK; i++) {
		al_draw_textf(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + posicao_y, ALLEGRO_ALIGN_CENTRE, "%d - %s - %d", i+1, jogadores[i].nome, jogadores[i].pontuacao);
		posicao_y += 20;
	}
	al_draw_text(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + 140, ALLEGRO_ALIGN_CENTRE, "Clique com o mouse para voltar ao menu");
	al_flip_display();
}

void tela_help() {
	al_draw_textf(fonte30, al_map_rgb(255, 100, 100), LARGURA_TELA / 2, ALTURA_TELA / 2 - 200, ALLEGRO_ALIGN_CENTRE, "Juggler Ball");
	al_draw_text(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2 , ALTURA_TELA / 2 - 100, ALLEGRO_ALIGN_CENTRE, "O jogo consiste em fazer malabarismo com as bolas");
	al_draw_text(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2 , ALTURA_TELA / 2 - 80, ALLEGRO_ALIGN_CENTRE, "tentanto evitar que elas toquem no chao. Caso elas");
	al_draw_text(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2 , ALTURA_TELA / 2 - 60, ALLEGRO_ALIGN_CENTRE, "toquem no chao (vidas -1), a medida que cada toque");
	al_draw_text(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2 , ALTURA_TELA / 2 - 40, ALLEGRO_ALIGN_CENTRE, "na barra (pontos +20). Quanto mais pontos, voce");
	al_draw_text(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 - 20, ALLEGRO_ALIGN_CENTRE, "pode entrar no top 10 do jogo.");
	al_draw_text(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + 20, ALLEGRO_ALIGN_CENTRE, "O jogo possui um sistema de fases:");
	al_draw_text(fonte10, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + 40, ALLEGRO_ALIGN_CENTRE, "*Fase 1: 2 bolinhas - plataforma normal - velocidade normal");
	al_draw_text(fonte10, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + 60, ALLEGRO_ALIGN_CENTRE, "*Fase 2: 3 bolinhas - plataforma normal - velocidade normal - inicia em 200 pontos");
	al_draw_text(fonte10, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + 80, ALLEGRO_ALIGN_CENTRE, "*Fase 3: 3 bolinhas - plataforma normal - velocidade rapida - inicia em 500 pontos");
	al_draw_text(fonte10, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + 100, ALLEGRO_ALIGN_CENTRE, "*Fase 4: 3 bolinhas - plataforma menor - velocidade rapida - inicia em 700 pontos");
	al_draw_text(fonte10, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + 120, ALLEGRO_ALIGN_CENTRE, "-O jogo termina quando o jogador nao possui mais vidas. Boa Sorte");
	al_draw_text(fonte20, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + 140, ALLEGRO_ALIGN_CENTRE, "Clique com o mouse para voltar ao menu");
	al_flip_display();
}

void tela_jogo(barra_t barra, bola_t *bolas) {
	bool sair = false;
	int vidas = 5;
	int pontuacao = 0;
	int fase = 1;
	float velocidade = 0.08;
	int modificado = 0;
	int seg = 0, min = 0;

	al_start_timer(contador);

	barra.x = LARGURA_TELA / 2 - al_get_bitmap_width(area_central) / 2;
	barra.y = ALTURA_TELA - al_get_bitmap_height(area_central) - 10;

	while (!sair)
	{
		if (vidas == 0) {			
			game_over(&sair, pontuacao);
		}
		if (!al_is_event_queue_empty(fila_eventos))
		{
			ALLEGRO_EVENT evento;
			al_wait_for_event(fila_eventos, &evento);

			if (evento.type == ALLEGRO_EVENT_MOUSE_AXES)
			{
				barra.x = evento.mouse.x;
				if (barra.x > LARGURA_TELA - 100) {
					barra.x = LARGURA_TELA - 100;
				}
				else if (barra.x < 0) {
					barra.x = 0;
				}
			}
			else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
			{
				pause(&sair);
			}

			if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				exit(0);
			}
		}

		if (!al_is_event_queue_empty(fila_contador))
		{
			ALLEGRO_EVENT evento;
			al_wait_for_event(fila_contador, &evento);

			if (evento.type == ALLEGRO_EVENT_TIMER)
			{
				seg++;
				if (seg == 60)
				{
					min++;
					seg = 0;
				}
			}
		}

		al_set_target_bitmap(area_central);
		al_clear_to_color(al_map_rgb(255, 255, 255));

		al_set_target_bitmap(al_get_backbuffer(janela));
		al_draw_bitmap(area_central, barra.x, barra.y, 0);
		al_draw_filled_circle(bolas[0].x, bolas[0].y, bolas[0].raio, al_map_rgb(255, 0, 0));
		al_draw_filled_circle(bolas[1].x, bolas[1].y, bolas[1].raio, al_map_rgb(255, 255, 0));
		al_draw_filled_circle(bolas[2].x, bolas[2].y, bolas[2].raio, al_map_rgb(255, 0, 255));
		al_draw_textf(fonte20, al_map_rgb(255, 0, 0), 10, 10, ALLEGRO_ALIGN_LEFT, "Vidas: %d", vidas);
		al_draw_textf(fonte20, al_map_rgb(255, 255, 255), 100, 10, ALLEGRO_ALIGN_LEFT, "Fase: %d", fase);
		al_draw_textf(fonte20, al_map_rgb(255, 100, 0), 10, 45, ALLEGRO_ALIGN_LEFT, "Pontos: %d", pontuacao);
		al_draw_textf(fonte20, al_map_rgb(255, 0, 0), 350, 10, ALLEGRO_ALIGN_LEFT, "Tempo: %d:%d", min, seg);
		al_draw_text(fonte10, al_map_rgb(255, 255, 255), 10, 30, ALLEGRO_ALIGN_LEFT, "Clique com o mouse na tela para pausar");
		al_flip_display();
		al_clear_to_color(al_map_rgb(0, 0, 0));

		controla_jogo(bolas, barra, LARGURA_TELA, ALTURA_TELA, &vidas, &pontuacao, fase, velocidade);
		gerencia_fase(pontuacao, &fase, &velocidade, bolas, &barra, &modificado);
		modela_barra(barra, &modificado);
		al_rest(0.00003);
	}
}

void modela_barra(barra_t barra, int* modificado) {
	if (*modificado == 1) {
		*modificado = 0;
		area_central = al_create_bitmap(barra.largura, barra.altura);
	}
}

void manipular_entrada(ALLEGRO_EVENT evento, char *nome)
{
	if (evento.type == ALLEGRO_EVENT_KEY_CHAR)
	{
		if (strlen(nome) <= 16)
		{
			char temp[] = { evento.keyboard.unichar, '\0' };
			if (evento.keyboard.unichar == ' ')
			{
				strcat(nome, temp);
			}
			else if (evento.keyboard.unichar >= '0' &&
				evento.keyboard.unichar <= '9')
			{
				strcat(nome, temp);
			}
			else if (evento.keyboard.unichar >= 'A' &&
				evento.keyboard.unichar <= 'Z')
			{
				strcat(nome, temp);
			}
			else if (evento.keyboard.unichar >= 'a' &&
				evento.keyboard.unichar <= 'z')
			{
				strcat(nome, temp);
			}
		}

		if (evento.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(nome) != 0)
		{
			nome[strlen(nome) - 1] = '\0';
		}
	}
}

void help() {
	ALLEGRO_EVENT evento;
	evento.type = NULL;
	while (1 == 1) {
		if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			exit(0);
		}
		if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			break;
		}
		else {
			tela_help();
		}
		al_wait_for_event(fila_eventos, &evento);
	}
}

void ranking() {
	ALLEGRO_EVENT evento;
	evento.type = NULL;
	jogador_t jogadores[TAM_RANK];

	abre_ranking(jogadores);
	while (1 == 1) {
		if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			exit(0);
		}
		if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			break;
		}
		else {
			tela_ranking(jogadores);
		}
		al_wait_for_event(fila_eventos, &evento);
	}
}

void game_over(bool *sair, int pontuacao) {
	ALLEGRO_EVENT evento;
	evento.type = NULL;
	jogador_t jogadores[TAM_RANK];
	jogador_t jogador_atual;
	bool concluido = false;
	char nome[30] = "";

	while (!concluido) {

		manipular_entrada(evento, nome);
		if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			exit(0);
		}
		if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP || (evento.type == ALLEGRO_EVENT_KEY_DOWN && evento.keyboard.keycode == ALLEGRO_KEY_ENTER)) {
			*sair = true;
			concluido = true;
			break;
		}
		else {
			tela_game_over(pontuacao, nome);
		}
		al_wait_for_event(fila_eventos, &evento);
	}

	strcpy(jogador_atual.nome, nome);
	jogador_atual.pontuacao = pontuacao;

	abre_ranking(jogadores);
	modifica_ranking(jogador_atual, jogadores);
}

void pause(bool *sair) {
	int parada = 1;
	ALLEGRO_EVENT evento;
	evento.type = NULL;
	while (parada == 1) {
		if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			exit(0);
		}
		if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			parada = 0;
		}
		else if (evento.type == ALLEGRO_EVENT_KEY_DOWN && evento.keyboard.keycode == ALLEGRO_KEY_R) {
			*sair = true;
			parada = 0;
		}
		else {
			tela_pause();
		}
		al_wait_for_event(fila_eventos, &evento);
	}
}

bool inicializar(barra_t barra)
{
	if (!al_init())
	{
		fprintf(stderr, "Falha ao inicializar Allegro.\n");
		return false;
	}

	if (!al_install_keyboard())
	{
		fprintf(stderr, "Falha ao inicializar teclado.\n");
		return false;
	}

	if (!al_init_primitives_addon())
	{
		fprintf(stderr, "Falha ao inicializar add-on allegro_primitives.\n");
		return false;
	}

	al_init_font_addon();

	if (!al_init_ttf_addon())
	{
		fprintf(stderr, "Falha ao inicializar add-on allegro_ttf.\n");
		return false;
	}

	janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
	if (!janela)
	{
		fprintf(stderr, "Falha ao criar janela.\n");
		return false;
	}

	fonte30 = al_load_font("C:/Windows/Fonts/arial.ttf", 30, NULL);
	if (!fonte30)
	{
		al_destroy_display(janela);
		fprintf(stderr, "Falha ao carregar fonte.\n");
		return false;
	}

	fonte20 = al_load_font("C:/Windows/Fonts/arial.ttf", 20, NULL);
	if (!fonte20)
	{
		al_destroy_display(janela);
		fprintf(stderr, "Falha ao carregar fonte.\n");
		return false;
	}

	fonte10 = al_load_font("C:/Windows/Fonts/arial.ttf", 10, NULL);
	if (!fonte10)
	{
		al_destroy_display(janela);
		fprintf(stderr, "Falha ao carregar fonte.\n");
		return false;
	}

	contador = al_create_timer(1.0);
	if (!contador)
	{
		al_destroy_display(janela);
		fprintf(stderr, "Falha ao criar timer.\n");
		return false;
	}

	fila_contador = al_create_event_queue();
	if (!fila_contador)
	{
		al_destroy_display(janela);
		fprintf(stderr, "Falha ao criar fila do contador.\n");
		return false;
	}

	al_set_window_title(janela, "Juggler");

	area_central = al_create_bitmap(barra.largura, barra.altura);
	if (!area_central)
	{
		fprintf(stderr, "Falha ao criar bitmap.\n");
		al_destroy_display(janela);
		return false;
	}

	if (!al_install_mouse())
	{
		fprintf(stderr, "Falha ao inicializar o mouse.\n");
		al_destroy_display(janela);
		return false;
	}

	if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT))
	{
		fprintf(stderr, "Falha ao atribuir ponteiro do mouse.\n");
		al_destroy_display(janela);
		return false;
	}

	fila_eventos = al_create_event_queue();
	if (!fila_eventos)
	{
		fprintf(stderr, "Falha ao criar fila de eventos.\n");
		al_destroy_display(janela);
		return false;
	}

	al_register_event_source(fila_eventos, al_get_display_event_source(janela));
	al_register_event_source(fila_eventos, al_get_mouse_event_source());
	al_register_event_source(fila_eventos, al_get_keyboard_event_source());
	al_register_event_source(fila_contador, al_get_timer_event_source(contador));

	return true;
}