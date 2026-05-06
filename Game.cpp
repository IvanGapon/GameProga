#include <SFML/Graphics.hpp>
#include <iostream>
#include "Class.h"

class Game {
private: 
    // Окно 
    sf::RenderWindow* window; 
    const unsigned int WINDOW_WIDTH = 1024;
    const unsigned int WINDOW_HEIGHT = 768;
    const std::string WINDOW_TITLE = "My Shooter Game";
    
    // Проверка работы игры
    bool working; 

    // Время для стабильного FPS
    sf::Clock deltaClock;
    float deltaTime;

    // Герой 
    Hero* hero;
    
    // Визуал героя — белый шарик
    sf::CircleShape heroView; 
    sf::RectangleShape hp_fon;  // фон полоски (красный)
    sf::RectangleShape hp_line;  // сама полоска (зелёная)
    
    // Игровое поле 
    sf::RectangleShape gameField;

    void initWindow() {
        window = new sf::RenderWindow(
            sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}),  
            WINDOW_TITLE,
            sf::Style::Titlebar | sf::Style::Close
        );
        window->setFramerateLimit(60);
    }

    void initGameField() {
        gameField.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));  
        gameField.setPosition(sf::Vector2f(0.0f, 0.0f));              
        gameField.setFillColor(sf::Color(20, 30, 20));
    }
    
    void initHeroView() {
        heroView.setRadius(15.0f);                       
        heroView.setFillColor(sf::Color::White);            
        heroView.setOrigin(sf::Vector2f(15.0f, 15.0f));    
    } 
    void initHPLine() {
      hp_fon.setSize(sf::Vector2f(50.0f, 6.0f));
      hp_fon.setFillColor(sf::Color::Red);
      hp_fon.setOrigin(sf::Vector2f(25.0f, 3.0f));
    
      hp_line.setSize(sf::Vector2f(50.0f, 6.0f));
      hp_line.setFillColor(sf::Color::Green);
      hp_line.setOrigin(sf::Vector2f(25.0f, 3.0f));
  } 
     
public:
    // Запуск игры
    Game() {
        hero = new Hero(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
        working = false;  
        deltaTime = 0.0f;
        initWindow();
        initGameField();
        initHeroView();
        initHPLine();
    }
    
    // я вот если честно ваще хз зачем это, но пишут так надо 
    ~Game() {
        delete hero; 
        delete window;
    }

    // Главный игровой цикл 
    void run() {
        working = true;
        
        while (working) {
            deltaTime = deltaClock.restart().asSeconds();
            
            handleEvents();
            update();
            render();
        }
    }

    void update() {
      // Движение персонажа 
        float dx = 0, dy = 0; 

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dy -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dy += 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) dx -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) dx += 1.0f;

        hero->move(dx * hero->getSpeed() * deltaTime, dy * hero->getSpeed() * deltaTime);

      //чтобы не уходил за крайй
        if (hero->getX() < 20.0f)  hero->set_pos(20.0f, hero->getY());
        if (hero->getX() > WINDOW_WIDTH - 20.0f)  
            hero->set_pos(WINDOW_WIDTH - 20.0f, hero->getY());
        if (hero->getY() < 20.0f)  hero->set_pos(hero->getX(), 20.0f);
        if (hero->getY() > WINDOW_HEIGHT - 20.0f)  
            hero->set_pos(hero->getX(), WINDOW_HEIGHT - 20.0f);
      
        float partHP = (float)hero->getHP() / hero->getMaxHP();
        hp_line.setSize(sf::Vector2f(50.0f * partHP, 6.0f));

      // Позиция над героем
        sf::Vector2f hpPos(hero->getX(), hero->getY() - 20.0f);
        hp_fon.setPosition(hpPos);
        hp_line.setPosition(hpPos);

      // Синхрон
        heroView.setPosition(sf::Vector2f(hero->getX(), hero->getY()));
    }

    // Выход через крестик 
    void handleEvents() {
        while (const auto event = window->pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                working = false;
                window->close();
            }
        }
    }
    
    // Отрисовка
    void render() {
        window->clear(sf::Color::Black);
        window->draw(gameField);
        window->draw(heroView);
        window->draw(hp_fon);
        window->draw(hp_line);
        window->display();
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}