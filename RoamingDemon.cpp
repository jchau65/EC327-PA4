#include "RoamingDemon.h"
#include "Mage.h"
#include "Model.h"

#include <random>
#include <fstream>

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
    m->Followed(this);
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
    cout << "Roaming Demon status:" << endl;
    GameObject::ShowStatus();
    if (state == IN_ENVIRONMENT) {
        cout << "Lurking in Environment" << endl;
    }
    else if (state == DEAD) {
        cout << "Dead" << endl;
    }
    else {
        cout << "Following mage " << current_mage->GetId() << endl;
    }
    cout << "Health: " << health << endl;
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

void RoamingDemon::save(ofstream& file) {
    GameObject::save(file);
    file << attack << "\n";
    file << health << "\n";
    file << (int)variant << "\n";
    file << (int)in_combat << "\n";
    // name: length on one line, name on the next (handles spaces)
    file << name.size() << "\n" << name << "\n";
    file << (current_mage ? current_mage->GetId() : -1) << "\n";
}

void RoamingDemon::restore(ifstream& file, Model& model) {
    GameObject::restore(file, model);
    file >> attack >> health;
    int v, ic; file >> v >> ic;
    variant = v; in_combat = ic;
    // restore name
    size_t len; file >> len; file.ignore();
    name.resize(len);
    if (len > 0) file.read(&name[0], len);
    file.ignore(); // skip trailing newline
    // restore current_mage pointer
    int mage_id; file >> mage_id;
    current_mage = (mage_id == -1) ? nullptr : model.GetMagePtr(mage_id);
}