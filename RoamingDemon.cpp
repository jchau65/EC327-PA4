#include "RoamingDemon.h"

using namespace std;

RoamingDemon::RoamingDemon(const string name, const double attack, const double health,
    const bool variant, const int id, Point2D& in_loc) : 
        attack(attack),
        health(health),
        variant(variant),
        in_combat(in_combat),
        name(name),
        GameObject(in_loc, id, 'W') {}

void RoamingDemon::follow(Mage* m) {
    this->current_mage = m;
}

bool RoamingDemon::get_variant() const {
    return variant;
}

double RoamingDemon::get_attack() const {
    return attack;
}

double RoamingDemon::get_health() const {
    return health;
}

bool RoamingDemon::get_in_combat() const {
    return in_combat;
}

bool RoamingDemon::Update() {
    // Update state and return true if Roaming Demon is dead
    if (!IsAlive()) {
        state = DEAD;
        return true;
    }

    // Update state and return true if Roaming Demon is following a mage
    if (current_mage != nullptr) {
        state = IN_HUNT;
        return true;
    }

    state = IN_ENVIRONMENT;
    return false;
}

void RoamingDemon::ShowStatus() const {
    cout << "Roaming Demon status: ";
    if (state == IN_ENVIRONMENT) {
        cout << "Lurking in Environment" << endl;
    }
    else if (state == DEAD) {
        cout << "Dead" << endl;
    }
    else {
        cout << "Following mage " << current_mage->GetId() << endl;
    }
}

bool RoamingDemon::IsAlive() const {
    return (health > 0);
}

bool RoamingDemon::ShouldBeVisible() const {
    return true;
}