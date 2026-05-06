#pragma once
#include <string>
#include <iostream>

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
    
    void setDamage(int dmg) { hero_damage = dmg; }
    void setSpeed(int spd) { hero_speed = spd; }

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
    Enemy(float start_x, float start_y) {
        enemy_name = "Enemy";
        enemy_spead = 100;     
        enemy_damage = 10;
        hp = 50;
        max_hp = 50;
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
	void trace ( float finish_x , float finish_y ){

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







class Gun {
private:
	string gun_name;
	int gun_speed;
	int gun_damage;
	int gun_weight;
public:
	Gun() {
		 gun_name = "";
		 gun_speed = -1;
		 gun_damage = -1;
		 gun_weight = -1;
	}
	Gun(string gun_name,int gun_speed,int gun_damage,int gun_weight) {
		this->gun_name = gun_name;
		this->gun_speed = gun_speed;
		this->gun_damage = gun_damage;
		this->gun_weight = gun_weight;
	}

	string Get_gun_name() { return gun_name; }
	int Get_gun_speed() { return gun_speed; }
	int Get_gun_damage() { return gun_damage; }
	int Get_gun_weight() { return gun_weight; }

	void Set_gun_name(string gun_name) { this->gun_name = gun_name; }
	void Set_gun_speed(int gun_speed) { this->gun_speed = gun_speed; }
	void Set_gun_damage(int gun_damage) { this->gun_damage = gun_damage; }
	void Set_gun_weight(int gun_weight) { this->gun_weight = gun_weight; }

	void Print() {
		cout << gun_name << gun_speed << gun_damage << gun_weight << endl;
	}


};

class Patron {
	
private:
	string patron_name;
	string patron_debaf;
	int patron_damage;

public:

	Patron() {
		 patron_name = "";
		 patron_debaf = "";
		 patron_damage = -1;
	}

	Patron(string patron_name, string patron_debaf, int patron_damage) {
		this->patron_name = patron_name;
		this->patron_debaf = patron_debaf;
		this->patron_damage = patron_damage;
	}
	
	string Get_patron_name() { return patron_name; }
	string Get_patron_debaf() { return patron_debaf; }
	int Get_patron_damage() { return patron_damage; }

	void Set_patron_name(string patron_name) { this->patron_name = patron_name; }
	void Set_patron_debaf(string patron_debaf) { this->patron_debaf = patron_debaf; }
	void Set_patron_damage(int patron_damage) { this->patron_damage = patron_damage; }

	void Print() {
		cout << patron_name << patron_debaf << patron_damage << endl;
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