#include <SFML/Graphics.hpp>
#include <cstdlib>  
#include <ctime>  
#include <iostream>
#include<vector>
#include<algorithm>
#include<cmath>
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
    float dt;

    // Герой 
    Hero* hero;

    // Визуал героя — белый шарик
    sf::CircleShape heroView;
    sf::RectangleShape hp_fon;  // фон полоски (красный)
    sf::RectangleShape hp_line;  // сама полоска (зелёная)

    // Игровое поле 
    sf::RectangleShape gameField;

    // по врагам всякие темки 
    std::vector<Enemy*> enemies;
    std::vector<sf::RectangleShape> enemyViews;

    // параметры для создания волн врагов
    bool waverun = false;
    int numWave = 0;
    int wasspawn = 0;
    //  времена для волн всякие разные 
    float waveTimer = 0.0f;
    float waveDelay = 5.0f;
    float spawnTimer = 0.0f;
    float spawnInterval = 1.0f;

    // Пули
        std::vector<Bullet> bullets;
    float shootCooldown = 0.0f;
    float shootDelay = 0.3f; // Задержка между выстрелами

    void shoot() {
        //считал направление мыши
        sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
        sf::Vector2f heroPos(hero->getX(), hero->getY());

        // вычисляем направление от мыши до курсора
        float dx = mousePos.x - heroPos.x;
        float dy = mousePos.y - heroPos.y;
        float length = sqrt(dx * dx + dy * dy);

        if (length != 0) {
            dx /= length;
            dy /= length;
        }

        // Создаем пулю с уроном от героя
        Bullet bullet(hero->getX(), hero->getY(), dx, dy, hero->getDamage());
        bullets.push_back(bullet);
    }

    void handleCollisions() {
        // Проверка столкновений пуль с врагами (идем с конца)
        for (int i = bullets.size() - 1; i >= 0; i--) {
            if (!bullets[i].isActive()) continue;

            for (int j = enemies.size() - 1; j >= 0; j--) {
                float dx = bullets[i].getX() - enemies[j]->getX();
                float dy = bullets[i].getY() - enemies[j]->getY();
                float distance = sqrt(dx * dx + dy * dy);

                if (distance < 30.0f) { // радиус 
                    enemies[j]->take_damage(bullets[i].getDamage());
                    bullets[i].setActive(false);

                    if (enemies[j]->getHP() <= 0) {
                        delete enemies[j];
                        enemies.erase(enemies.begin() + j);
                        enemyViews.erase(enemyViews.begin() + j);
                    }
                    break;
                }
            }
        }

        // Удаляем неактивные пули
        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(),
                [](const Bullet& b) { return !b.isActive(); }),
            bullets.end()
        );

        // Проверка столкновений героя с врагами так надо
        for (int i = enemies.size() - 1; i >= 0; i--) {
            float dx = hero->getX() - enemies[i]->getX();
            float dy = hero->getY() - enemies[i]->getY();
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < 35.0f) { // столкновение это тоже надо
                hero->take_damage(enemies[i]->getDamage());

                // Отбрасываем врага
                float angle = atan2(dy, dx);
                enemies[i]->set_pos(
                    enemies[i]->getX() - cos(angle) * 60,
                    enemies[i]->getY() - sin(angle) * 60
                );

                
            }
        }
    }

    void initWindow() {
        window = new sf::RenderWindow(
            sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }),
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

    void SpawnEnemies(int type) {
        // тут задается рандомная координата для спавна гадины ползучей 
        float x = (float)(rand() % WINDOW_WIDTH);
        float y = (float)(rand() % WINDOW_HEIGHT);

        switch (type) {
        case 0: { // быстрый бесячий гад

            Enemy* e = new Enemy(x, y, 30, 200, 5, "Fastic");
            enemies.push_back(e);

            sf::RectangleShape sq;
            sq.setSize(sf::Vector2f(25.0f, 25.0f));
            sq.setFillColor(sf::Color::Red);
            sq.setOrigin(sf::Vector2f(12.5f, 12.5f));

            enemyViews.push_back(sq);
            break;
        }
        case 1: { // дефолтнич 
            Enemy* e = new Enemy(x, y, 60, 120, 10, "Normis");
            enemies.push_back(e);

            sf::RectangleShape sq;
            sq.setSize(sf::Vector2f(30.0f, 30.0f));
            sq.setFillColor(sf::Color(200, 100, 0));
            sq.setOrigin(sf::Vector2f(15.0f, 15.0f));

            enemyViews.push_back(sq);
            break;
        }
        case 2: { // Гад жиртрест 
            Enemy* e = new Enemy(x, y, 150, 60, 20, "Tank");
            enemies.push_back(e);

            sf::RectangleShape sq;
            sq.setSize(sf::Vector2f(40.0f, 40.0f));
            sq.setFillColor(sf::Color(150, 0, 150));
            sq.setOrigin(sf::Vector2f(20.0f, 20.0f));

            enemyViews.push_back(sq);
            break;
        }
        case 3: { // Мелкий гад

            Enemy* e = new Enemy(x, y, 15, 150, 3, "Swarm");
            enemies.push_back(e);

            sf::RectangleShape sq;
            sq.setSize(sf::Vector2f(18.0f, 18.0f));
            sq.setFillColor(sf::Color(255, 100, 100));
            sq.setOrigin(sf::Vector2f(9.0f, 9.0f));

            enemyViews.push_back(sq);
            break;
        }
        case 4: { // Босс —  зеленое чудище 
            Enemy* e = new Enemy(x, y, 500, 50, 30, "Boss");
            enemies.push_back(e);

            sf::RectangleShape sq;
            sq.setSize(sf::Vector2f(70.0f, 70.0f));
            sq.setFillColor(sf::Color::Green);
            sq.setOrigin(sf::Vector2f(35.0f, 35.0f));

            enemyViews.push_back(sq);
            break;
        }
        }
    }

    void startNextWave() { // ну вот войд функция перезагрузки волны по сути 
        numWave++;
        wasspawn = 0;
        spawnTimer = 0.0f;
        waverun = true;

        if (numWave > 5) {// тут ограничение на волны если надо можно править тут 
            waverun = false;
            return;
        }
    }


public:
    // Запуск игры
    Game() {
        hero = new Hero(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
        working = false;
        dt = 0.0f;

        srand(time(nullptr)); // для рандомности и всякости со спавном 

        // тут запускаем отрисовки 
        initWindow();
        initGameField();
        initHeroView();
        initHPLine();
        // сразу по началу игры создаем волну 
        startNextWave();
    }

    // я вот если честно ваще хз зачем это, но пишут так надо 
    ~Game() {
        // ну тут думаю все понятно 
        for (auto e : enemies)
            delete e;
        enemies.clear();
        delete hero;
        delete window;
    }

    // Главный игровой цикл 
    void run() {
        working = true;

        while (working) {
            dt = deltaClock.restart().asSeconds();

            handleEvents();
            update();
            render();
        }
    }

    void update() {
        // -- ГЕРОЙ 
        // Движение персонажа 
        float dx = 0, dy = 0;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dy -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dy += 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) dx -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) dx += 1.0f;

        hero->move(dx * hero->getSpeed() * dt, dy * hero->getSpeed() * dt);

        //чтобы не уходил за крайй
        if (hero->getX() < 20.0f)  hero->set_pos(20.0f, hero->getY());
        if (hero->getX() > WINDOW_WIDTH - 20.0f)
            hero->set_pos(WINDOW_WIDTH - 20.0f, hero->getY());
        if (hero->getY() < 20.0f)  hero->set_pos(hero->getX(), 20.0f);
        if (hero->getY() > WINDOW_HEIGHT - 20.0f)
            hero->set_pos(hero->getX(), WINDOW_HEIGHT - 20.0f);

        float partHP = (float)hero->getHP() / hero->getMaxHP();
        hp_line.setSize(sf::Vector2f(50.0f * partHP, 6.0f));

        //--- ВРАЖИНЫ 
        if (waverun) {
            // короче 5 волн, вражины на волне по формуле где енеми ин вейв 
            spawnTimer += dt;
            int eneminwave = 3 + 2 * numWave;
            if (spawnTimer >= spawnInterval && wasspawn < eneminwave) { // проверяем временной интервал и что не набили максимум 
                int type;
                if (numWave == 5) {
                    type = 4;
                    eneminwave = 1;
                }
                else {
                    type = rand() % 4;  // случайный тип
                }
                SpawnEnemies(type);
                wasspawn++;
                spawnTimer = 0.0f;
                // ну вот сверху зарандомили и увеличили счетчики 
            }

            if (wasspawn >= eneminwave && enemies.empty()) {
                waverun = false;
                waveTimer = 0.0f;
            }
        }
        else {
            // если у нас не запущена волна в данный момент то дается кд в размере Delay и стартует следующая
            waveTimer += dt;
            if (waveTimer >= waveDelay) {
                startNextWave();
            }
        }

        // --- Графические приколы 
          // Позиция над героем
        sf::Vector2f hpPos(hero->getX(), hero->getY() - 20.0f);
        hp_fon.setPosition(hpPos);
        hp_line.setPosition(hpPos);

        // Синхрон
        heroView.setPosition(sf::Vector2f(hero->getX(), hero->getY()));

        // синхрон для енемей 
        for (int i = 0; i < enemies.size(); i++) {
            enemies[i]->trace(hero->getX(), hero->getY(), dt); // шаг 
            enemyViews[i].setPosition(sf::Vector2f(enemies[i]->getX(), enemies[i]->getY())); //перемещение 
        }
        // стрельба
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
            if (shootCooldown <= 0.0f) {
                shoot();
                shootCooldown = shootDelay;
            }
        }

        if (shootCooldown > 0.0f) {
            shootCooldown -= dt;
        }

        // Обновление пуль
        for (auto& bullet : bullets) {
            bullet.update(dt);
        }

        // Обработка столкновений
        handleCollisions();
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

    // Отрисовки
    void render() {
        window->clear(sf::Color::Black);
        window->draw(gameField);
        window->draw(heroView);
        window->draw(hp_fon);
        window->draw(hp_line);

        for (auto& ev : enemyViews) {
            window->draw(ev);
        }
        // ну понятно думаю отрисовка пуль
        for (auto& bullet : bullets) {
            bullet.draw(window);
        }

        window->display();
    }
};

int main() {
    Game game;
    game.run();

    return 0;
}
