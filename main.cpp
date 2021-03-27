#include <SFML/Graphics.hpp>

using namespace sf;

/*
	моно - 1
	ди либо до - 2
	три - 3
	тетра - 4
*/

// Массив фигурок-тетрамино
int figures[7][4]=
{
	1,3,5,7, // I
	2,4,5,7, // Z
	3,5,4,6, // S
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // O
};


int main()
{
	RenderWindow window(VideoMode(320, 480), "Tetris");

	// Создание и загрузка текстуры
	Texture texture;
	texture.loadFromFile("tiles.png");

	// Создание спрайта
	Sprite sprite(texture);
	
	// Вырезаем из спрайта отдельный квадратик размером 18х18 пикселей
	sprite.setTextureRect(IntRect(0, 0, 18, 18));

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
		}

		// TODO: Отрисовка тетрамино
	
		// Задаем цвет фона - белый
		window.clear(Color::White);
		
		window.display();
	}

	return 0;
}
