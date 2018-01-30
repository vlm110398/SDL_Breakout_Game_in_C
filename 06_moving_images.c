/*
 * 06_moving_images.c
 *
 * Nome: Joao Vitor de Freitas Barbosa
 * DRE: 117055449
 *
 * Este programa move duas imagens pela tela. Ao colidir com uma
 * parede as imagens irão ser refletidas.
 * 25/05/2017
 * @adriano cruz
 */

/*Using SDL, SDL_image, standard IO, and strings*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

/*
 * Constants
 */

/*Screen dimension constants*/
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int false = 0;
const int true = 1;

const int IMAGE_WIDTH = 61;
const int IMAGE_HEIGHT = 61;

const int BLOCK_WIDTH = 91;
const int BLOCK_HEIGHT = 41;

const int PLAYER_WIDTH = 121;
const int PLAYER_HEIGHT = 31;

typedef struct _NPC {
    int posX;
    int posY;
    int stepX;
    int stepY;
    SDL_Surface* image;
    int imgW;
    int imgH;
    int armor;
} NPC;

/*
 * Global Variables
 */

/*The window we'll be rendering to*/
SDL_Window* gWindow = NULL;

/*The imagem character*/
NPC ball;
NPC ball2;
NPC blocks[2][8];
NPC player;

/*The surface contained by the window*/
SDL_Surface* gScreenSurface = NULL;

/*Current displayed PNG image*/
SDL_Surface* gJPGSurface = NULL;

SDL_Surface* gJPGTEMPSurface = NULL;
SDL_Surface* gJPGSurface23 = NULL;
SDL_Surface* gJPGSurface22 = NULL;
SDL_Surface* gJPGSurface21 = NULL;

SDL_Surface* gJPGSurface3 = NULL;

SDL_Surface* initial = NULL;
SDL_Surface* initial2 = NULL;
SDL_Surface* initial3 = NULL;
SDL_Surface* initialTEMP = NULL;

SDL_Surface* surface_vida = NULL;
SDL_Surface* pontos = NULL;
SDL_Surface* nivel = NULL;

/*SDL TTF*/
TTF_Font *font = NULL;
SDL_Color WHITE = {255, 255 ,255};


char life[15] = "VIDAS: X3";
char snivel[15] = "NIVEL: 1";
char spontucao[15] = "PONTOS: 0";


int vida = 3;
int o_nivel = 1;
int pontuacao = 0;
int blocos_destruidos = 0;

Mix_Chunk* sound;
Mix_Chunk* sound2;
Mix_Chunk* sound3;

int direita = 0;
int esquerda = 0;

/*
 * function prototypes
 */

/*Starts up SDL and creates window*/
int init();

/*Loads media*/
int loadMedia();

/*Frees media and shuts down SDL*/
void closing();

/*Loads individual image*/
SDL_Surface* loadSurface( char *path );

/*Create NPC*/
NPC createNPC( int posX, int posY, int stepX, int stepY, SDL_Surface *image, int armor);


/*Move NPC*/
void moveBALL(NPC *b);
void movePLAYER(NPC *p);
void BallVSPlayer(NPC *b, NPC *p);
void BallVSBlock(NPC *b, NPC *bloc);
void DestroyBlock(NPC *bloc);
void onCleanup(void);
void reconstruir();

int main( int argc, char* args[] ) {
	int i;
	int j;
	int k;
	int l;
	int armorTEMP;
	int play;
	play = 0;
	armorTEMP = 0;
    SDL_Rect srcRect, dstRect; /*bola 1*/
    SDL_Rect srcRect2, dstRect2; /*bola 2*/
    SDL_Rect rect_vida;
    SDL_Rect srcRect3[2][8], dstRect3[2][8];  /*blocos*/
    SDL_Rect srcRect4, dstRect4; /*player*/
    SDL_Rect rect_nivel;
    SDL_Rect rect_pontos;
    SDL_Rect srcRecti, dstRecti; /*tela inicial*/

    int quit;
    /*Event handler*/
    SDL_Event e;

    /*Start up SDL and create window*/
    if( !init() ) {
        printf( "Failed to initialize!\n" );
    }
    else {
        /*Load media*/
        if( !loadMedia() ) {
            printf( "Failed to load media!\n" );
        }
        else {


            /*Create NPC*/
            ball = createNPC(rand() % (SCREEN_WIDTH - IMAGE_WIDTH),
                             rand() % ( ( (SCREEN_HEIGHT - IMAGE_HEIGHT - (PLAYER_HEIGHT - 15) ) - ((2*BLOCK_HEIGHT)+10) ) + 1 ) + ((2*BLOCK_HEIGHT)+10) ,
                             rand() % 2 ? -1: 1,
                             rand() % 2 ? -1: 1,
                             gJPGSurface,
                             0);
            ball2 = createNPC(rand() % (SCREEN_WIDTH - IMAGE_WIDTH),
                             rand() %  ( ( (SCREEN_HEIGHT - IMAGE_HEIGHT - (PLAYER_HEIGHT - 15) ) - ((2*BLOCK_HEIGHT)+5) ) + 1 ) + ((2*BLOCK_HEIGHT)+5),
                             rand() % 2 ? -1: 1,
                             rand() % 2 ? -1: 1,
                             gJPGSurface,
                             0);

			player = createNPC( SCREEN_WIDTH/2 - PLAYER_WIDTH/2,
								SCREEN_HEIGHT - PLAYER_HEIGHT - 15,
								5,
								0,
								gJPGSurface3,
								0);

            for(i = 0, l = 0; i < 2; i++)
            {
				for(j = 0, k = 25; j < 8; j++)
				{
					/*decidindo a cor*/
					armorTEMP = (rand() % ((3 - 1) +1)) + 1;
					if(armorTEMP == 3)
						gJPGTEMPSurface = gJPGSurface23;
					else if(armorTEMP == 2)
						gJPGTEMPSurface = gJPGSurface22;
					else if(armorTEMP == 1)
						gJPGTEMPSurface = gJPGSurface21;

					blocks[i][j] = createNPC(k,
											 l,
											 0,
											 0,
											 gJPGTEMPSurface,
											 armorTEMP);

					k+= BLOCK_WIDTH+1;
				}
				l+= BLOCK_HEIGHT+1;
			}

			initialTEMP = initial;
            /*Main loop flag*/
            quit = false;

            /*While application is running*/
            while( !quit ) {
                while( SDL_PollEvent( &e ) != 0 ) {
                    switch (e.type) {
                        case SDL_QUIT:
                            quit = true;
                            break;
                        case SDL_KEYDOWN:
							if (e.key.keysym.sym == SDLK_ESCAPE)
							{
								quit = true;
							}
							else if(play)
							{
								if(e.key.keysym.sym == SDLK_LEFT)
								{
									esquerda = 1;
								}
								if(e.key.keysym.sym == SDLK_RIGHT)
								{
									direita = 1;
								}
							}
							else
							{
								if(e.key.keysym.sym == SDLK_RETURN)
								{
									play = 1;
								}
								else if(e.key.keysym.sym == SDLK_x)
								{
									initialTEMP = initial2;
								}
								else if(e.key.keysym.sym == SDLK_c)
								{
									initialTEMP = initial3;
								}
								else if(e.key.keysym.sym == SDLK_z)
								{
									initialTEMP = initial;
								}
							}
							break;
                        case SDL_KEYUP:
							if(e.key.keysym.sym == SDLK_LEFT)
							{
								esquerda = 0;
							}
							if(e.key.keysym.sym == SDLK_RIGHT)
							{
								direita = 0;
							}
							break;
                    }
                }

				/*Fill the surface white*/
				SDL_FillRect( gScreenSurface, NULL,
							  SDL_MapRGB( gScreenSurface->format,
							  000, 000, 000) );
                if(play)
                {

					/*SDL_FreeSurface( initial );
					initial = NULL;*/

					for(i = 0; i < 2; i++)
					{
						for(j = 0; j < 8; j++)
						{
							/*moveNPC(&blocks[i][j]);*/

							BallVSBlock(&ball, &blocks[i][j]);
							BallVSBlock(&ball2, &blocks[i][j]);

							srcRect3[i][j].x = 0; srcRect3[i][j].y = 0;
							srcRect3[i][j].w = BLOCK_WIDTH;
							srcRect3[i][j].h = BLOCK_HEIGHT;
							dstRect3[i][j].x = blocks[i][j].posX;
							dstRect3[i][j].y = blocks[i][j].posY;

						}
					}

					/*bola1*/
					srcRect.x = 0; srcRect.y = 0;
					srcRect.w = IMAGE_WIDTH;
					srcRect.h = IMAGE_HEIGHT;
					dstRect.x = ball.posX;
					dstRect.y = ball.posY;

					/*bola2*/
					srcRect2.x = 0; srcRect2.y = 0;
					srcRect2.w = IMAGE_WIDTH;
					srcRect2.h = IMAGE_HEIGHT;
					dstRect2.x = ball2.posX;
					dstRect2.y = ball2.posY;

					/*player*/
					srcRect4.x = 0; srcRect4.y = 0;
					srcRect4.w = PLAYER_WIDTH;
					srcRect4.h = PLAYER_HEIGHT;
					dstRect4.x = player.posX;
					dstRect4.y = player.posY;

					movePLAYER(&player);
					moveBALL(&ball);
					moveBALL(&ball2);
					BallVSPlayer(&ball, &player);
					BallVSPlayer(&ball2, &player);

					rect_vida.h = 20;
					rect_vida.w = 20;
					rect_vida.x = 350;
					rect_vida.y = 400;

                    rect_pontos.h = 20;
                    rect_pontos.w = 20;
                    rect_pontos.x = 350;
                    rect_pontos.y = 370;

                    rect_nivel.h = 20;
                    rect_nivel.w = 20;
                    rect_nivel.x = 350;
                    rect_nivel.y = 340;

					if( SDL_BlitSurface( ball.image, &srcRect,
										 gScreenSurface, &dstRect ) < 0 ) {
						printf( "SDL could not blit! SDL Error: %s\n", SDL_GetError() );
						quit = true;
					}

					if( SDL_BlitSurface( ball2.image, &srcRect2,
										 gScreenSurface, &dstRect2 ) < 0 )
					{
						printf( "SDL could not blit! SDL Error: %s\n", SDL_GetError() );
						quit = true;
					}

					if( SDL_BlitSurface( player.image, &srcRect4,
										 gScreenSurface, &dstRect4 ) < 0 )
					{
						printf( "SDL could not blit! SDL Error: %s\n", SDL_GetError() );
						quit = true;
					}
					if(SDL_BlitSurface(surface_vida,NULL, gScreenSurface, &rect_vida) < 0){
                        printf( "SDL could not blit! SDL Error: %s\n", SDL_GetError());

					}
					if(SDL_BlitSurface(pontos,NULL, gScreenSurface, &rect_pontos) < 0){
                        printf( "SDL could not blit! SDL Error: %s\n", SDL_GetError());

					}
					if(SDL_BlitSurface(nivel,NULL, gScreenSurface, &rect_nivel) < 0){
                        printf( "SDL could not blit! SDL Error: %s\n", SDL_GetError());

					}

					for(i = 0; i < 2; i++)
					{
						for(j = 0; j < 8; j++)
						{
							if( SDL_BlitSurface( blocks[i][j].image, &srcRect3[i][j],
												 gScreenSurface, &dstRect3[i][j] ) < 0 )
							{
								printf( "SDL could not blit! SDL Error: %s\n", SDL_GetError() );
								quit = true;
							}
						}
					}
				}
				else
				{
					srcRecti.x = 0; srcRecti.y = 0;
					srcRecti.w = SCREEN_WIDTH;
					srcRecti.h = SCREEN_HEIGHT;
					dstRecti.x = 0;
					dstRecti.y = 0;

					if( SDL_BlitSurface( initialTEMP, &srcRecti,
										 gScreenSurface, &dstRecti ) < 0 )
					{
						printf( "SDL could not blit! SDL Error: %s\n", SDL_GetError() );
						quit = true;
					}
				}

				/*Update the surface*/
				SDL_UpdateWindowSurface( gWindow );

				/* Not so good solution, depends on your computer*/
				SDL_Delay(4);

            }
        }
    }

    /*Free resources and closing SDL*/
    closing();

    return 0;
}

/*Collision between ball and block*/
void BallVSBlock(NPC *b, NPC *bloc)
{
	/*p é a bola, e b é o bloco*/
	if( b->posY < bloc->posY + BLOCK_HEIGHT &&
		b->posY + IMAGE_HEIGHT > bloc->posY &&
		b->posX + IMAGE_WIDTH > bloc->posX &&
		b->posX < bloc->posX + BLOCK_WIDTH)
	{
		b->stepY = -b->stepY;
		b->posY += b->stepY;
		if(bloc->armor == 1)
		{
			DestroyBlock(bloc);
		}
		else
			bloc->armor-=1;
	}

}

/*"destruction" of block*/
void DestroyBlock(NPC *bloc)
{
	bloc->posX = 20000;
	pontuacao += 100;
	blocos_destruidos += 1;
    sprintf(spontucao, "PONTOS: %d", pontuacao);
    pontos = TTF_RenderText_Solid(font, spontucao, WHITE);
    if(blocos_destruidos == 16){
        reconstruir();
        blocos_destruidos = 0;
        pontuacao += 1000;
        sprintf(spontucao, "PONTOS: %d", pontuacao);
        pontos = TTF_RenderText_Solid(font, spontucao, WHITE);
        o_nivel += 1;
        sprintf(snivel, "NIVEL: %d", o_nivel);
        nivel = TTF_RenderText_Solid(font, snivel, WHITE);
    }
    if(pontuacao > 10000){
        vida += 1;
        pontuacao -= 10000;
        sprintf(spontucao, "PONTOS: %d", pontuacao);
        pontos = TTF_RenderText_Solid(font, spontucao, WHITE);
        sprintf(life, "VIDAS: X%d", vida);
        surface_vida = TTF_RenderText_Solid(font, life, WHITE);

    }
}

/*Collision between ball and player*/
void BallVSPlayer (NPC *b, NPC *p)
{
	if(b->stepY > 0)
	{
		if( b->posY < p->posY + PLAYER_HEIGHT &&
			b->posY + IMAGE_HEIGHT > p->posY &&
			b->posX + IMAGE_WIDTH > p->posX &&
			b->posX < p->posX + PLAYER_WIDTH)
		{
			b->stepY = -b->stepY;
			b->posY += b->stepY;
			if(b->posY + IMAGE_HEIGHT > p->posY &&
			   b->posX + IMAGE_WIDTH > p->posX &&
			   b->posX < p->posX + PLAYER_WIDTH)
			{
				b->stepX = -b->stepX;
				b->posX += b->stepX;
			}
		}
	}
}

/*Move of the ball*/
void moveBALL(NPC *b) {
    b->posX += b->stepX;
    b->posY += b->stepY;

    /*colisao com os cantos laterais da tela
    if ( (b->posX + IMAGE_WIDTH > SCREEN_WIDTH) ||
         (b->posX < 0) ) {
        b->stepX = -b->stepX;
        b->posX += b->stepX;
        Mix_PlayChannel(-1, sound, 0);
    }*/

    /*Fazendo a transicao da bola entre os cantos da tela, nao ha colisao*/
    if (b->posX > SCREEN_WIDTH)
    {
		b->posX = 0 - IMAGE_WIDTH;
	}
	if (b->posX + IMAGE_WIDTH < 0)
	{
		b->posX = SCREEN_WIDTH;
	}
	/*fim da transicao*/

	/*colisao com os cantos verticais da tela*/
    if ( (b->posY + IMAGE_HEIGHT > SCREEN_HEIGHT) ||
         (b->posY < 0) )
	{
		if(b->posY + IMAGE_HEIGHT > SCREEN_HEIGHT)
		{
			Mix_PlayChannel(-1, sound2, 0);
            if(vida > 0){
                vida -= 1;
                sprintf(life, "VIDAS: X%d", vida);
                surface_vida = TTF_RenderText_Solid(font, life, WHITE);
            }
            if(vida == 0){
                blocos_destruidos = 0;
                reconstruir();
                vida = 3;
                sprintf(life, "VIDAS: X%d", vida);
                surface_vida = TTF_RenderText_Solid(font, life, WHITE);
                pontuacao = 0;
                sprintf(spontucao, "PONTOS: %d", pontuacao);
                pontos = TTF_RenderText_Solid(font, spontucao, WHITE);
                o_nivel = 1;
                sprintf(snivel, "NIVEL: %d", o_nivel);
                nivel = TTF_RenderText_Solid(font, snivel, WHITE);
            }



		}
		if( b->posY < 0 )
		{
			Mix_PlayChannel(-1, sound3, 0);
		}

        b->stepY = -b->stepY;
        b->posY += b->stepY;
    }
}

/*Move of the player*/
void movePLAYER(NPC *p)
{

	if( (p->posX + PLAYER_WIDTH + p->stepX > SCREEN_WIDTH) )
	{
		p->posX = SCREEN_WIDTH - PLAYER_WIDTH - p->stepX;
	}
	else if( (p->posX - p->stepX < 0) )
	{
		p->posX = 0 + p->stepX;
	}

	if( esquerda )
	{
		p->posX += -p->stepX;
	}
	if( direita )
	{
		p->posX += p->stepX;
	}
}


/*Create NPC*/
NPC createNPC( int posX, int posY, int stepX, int stepY,
               SDL_Surface *image, int armor) {
    NPC p;

    p.posX = posX;
    p.posY = posY;
    p.stepX = stepX;
    p.stepY = stepY;
    p.image = image;
    p.armor = armor;
    return p;
}

int init() {
    /*Initialization flag*/
    int success = true;

    srand(time(NULL));

    /*Initialize SDL*/
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else {
        /*Create window*/
        gWindow = SDL_CreateWindow( "SDL Tutorial: 06_moving_images", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL ) {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            /*Initialize JPG and PNG loading */
            int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
            if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
                printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                success = false;
            }
            if(TTF_Init() < 0){
                printf("Erro %s", TTF_GetError);
            }
            else {
                /*Get window surface*/
                gScreenSurface = SDL_GetWindowSurface( gWindow );
            }
        }
    }

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_AllocateChannels(16);

    sound = Mix_LoadWAV("sounds/xp.wav");
    if (!sound) {
        printf("Wav: SDL error=%s\n", SDL_GetError());
        success = false;
    }
    sound2 = Mix_LoadWAV("sounds/errou.wav");
    if (!sound2) {
        printf("Wav: SDL error=%s\n", SDL_GetError());
        success = false;
    }
    sound3 = Mix_LoadWAV("sounds/fon.wav");
    if (!sound3) {
        printf("Wav: SDL error=%s\n", SDL_GetError());
        success = false;
    }
    font = TTF_OpenFont("vintage.ttf", 20);
    if(!font){
        printf("Erro ao carregar a fonte ttf");
    }

    return success;
}

int loadMedia() {
    /*Loading success flag*/
    int success = true;
    /*uint32_t colorKey;*/

    /*Load PNG surface*/
    gJPGSurface = loadSurface( "./images/circle.jpeg" );

	gJPGSurface23 = loadSurface( "./images/block3.jpeg" );
	gJPGSurface22 = loadSurface( "./images/block2.jpeg" );
	gJPGSurface21 = loadSurface( "./images/block1.jpeg" );

    surface_vida = TTF_RenderText_Solid(font, life, WHITE);
    nivel = TTF_RenderText_Solid(font, snivel, WHITE);
    pontos = TTF_RenderText_Solid(font, spontucao, WHITE);

	gJPGSurface3 = loadSurface ( "./images/player.jpeg" );

	initial = loadSurface ( "./images/inicio.jpeg" );
	initial2 = loadSurface ( "./images/instrucoes.jpeg" );
	initial3 = loadSurface ( "./images/creditos.jpeg" );

	if( initial == NULL ) {
        printf( "Failed to load image! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    if( initial2 == NULL ) {
        printf( "Failed to load image! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    if( initial3 == NULL ) {
        printf( "Failed to load image! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }

    if( gJPGSurface == NULL ) {
        printf( "Failed to load image! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }

	if( gJPGSurface23 == NULL ) {
		printf( "Failed to load image! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	if( gJPGSurface22 == NULL ) {
		printf( "Failed to load image! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	if( gJPGSurface21 == NULL ) {
		printf( "Failed to load image! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}

	if( gJPGSurface3 == NULL ) {
		printf( "Failed to load image! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}

    if (surface_vida == NULL){
        printf("Erro ao carregar superficie TTF");
        success = false;
    }
    if (nivel == NULL){
        printf("Erro ao carregar superficie TTF");
        success = false;
    }
    if (pontos == NULL){
        printf("Erro ao carregar superficie TTF");
        success = false;
    }

    return success;
}

void closing() {
    /*Free loaded image*/
    SDL_FreeSurface( initial );
    initial = NULL;

    SDL_FreeSurface( gJPGSurface );
    gJPGSurface = NULL;

	SDL_FreeSurface( gJPGSurface23 );
	gJPGSurface23 = NULL;

	SDL_FreeSurface( gJPGSurface22 );
	gJPGSurface22 = NULL;

	SDL_FreeSurface( gJPGSurface21 );
	gJPGSurface21 = NULL;

	SDL_FreeSurface( gJPGTEMPSurface );
	gJPGTEMPSurface = NULL;

	SDL_FreeSurface( gJPGSurface3 );
    gJPGSurface3 = NULL;

    SDL_FreeSurface(surface_vida);
    surface_vida = NULL;

    SDL_FreeSurface(pontos);
    pontos = NULL;

	SDL_FreeSurface(nivel);
	nivel = NULL;


	Mix_FreeChunk(sound);
	Mix_FreeChunk(sound2);
	Mix_FreeChunk(sound3);

    TTF_Quit();
    /*Destroy window*/
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    /*Quit SDL subsystems*/
    IMG_Quit();
    SDL_Quit();
}
SDL_Surface* loadSurface( char *path ) {
    /*The final optimized image*/
    SDL_Surface* optimizedSurface = NULL;

    /*Load image at specified path*/
    SDL_Surface* loadedSurface = IMG_Load( path );
    if( loadedSurface == NULL ) {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
    }
    else {

		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( gScreenSurface->format, 255, 0, 255 ) );

        /*Convert surface to screen format*/
        optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
        if( optimizedSurface == NULL ) {
            printf( "Unable to optimize image %s! SDL Error: %s\n", path, SDL_GetError() );
        }

        /*Get rid of old loaded surface*/
        SDL_FreeSurface( loadedSurface );
    }

    return optimizedSurface;
}
void reconstruir(){
    int i, j, l, k;
    int armorTEMP = 0;
    for(i = 0, l = 0; i < 2; i++)
            {
				for(j = 0, k = 25; j < 8; j++)
				{
					/*decidindo a cor*/
					armorTEMP = (rand() % ((3 - 1) +1)) + 1;
					if(armorTEMP == 3)
						gJPGTEMPSurface = gJPGSurface23;
					else if(armorTEMP == 2)
						gJPGTEMPSurface = gJPGSurface22;
					else if(armorTEMP == 1)
						gJPGTEMPSurface = gJPGSurface21;

					blocks[i][j] = createNPC(k,
											 l,
											 0,
											 0,
											 gJPGTEMPSurface,
											 armorTEMP);

					k+= BLOCK_WIDTH+1;
				}
				l+= BLOCK_HEIGHT+1;
			}

}
