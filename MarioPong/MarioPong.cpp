/*	Arquivo: MarioPong.cpp
	Descricao: Game Pong com o tema de Super Mario
	Desenvolvedor: Luan Mantuani
*/

#include "stdafx.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdlib.h>

// Janela da aplicacao
ALLEGRO_DISPLAY *janela = NULL;

// VariÃ¡veis representando: jogador, fundo, placar
ALLEGRO_BITMAP *barra = NULL;
ALLEGRO_BITMAP *fundo = NULL;
ALLEGRO_BITMAP *placar = NULL;
ALLEGRO_BITMAP *moeda = NULL;
ALLEGRO_BITMAP *bolaFogo = NULL;
ALLEGRO_BITMAP *bola = NULL;
ALLEGRO_BITMAP *imgMenuInicial = NULL;
ALLEGRO_BITMAP *imgMenuPausa = NULL;
ALLEGRO_BITMAP *imgMarioVitoria = NULL;
ALLEGRO_BITMAP *imgLuigiVitoria = NULL;
ALLEGRO_BITMAP *imgVitoria = NULL;



//Variavel representando os pontos dos jogadores
ALLEGRO_FONT *fontPontos = NULL;
ALLEGRO_FONT *fontPontosSombra = NULL;


// Evento a ser processado no loop principal
ALLEGRO_EVENT evento;

// Fila de eventos a ser processados
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;

// Temporizador da renderizacao
ALLEGRO_TIMER *timer_renderizacao = NULL;

// Determina se o loop principal deve executar
bool executarLoopPrincipal = false;

// Determina se em uma interacao do loop princial deve-se renderizar o quadro
bool necessarioAtualizarRenderizacao = true;

// Constantes para o tamanho da janela
const int JAN_LARGURA = 1366;//1100
const int JAN_ALTURA = 768;//650
const int MEIO_X = (JAN_LARGURA / 2) - (1100 / 2);
const int MEIO_Y = (JAN_ALTURA / 2) - (650 / 2);

// Estruturas: jogador, bola
struct jogador {
	float px;
	float py = (JAN_ALTURA - 100) / 2;
	int pontos = 0;
	char cpontos[10];
	int altura;
	int largura;
	int sentidoX = 1;
	int sentidoY = -1;
	bool vitoria = false;
};

jogador p1, p2, b1;

// Velocidade que o jogador vai se mover
const float jogadorVelocidade = 8.0;
float bolaVelocidade = jogadorVelocidade;
int numeroAtaques = 0;
bool contraAtaque = false;

// Entrada do teclado do jogador (para as teclas W, S, Up, Down)
bool pressionandoW = false;
bool pressionandoS = false;
bool pressionandoUp = false;
bool pressionandoDown = false;
bool pressionandoEspaco = false;

bool pausaMenu = false;
bool telaVitoria = false;

// Declara o metodo de renderizacao
void renderizar();

/* Funcao principal */
int main()
{
	// Inicializa o Allegro
	al_install_system(ALLEGRO_VERSION_INT, NULL);

	//CONFIGURAR FULLSCREEN
	ALLEGRO_MONITOR_INFO info;
	int res_x_comp, res_y_comp;
	al_get_monitor_info(0, &info);
	res_x_comp = info.x2 - info.x1;
	res_y_comp = info.y2 - info.y1;
	al_set_new_display_flags(ALLEGRO_FULLSCREEN);

	// Cria a janela
	//janela = al_create_display(JAN_LARGURA, JAN_ALTURA);
	janela = al_create_display(res_x_comp, res_y_comp);

	/*float red_x = res_x_comp / (float)JAN_LARGURA;
	float red_y = res_y_comp / (float)JAN_ALTURA;
	ALLEGRO_TRANSFORM tranformar;
	al_identity_transform(&tranformar);
	al_scale_transform(&tranformar, red_x, red_y);
	al_use_transform(&tranformar);*/

	//Inicializa o add-on da font
	al_init_font_addon();

	//Configura o titulo da janela
	al_set_window_title(janela, "Mario Pong");

	//define a posiÃ§Ã£o da bola no meio da tela
	b1.px = JAN_LARGURA / 2;

	// Inicializa o add-on para utilizaÃ§Ã£o de imagens
	al_init_image_addon();

	//Inicializa o add-on para utilizaÃ§Ã£o de fontes ttf
	al_init_ttf_addon();

	//carrega as imagens: jogador, fundo, placar
	barra = al_load_bitmap("recursos/barra.png");
	fundo = al_load_bitmap("recursos/fundo.jpg");
	placar = al_load_bitmap("recursos/placar2.png");
	moeda = al_load_bitmap("recursos/moeda.png");
	bolaFogo = al_load_bitmap("recursos/bola_fogo.png");
	imgMenuInicial = al_load_bitmap("recursos/menu_inicial.png");
	imgMenuPausa = al_load_bitmap("recursos/menu_pausa.png");
	imgMarioVitoria = al_load_bitmap("recursos/mario_vitoria.png");
	imgLuigiVitoria = al_load_bitmap("recursos/luigi_vitoria.png");
	bola = moeda;



	//carrega a fonte do super mario
	fontPontos = al_load_font("recursos/Font.ttf", 54, 0);
	fontPontosSombra = al_load_font("recursos/Font.ttf", 64, 0);

	//define o valor de altura das barra e da bola
	p1.altura = al_get_bitmap_height(barra);
	p2.altura = al_get_bitmap_height(barra);
	b1.altura = al_get_bitmap_height(moeda);

	//define o valor da largura da barra e da bola
	p1.largura = al_get_bitmap_width(barra);
	p2.largura = al_get_bitmap_width(barra);
	b1.largura = al_get_bitmap_width(moeda);

	// Instala o addon para a entrada do teclado
	al_install_keyboard();

	// Cria a fila de eventos
	fila_eventos = al_create_event_queue();

	// Registra os eventos de teclado
	al_register_event_source(fila_eventos, al_get_keyboard_event_source());

	// Registra eventos da janela
	al_register_event_source(fila_eventos, al_get_display_event_source(janela));

	/* Cria o temporizador. O tempo Ã© em segundos, entao dividimos 1 segundo
		pela quantidade de quadros que queremos a cada segundo. Tenha em mente
		que quanto mais quadros, mais pesado fica a renderizacao.
		60 pareceu otimo pra este trabalho.*/
	timer_renderizacao = al_create_timer(1.0 / 30.0);

	// Registra os eventos do temporizador
	al_register_event_source(fila_eventos, al_get_timer_event_source(timer_renderizacao));

	// Inicia o temporizador
	al_start_timer(timer_renderizacao);

	//Loop Inicial
	al_draw_scaled_bitmap(imgMenuInicial, 0, 0, 1920, 1080, 0, 0, JAN_LARGURA, JAN_ALTURA, 0);
	al_flip_display();
	while (!executarLoopPrincipal) {
		al_wait_for_event(fila_eventos, &evento);
		if ((evento.type == ALLEGRO_EVENT_KEY_DOWN) && (evento.keyboard.keycode == ALLEGRO_KEY_SPACE)) {
			executarLoopPrincipal = true;
		}
	}

	// Loop principal
	while (executarLoopPrincipal) {

		// Loop para interagir com todos os eventos na lista de eventos
		while (!al_is_event_queue_empty(fila_eventos)) {

			// Busca pelo proximo evento na lista de eventos
			al_wait_for_event(fila_eventos, &evento);

			switch (evento.type) {
				// Caso o evento for do tipo 'Temporizador'...
			case ALLEGRO_EVENT_TIMER:
				// Se o temporizador for o timer_renderizacao...
				if (evento.timer.source == timer_renderizacao)
					necessarioAtualizarRenderizacao = true;
				break;

				//	// Caso o evento for do tipo 'Pressionar Tecla'...
			case ALLEGRO_EVENT_KEY_DOWN:
				if (evento.keyboard.keycode == ALLEGRO_KEY_W)
					pressionandoW = true;
				else if (evento.keyboard.keycode == ALLEGRO_KEY_S)
					pressionandoS = true;
				else if (evento.keyboard.keycode == ALLEGRO_KEY_UP)
					pressionandoUp = true;
				else if (evento.keyboard.keycode == ALLEGRO_KEY_DOWN)
					pressionandoDown = true;
				else if (evento.keyboard.keycode == ALLEGRO_KEY_SPACE)
					pressionandoEspaco = true;
				else if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					executarLoopPrincipal = false;
				break;

				//	// Caso o evento for do tipo 'Soltar Tecla'...
			case ALLEGRO_EVENT_KEY_UP:
				if (evento.keyboard.keycode == ALLEGRO_KEY_W)
					pressionandoW = false;
				else if (evento.keyboard.keycode == ALLEGRO_KEY_S)
					pressionandoS = false;
				else if (evento.keyboard.keycode == ALLEGRO_KEY_UP)
					pressionandoUp = false;
				else if (evento.keyboard.keycode == ALLEGRO_KEY_DOWN)
					pressionandoDown = false;
				break;

				// Caso o evento for do tipo 'Clique no botao de fechar a janela'...
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				executarLoopPrincipal = false; // Finaliza o loop principal
				break;
			}
		}

		if (telaVitoria) {
			al_draw_scaled_bitmap(imgVitoria, 0, 0, 1920, 1080, 0, 0, JAN_LARGURA, JAN_ALTURA, 0);
			al_flip_display();
			while (telaVitoria) {
				// Busca pelo proximo evento na lista de eventos
				al_wait_for_event(fila_eventos, &evento);
				if ((evento.type == ALLEGRO_EVENT_KEY_DOWN) && (evento.keyboard.keycode == ALLEGRO_KEY_SPACE)) {
					telaVitoria = false;
					p1.pontos = 0;
					p2.pontos = 0;
				}
				else if ((evento.type == ALLEGRO_EVENT_KEY_DOWN) && (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
					telaVitoria = false;
					executarLoopPrincipal = false;
				}
			}
		}

		//Se for necessario renderizar nessa interacao do loop...
		if (necessarioAtualizarRenderizacao && pressionandoEspaco == false) {
			renderizar(); // Chama a funcao de renderizacao
			necessarioAtualizarRenderizacao = false;
		}
		//Quando pressionar espaÃ§o o jogo Ã© pausado e mostra um painel com os controles e os nomes dos alunos
		else if (pressionandoEspaco == true) {
			al_draw_bitmap(imgMenuPausa, MEIO_X, MEIO_Y, 0);
			al_flip_display();
			while (pressionandoEspaco) {
				// Busca pelo proximo evento na lista de eventos
				al_wait_for_event(fila_eventos, &evento);
				if ((evento.type == ALLEGRO_EVENT_KEY_DOWN) && (evento.keyboard.keycode == ALLEGRO_KEY_SPACE)) {
					pressionandoEspaco = false;
				}
				else if ((evento.type == ALLEGRO_EVENT_KEY_DOWN) && (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
					pressionandoEspaco = false;
					executarLoopPrincipal = false;
				}
			}
		}


	}
}

/* Funcao de renderizacao e logica dos objetos */
void renderizar() {

	if (contraAtaque && numeroAtaques > 3) {
		bola = bolaFogo;
		bolaVelocidade = bolaVelocidade * 2;
		contraAtaque = false;
	}

	//define as posiÃ§oes de X dos jogadores
	p1.px = 20;
	p2.px = JAN_LARGURA - (20 + p2.largura);

	// Manipula a posicao vertical do jogador 1 baseado na entrada
	if (pressionandoW == true) {
		if (p1.py > 100)
			p1.py -= jogadorVelocidade;
	}
	else if (pressionandoS == true) {
		if (p1.py + p1.altura < JAN_ALTURA)
			p1.py += jogadorVelocidade;
	}

	// Manipula a posicao vertical do jogador 2 baseado na entrada
	if (pressionandoUp == true) {
		if (p2.py > 100)
			p2.py -= jogadorVelocidade;
	}
	else if (pressionandoDown == true) {
		if (p2.py + p2.altura < JAN_ALTURA)
			p2.py += jogadorVelocidade;
	}

	//calcula trajetoria X da bola, sendo sentidoX = 1(indo para direita)
	if (b1.sentidoX == 1) {
		if ((b1.px + b1.largura >= p2.px) && (b1.py + b1.altura >= p2.py) && (b1.py <= p2.py + p2.altura)) {
			b1.sentidoX = -1;
			numeroAtaques++;
			contraAtaque = true;
		}
		else if (b1.px + b1.largura >= JAN_LARGURA) {
			b1.px = JAN_LARGURA / 2;
			b1.py = p2.py + p2.altura / 2;
			b1.sentidoX = -1;
			p1.pontos++;
			bolaVelocidade = jogadorVelocidade;
			numeroAtaques = 0;
			bola = moeda;
		}
		else b1.px += bolaVelocidade;
	}
	//calcula trajetoria X da bola, sendo sentidoX = -1(indo para esquerda)
	else if (b1.sentidoX == -1) {
		if ((b1.px <= p1.px + p1.largura) && (b1.py + b1.altura >= p1.py) && (b1.py <= p1.py + p1.altura)) {
			b1.sentidoX = 1;
			numeroAtaques++;
			contraAtaque = true;
		}
		else if (b1.px <= 0) {
			b1.px = JAN_LARGURA / 2;
			b1.py = p1.py + p1.altura / 2;
			b1.sentidoX = 1;
			p2.pontos++;
			bolaVelocidade = jogadorVelocidade;
			numeroAtaques = 0;
			bola = moeda;
		}
		else b1.px -= bolaVelocidade;
	}

	//Calcula a trgetoria Y da bola, sendo sentidoY = 1 (indo para baixo)
	if (b1.sentidoY == 1) {
		if (b1.py + b1.altura >= JAN_ALTURA) {
			b1.sentidoY = -1;
		}
		else b1.py += bolaVelocidade;
	}
	//Calcula a trgetoria Y da bola, sendo sentidoY = -1 (indo para cima)
	else if (b1.sentidoY == -1) {
		if (b1.py <= 100) {
			b1.sentidoY = 1;
		}
		else b1.py -= bolaVelocidade;
	}

	//calcula pontos
	if (p1.pontos > 3) {
		imgVitoria = imgMarioVitoria;
		telaVitoria = true;
	}
	else if (p2.pontos > 3) {
		imgVitoria = imgLuigiVitoria;
		telaVitoria = true;
	}

	// Desenha o fundo
	al_draw_scaled_bitmap(fundo, 0, 0, 1100, 650, 0, 0, JAN_LARGURA, JAN_ALTURA, 0);

	// Desenha o placar
	al_draw_scaled_bitmap(placar, 0, 0, 1100, 650, 0, 0, JAN_LARGURA, 650, 0);

	// Desenha os jogadores
	al_draw_bitmap(barra, p1.px, p1.py, 0);
	al_draw_bitmap(barra, p2.px, p2.py, 0);

	//Desenha a bola
	al_draw_bitmap(bola, b1.px, b1.py, 0);

	//Desenha os pontos de cada jogador
	_itoa_s(p1.pontos, p1.cpontos, 10);
	al_draw_text(fontPontosSombra, al_map_rgb(0, 0, 0), JAN_LARGURA / 4, 15, ALLEGRO_ALIGN_CENTER, p1.cpontos);
	al_draw_text(fontPontos, al_map_rgb(255, 255, 255), JAN_LARGURA / 4, 23, ALLEGRO_ALIGN_CENTER, p1.cpontos);

	_itoa_s(p2.pontos, p2.cpontos, 10);
	al_draw_text(fontPontosSombra, al_map_rgb(0, 0, 0), 3 * JAN_LARGURA / 4, 15, ALLEGRO_ALIGN_CENTER, p2.cpontos);
	al_draw_text(fontPontos, al_map_rgb(255, 255, 255), 3 * JAN_LARGURA / 4, 23, ALLEGRO_ALIGN_CENTER, p2.cpontos);

	al_draw_text(fontPontosSombra, al_map_rgb(0, 0, 0), JAN_LARGURA / 2, 15, ALLEGRO_ALIGN_CENTER, "VS");
	al_draw_text(fontPontos, al_map_rgb(255, 255, 255), JAN_LARGURA / 2, 23, ALLEGRO_ALIGN_CENTER, "VS");

	// Renderiza o buffer na tela
	al_flip_display();
}