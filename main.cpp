#include <SFML/Graphics.hpp>
#include "algorithm"

using namespace sf;

const int width = 18;
const int height = 18;
const int total_colors = 8; // 0...7

const int offset_x = 2;
const int offset_y = 3;

// Кол-во рядов (вертикаль)
const int rows = 20;
// Кол-во строк (горизонталь)
const int columns = 10;

int field[columns][rows]; // игровое поле
int new_field[columns][rows]; // игровое поле после удаления строк
bool rows_were_deleted = false;
bool able_to_place_tetramino = true;
int points = 0;

// Текущий тип нашей фигуры (индекс для figures)
int ftype;

/*
	моно - 1
	ди либо до - 2
	три - 3
	тетра - 4
*/

// Массив фигурок-тетрамино
int figures[7][4] =
{
	1,3,5,7, // I
	2,4,5,7, // Z
	3,5,4,6, // S
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // O
};

// Дельта Х (Куда двигаться относительно горизонтали)
int dx = 0;

// Задержка перед с двигом фигуры вниз.
float delay = 0.5f;
// Создаем таймер
sf::Clock timer;

struct Point {
	int x;
	int y;
};

Point tetramino[4];
Point shadow_coords[4];

bool collision_check(Point t[4]) {
	for (int i = 0; i < 4; i++) {
		if (t[i].y >= (rows - 1)) {
			return true;
		}
	}
	for (int i = 0; i < 4; i++) {
		int x = t[i].x;
		int y = t[i].y;
		if (field[x][y + 1] != -1) {
			return true;
		}
	}
	return false;
}

bool out_of_bounds(Point t[4]) {
	bool out_of_bound = false;
	for (int i = 0; i <= 3; i++) {
		if (t[i].x >= columns || t[i].x < 0)
		{
			out_of_bound = true;
		}
		if (t[i].y >= rows)
		{
			out_of_bound = true;
		}
	}
	return out_of_bound;
};

bool is_trying_to_replace(Point t[4]) {
	for (int i = 0; i < 4; i++) {
		int x = t[i].x;
		int y = t[i].y;
		if (field[x][y] != -1) {
			return true;
		}
	}
	return false;
}

void delete_rows() {
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < columns; x++) {
			new_field[x][y] = field[x][y];
		}
	}

	for (int y = 0; y < rows; y++) {
		bool have_empty_slots = false;
		for (int x = 0; x < columns; x++) {
			if (new_field[x][y] == -1) {
				have_empty_slots = true;
			}
		}
		if (!have_empty_slots) {
			points += 10;
			rows_were_deleted = true;
			for (; y >= 0; y--) {
				for (int x = 0; x < columns; x++) {
					if (y == 0) {
						new_field[x][y] = -1;
					} else {
						new_field[x][y] = new_field[x][y - 1];
					}
				}
			}
		}
	}
}

void rotate() {
	Point XO = tetramino[1];
	Point new_tetramino[4];
	for (int i = 0; i <= 3; i++) {
		int new_x = XO.x - tetramino[i].y + XO.y;
		int new_y = XO.y + tetramino[i].x - XO.x;
		new_tetramino[i].x = new_x;
		new_tetramino[i].y = new_y;
	}
	if (!out_of_bounds(new_tetramino) 
		&& !is_trying_to_replace(new_tetramino)) {
		for (int i = 0; i <= 3; i++) {
			tetramino[i] = new_tetramino[i];
		}
	}
}

void move() {
	int dy = 0;
	if (timer.getElapsedTime().asSeconds() > delay) {
		dy = 1;
		timer.restart();
	}
	Point new_tetramino[4];
	for (int i = 0; i <= 3; i++) {
		new_tetramino[i].x = tetramino[i].x + dx;
		new_tetramino[i].y = tetramino[i].y + dy;
	}
	if (!out_of_bounds(new_tetramino) 
		&& !is_trying_to_replace(new_tetramino)) {
		for (int i = 0; i <= 3; i++) {
			tetramino[i].y = new_tetramino[i].y;
			tetramino[i].x = new_tetramino[i].x;
		}
	}
	dx = 0;
}

void generate_new_tetramino()
{
	ftype = (rand() % 7);

	for (int i = 0; i < 4; i++) {
		tetramino[i].x = figures[ftype][i] % 2;
		tetramino[i].y = (figures[ftype][i] / 2) - 1;
	}
}

void draw_field(sf::Sprite sprite, sf::RenderWindow& window) {
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < columns; x++) {
			if (field[x][y] == -1)
			{
				continue;
			}
			sprite.setTextureRect(IntRect(field[x][y] * 18, 0, 18, 18));
			sprite.setPosition(x * width, y * height);
			// Смещение относительно подложки.
			sprite.move(70, 59);
			window.draw(sprite);
		}
	}
}

void restart() {
	for (int i = 0; i < columns; i++)
	{
		std::fill(std::begin(field[i]), std::end(field[i]), -1);
	}
	generate_new_tetramino();
	dx = 0;
	points = 0;
	delay = 0.5f;
	able_to_place_tetramino = true;
}

int main()
{
	srand(time(0));
	restart();
	RenderWindow window(VideoMode(320, 480), "Tetris", Style::Close);

	// Создание и загрузка текстуры
	Texture texture;
	Texture texture_shadow;
	Texture background_texture;
	texture.loadFromFile("tiles.png");
	texture_shadow.loadFromFile("tiles_shadow.png");
	background_texture.loadFromFile("background.png");

	// Создание спрайта
	Sprite sprite(texture);
	Sprite shadow(texture_shadow);
	Sprite background(background_texture);

	Font arial_font;
	if (!arial_font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
		throw new std::exception("Unable to load arial.ttf font.");
	}

	Text text("Score: " + std::to_string(points), arial_font);
	text.setCharacterSize(30);
	text.setFillColor(Color::Black);
	text.setPosition(70 + width + width, 20);

	Text text2("Press enter to restart", arial_font);
	text2.setCharacterSize(30);
	text2.setFillColor(Color::Black);
	text2.setPosition(18, 437);

	generate_new_tetramino();

	// Главный цикл приложения. Выполняется, пока открыто окно.
	while (window.isOpen())
	{
		// Обрабатываем очередь событий в цикле
		Event event;
		while (window.pollEvent(event))
		{
			// Пользователь нажал на «крестик» и хочет окно закрыть?
			if (event.type == Event::Closed)
				// тогда закрываем его
				window.close();

			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Down) {
					delay = 0.1f;
				}
			}
			
			if (event.type == Event::KeyReleased) {
				if (event.key.code == Keyboard::Up) {
					rotate();
				}
				if (event.key.code == Keyboard::Down) {
					delay = 0.5f;
				}
				if (event.key.code == Keyboard::Enter && !able_to_place_tetramino) {
					restart();
					text.setPosition(70 + width + width, 20);
					window.setTitle("Tetris");
				}
			}

			if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				dx = -1;
			}
			else if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				dx = 1;
			}
		}

		// Задаем цвет фона - белый
		window.clear(Color::White);
		window.draw(background);

		/*
		// Заполнение поля целиком.
		for (int y = 0; y < rows; y++) {
			sprite.setTextureRect(IntRect((y % total_colors) * width, 0, 18, 18));
			for (int x = 0; x < columns; x++) {
				sprite.setPosition(width * x, height * y);
				window.draw(sprite);
			}
		}
		*/
		

		/*
		// Вывод всех типов фигур
		for (int type = 0; type < 7; type++) {
			for (int i = 0; i < 4; i++) {
				tetramino[i].x = figures[type][i] % 2;
				tetramino[i].y = figures[type][i] / 2;
			}

			sprite.setTextureRect(IntRect((type % total_colors) * width, 0, 18, 18));
			int offset = type * 3 * width;
			for (int i = 0; i < 4; i++) {
				sprite.setPosition((tetramino[i].x * width) + offset, tetramino[i].y * height);
				window.draw(sprite);
			}
		}
		*/

		if (able_to_place_tetramino) {
			move();
			if (collision_check(tetramino)) {
				for (int i = 0; i < 4; i++) {
					int x = tetramino[i].x;
					int y = tetramino[i].y;
					field[x][y] = ftype;
				}
				generate_new_tetramino();
				// Удаляем уже полностью заполненные строчки.
				delete_rows();
				for (int i = 0; i < 4; i++) {
					int x = tetramino[i].x;
					int y = tetramino[i].y + 1;
					if (rows_were_deleted) {
						if (new_field[x][y] != -1) {
							able_to_place_tetramino = false;
						}
					}
					else {
						if (field[x][y] != -1) {
							able_to_place_tetramino = false;
						}
					}
				}
			}
			text.setString("Score: " + std::to_string(points));
		} else {
			sf::String game_over = "Game over! Points: " + std::to_string(points);
			text.setString(game_over);
			text.setPosition(4, 20);
			window.setTitle(game_over);
			window.draw(text2);
		}
		
		// Отрисовка поля
		draw_field(sprite, window);

		if (rows_were_deleted) {
			window.draw(text);
			window.display();
			rows_were_deleted = false;
			for (int y = 0; y < rows; y++) {
				for (int x = 0; x < columns; x++) {
					field[x][y] = new_field[x][y];
				}
			}

			timer.restart();
			while (timer.getElapsedTime().asSeconds() < 0.5f) {
				continue;
			}
			timer.restart();

			window.clear(Color::White);
			window.draw(background);

			draw_field(sprite, window);
		}



		// Отрисовка фигуры которой мы управляем
		sprite.setTextureRect(IntRect(ftype * 18, 0, 18, 18));
		shadow.setTextureRect(IntRect(ftype * 18, 0, 18, 18));
		for (int i = 0; i < 4; i++) {
			shadow_coords[i] = tetramino[i];
		}
		while (!collision_check(shadow_coords)) {
			for (int i = 0; i < 4; i++) {
				shadow_coords[i].y++;
			}
		}
		for (int i = 0; i < 4; i++) {
			int sx = shadow_coords[i].x;
			int sy = shadow_coords[i].y;
			shadow.setPosition(sx * width, sy * height);
			// Смещение относительно подложки.
			shadow.move(70, 59);
			window.draw(shadow);
		}
		for (int i = 0; i < 4; i++) {
			int x = tetramino[i].x;
			int y = tetramino[i].y;
			sprite.setPosition(x * width, y * height);
			// Смещение относительно подложки.
			sprite.move(70, 59);
			window.draw(sprite);
		}

		window.draw(text);

		window.display();
	}

	return 0;
}
