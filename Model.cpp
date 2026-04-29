#include <iostream>
#include <list>
#include <limits>
#include <string>

#include "Model.h"
#include "View.h"
#include "Input_Handling.h"

using namespace std;

static int read_int() {
    int val;
    if (!(cin >> val)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw Invalid_Input("Expected an integer");
    }
    return val;
}

Model::Model() : time(0) {
    // Create mages
    Mage* m1 = new Mage("Serena", 1, 'M', 1, Point2D(5, 1));
    Mage* m2 = new Mage("Rumi", 2, 'M', 2, Point2D(10, 1));

    object_ptrs.push_back(m1);
    object_ptrs.push_back(m2);
    active_ptrs.push_back(m1);
    active_ptrs.push_back(m2);
    mage_ptrs.push_back(m1);
    mage_ptrs.push_back(m2);

    // Create ManaSpires
    ManaSpire* s1 = new ManaSpire(1, 1, 100, Point2D(1, 20));
    ManaSpire* s2 = new ManaSpire(2, 2, 200, Point2D(10, 20));

    object_ptrs.push_back(s1);
    object_ptrs.push_back(s2);
    active_ptrs.push_back(s1);
    active_ptrs.push_back(s2);
    spire_ptrs.push_back(s1);
    spire_ptrs.push_back(s2);

    // Create DemonHideouts
    DemonHideout* d1 = new DemonHideout(10, 1, 2, 3, 1, Point2D(0, 0));
    DemonHideout* d2 = new DemonHideout(20, 5, 7.5, 4, 2, Point2D(5, 5));

    object_ptrs.push_back(d1);
    object_ptrs.push_back(d2);
    active_ptrs.push_back(d1);
    active_ptrs.push_back(d2);
    hideout_ptrs.push_back(d1);
    hideout_ptrs.push_back(d2);

    // Create RoamingDemons
    RoamingDemon* r1 = new RoamingDemon("Demon 1", 2, 20, false, 1, Point2D(10, 12));
    RoamingDemon* r2 = new RoamingDemon("Demon 2", 10, 20, false, 2, Point2D(15, 5));

    object_ptrs.push_back(r1);
    object_ptrs.push_back(r2);
    active_ptrs.push_back(r1);
    active_ptrs.push_back(r2);
    roamingdemon_ptrs.push_back(r1);
    roamingdemon_ptrs.push_back(r2);

    cout << "Model default constructed" << endl;
}

Model::~Model() {
    for (GameObject* obj : object_ptrs) {
        delete obj;
    }
    cout << "Model destructed." << endl;
}

Mage* Model::GetMagePtr(const int id) {
    for (Mage* m : mage_ptrs) {
        if (m->GetId() == id) {
            return m;
        }
    }
    return 0;
}

ManaSpire* Model::GetManaSpirePtr(const int id) {
    for (ManaSpire* s : spire_ptrs) {
        if (s->GetId() == id) {
            return s;
        }
    }
    return 0;
}

DemonHideout* Model::GetDemonHideoutPtr(const int id) {
    for (DemonHideout* d : hideout_ptrs) {
        if (d->GetId() == id) {
            return d;
        }
    }
    return 0;
}

RoamingDemon* Model::GetRoamingDemonPtr(const int id) {
    for (RoamingDemon* r : roamingdemon_ptrs) {
        if (r->GetId() == id) {
            return r;
        }
    }
    return 0;
}

bool Model::Update() {
    time++;

    bool returnedTrue = false;
    bool gameOverHideouts = true;
    bool gameOverMages = true;

    // Check proximity: living demons follow nearby mages
    for (RoamingDemon* rd : roamingdemon_ptrs) {
        if (!rd->IsAlive()) continue;
        for (Mage* m : mage_ptrs) {
            double dist = GetDistanceBetween(rd->GetLocation(), m->GetLocation());
            if (dist < 4) {
                rd->follow(m);
            }
        }
    }

    // Update each active object
    for (GameObject* obj : active_ptrs) {
        if (obj->Update()) {
            returnedTrue = true;
        }
    }

    // Remove dead objects from active_ptrs
    auto it = active_ptrs.begin();
    while (it != active_ptrs.end()) {
        if (!(*it)->ShouldBeVisible()) {
            cout << "Dead object removed." << endl;
            it = active_ptrs.erase(it);
        } else {
            ++it;
        }
    }

    // Check win condition: all hideouts defeated
    for (DemonHideout* d : hideout_ptrs) {
        if (!d->passed()) {
            gameOverHideouts = false;
        }
    }
    if (gameOverHideouts) {
        cout << "GAME OVER! You win! All battles done!" << endl;
        exit(0);
    }

    // Check loss condition: all mages knocked out
    for (Mage* m : mage_ptrs) {
        if (!m->IsKnockedOut()) {
            gameOverMages = false;
        }
    }
    if (gameOverMages) {
        cout << "GAME OVER: You lose! All of your Mages' mana is lost!" << endl;
        exit(0);
    }

    return returnedTrue;
}

void Model::Display(View& view) {
    view.Clear();
    for (GameObject* obj : active_ptrs) {
        view.Plot(obj);
    }
    view.Draw();
}

void Model::ShowStatus() {
    cout << "Time: " << time << endl;
    for (GameObject* obj : object_ptrs) {
        obj->ShowStatus();
    }
}

void Model::NewCommand() {
    char type;
    if (!(cin >> type)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw Invalid_Input("Expected a type character (g, s, d, o)");
    }

    int id = read_int();
    int x  = read_int();
    int y  = read_int();
    Point2D loc(x, y);

    switch (type) {
        case 'g': {
            if (GetMagePtr(id) != nullptr)
                throw Invalid_Input("A Mage with that ID already exists");
            Mage* m = new Mage("Mage" + to_string(id), id, 'M', 5, loc);
            object_ptrs.push_back(m);
            active_ptrs.push_back(m);
            mage_ptrs.push_back(m);
            break;
        }
        case 's': {
            if (GetManaSpirePtr(id) != nullptr)
                throw Invalid_Input("A ManaSpire with that ID already exists");
            ManaSpire* s = new ManaSpire(id, 5.0, 100, loc);
            object_ptrs.push_back(s);
            active_ptrs.push_back(s);
            spire_ptrs.push_back(s);
            break;
        }
        case 'd': {
            if (GetDemonHideoutPtr(id) != nullptr)
                throw Invalid_Input("A DemonHideout with that ID already exists");
            DemonHideout* d = new DemonHideout(10, 1, 1.0, 2, id, loc);
            object_ptrs.push_back(d);
            active_ptrs.push_back(d);
            hideout_ptrs.push_back(d);
            break;
        }
        case 'o': {
            if (GetRoamingDemonPtr(id) != nullptr)
                throw Invalid_Input("A RoamingDemon with that ID already exists");
            RoamingDemon* r = new RoamingDemon("Demon" + to_string(id), 5.0, 2.0, false, id, loc);
            object_ptrs.push_back(r);
            active_ptrs.push_back(r);
            roamingdemon_ptrs.push_back(r);
            break;
        }
        default:
            throw Invalid_Input("Unknown type - use g (Mage), s (ManaSpire), d (DemonHideout), o (RoamingDemon)");
    }
}

void Model::save(ofstream& file) {
    // Write simulation time
    file << time << "\n";

    // Write catalog: count then type-code + id for each active object
    file << active_ptrs.size() << "\n";
    for (GameObject* obj : active_ptrs) {
        char code;
        if      (dynamic_cast<Mage*>(obj))         code = 'g';
        else if (dynamic_cast<ManaSpire*>(obj))    code = 's';
        else if (dynamic_cast<DemonHideout*>(obj)) code = 'd';
        else                                        code = 'o'; // RoamingDemon
        file << code << " " << obj->GetId() << "\n";
    }

    // Write each object's full state
    for (GameObject* obj : active_ptrs) {
        obj->save(file);
    }
}

void Model::restore(ifstream& file) {
    // Delete all existing objects and clear every list
    for (GameObject* obj : object_ptrs) {
        delete obj;
    }
    object_ptrs.clear();
    active_ptrs.clear();
    mage_ptrs.clear();
    spire_ptrs.clear();
    hideout_ptrs.clear();
    roamingdemon_ptrs.clear();

    // Restore simulation time
    file >> time;

    // Read catalog and create skeleton objects (constructors fire here)
    int count; file >> count;
    for (int i = 0; i < count; i++) {
        char code; int id;
        file >> code >> id;
        switch (code) {
            case 'g': {
                Mage* m = new Mage("", id, 'M', 5, Point2D(0, 0));
                object_ptrs.push_back(m);
                active_ptrs.push_back(m);
                mage_ptrs.push_back(m);
                break;
            }
            case 's': {
                ManaSpire* s = new ManaSpire(id, 5.0, 100, Point2D(0, 0));
                object_ptrs.push_back(s);
                active_ptrs.push_back(s);
                spire_ptrs.push_back(s);
                break;
            }
            case 'd': {
                DemonHideout* d = new DemonHideout(10, 1, 1.0, 2, id, Point2D(0, 0));
                object_ptrs.push_back(d);
                active_ptrs.push_back(d);
                hideout_ptrs.push_back(d);
                break;
            }
            case 'o': {
                RoamingDemon* r = new RoamingDemon("", 5.0, 2.0, false, id, Point2D(0, 0));
                object_ptrs.push_back(r);
                active_ptrs.push_back(r);
                roamingdemon_ptrs.push_back(r);
                break;
            }
        }
    }

    // All skeletons exist now — restore full state (pointer lookups work correctly)
    for (GameObject* obj : active_ptrs) {
        obj->restore(file, *this);
    }
}
