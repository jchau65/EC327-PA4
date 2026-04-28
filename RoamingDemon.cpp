#include "RoamingDemon.h"
#include "Mage.h"

#include <random>

using namespace std;

RoamingDemon::RoamingDemon(const string name, const double attack, const double health,
    const bool variant, const int id, const Point2D& in_loc) : 
        attack(attack),
        health(health),
        variant(variant),
        in_combat(false),
        name(name),
        current_mage(nullptr),
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
        display_code = 'w';
        return true;
    }

    // Update state and return true if Roaming Demon is following a mage
    if (current_mage != nullptr) {
        state = IN_HUNT;

        // Reduce health by attack (higher attack -> shorter lifespan)
        health -= attack;

        // Follow mage
        location = current_mage->GetLocation();
        return true;
    }

    // If the demon is just in environment, move randomly
    state = IN_ENVIRONMENT;

    // Set up random number generator
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> distrib(-2, 2);

    // Move in both directions in random numbers
    location = location + Vector2D(distrib(gen), distrib(gen));
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
    if (!IsAlive()) {
        return false;
    }
    return true;
}