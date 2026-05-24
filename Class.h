#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <cmath>

using namespace std;

class Hero {
private:
	string hero_name;
	string hero_ability;

    int hp ;
    int max_hp ;

    float pos_x , pos_y ;

	int hero_speed;
	int hero_damage;

    string weapon ; 
    string current_weapon_type;
    float weapon_fire_rate;
    int weapon_bullets_per_shot;
    float weapon_spread_angle;
    float weapon_damage_multiplier;
	float weapon_timer;

public:
    Hero(float start_x, float start_y) {
        hero_name = "Player";
        hero_ability = "";
        hero_speed = 200;     
        hero_damage = 10;
        hp = 100;
        max_hp = 100;
        pos_x = start_x;
        pos_y = start_y;

		current_weapon_type = "rifle";
        weapon_fire_rate = 0.25f;
        weapon_bullets_per_shot = 1;
        weapon_spread_angle = 0.1f;
        weapon_damage_multiplier = 1.0f;
		weapon_timer = 0.0f;
    }

    void move ( float dx , float dy ){
        pos_x += dx ; 
        pos_y += dy ; 
    }
    void set_pos ( float st_x , float st_y){
        pos_x = st_x; 
        pos_y = st_y ; 
    }

    void take_damage ( int damage ){
        hp -= damage ; 
        if ( hp < 0 ){
            hp = 0 ; 
        }
    }

    float getX() const { return pos_x; }
    float getY() const { return pos_y; }
    int getHP() const { return hp; }
    int getMaxHP() const { return max_hp; }
    int getSpeed() const { return hero_speed; }
    int getDamage() const { return hero_damage; }

	void setWeapon(string type, float rate, int bullets, float spread, float dmg_mult) {
        current_weapon_type = type;
        weapon_fire_rate = rate;
        weapon_bullets_per_shot = bullets;
        weapon_spread_angle = spread;
        weapon_damage_multiplier = dmg_mult;
        
        if (type == "rifle") {
            weapon_timer = 0.0f;
        } 
		else {
            weapon_timer = 8.0f;
        }
    }
    
    float getWeaponFireRate() const { return weapon_fire_rate; }
    int getWeaponBulletsPerShot() const { return weapon_bullets_per_shot; }
    float getWeaponSpreadAngle() const { return weapon_spread_angle; }
    float getWeaponDamageMultiplier() const { return weapon_damage_multiplier; }
	float getWeaponTimer() const { return weapon_timer; }

	void updateWeaponTimer(float dt) {
        if (weapon_timer > 0.0f) {
            weapon_timer -= dt;
            if (weapon_timer <= 0.0f) {
                current_weapon_type = "rifle";
                weapon_fire_rate = 0.25f;
                weapon_bullets_per_shot = 1;
                weapon_spread_angle = 0.1f;
                weapon_damage_multiplier = 1.0f;
                weapon_timer = 0.0f;
            }
        }
    }
    
    
};

class Enemy {
private:
	string enemy_name;

    int hp ;
    int max_hp ;

    float pos_x , pos_y ;

	int enemy_spead;
	int enemy_damage;

public:
    Enemy(float start_x, float start_y, int hp_val, int spd, int dmg, std::string name ) {
    	enemy_name = name;
    	enemy_spead = spd;
    	enemy_damage = dmg;
   		hp = hp_val;
    	max_hp = hp_val;
    	pos_x = start_x;
    	pos_y = start_y;
	}

    void set_pos ( float st_x , float st_y){
        pos_x = st_x; 
        pos_y = st_y ; 
    }

    void take_damage ( int damage ){
        hp -= damage ; 
        if ( hp < 0 ){
            hp = 0 ; 
        }
    }
	// ну короче какая то идея += я на просторах инета видел что там через теорему пифагора просто в тик он делает 
	// шаг по кротчайшему расстоянию типо на тебя за условную dt п р впемени 
	void trace ( float finish_x , float finish_y, float dt  ){
		float dx = finish_x - pos_x ;
		float dy = finish_y - pos_y ;

		float dist = sqrt( dx * dx + dy *dy) ; 

		if ( dist != 0) {
			dx /= dist; 
			dy /= dist ;
		}

		pos_x += dx * enemy_spead * dt ; 
		pos_y += dy * enemy_spead * dt ; 
	}

    float getX() const { return pos_x; }
    float getY() const { return pos_y; }
    int getHP() const { return hp; }
    int getMaxHP() const { return max_hp; }
    int getSpeed() const { return enemy_spead; }
    int getDamage() const { return enemy_damage; }
    
    void setDamage(int dmg) { enemy_damage = dmg; }
    void setSpeed(int spd) { enemy_spead = spd; }

};

class Bullet {
private:
    float pos_x, pos_y;
    float direction_x, direction_y;
    float speed;
    int damage;
    bool active;
    sf::RectangleShape shape;

public:
    Bullet(float start_x, float start_y, float dir_x, float dir_y, int dmg) {
        pos_x = start_x;
        pos_y = start_y;
        direction_x = dir_x;
        direction_y = dir_y;
        speed = 500.0f;
        damage = dmg;
        active = true;

        shape.setSize(sf::Vector2f(5.0f, 5.0f));
        shape.setFillColor(sf::Color::Yellow);
        shape.setOrigin(sf::Vector2f(2.5f, 2.5f));
    }

    void update(float dt) {
        pos_x += direction_x * speed * dt;
        pos_y += direction_y * speed * dt;
        shape.setPosition(sf::Vector2f(pos_x, pos_y));
    }

    void draw(sf::RenderWindow* window) {
        if (active) {
            window->draw(shape);
        }
    }

    bool isActive() const { return active; }
    void setActive(bool act) { active = act; }
	void setSpeed(float spd) { speed = spd; }
    float getX() const { return pos_x; }
    float getY() const { return pos_y; }
    int getDamage() const { return damage; }
};


class Weapon {
private:
    string weapon_type;     
    float fire_rate;        // задержка между выстрелами
    int bullets_per_shot;   // сколько пуль за выстрел
    float spread_angle;     // угол разброса
    float damage_multiplier;
    float pos_x, pos_y;
    bool on_ground;  // лежит на земле или уже подобрано
    sf::RectangleShape view;
    float respawn_timer; // таймер до следующего появления
    float ground_time; // таймер сколько валяется 
    bool active;// активно ли оружие сейчас
  
public:
// конструктор соотв 
    Weapon(string type, float x, float y) {
        weapon_type = type;
        pos_x = x;
        pos_y = y;
        on_ground = true;
        active = true;
        respawn_timer = 0.0f;
        ground_time = 0.0f;
        
        if (type == "rifle") { // автомат типо быстро стреляет небольшой разброс 
            fire_rate = 0.25f;
            bullets_per_shot = 1;
            spread_angle = 0.1f;
            damage_multiplier = 1.0f;
            view.setSize(sf::Vector2f(15.0f, 10.0f));
            view.setFillColor(sf::Color(100, 100, 100));
        }
        else if (type == "shotgun") { // ну дробаш куча дерьма вылетает и дамажит 
            fire_rate = 0.8f;
            bullets_per_shot = 5;
            spread_angle = 0.3f;
            damage_multiplier = 0.4f;
            view.setSize(sf::Vector2f(20.0f, 12.0f));
            view.setFillColor(sf::Color(200, 100, 0));
        }
        else if (type == "sniper") { // винтовка типо супер медленная но 10х урона 
            fire_rate = 1.5f;
            bullets_per_shot = 1;
            spread_angle = 0.0f;
            damage_multiplier = 10.0f;
            view.setSize(sf::Vector2f(30.0f, 5.0f));
            view.setFillColor(sf::Color(180, 180, 180));
        }
        
        view.setOrigin(sf::Vector2f(view.getSize().x / 2, view.getSize().y / 2));
        view.setPosition(sf::Vector2f(pos_x, pos_y));
    }
    
    string getType() const { return weapon_type; }
    float getFireRate() const { return fire_rate; }
    int getBulletsPerShot() const { return bullets_per_shot; }
    float getSpreadAngle() const { return spread_angle; }
    float getDamageMultiplier() const { return damage_multiplier; }
    float getX() const { return pos_x; }
    float getY() const { return pos_y; }
    bool isOnGround() const { return on_ground; }
    bool isActive() const { return active; }
    
    void pickUp() { 
        on_ground = false; 
        active = false;
    }
    
    void updateRespawn(float dt) {
        if (on_ground && active) {
            ground_time += dt;
            if (ground_time >= 8.0f) {
                active = false;
                on_ground = false;
                ground_time = 0.0f;
            }
        }
        else if (!active) {
            respawn_timer += dt;
            if (respawn_timer >= 15.0f) {
                active = true;
                on_ground = true;
                respawn_timer = 0.0f;
                ground_time = 0.0f;
                
                pos_x = rand() % 900 + 62;
                pos_y = rand() % 650 + 59;
                view.setPosition(sf::Vector2f(pos_x, pos_y));
            }
        }
    }
    
    void draw(sf::RenderWindow* window) {
        if (active && on_ground) {
            window->draw(view);
        }
    }
};




class Battlefield {

private:

	string battlefield_name;
	string battlefield_debaf_speed;
	string battlefield_debaf_ability;
	string battlefield_floor;

public:
	Battlefield() {
		 battlefield_name = "";
		 battlefield_debaf_speed = "";
		 battlefield_debaf_ability = "";
		 battlefield_floor = "";
	}
	Battlefield(string battlefield_name,string battlefield_debaf_speed,string battlefield_debaf_ability, string battlefield_floor) {

		this->battlefield_name = battlefield_name;
		this->battlefield_debaf_speed = battlefield_debaf_speed;
		this->battlefield_debaf_ability = battlefield_debaf_ability;
		this->battlefield_floor = battlefield_floor;
	}

	string Get_battlefield_name() { return battlefield_name; }
	string Get_battlefield_debaf_speed() {return battlefield_debaf_speed;}
	string Get_battlefield_debaf_ability() { return battlefield_debaf_ability; }
	string Get_battlefield_floor() { return battlefield_floor; }

	void Set_battlefield_name(string battlefield_name) { this->battlefield_name = battlefield_name; }
	void Set_battlefield_debaf_speed(string battlefield_debaf_speed) { this->battlefield_debaf_speed = battlefield_debaf_speed; }
	void Set_battlefield_debaf_ability(string battlefield_debaf_ability) { this->battlefield_debaf_ability = battlefield_debaf_ability; }
	void Set_battlefield_floor(string battlefield_floor) { this->battlefield_floor = battlefield_floor; }

	void Print() {
		cout << battlefield_name << battlefield_debaf_speed << battlefield_debaf_ability <<battlefield_floor << endl;
	}

};
