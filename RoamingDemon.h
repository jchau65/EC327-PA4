#ifndef ROAMINGDEMON_H
#define ROAMINGDEMON_H

#include <iostream>
#include <string>
#include <fstream>

#include "GameObject.h"

class Mage;
class Model;

using namespace std;

/**
 * Enumerated objects to represent RoamingDemon state.
 */
enum RoamingDemonStates {
    IN_ENVIRONMENT = 0,
    DEAD = 1,
    IN_HUNT = 2,
};

class RoamingDemon : public GameObject {
    protected:
        double attack;
        double health;
        bool variant;
        bool in_combat;
        string name;
        Mage* current_mage;

    public:
        /** 
         * Constructor for RoamingDemon.
         */
        RoamingDemon(const string name, const double attack, const double health, 
                     const bool variant, const int id, const Point2D& in_loc);

        /**
         * Follows a mage.
         * 
         * @param m Mage to follow
         */
        void follow(Mage* m);

        /** 
         * @return variant
         */
        bool get_variant() const;

        /**
         * @return attack
         */
        double get_attack() const;

        /**
         * @return health
         */
        double get_health() const;

        /**
         * @return in_combat
         */
        bool get_in_combat() const;

        /**
         * Updates the RoamingDemon object as follows:
         * - state 'IN_ENVIRONMENT' - default state when RoamingDemon is created and returns false
         * - state 'DEAD' - when the RoamingDemon's health is 0 or below
         * - state 'IN_HUNT': when the RoamingDemon is following a mage
         */
        bool Update() override;

        /**
         * Outputs something like:
         * 
         * "Roaming Demon status:" -> calls GameObject::ShowStatus -> outputs state specific information
         */
        void ShowStatus() const;

        /**
         * Checks the health of the Roaming Demon and return True if it is alive
         */
        bool IsAlive() const;

        /**
         * @return If this RoamingDemon should be visible
         */
        bool ShouldBeVisible() const override;

        void save(ofstream& file) override;
        void restore(ifstream& file, Model& model) override;
};

#endif