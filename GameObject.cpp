#include <iostream>
#include <fstream>

#include "Point2D.h"
#include "GameObject.h"

GameObject::GameObject(const char in_code) : location(Point2D()), display_code(in_code), id_num(1), state(0) {
    cout << "GameObject constructed" << endl; // Default constructor
}

GameObject::GameObject(const Point2D& in_loc, const int in_id, const char in_code) : location(in_loc), id_num(in_id), display_code(in_code), state(0) {
    cout << "GameObject constructed" << endl; // Parameterized constructor
}

Point2D GameObject::GetLocation() const {
    return location;
}

int GameObject::GetId() const {
    return id_num;
}

char GameObject::GetState() const {
    return state;
}

void GameObject::ShowStatus() const {
    cout << display_code << id_num << " at " << location << endl;
}

GameObject::~GameObject() {
    cout << "GameObject destructed" << endl;
}

void GameObject::DrawSelf(char* ptr) {
    ptr[0] = display_code;
    ptr[1] = '0' + id_num;
}

void GameObject::save(ofstream& file) {
    file << location.x << "\n" << location.y << "\n";
    file << id_num << "\n";
    file << display_code << "\n";
    file << (int)state << "\n";
}

void GameObject::restore(ifstream& file, Model& /*model*/) {
    file >> location.x >> location.y;
    file >> id_num;
    file >> display_code;
    int s; file >> s; state = (char)s;
}