#pragma region 1

#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<iostream>
#include<Windows.h>

#define WIDTH 1280
#define HEIGHT 720

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

void deInit(int error)
{
	if (renderer != NULL) SDL_DestroyRenderer(renderer);

	if (window != NULL) SDL_DestroyWindow(window);

	TTF_Quit();

	IMG_Quit();

	SDL_Quit();

	exit(error);
}

void init()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("Error: %s", SDL_GetError());
		system("pause");
		deInit(1);
	}

	int img;

	if ((img = IMG_Init(IMG_INIT_PNG)) == 0)
	{
		printf("Error: %s", SDL_GetError());
		system("pause");
		deInit(1);
	}

	if (img and IMG_INIT_PNG) printf("PNG +");
	else printf("PNG -");

	TTF_Init();

	window = SDL_CreateWindow("Sea Battle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

	if (window == NULL)
	{
		printf("Error: %s", SDL_GetError());
		system("pause");
		deInit(1);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == NULL)
	{
		printf("Error: %s", SDL_GetError());
		system("pause");
		deInit(1);
	}
}

SDL_Texture* loadTextureFromFile(const char* filename, SDL_Rect* rect)
{
	SDL_Surface* surface = IMG_Load(filename);

	if (surface == NULL)
	{
		printf("Error: %s - %s", SDL_GetError(), filename);
		system("pause");
		deInit(1);
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	*rect = { 0, 0, surface->w, surface->h };

	SDL_FreeSurface(surface);

	return texture;
}

void hideConsole(bool flag)
{
	HWND hide_console;

	AllocConsole();

	hide_console = FindWindowA("ConsoleWindowClass", NULL);

	ShowWindow(hide_console, !flag);
}

#pragma endregion

struct Loop
{
	bool info = false;
	bool launched = true;
	bool menu = true;
	bool play = false;
	bool preparation = true;
	bool war = false;
};

struct Mouse
{
	int x = 0;
	int y = 0;
};

struct Png
{
	// background_for_menu
	SDL_Rect bfm_rect;
	SDL_Texture* bfm_texture = loadTextureFromFile("png/bfm.png", &bfm_rect);

	// play_button_for_menu
	SDL_Rect pbfm_rect;
	SDL_Texture* pbfm_texture = loadTextureFromFile("png/pbfm.png", &pbfm_rect);
	SDL_Rect pbfm_rect_dst = { int(WIDTH / 1.25), int(HEIGHT / 10), pbfm_rect.w / 8, pbfm_rect.h / 8 };

	// info_button_for_menu
	SDL_Rect ibfm_rect;
	SDL_Texture* ibfm_texture = loadTextureFromFile("png/ibfm.png", &ibfm_rect);
	SDL_Rect ibfm_rect_dst = { int(WIDTH / 1.25), int(HEIGHT / 3), ibfm_rect.w / 8, ibfm_rect.h / 8 };

	// exit_button_for_menu
	SDL_Rect ebfm_rect;
	SDL_Texture* ebfm_texture = loadTextureFromFile("png/ebfm.png", &ebfm_rect);
	SDL_Rect ebfm_rect_dst = { int(WIDTH / 1.25), int(HEIGHT / 1.75), ebfm_rect.w / 8, ebfm_rect.h / 8 };

	// accept
	SDL_Rect a_rect;
	SDL_Texture* a_texture = loadTextureFromFile("png/a.png", &a_rect);
	SDL_Rect a_rect_dst = { WIDTH / 4 * 3 + 20, HEIGHT / 4 * 3, a_rect.w / 8, a_rect.h / 8 };
};

#define MAX_SHIP 10
#define MAX_SHIP_4 1
#define MAX_SHIP_3 2
#define MAX_SHIP_2 3
#define MAX_SHIP_1 4

struct First_Player
{
	// 0 - empty
	// 1 - ship
	int field[12][12] = { 0 };
	int shoot[12][12] = { 0 };
	int ship_counter = 0;

	bool _4 = true;
	bool _3 = false;
	bool _2 = false;
	bool _1 = false;

	SDL_Rect f_rect = { 50, 50, 50, 50 };

	bool queue = true;
	bool accept = false;
	//bool ready = false;

	int x = 0;
	int y = 0;
};

struct Second_Player
{
	// 0 - empty
	// 1 - ship
	int field[12][12] = { 0 };
	int shoot[12][12] = { 0 };
	int ship_counter = 0;

	bool _4 = true;
	bool _3 = false;
	bool _2 = false;
	bool _1 = false;

	SDL_Rect f_rect = { 730, 50, 50, 50 };

	bool queue = false;
	bool accept = false;
	//bool ready = false;

	int x = 0;
	int y = 0;
};

struct Game
{
	Loop loop;
	Mouse mouse;
	Png png;

	First_Player first_player;
	Second_Player second_player;

	SDL_Event event;
	TTF_Font* font = TTF_OpenFont("fonts/font.ttf", 25);
	const int fps = 60;

	bool rotate = true;
};

void destroyTexture(Game& game)
{
	SDL_DestroyTexture(game.png.bfm_texture);
	SDL_DestroyTexture(game.png.pbfm_texture);
	SDL_DestroyTexture(game.png.ibfm_texture);
	SDL_DestroyTexture(game.png.ebfm_texture);
	SDL_DestroyTexture(game.png.a_texture);
}

int getTextSize(const char* text)
{
	int counter = 0;

	while (text[counter] != '\0') counter++;

	return counter;
}

#define MAX_TEXT 10

void printValue(Game game, int value, int x, int y, int size)
{
	char text[MAX_TEXT];

	_itoa_s(value, text, MAX_TEXT, 10);

	SDL_Surface* text_surface = TTF_RenderText_Blended(game.font, text, { 0, 0, 0, 255 });

	SDL_Rect rect = { x, y, 0.75 * getTextSize(text) * size , size };

	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

	SDL_RenderCopy(renderer, text_texture, NULL, &rect);

	SDL_FreeSurface(text_surface);

	SDL_DestroyTexture(text_texture);
}

void printText(Game game, const char* text, int x, int y, int size)
{
	SDL_Surface* text_surface = TTF_RenderText_Blended(game.font, text, { 0, 0, 0, 255 });

	SDL_Rect rect = { x, y, 0.75 * getTextSize(text) * size, size };

	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

	SDL_RenderCopy(renderer, text_texture, NULL, &rect);

	SDL_FreeSurface(text_surface);

	SDL_DestroyTexture(text_texture);
}

void events(Game& game)
{
	while (SDL_PollEvent(&game.event))
	{
		switch (game.event.type)
		{
		case SDL_QUIT:
			game.loop.launched = false;
			break;
		case SDL_KEYDOWN:
			switch (game.event.key.keysym.scancode)
			{
			case SDL_SCANCODE_R:
				if (game.rotate)
				{
					if (game.first_player.x < 8 and game.first_player._4) game.rotate = false; // 4
					if (game.first_player.x < 9 and game.first_player._3) game.rotate = false; // 3
					if (game.first_player.x < 10 and game.first_player._2) game.rotate = false; // 2

					if (game.second_player.x < 8 and game.second_player._4) game.rotate = false; // 4
					if (game.second_player.x < 9 and game.second_player._3) game.rotate = false; // 3
					if (game.second_player.x < 10 and game.second_player._2) game.rotate = false; // 2
				}
				else
				{
					if (game.first_player.y < 8 and game.first_player._4) game.rotate = true; // 4
					if (game.first_player.y < 9 and game.first_player._3) game.rotate = true; // 3
					if (game.first_player.y < 10 and game.first_player._2) game.rotate = true; // 2

					if (game.second_player.y < 8 and game.second_player._4) game.rotate = true; // 4
					if (game.second_player.y < 9 and game.second_player._3) game.rotate = true; // 3
					if (game.second_player.y < 10 and game.second_player._2) game.rotate = true; // 2
				}

				game.first_player.x = 0;
				game.first_player.y = 0;

				game.second_player.x = 0;
				game.second_player.y = 0;
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			game.mouse.x = game.event.motion.x;
			game.mouse.y = game.event.motion.y;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if ((game.png.pbfm_rect_dst.x + game.png.pbfm_rect_dst.w) >= game.mouse.x and game.mouse.x >= game.png.pbfm_rect_dst.x and
				(game.png.pbfm_rect_dst.y + game.png.pbfm_rect_dst.h) >= game.mouse.y and game.mouse.y >= game.png.pbfm_rect_dst.y and
				game.loop.menu)
			{
				game.loop.menu = false;
				game.loop.play = true;
			}

			if ((game.png.ibfm_rect_dst.x + game.png.ibfm_rect_dst.w) >= game.mouse.x and game.mouse.x >= game.png.ibfm_rect_dst.x and
				(game.png.ibfm_rect_dst.y + game.png.ibfm_rect_dst.h) >= game.mouse.y and game.mouse.y >= game.png.ibfm_rect_dst.y and
				game.loop.menu)
			{
				game.loop.menu = false;
				game.loop.info = true;
			}

			if ((game.png.ebfm_rect_dst.x + game.png.ebfm_rect_dst.w) >= game.mouse.x and game.mouse.x >= game.png.ebfm_rect_dst.x and
				(game.png.ebfm_rect_dst.y + game.png.ebfm_rect_dst.h) >= game.mouse.y and game.mouse.y >= game.png.ebfm_rect_dst.y and
				game.loop.menu)
			{
				game.loop.menu = false;
				game.loop.launched = false;
			}

			if ((game.png.a_rect_dst.x + game.png.a_rect_dst.w) >= game.mouse.x and game.mouse.x >= game.png.a_rect_dst.x and
				(game.png.a_rect_dst.y + game.png.a_rect_dst.h) >= game.mouse.y and game.mouse.y >= game.png.a_rect_dst.y and
				game.loop.play)
			{
				if (game.first_player.queue) game.first_player.accept = true;
				if (game.second_player.queue) game.second_player.accept = true;
			}

			if ((game.loop.preparation and game.first_player.queue) or (game.loop.war and game.second_player.queue))
			{
				SDL_Rect first_temporary = { game.first_player.f_rect.x, game.first_player.f_rect.y, game.first_player.f_rect.w, game.first_player.f_rect.h };

				for (int i = 1; i < 11; i++)
				{
					for (int j = 1; j < 11; j++)
					{
						if ((first_temporary.x + first_temporary.w) >= game.mouse.x and game.mouse.x >= first_temporary.x and
							(first_temporary.y + first_temporary.h) >= game.mouse.y and game.mouse.y >= first_temporary.y)
						{
							if (game.loop.preparation)
							{
								// 4
								if (game.first_player._4 and game.rotate and j < 8)
								{
									game.first_player.x = i;
									game.first_player.y = j;
								}
								else if (game.first_player._4 and !game.rotate and i < 8)
								{
									game.first_player.x = i;
									game.first_player.y = j;
								}
								// 4
								
								// 3
								if (game.first_player._3)
								{
									if (game.rotate)
									{
										bool flag = true;
										for (int k = -1; k <= 1; k++)
											for (int l = -1; l <= 3; l++)
											{
												if (game.first_player.field[i + k][j + l] == 1) flag = false;
											}

										if (j < 9 and flag)
										{
											game.first_player.x = i;
											game.first_player.y = j;
										}
									}
									else if (!game.rotate)
									{
										bool flag = true;
										for (int k = -1; k <= 3; k++)
											for (int l = -1; l <= 1; l++)
											{
												if (game.first_player.field[i + k][j + l] == 1) flag = false;
											}

										if (i < 9 and flag)
										{
											game.first_player.x = i;
											game.first_player.y = j;
										}
									}
								}
								// 3

								// 2
								if (game.first_player._2)
								{
									if (game.rotate)
									{
										bool flag = true;
										for (int k = -1; k <= 1; k++)
											for (int l = -1; l <= 2; l++)
											{
												if (game.first_player.field[i + k][j + l] == 1) flag = false;
											}

										if (j < 10 and flag)
										{
											game.first_player.x = i;
											game.first_player.y = j;
										}
									}
									else if (!game.rotate)
									{
										bool flag = true;
										for (int k = -1; k <= 2; k++)
											for (int l = -1; l <= 1; l++)
											{
												if (game.first_player.field[i + k][j + l] == 1) flag = false;
											}

										if (i < 10 and flag)
										{
											game.first_player.x = i;
											game.first_player.y = j;
										}
									}
								}
								// 2

								// 1
								if (game.first_player._1)
								{
									bool flag = true;
									for (int k = -1; k <= 1; k++)
										for (int l = -1; l <= 1; l++)
										{
											if (game.first_player.field[i + k][j + l] == 1) flag = false;
										}

									if (j < 11 and flag)
									{
										game.first_player.x = i;
										game.first_player.y = j;
									}
								}
								// 1
							}
							if (game.loop.war)
							{
								game.second_player.x = i;
								game.second_player.y = j;
							}
						}
						first_temporary.x += game.first_player.f_rect.w;
					}
					first_temporary.x = game.first_player.f_rect.x;
					first_temporary.y += game.first_player.f_rect.h;
				}
			}

			if ((game.loop.preparation and game.second_player.queue) or (game.loop.war and game.first_player.queue))
			{
				SDL_Rect second_temporary = { game.second_player.f_rect.x, game.second_player.f_rect.y, game.second_player.f_rect.w, game.second_player.f_rect.h };

				for (int i = 1; i < 11; i++)
				{
					for (int j = 1; j < 11; j++)
					{
						if ((second_temporary.x + second_temporary.w) >= game.mouse.x and game.mouse.x >= second_temporary.x and
							(second_temporary.y + second_temporary.h) >= game.mouse.y and game.mouse.y >= second_temporary.y)
						{
							if (game.loop.preparation)
							{
								// 4
								if (game.second_player._4 and game.rotate and j < 8)
								{
									game.second_player.x = i;
									game.second_player.y = j;
								}
								else if (game.second_player._4 and !game.rotate and i < 8)
								{
									game.second_player.x = i;
									game.second_player.y = j;
								}
								// 4

								// 3
								if (game.second_player._3)
								{
									if (game.rotate)
									{
										bool flag = true;
										for (int k = -1; k <= 1; k++)
											for (int l = -1; l <= 3; l++)
											{
												if (game.second_player.field[i + k][j + l] == 1) flag = false;
											}

										if (j < 9 and flag)
										{
											game.second_player.x = i;
											game.second_player.y = j;
										}
									}
									else if (!game.rotate)
									{
										bool flag = true;
										for (int k = -1; k <= 3; k++)
											for (int l = -1; l <= 1; l++)
											{
												if (game.second_player.field[i + k][j + l] == 1) flag = false;
											}

										if (i < 9 and flag)
										{
											game.second_player.x = i;
											game.second_player.y = j;
										}
									}
								}
								// 3

								// 2
								if (game.second_player._2)
								{
									if (game.rotate)
									{
										bool flag = true;
										for (int k = -1; k <= 1; k++)
											for (int l = -1; l <= 2; l++)
											{
												if (game.second_player.field[i + k][j + l] == 1) flag = false;
											}

										if (j < 10 and flag)
										{
											game.second_player.x = i;
											game.second_player.y = j;
										}
									}
									else if (!game.rotate)
									{
										bool flag = true;
										for (int k = -1; k <= 2; k++)
											for (int l = -1; l <= 1; l++)
											{
												if (game.second_player.field[i + k][j + l] == 1) flag = false;
											}

										if (i < 10 and flag)
										{
											game.second_player.x = i;
											game.second_player.y = j;
										}
									}
								}
								// 2

								// 1
								if (game.second_player._1)
								{
									bool flag = true;
									for (int k = -1; k <= 1; k++)
										for (int l = -1; l <= 1; l++)
										{
											if (game.second_player.field[i + k][j + l] == 1) flag = false;
										}

									if (j < 11 and flag)
									{
										game.second_player.x = i;
										game.second_player.y = j;
									}
								}
								// 1
							}
							if (game.loop.war)
							{
								game.first_player.x = i;
								game.first_player.y = j;
							}
						}
						second_temporary.x += game.second_player.f_rect.w;
					}
					second_temporary.x = game.second_player.f_rect.x;
					second_temporary.y += game.second_player.f_rect.h;
				}
			}
			break;
		}
	}
}

void info(Game game)
{

}

void menu(Game game)
{
	SDL_RenderCopy(renderer, game.png.bfm_texture, &game.png.bfm_rect, NULL);
	SDL_RenderCopy(renderer, game.png.pbfm_texture, &game.png.pbfm_rect, &game.png.pbfm_rect_dst);
	SDL_RenderCopy(renderer, game.png.ibfm_texture, &game.png.ibfm_rect, &game.png.ibfm_rect_dst);
	SDL_RenderCopy(renderer, game.png.ebfm_texture, &game.png.ebfm_rect, &game.png.ebfm_rect_dst);
}

// 1 - first_player
// 2 - second_player
void field(Game game, int grid, int ship, int shoot)
{
	if (grid == 1)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black

		SDL_Rect first_temporary = { game.first_player.f_rect.x, game.first_player.f_rect.y, game.first_player.f_rect.w, game.first_player.f_rect.h };

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				SDL_RenderDrawRect(renderer, &first_temporary);
				first_temporary.x += game.first_player.f_rect.w;
			}
			first_temporary.x = game.first_player.f_rect.x;
			first_temporary.y += game.first_player.f_rect.h;
		}

		printText(game, "1  2  3  4  5  6  7  8  9  10", game.first_player.f_rect.x + 15, game.first_player.f_rect.y - 20, 22);

		//printText(game, "1", 15, 70, 22);
		//printText(game, "2", 15, 120, 22);
		//printText(game, "3", 15, 170, 22);
		//printText(game, "4", 15, 220, 22);
		//printText(game, "5", 15, 270, 22);
		//printText(game, "6", 15, 320, 22);
		//printText(game, "7", 15, 370, 22);
		//printText(game, "8", 15, 420, 22);
		//printText(game, "9", 15, 470, 22);
		//printText(game, "10", 15, 520, 22);

		for (int i = 0, y = game.first_player.f_rect.y + 20; i < 10; i++, y += game.first_player.f_rect.h)
			printValue(game, i + 1, game.first_player.f_rect.x - 35, y, 22);
	}

	if (ship == 1)
	{
		SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255); // gray

		SDL_Rect first_temporary = { game.first_player.f_rect.x, game.first_player.f_rect.y, game.first_player.f_rect.w, game.first_player.f_rect.h };

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (game.first_player.field[i + 1][j + 1] == 1) SDL_RenderFillRect(renderer, &first_temporary);
				first_temporary.x += game.first_player.f_rect.w;
			}
			first_temporary.x = game.first_player.f_rect.x;
			first_temporary.y += game.first_player.f_rect.h;
		}
	}

	if (shoot == 1)
	{
		SDL_Rect first_temporary = { game.first_player.f_rect.x, game.first_player.f_rect.y, game.first_player.f_rect.w, game.first_player.f_rect.h };

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (game.first_player.shoot[i + 1][j + 1] == 2)
				{
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red
					SDL_RenderFillRect(renderer, &first_temporary);
				}
				else if (game.first_player.shoot[i + 1][j + 1] == 1)
				{
					SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255); // gray
					SDL_RenderFillRect(renderer, &first_temporary);
				}
				first_temporary.x += game.first_player.f_rect.w;
			}
			first_temporary.x = game.first_player.f_rect.x;
			first_temporary.y += game.first_player.f_rect.h;
		}
	}

	if (game.loop.preparation and game.first_player.queue)
	{
		SDL_Rect first_temporary = { game.first_player.f_rect.x, game.first_player.f_rect.y, game.first_player.f_rect.w, game.first_player.f_rect.h };

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (game.first_player.x - 1 == i and game.first_player.y - 1 == j)
				{
					SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255); // gray
					// 4
					if (game.first_player._4)
					{
						if (game.rotate)
						{
							for (int k = 1; k < 5; k++)
							{
								SDL_RenderFillRect(renderer, &first_temporary);
								first_temporary.x += game.first_player.f_rect.w;
							}
						}
						else
						{
							for (int k = 1; k < 5; k++)
							{
								SDL_RenderFillRect(renderer, &first_temporary);
								first_temporary.y += game.first_player.f_rect.h;
							}
						}
					}
					// 4

					// 3
					if (game.first_player._3)
					{
						if (game.rotate)
						{
							for (int k = 1; k < 4; k++)
							{
								SDL_RenderFillRect(renderer, &first_temporary);
								first_temporary.x += game.first_player.f_rect.w;
							}
						}
						else
						{
							for (int k = 1; k < 4; k++)
							{
								SDL_RenderFillRect(renderer, &first_temporary);
								first_temporary.y += game.first_player.f_rect.h;
							}
						}
					}
					// 3

					// 2
					if (game.first_player._2)
					{
						if (game.rotate)
						{
							for (int k = 1; k < 3; k++)
							{
								SDL_RenderFillRect(renderer, &first_temporary);
								first_temporary.x += game.first_player.f_rect.w;
							}
						}
						else
						{
							for (int k = 1; k < 3; k++)
							{
								SDL_RenderFillRect(renderer, &first_temporary);
								first_temporary.y += game.first_player.f_rect.h;
							}
						}
					}
					// 2

					// 1
					if (game.first_player._1) SDL_RenderFillRect(renderer, &first_temporary);
					// 1
				}
				first_temporary.x += game.first_player.f_rect.w;
			}
			first_temporary.x = game.first_player.f_rect.x;
			first_temporary.y += game.first_player.f_rect.h;
		}
	}

	if (grid == 2)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black

		SDL_Rect second_temporary = { game.second_player.f_rect.x, game.second_player.f_rect.y, game.second_player.f_rect.w, game.second_player.f_rect.h };

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				SDL_RenderDrawRect(renderer, &second_temporary);
				second_temporary.x += game.second_player.f_rect.w;
			}
			second_temporary.x = game.second_player.f_rect.x;
			second_temporary.y += game.second_player.f_rect.h;
		}

		printText(game, "1  2  3  4  5  6  7  8  9  10", game.second_player.f_rect.x + 15, game.second_player.f_rect.y - 20, 22);

		//printText(game, "1", 695, 70, 22);
		//printText(game, "2", 695, 120, 22);
		//printText(game, "3", 695, 170, 22);
		//printText(game, "4", 695, 220, 22);
		//printText(game, "5", 695, 270, 22);
		//printText(game, "6", 695, 320, 22);
		//printText(game, "7", 695, 370, 22);
		//printText(game, "8", 695, 420, 22);
		//printText(game, "9", 695, 470, 22);
		//printText(game, "10", 695, 520, 22);

		for (int i = 0, y = game.second_player.f_rect.y + 20; i < 10; i++, y += game.second_player.f_rect.h)
			printValue(game, i + 1, game.second_player.f_rect.x - 35, y, 22);
	}

	if (ship == 2)
	{
		SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255); // gray

		SDL_Rect second_temporary = { game.second_player.f_rect.x, game.second_player.f_rect.y, game.second_player.f_rect.w, game.second_player.f_rect.h };

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (game.second_player.field[i + 1][j + 1] == 1) SDL_RenderFillRect(renderer, &second_temporary);
				second_temporary.x += game.second_player.f_rect.w;
			}
			second_temporary.x = game.second_player.f_rect.x;
			second_temporary.y += game.second_player.f_rect.h;
		}
	}

	if (shoot == 2)
	{
		SDL_Rect second_temporary = { game.second_player.f_rect.x, game.second_player.f_rect.y, game.second_player.f_rect.w, game.second_player.f_rect.h };

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (game.second_player.shoot[i + 1][j + 1] == 2)
				{
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red
					SDL_RenderFillRect(renderer, &second_temporary);
				}
				else if (game.second_player.shoot[i + 1][j + 1] == 1)
				{
					SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255); // gray
					SDL_RenderFillRect(renderer, &second_temporary);
				}
				second_temporary.x += game.second_player.f_rect.w;
			}
			second_temporary.x = game.second_player.f_rect.x;
			second_temporary.y += game.second_player.f_rect.h;
		}
	}

	if (game.loop.preparation and game.second_player.queue)
	{
		SDL_Rect second_temporary = { game.second_player.f_rect.x, game.second_player.f_rect.y, game.second_player.f_rect.w, game.second_player.f_rect.h };

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (game.second_player.x - 1 == i and game.second_player.y - 1 == j)
				{
					SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255); // gray
					// 4
					if (game.second_player._4)
					{
						if (game.rotate)
						{
							for (int k = 1; k < 5; k++)
							{
								SDL_RenderFillRect(renderer, &second_temporary);
								second_temporary.x += game.second_player.f_rect.w;
							}
						}
						else
						{
							for (int k = 1; k < 5; k++)
							{
								SDL_RenderFillRect(renderer, &second_temporary);
								second_temporary.y += game.second_player.f_rect.h;
							}
						}
					}
					// 4

					// 3
					if (game.second_player._3)
					{
						if (game.rotate)
						{
							for (int k = 1; k < 4; k++)
							{
								SDL_RenderFillRect(renderer, &second_temporary);
								second_temporary.x += game.second_player.f_rect.w;
							}
						}
						else
						{
							for (int k = 1; k < 4; k++)
							{
								SDL_RenderFillRect(renderer, &second_temporary);
								second_temporary.y += game.second_player.f_rect.h;
							}
						}
					}
					// 3

					// 2
					if (game.second_player._2)
					{
						if (game.rotate)
						{
							for (int k = 1; k < 3; k++)
							{
								SDL_RenderFillRect(renderer, &second_temporary);
								second_temporary.x += game.second_player.f_rect.w;
							}
						}
						else
						{
							for (int k = 1; k < 3; k++)
							{
								SDL_RenderFillRect(renderer, &second_temporary);
								second_temporary.y += game.second_player.f_rect.h;
							}
						}
					}
					// 2

					// 1
					if (game.second_player._1) SDL_RenderFillRect(renderer, &second_temporary);
					// 1
				}
				second_temporary.x += game.second_player.f_rect.w;
			}
			second_temporary.x = game.second_player.f_rect.x;
			second_temporary.y += game.second_player.f_rect.h;
		}
	}
}

void preparation(Game& game)
{
	if (game.first_player.queue)
	{
		field(game, 1, 1, 0);

		printText(game, "Player 1", WIDTH / 4 * 3 - game.first_player.f_rect.w, HEIGHT / 2, 50);
		printText(game, "x:", WIDTH / 4 * 3 - game.first_player.f_rect.w, HEIGHT / 4 * 3 - game.first_player.f_rect.h, 50);
		printText(game, "y:", WIDTH / 4 * 3 - game.first_player.f_rect.w - 100, HEIGHT / 4 * 3 + game.first_player.f_rect.h, 50);

		printValue(game, game.first_player.x, WIDTH / 4 * 3 - game.first_player.f_rect.w, HEIGHT / 4 * 3 + game.first_player.f_rect.h, 50);
		printValue(game, game.first_player.y, WIDTH / 4 * 3 + game.first_player.f_rect.w, HEIGHT / 4 * 3 - game.first_player.f_rect.h, 50);

		if (game.first_player.x != 0 and game.first_player.y != 0)
		{
			game.png.a_rect_dst.x = WIDTH / 4 * 3 + 20;
			game.png.a_rect_dst.y = HEIGHT / 4 * 3;
			SDL_RenderCopy(renderer, game.png.a_texture, &game.png.a_rect, &game.png.a_rect_dst);
			if (game.first_player.accept)
			{
				// 1
				if (game.first_player._1)
				{
				game.first_player.field[game.first_player.x][game.first_player.y] = 1;

					game.first_player.x = 0;
					game.first_player.y = 0;
					game.first_player.accept = false;
					game.first_player.ship_counter++;
					if (game.first_player.ship_counter == MAX_SHIP_4 + MAX_SHIP_3 + MAX_SHIP_2 + MAX_SHIP_1) game.first_player._1 = false;

				}
				// 1

				// 2
				if (game.first_player._2)
				{
					for (int i = 0; i < 2; i++)
					{
						if (game.rotate) game.first_player.field[game.first_player.x][game.first_player.y + i] = 1;
						else game.first_player.field[game.first_player.x + i][game.first_player.y] = 1;
					}

					game.first_player.x = 0;
					game.first_player.y = 0;
					game.first_player.accept = false;
					game.first_player.ship_counter++;
					if (game.first_player.ship_counter == MAX_SHIP_4 + MAX_SHIP_3 + MAX_SHIP_2)
					{
						game.first_player._2 = false;
						game.first_player._1 = true;
					}
				}
				// 2

				// 3
				if (game.first_player._3)
				{
					for (int i = 0; i < 3; i++)
					{
						if (game.rotate) game.first_player.field[game.first_player.x][game.first_player.y + i] = 1;
						else game.first_player.field[game.first_player.x + i][game.first_player.y] = 1;
					}

					game.first_player.x = 0;
					game.first_player.y = 0;
					game.first_player.accept = false;
					game.first_player.ship_counter++;
					if (game.first_player.ship_counter == MAX_SHIP_4 + MAX_SHIP_3)
					{
						game.first_player._3 = false;
						game.first_player._2 = true;
					}
				}
				// 3

				// 4
				if (game.first_player._4)
				{
					for (int i = 0; i < 4; i++)
					{
						if (game.rotate) game.first_player.field[game.first_player.x][game.first_player.y + i] = 1;
						else game.first_player.field[game.first_player.x + i][game.first_player.y] = 1;
					}

					game.first_player.x = 0;
					game.first_player.y = 0;
					game.first_player.accept = false;
					game.first_player._4 = false;
					game.first_player._3 = true;
					game.first_player.ship_counter++;
				}
				// 4
			}
		}
	}

	if (game.first_player.ship_counter == MAX_SHIP)
	{
		game.first_player.queue = false;
		game.second_player.queue = true;
	}

	if (game.second_player.queue)
	{
		field(game, 2, 2, 0);

		printText(game, "Player 2", WIDTH / 4 * 1 - game.first_player.f_rect.w, HEIGHT / 2, 50);
		printText(game, "x:", WIDTH / 4 * 1 - game.second_player.f_rect.w, HEIGHT / 4 * 3 - game.second_player.f_rect.h, 50);
		printText(game, "y:", WIDTH / 4 * 1 - game.second_player.f_rect.w - 100, HEIGHT / 4 * 3 + game.second_player.f_rect.h, 50);

		printValue(game, game.second_player.x, WIDTH / 4 * 1 - game.second_player.f_rect.w, HEIGHT / 4 * 3 + game.second_player.f_rect.h, 50);
		printValue(game, game.second_player.y, WIDTH / 4 * 1 + game.second_player.f_rect.w, HEIGHT / 4 * 3 - game.second_player.f_rect.h, 50);

		if (game.second_player.x != 0 and game.second_player.y != 0)
		{
			game.png.a_rect_dst.x = WIDTH / 4 * 1 + 20;
			game.png.a_rect_dst.y = HEIGHT / 4 * 3;
			SDL_RenderCopy(renderer, game.png.a_texture, &game.png.a_rect, &game.png.a_rect_dst);
			if (game.second_player.accept)
			{
				// 1
				if (game.second_player._1)
				{
					game.second_player.field[game.second_player.x][game.second_player.y] = 1;

					game.second_player.x = 0;
					game.second_player.y = 0;
					game.second_player.accept = false;
					game.second_player.ship_counter++;
					if (game.second_player.ship_counter == MAX_SHIP_4 + MAX_SHIP_3 + MAX_SHIP_2 + MAX_SHIP_1) game.second_player._1 = false;

				}
				// 1

				// 2
				if (game.second_player._2)
				{
					for (int i = 0; i < 2; i++)
					{
						if (game.rotate) game.second_player.field[game.second_player.x][game.second_player.y + i] = 1;
						else game.second_player.field[game.second_player.x + i][game.second_player.y] = 1;
					}

					game.second_player.x = 0;
					game.second_player.y = 0;
					game.second_player.accept = false;
					game.second_player.ship_counter++;
					if (game.second_player.ship_counter == MAX_SHIP_4 + MAX_SHIP_3 + MAX_SHIP_2)
					{
						game.second_player._2 = false;
						game.second_player._1 = true;
					}
				}
				// 2

				// 3
				if (game.second_player._3)
				{
					for (int i = 0; i < 3; i++)
					{
						if (game.rotate) game.second_player.field[game.second_player.x][game.second_player.y + i] = 1;
						else game.second_player.field[game.second_player.x + i][game.second_player.y] = 1;
					}

					game.second_player.x = 0;
					game.second_player.y = 0;
					game.second_player.accept = false;
					game.second_player.ship_counter++;
					if (game.second_player.ship_counter == MAX_SHIP_4 + MAX_SHIP_3)
					{
						game.second_player._3 = false;
						game.second_player._2 = true;
					}
				}
				// 3

				// 4
				if (game.second_player._4)
				{
					for (int i = 0; i < 4; i++)
					{
						if (game.rotate) game.second_player.field[game.second_player.x][game.second_player.y + i] = 1;
						else game.second_player.field[game.second_player.x + i][game.second_player.y] = 1;
					}

					game.second_player.x = 0;
					game.second_player.y = 0;
					game.second_player.accept = false;
					game.second_player._4 = false;
					game.second_player._3 = true;
					game.second_player.ship_counter++;
				}
				// 4
			}
		}
	}

	if (game.second_player.ship_counter == MAX_SHIP)
	{
		game.loop.preparation = false;
		game.loop.war = true;

		game.first_player.queue = true;
		game.second_player.queue = false;
	}
}

void play(Game& game)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white
	SDL_RenderClear(renderer);
	if (game.loop.preparation) preparation(game);
	if (game.loop.war)
	{
		if (game.first_player.queue)
		{
			printText(game, "Player 1", game.first_player.f_rect.x, HEIGHT - game.first_player.f_rect.h, 25);
			printText(game, "x:", game.first_player.f_rect.x * 6, HEIGHT - game.first_player.f_rect.h, 25);
			printText(game, "y:", game.first_player.f_rect.x * 6 + game.first_player.f_rect.w, HEIGHT - game.first_player.f_rect.h * 2, 25);

			printValue(game, game.first_player.x, game.first_player.f_rect.x * 6 + game.first_player.f_rect.w, HEIGHT - game.first_player.f_rect.h, 25);
			printValue(game, game.first_player.y, game.first_player.f_rect.x * 6 + game.first_player.f_rect.w * 2, HEIGHT - game.first_player.f_rect.h * 2, 25);

			if (game.first_player.x != 0 and game.first_player.y != 0)
			{
				game.png.a_rect_dst.x = game.first_player.f_rect.x + game.first_player.f_rect.w * 8;
				game.png.a_rect_dst.y = HEIGHT - game.first_player.f_rect.h * 3;
				SDL_RenderCopy(renderer, game.png.a_texture, &game.png.a_rect, &game.png.a_rect_dst);
			}

			if (game.first_player.accept)
			{
				if (game.second_player.field[game.first_player.x][game.first_player.y] == 1)
				{
					game.second_player.shoot[game.first_player.x][game.first_player.y] = 2;
				}
				else
				{
					game.second_player.shoot[game.first_player.x][game.first_player.y] = 1;
				}

				game.first_player.accept = false;
				game.second_player.queue = true;
				game.first_player.queue = false;

				game.first_player.x = 0;
				game.first_player.y = 0;
			}
		}

		if (game.second_player.queue)
		{
			printText(game, "Player 2", game.second_player.f_rect.x, HEIGHT - game.second_player.f_rect.h, 25);
			printText(game, "x:", game.second_player.f_rect.x + game.second_player.f_rect.w * 5, HEIGHT - game.second_player.f_rect.h, 25);
			printText(game, "y:", game.second_player.f_rect.x + game.second_player.f_rect.w * 6, HEIGHT - game.second_player.f_rect.h * 2, 25);

			printValue(game, game.second_player.x, game.second_player.f_rect.x + game.second_player.f_rect.w * 6, HEIGHT - game.second_player.f_rect.h, 25);
			printValue(game, game.second_player.y, game.second_player.f_rect.x + game.second_player.f_rect.w * 7, HEIGHT - game.second_player.f_rect.h * 2, 25);

			if (game.second_player.x != 0 and game.second_player.y != 0)
			{
				game.png.a_rect_dst.x = game.second_player.f_rect.x + game.second_player.f_rect.w * 8;
				game.png.a_rect_dst.y = HEIGHT - game.second_player.f_rect.h * 3;
				SDL_RenderCopy(renderer, game.png.a_texture, &game.png.a_rect, &game.png.a_rect_dst);
			}

			if (game.second_player.accept)
			{
				if (game.first_player.field[game.second_player.x][game.second_player.y] == 1)
				{
					game.first_player.shoot[game.second_player.x][game.second_player.y] = 2;
				}
				else
				{
					game.first_player.shoot[game.second_player.x][game.second_player.y] = 1;
				}

				game.second_player.accept = false;
				game.first_player.queue = true;
				game.second_player.queue = false;

				game.second_player.x = 0;
				game.second_player.y = 0;
			}
		}

		field(game, 1, 0, 1);
		field(game, 2, 0, 2);
	}
}

#undef main
int main()
{
	system("chcp 1251");
	system("cls");

	hideConsole(false);

	init();

	Game game;

	printf("\n");

	while (game.loop.launched)
	{
		events(game);

		if (game.loop.info) info(game);
		if (game.loop.menu) menu(game);
		if (game.loop.play) play(game);

		SDL_RenderPresent(renderer);
		SDL_Delay(1000 / game.fps);
	}

	destroyTexture(game);

	deInit(0);
}