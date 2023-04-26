#include "interface.h"

Interface::Interface(
	SDL_Window* window,
	SDL_Surface* surface,
	SDL_Surface* sprites
):
	window_{window},
	surface_{surface},
	sprites_{sprites}
{};

Interface::~Interface() {};

/**
 * @brief Affiche le message "Push space key"
 * 
 * @param windowWidth of the window
 * @param windowHeight of the window
 */
void Interface::displayPushSpace(int windowWidth, int windowHeight) {
	SDL_Rect positionLettre = Coordinate::alphabet_texture;

	// Affichage de "Press escape key"
	positionLettre.x = (windowWidth -
		(Coordinate::indexPressSpace.size() *
		ALPHABET_TEXTURE_WIDTH))/2;
	positionLettre.y = windowHeight/3;

	for (int i: Coordinate::indexPressSpace) {
		if (i != -1) {
			SDL_BlitScaled(this->getSprites(), &Coordinate::alphabet[i],
				this->getSurface(), &positionLettre);
		}
		positionLettre.x += ALPHABET_TEXTURE_WIDTH;
	}
}

/**
 * @brief Affiche l'écran titre
 * 		- Logo Pacman
 * 		- Les scores (actuel et maximum)
 * 		- Le message "Push space key"
 * 		- Les 10 meilleurs scores
 * 		- Le logo Namco
 */
void Interface::titleScreen() {
	int windowWidth, windowHeight;
	SDL_GetWindowSize(this->getWindow(), &windowWidth, &windowHeight);

	// Affichage des logos
	SDL_BlitScaled(this->getSprites(), &Coordinate::pacmanLogo,
		this->getSurface(), &Coordinate::posPacmanLogo);

	SDL_BlitScaled(this->getSprites(), &Coordinate::namcoLogo,
		this->getSurface(), &Coordinate::posNamcoLogo);

	// Affichage de "SCORE"
	SDL_Rect positionLettre = Coordinate::alphabet_texture;
	SDL_Rect positionDigit = Coordinate::number_texture;
	SDL_BlitScaled(this->getSprites(), &Coordinate::number[0],
		this->getSurface(), &positionDigit);

	for (int i: Coordinate::indexScore) {
		SDL_BlitScaled(this->getSprites(), &Coordinate::alphabet[i],
			this->getSurface(), &positionLettre);
		positionLettre.x += ALPHABET_TEXTURE_WIDTH;
	}

	// Affichage de "HIGH SCORE"
	positionLettre = Coordinate::alphabet_texture;
	positionLettre.x = windowWidth - (positionLettre.x * 2);

	// Affichage du score le plus élevé
	int highScore = Stats::readScores(1).front();
	std::vector<int> digits = Stats::uncomposeNumber(highScore);

	positionDigit = Coordinate::number_texture;
	positionDigit.x = windowWidth - (positionDigit.x * 2);

	for (int i: digits) {
		SDL_BlitScaled(this->getSprites(), &Coordinate::number[i],
			this->getSurface(), &positionDigit);
		positionDigit.x -= NUMBER_TEXTURE_WIDTH;
	}

	// On part de l'extrémité droite de l'écran, donc on inverse l'ordre
	std::vector<int> tamponIndexScore = Coordinate::indexScore;
	std::reverse(tamponIndexScore.begin(), tamponIndexScore.end());

	std::vector<int> tamponIndexHigh = Coordinate::indexHigh;
	std::reverse(tamponIndexHigh.begin(), tamponIndexHigh.end());

		// Score
	for (int i: tamponIndexScore) {
		SDL_BlitScaled(this->getSprites(), &Coordinate::alphabet[i],
			this->getSurface(), &positionLettre);
		positionLettre.x -= ALPHABET_TEXTURE_WIDTH;
	}

		// Espace
	positionLettre.x -= ALPHABET_TEXTURE_WIDTH;

		// High
	for (int i: tamponIndexHigh) {
		SDL_BlitScaled(this->getSprites(), &Coordinate::alphabet[i],
			this->getSurface(), &positionLettre);
		positionLettre.x -= ALPHABET_TEXTURE_WIDTH;
	}

	// Affichage de "Press escape key"
	displayPushSpace(windowWidth, windowHeight);

	// Affichage de "RANK"
	positionLettre.x = (windowWidth - (Coordinate::indexRank.size() *
		ALPHABET_TEXTURE_WIDTH))/2;
	positionLettre.y = windowHeight/2 - ALPHABET_TEXTURE_WIDTH;

	for (int i: Coordinate::indexRank) {
		SDL_BlitScaled(this->getSprites(), &Coordinate::alphabet[i],
			this->getSurface(), &positionLettre);
		positionLettre.x += ALPHABET_TEXTURE_WIDTH;
	}

	// Affichage des 10 meilleures scores
	positionDigit = Coordinate::number_texture;
	positionDigit.y = windowHeight/2;

	std::vector<unsigned int> scores = Stats::readScores(10);
	for (int i: scores) {
		digits = Stats::uncomposeNumber(i);

		float offsetDigit = std::floor(digits.size()/1.5);

		// Hack pour centrer les scores à 3 chiffres
		if (offsetDigit == 2 && digits.size() == 3)
			offsetDigit = 1;
	
		positionDigit.x = (windowWidth + offsetDigit *
			NUMBER_TEXTURE_WIDTH)/2;

		for (int j: digits) {
			SDL_BlitScaled(this->getSprites(), &Coordinate::number[j],
				this->getSurface(), &positionDigit);
			positionDigit.x -= NUMBER_TEXTURE_WIDTH;
		}

		positionDigit.y += NUMBER_TEXTURE_WIDTH;
	}

	// Mets à jour la fenêtre
	SDL_UpdateWindowSurface(this->getWindow());

	// Attend l'entrée de l'utilisateur
	bool quit = false;
	int count = 0;
	while (!quit) {
		SDL_Event event;
		while (!quit && SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					quit = true;
					break;

				default:
					break;
			}
		}

		// Gestion du clavier
		int nbk;
		const Uint8* keys = SDL_GetKeyboardState(&nbk);

		// Quitter
		if (keys[SDL_SCANCODE_ESCAPE])
			quit = true;

		// Commence la partie
		if (keys[SDL_SCANCODE_SPACE]) {
			// Créé un rectangle rempli, à la taille exacte du score à afficher
			SDL_Rect rect = {0, 0, windowWidth, windowHeight};
			SDL_Color color = {0, 0, 0, 255};
			SDL_Surface* surface = SDL_CreateRGBSurface(0, rect.w, rect.h,
				32, 0, 0, 0, 0);
			SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, color.r,
				color.g, color.b));
			SDL_BlitScaled(surface, NULL, this->getSurface(), &rect);
			SDL_FreeSurface(surface);
			break;
		}

		/**
		 * Attend 0,5s avant de passer d'un message à l'autre
		 * 1 tour = 16m et 1s = 62*16ms, d'où le modulo 62
		*/
		if ((count % 62) == 1) {
			positionLettre.x = (windowWidth -
				(Coordinate::indexPressSpace.size() *
				ALPHABET_TEXTURE_WIDTH))/2;
			positionLettre.y = windowHeight/3;

			// Créé un rectangle rempli, à la taille exacte de la phrase
			SDL_Rect rect = {
				positionLettre.x,
				positionLettre.y,
				static_cast<int>(ALPHABET_TEXTURE_WIDTH *
					Coordinate::indexPressSpace.size()),
				ALPHABET_TEXTURE_WIDTH
			};

			SDL_Color color = {0, 0, 0, 255};
			SDL_Surface* surface = SDL_CreateRGBSurface(0, rect.w, rect.h,
				32, 0, 0, 0, 0);
			SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, color.r,
				color.g, color.b));
			SDL_BlitScaled(surface, NULL, this->getSurface(), &rect);
			SDL_FreeSurface(surface);
		}
		else if ((count % 31) == 1)
			displayPushSpace(windowWidth, windowHeight);

		count++;
		SDL_UpdateWindowSurface(this->getWindow());
		SDL_Delay(16);
	}
}