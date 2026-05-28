#include <SFML/Graphics.hpp>
#include <cstdlib>  
#include <ctime>  
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
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
    sf::RectangleShape hp_fon;
    sf::RectangleShape hp_line;

    // Игровое поле 
    sf::RectangleShape gameField;

    // Враги
    std::vector<Enemy*> enemies;
    std::vector<sf::RectangleShape> enemyViews;

    // Волны
    bool waverun = false;
    int numWave = 0;
    int wasspawn = 0;
    float waveTimer = 0.0f;
    float waveDelay = 5.0f;
    float spawnTimer = 0.0f;
    float spawnInterval = 1.0f;

    // Пули
    std::vector<Bullet> bullets;
    float shootCooldown = 0.0f;

    // Оружие на карте
    std::vector<Weapon> groundWeapons;
    float weaponSpawnTimer = 0.0f;
    float weaponSpawnDelay = 15.0f;

    // Аптечка
    HealthPack* healthPack; 
    
    // Стены
    std::vector<sf::RectangleShape> walls;
    // ========== ПРОСТАЯ РЕАЛИЗАЦИЯ ТЕКСТА ==========
    sf::Font font;
    bool fontLoaded;

    // Текст для отображения (будем создавать каждый кадр, а не хранить в классе)

    float waveCompleteTimer = 0.0f;
    float gameCompleteTimer = 0.0f;
    bool gameComplete = false;
    int enemiesKilledInWave = 0;

    void shoot() {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
        sf::Vector2f heroPos(hero->getX(), hero->getY());

        float dx = mousePos.x - heroPos.x;
        float dy = mousePos.y - heroPos.y;
        float length = sqrt(dx * dx + dy * dy);

        if (length != 0) {
            dx /= length;
            dy /= length;
        }

        int bulletCount = hero->getWeaponBulletsPerShot();
        float spread = hero->getWeaponSpreadAngle();

        for (int i = 0; i < bulletCount; i++) {
            float angle = atan2(dy, dx);

            if (bulletCount > 1) {
                float offset = (i - (bulletCount - 1) / 2.0f) * (spread / (bulletCount - 1));
                angle += offset;
            }
            else if (spread > 0) {
                angle += ((rand() % 100) / 100.0f - 0.5f) * spread;
            }

            float newDx = cos(angle);
            float newDy = sin(angle);

            int dmg = hero->getDamage() * hero->getWeaponDamageMultiplier();
            Bullet bullet(hero->getX(), hero->getY(), newDx, newDy, dmg);
            bullets.push_back(bullet);
        }
    }

    void handleCollisions() {
        for (int i = bullets.size() - 1; i >= 0; i--) {
            if (!bullets[i].isActive()) continue;

            for (int j = enemies.size() - 1; j >= 0; j--) {
                float dx = bullets[i].getX() - enemies[j]->getX();
                float dy = bullets[i].getY() - enemies[j]->getY();
                float distance = sqrt(dx * dx + dy * dy);

                if (distance < 30.0f) {
                    enemies[j]->take_damage(bullets[i].getDamage());
                    bullets[i].setActive(false);

                    if (enemies[j]->getHP() <= 0) {
                        delete enemies[j];
                        enemies.erase(enemies.begin() + j);
                        enemyViews.erase(enemyViews.begin() + j);
                        enemiesKilledInWave++;
                    }
                    break;
                }
            }
        }

        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(),
                [](const Bullet& b) { return !b.isActive(); }),
            bullets.end()
        );

        for (int i = enemies.size() - 1; i >= 0; i--) {
            float dx = hero->getX() - enemies[i]->getX();
            float dy = hero->getY() - enemies[i]->getY();
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < 35.0f) {
                hero->take_damage(enemies[i]->getDamage());
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

    void initFont() {
        fontLoaded = false;
        if (font.openFromFile("arial.ttf")) {
            fontLoaded = true;
        }
        else if (font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;
        }
        else if (font.openFromFile("C:/Windows/Fonts/Arial.ttf")) {
            fontLoaded = true;
        }

        if (!fontLoaded) {
            std::cout << "Warning: Could not load font. Text will not be displayed.\n";
        }
    }

    void SpawnEnemies(int type) {
        float x = (float)(rand() % WINDOW_WIDTH);
        float y = (float)(rand() % WINDOW_HEIGHT);

        switch (type) {
        case 0: {
            Enemy* e = new Enemy(x, y, 30, 200, 5, "Fastic");
            enemies.push_back(e);
            sf::RectangleShape sq;
            sq.setSize(sf::Vector2f(25.0f, 25.0f));
            sq.setFillColor(sf::Color::Red);
            sq.setOrigin(sf::Vector2f(12.5f, 12.5f));
            enemyViews.push_back(sq);
            break;
        }
        case 1: {
            Enemy* e = new Enemy(x, y, 60, 120, 10, "Normis");
            enemies.push_back(e);
            sf::RectangleShape sq;
            sq.setSize(sf::Vector2f(30.0f, 30.0f));
            sq.setFillColor(sf::Color(200, 100, 0));
            sq.setOrigin(sf::Vector2f(15.0f, 15.0f));
            enemyViews.push_back(sq);
            break;
        }
        case 2: {
            Enemy* e = new Enemy(x, y, 150, 60, 20, "Tank");
            enemies.push_back(e);
            sf::RectangleShape sq;
            sq.setSize(sf::Vector2f(40.0f, 40.0f));
            sq.setFillColor(sf::Color(150, 0, 150));
            sq.setOrigin(sf::Vector2f(20.0f, 20.0f));
            enemyViews.push_back(sq);
            break;
        }
        case 3: {
            Enemy* e = new Enemy(x, y, 15, 150, 3, "Swarm");
            enemies.push_back(e);
            sf::RectangleShape sq;
            sq.setSize(sf::Vector2f(18.0f, 18.0f));
            sq.setFillColor(sf::Color(255, 100, 100));
            sq.setOrigin(sf::Vector2f(9.0f, 9.0f));
            enemyViews.push_back(sq);
            break;
        }
        case 4: {
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

    void startNextWave() {
        numWave++;
        spawnWallsForWave();
        wasspawn = 0;
        enemiesKilledInWave = 0;
        spawnTimer = 0.0f;
        waverun = true;
        waveCompleteTimer = 0.0f;

        if (numWave > 5) {
            waverun = false;
            gameComplete = true;
            gameCompleteTimer = 3.0f;
            return;
        }
    }

    void showWaveComplete() {
        waveCompleteTimer = 2.0f;
    }

    // Функция для отрисовки текста (создаем текст каждый раз при отрисовке)
    void drawText(float x, float y, const std::string& text, int size, sf::Color color, bool centered = false) {
        if (!fontLoaded) return;

        sf::Text txt(font);
        txt.setString(text);
        txt.setCharacterSize(size);
        txt.setFillColor(color);

        if (centered) {
            sf::FloatRect bounds = txt.getLocalBounds();
            txt.setOrigin(sf::Vector2f(bounds.size.x / 2, bounds.size.y / 2));
            txt.setPosition(sf::Vector2f(x, y));
        }
        else {
            txt.setPosition(sf::Vector2f(x, y));
        }

        window->draw(txt);
    }

    // спавнер припятсвий в зависимости от волны 
    void spawnWallsForWave() {
        walls.clear();
        
        if (numWave == 1) {
            // 5 белых квадратов
            sf::RectangleShape wall1(sf::Vector2f(50.0f, 50.0f));
            wall1.setFillColor(sf::Color::White);
            wall1.setPosition(sf::Vector2f(200.0f, 200.0f));
            walls.push_back(wall1);
            
            sf::RectangleShape wall2(sf::Vector2f(50.0f, 50.0f));
            wall2.setFillColor(sf::Color::White);
            wall2.setPosition(sf::Vector2f(400.0f, 300.0f));
            walls.push_back(wall2);
            
            sf::RectangleShape wall3(sf::Vector2f(50.0f, 50.0f));
            wall3.setFillColor(sf::Color::White);
            wall3.setPosition(sf::Vector2f(600.0f, 200.0f));
            walls.push_back(wall3);
            
            sf::RectangleShape wall4(sf::Vector2f(50.0f, 50.0f));
            wall4.setFillColor(sf::Color::White);
            wall4.setPosition(sf::Vector2f(300.0f, 500.0f));
            walls.push_back(wall4);
            
            sf::RectangleShape wall5(sf::Vector2f(50.0f, 50.0f));
            wall5.setFillColor(sf::Color::White);
            wall5.setPosition(sf::Vector2f(700.0f, 500.0f));
            walls.push_back(wall5);
        }
        else if (numWave == 2) {
            // 4 стены по углам + 1 в центре
            for (int i = 0; i < 4; i++) {
                sf::RectangleShape wall(sf::Vector2f(80.0f, 80.0f));
                wall.setFillColor(sf::Color(150, 150, 150));
                float x = (i % 2 == 0) ? 100.0f : WINDOW_WIDTH - 180.0f;
                float y = (i / 2 == 0) ? 100.0f : WINDOW_HEIGHT - 180.0f;
                wall.setPosition(sf::Vector2f(x, y));
                walls.push_back(wall);
            }
            
            // Квадрат в центре
            sf::RectangleShape centerWall(sf::Vector2f(100.0f, 100.0f));
            centerWall.setFillColor(sf::Color(200, 200, 200));
            centerWall.setPosition(sf::Vector2f(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 - 50));
            walls.push_back(centerWall);
        }
        else if (numWave == 3) {
            // Коридор из стен
            sf::RectangleShape wall1(sf::Vector2f(400.0f, 30.0f));
            wall1.setFillColor(sf::Color(100, 100, 200));
            wall1.setPosition(sf::Vector2f(312.0f, 250.0f));
            walls.push_back(wall1);
            
            sf::RectangleShape wall2(sf::Vector2f(400.0f, 30.0f));
            wall2.setFillColor(sf::Color(100, 100, 200));
            wall2.setPosition(sf::Vector2f(312.0f, 450.0f));
            walls.push_back(wall2);
        }
        else if (numWave == 4) {
            // Нормальный лабиринт
            sf::RectangleShape wall1(sf::Vector2f(30.0f, 400.0f));
            wall1.setFillColor(sf::Color(200, 100, 100));
            wall1.setPosition(sf::Vector2f(250.0f, 100.0f));
            walls.push_back(wall1);
            
            sf::RectangleShape wall2(sf::Vector2f(30.0f, 400.0f));
            wall2.setFillColor(sf::Color(200, 100, 100));
            wall2.setPosition(sf::Vector2f(500.0f, 268.0f));
            walls.push_back(wall2);
            
            sf::RectangleShape wall3(sf::Vector2f(30.0f, 400.0f));
            wall3.setFillColor(sf::Color(200, 100, 100));
            wall3.setPosition(sf::Vector2f(750.0f, 100.0f));
            walls.push_back(wall3);
            
            sf::RectangleShape wall4(sf::Vector2f(280.0f, 30.0f));
            wall4.setFillColor(sf::Color(200, 100, 100));
            wall4.setPosition(sf::Vector2f(250.0f, 400.0f));
            walls.push_back(wall4);
            
            sf::RectangleShape wall5(sf::Vector2f(250.0f, 30.0f));
            wall5.setFillColor(sf::Color(200, 100, 100));
            wall5.setPosition(sf::Vector2f(500.0f, 300.0f));
            walls.push_back(wall5);
        }
    }

public:
    Game() {
        hero = new Hero(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
        working = false;
        dt = 0.0f;
        gameComplete = false;
        waveCompleteTimer = 0.0f;
        gameCompleteTimer = 0.0f;
        enemiesKilledInWave = 0;

        srand(time(nullptr));

        initWindow();
        initGameField();
        initHeroView();
        initHPLine();
        initFont();  // Загружаем шрифт
        startNextWave();

        float hx = rand() % 900 + 62;
        float hy = rand() % 650 + 59;
        healthPack = new HealthPack(hx, hy);
    }

    ~Game() {
        for (auto e : enemies)
            delete e;
        enemies.clear();
        delete hero;
        delete window;
        delete healthPack;
    }

    void run() {
        working = true;

        while (working) {
            dt = deltaClock.restart().asSeconds();

            handleEvents();

            if (!gameComplete) {
                update();
            }
            else {
                if (gameCompleteTimer > 0.0f) {
                    gameCompleteTimer -= dt;
                }
            }

            render();

            if (gameComplete && gameCompleteTimer <= 0.0f) {
                working = false;
                window->close();
            }
        }
    }

    void update() {
        if (gameComplete) {
            if (gameCompleteTimer > 0.0f) {
                gameCompleteTimer -= dt;
            }
            return;
        }

        if (waveCompleteTimer > 0.0f) {
            waveCompleteTimer -= dt;
        }

        // Движение героя
        float dx = 0, dy = 0;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dy -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dy += 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) dx -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) dx += 1.0f;

        // Сохраняем старую позицию
        float oldHeroX = hero->getX();
        float oldHeroY = hero->getY();

        hero->move(dx * hero->getSpeed() * dt, dy * hero->getSpeed() * dt);

        // Проверка столкновений героя со стенами и выталкивание
        for (auto& wall : walls) {
            sf::FloatRect heroBounds(
                sf::Vector2f(hero->getX() - 15.0f, hero->getY() - 15.0f), 
                sf::Vector2f(30.0f, 30.0f)
            );
            
            if (wall.getGlobalBounds().findIntersection(heroBounds)) {
                // Находим пересечение для выталкивания
                sf::FloatRect wallBounds = wall.getGlobalBounds();
                auto intersection = heroBounds.findIntersection(wallBounds);

                // Выталкиваем по наименьшей оси
                if (intersection && intersection->size.x < intersection->size.y) {
                    // Выталкиваем по X
                    if (hero->getX() < wall.getPosition().x + wall.getSize().x / 2) {
                        hero->set_pos(wall.getPosition().x - 15.0f, hero->getY());
                    } else {
                        hero->set_pos(wall.getPosition().x + wall.getSize().x + 15.0f, hero->getY());
                    }
                } else {
                    // Выталкиваем по Y
                    if (hero->getY() < wall.getPosition().y + wall.getSize().y / 2) {
                        hero->set_pos(hero->getX(), wall.getPosition().y - 15.0f);
                    } else {
                        hero->set_pos(hero->getX(), wall.getPosition().y + wall.getSize().y + 15.0f);
                    }
                }
            }
        }

        if (hero->getX() < 20.0f) hero->set_pos(20.0f, hero->getY());
        if (hero->getX() > WINDOW_WIDTH - 20.0f) hero->set_pos(WINDOW_WIDTH - 20.0f, hero->getY());
        if (hero->getY() < 20.0f) hero->set_pos(hero->getX(), 20.0f);
        if (hero->getY() > WINDOW_HEIGHT - 20.0f) hero->set_pos(hero->getX(), WINDOW_HEIGHT - 20.0f);

        float partHP = (float)hero->getHP() / hero->getMaxHP();
        hp_line.setSize(sf::Vector2f(50.0f * partHP, 6.0f));

        // Волны
        if (waverun) {
            spawnTimer += dt;
            int eneminwave = (numWave == 5) ? 1 : (3 + 2 * numWave);

            if (spawnTimer >= spawnInterval && wasspawn < eneminwave) {
                int type = (numWave == 5) ? 4 : (rand() % 4);
                SpawnEnemies(type);
                wasspawn++;
                spawnTimer = 0.0f;
            }

            if (wasspawn >= eneminwave && enemies.empty()) {
                waverun = false;
                waveTimer = 0.0f;
                showWaveComplete();
            }
        }
        else {
            waveTimer += dt;
            if (waveTimer >= waveDelay) {
                startNextWave();
            }
        }

        // Позиции
        sf::Vector2f hpPos(hero->getX(), hero->getY() - 20.0f);
        hp_fon.setPosition(hpPos);
        hp_line.setPosition(hpPos);
        heroView.setPosition(sf::Vector2f(hero->getX(), hero->getY()));

        // проверяем на то что бы враги не тыкались мордой в стену 
        for (int i = 0; i < enemies.size(); i++) {
            // Сохраняем старую позицию
            float oldX = enemies[i]->getX();
            float oldY = enemies[i]->getY();
            
            enemies[i]->trace(hero->getX(), hero->getY(), dt);
            
            // Проверка столкновений со стенами
            bool collidesWithWall = false;
            for (auto& wall : walls) {
                sf::FloatRect enemyBounds(
                    sf::Vector2f(enemies[i]->getX() - 15.0f, enemies[i]->getY() - 15.0f), 
                    sf::Vector2f(30.0f, 30.0f)
                );
                
                if (wall.getGlobalBounds().findIntersection(enemyBounds)) {
                    collidesWithWall = true;
                }
            }
            
            // Если есть столкновение, возвращаем на старую позицию
            if (collidesWithWall) {
                enemies[i]->set_pos(oldX, oldY);
            }
            
            enemyViews[i].setPosition(sf::Vector2f(enemies[i]->getX(), enemies[i]->getY()));
        }

        hero->updateWeaponTimer(dt);

        // Оружие на карте
        weaponSpawnTimer += dt;

        // Считаем только активное оружие на земле
        int activeWeaponsCount = 0;
        for (int i = 0; i < groundWeapons.size(); i++) {
            if (groundWeapons[i].isActive() && groundWeapons[i].isOnGround()) {
                activeWeaponsCount++;
            }
        }

        if (weaponSpawnTimer >= weaponSpawnDelay && activeWeaponsCount < 2) {
            string type = (rand() % 2 == 0) ? "shotgun" : "sniper";
            float wx = rand() % 900 + 62;
            float wy = rand() % 650 + 59;
            groundWeapons.push_back(Weapon(type, wx, wy));
            weaponSpawnTimer = 0.0f;
        }

        for (auto& weapon : groundWeapons) {
            weapon.updateRespawn(dt);
        }

        for (auto& weapon : groundWeapons) {
            if (weapon.isActive() && weapon.isOnGround()) {
                float dx = hero->getX() - weapon.getX();
                float dy = hero->getY() - weapon.getY();
                float dist = sqrt(dx * dx + dy * dy);
                if (dist < 30.0f) {
                    hero->setWeapon(weapon.getType(),
                        weapon.getFireRate(),
                        weapon.getBulletsPerShot(),
                        weapon.getSpreadAngle(),
                        weapon.getDamageMultiplier());
                    weapon.pickUp();
                }
            }
        }

        // Стрельба
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
            if (shootCooldown <= 0.0f) {
                shoot();
                shootCooldown = hero->getWeaponFireRate();
            }
        }
        if (shootCooldown > 0.0f) shootCooldown -= dt;

        for (auto& bullet : bullets) bullet.update(dt);
        handleCollisions();

        // Аптечка
        healthPack->update(dt);
        if (healthPack->isActive()) {
            float dx = hero->getX() - healthPack->getX();
            float dy = hero->getY() - healthPack->getY();
            float dist = sqrt(dx * dx + dy * dy);
            if (dist < 30.0f) {
                int healAmount = hero->getMaxHP() * 0.3f;
                hero->take_damage(-healAmount);
                healthPack->pickUp();
            }
        }

        // Смерть героя
        if (hero->getHP() <= 0) {
            gameComplete = true;
            gameCompleteTimer = 3.0f;
        }
    }

    void handleEvents() {
        while (const auto event = window->pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                working = false;
                window->close();
            }
        }
    }

    void render() {
        window->clear(sf::Color::Black);
        window->draw(gameField);
        window->draw(heroView);
        window->draw(hp_fon);
        window->draw(hp_line);
        healthPack->draw(window);
        for (auto& wall : walls) {
            window->draw(wall);
        }
        for (auto& ev : enemyViews) window->draw(ev);
        for (auto& bullet : bullets) bullet.draw(window);

        // Оружие с подписями
        for (auto& weapon : groundWeapons) {
            if (weapon.isActive() && weapon.isOnGround()) {
                weapon.draw(window);

                string label = (weapon.getType() == "shotgun") ? "SHOTGUN" :
                    (weapon.getType() == "sniper") ? "SNIPER" : "WEAPON";
                drawText(weapon.getX(), weapon.getY() - 25, label, 14, sf::Color::White, true);
            }
        }

        // Информация о волне
        int eneminwave = (numWave == 5) ? 1 : (3 + 2 * numWave);
        std::string waveInfo = "WAVE: " + std::to_string(numWave) + "/5   Enemies: " +
            std::to_string(enemiesKilledInWave) + "/" + std::to_string(eneminwave);
        drawText(20, 20, waveInfo, 24, sf::Color::White, false);

        // Сообщение о завершении волны
        if (waveCompleteTimer > 0.0f) {
            drawText(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2,
                "WAVE " + std::to_string(numWave) + " COMPLETE!",
                36, sf::Color::Yellow, true);
        }

        // Сообщение о завершении игры
        if (gameComplete) {
            if (numWave > 5 && hero->getHP() > 0) {
                drawText(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 50,
                    "GAME COMPLETE!\nYou defeated the boss!",
                    48, sf::Color::Red, true);
            }
            else if (hero->getHP() <= 0) {
                drawText(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 50,
                    "GAME OVER! You died...",
                    48, sf::Color::Red, true);
            }
            drawText(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 50,
                "Press ESC or close window to exit",
                20, sf::Color::White, true);
        }

        window->display();
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
