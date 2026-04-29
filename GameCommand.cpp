#include <iostream>
#include <fstream>
#include <limits>
#include <string>

#include "GameCommand.h"
#include "Input_Handling.h"

using namespace std;

static double read_double() {
    double val;
    if (!(cin >> val)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw Invalid_Input("Expected a number");
    }
    return val;
}

void DoMoveCommand(Model& model, const int mage_id, Point2D& p1) {
    // Get the mage ptr
    Mage* mage = model.GetMagePtr(mage_id);

    // Check if mage exists
    if (mage != nullptr) {
        cout << "Moving " << mage->GetName() << " to " << p1 << endl;
        mage->StartMoving(p1);
    }

    // If mage doesn't exist
    else {
        throw Invalid_Input("Mage doesn't exist");
    }
}

void DoMoveToSpireCommand(Model& model, const int mage_id, const int spire_id) {
    // Get the mage ptr
    Mage* mage = model.GetMagePtr(mage_id);

    // Get the ManaSpire ptr
    ManaSpire* spire = model.GetManaSpirePtr(spire_id);

    // Check if mage and spire exist
    if (mage != nullptr && spire != nullptr) {
        cout << "Moving " << mage->GetName() << " to Mana Spire " << spire->GetId() << endl;
        mage->StartMovingToSpire(spire);
    }

    // If mage or spire doesn't exist
    else {
        if (mage == nullptr) {
            throw Invalid_Input("Mage doesn't exist");
        }

        else if (spire == nullptr) {
            throw Invalid_Input("Spire doesn't exist");
        }
    }
}

void DoMoveToHideoutCommand(Model& model, const int mage_id, const int hideout_id) {
    // Get the mage ptr
    Mage* mage = model.GetMagePtr(mage_id);

    // Get the DemonHideout ptr
    DemonHideout* hideout = model.GetDemonHideoutPtr(hideout_id);

    // Check if mage and hideout exist
    if (mage != nullptr && hideout != nullptr) {
        cout << "Moving " << mage->GetName() << " to Demon Hideout " << hideout->GetId() << endl;
        mage->StartMovingToHideout(hideout);
    }

    // If mage or hideout doesn't exist
    else {
        if (mage == nullptr) {
            throw Invalid_Input("Mage doesn't exist");
        }

        else if (hideout == nullptr) {
            throw Invalid_Input("Hideout doesn't exist");
        }
    }
}

void DoStopCommand(Model& model, const int mage_id) {
    // Get the mage ptr
    Mage* mage = model.GetMagePtr(mage_id);

    // Check if mage exists
    if (mage != nullptr) {
        cout << "Stopping " << mage->GetName() << endl;
        mage->Stop();
    }

    // If mage doesn't exist
    else {
        throw Invalid_Input("Mage doesn't exist");
    }
}

void DoBattleCommand(Model& model, const int mage_id, const unsigned int battles) {
    // Get the mage ptr
    Mage* mage = model.GetMagePtr(mage_id);

    // Check if mage exists and if they are in a hideout
    if (mage != nullptr && mage->GetState() == IN_HIDEOUT) {
        cout << mage->GetName() << " is battling" << endl;
        mage->StartBattling(battles);
    }

    // If mage doesn't exist or they are not in a hideout
    else {
        if (mage == nullptr) {
            throw Invalid_Input("Mage doesn't exist");
        }

        else if (mage->GetState() != IN_HIDEOUT) {
            throw Invalid_Input("Mage is not in a hideout");
        }
    }
}

void DoRecoverInSpireCommand(Model& model, const int mage_id, const unsigned int crystals_needed) {
    // Get the mage ptr
    Mage* mage = model.GetMagePtr(mage_id);

    // Check if mage exists and if they are in a spire
    if (mage != nullptr && mage->GetState() == AT_SPIRE) {
        cout << "Recovering " << mage->GetName() << "'s mana" << endl;
        mage->StartRecoveringMana(crystals_needed);
    }

    // If mage doesn't exist or they are not at a spire
    else {
        if (mage == nullptr) {
            throw Invalid_Input("Mage doesn't exist");
        }
        
        else if (mage->GetState() != AT_SPIRE) {
            throw Invalid_Input("Mage is not in a spire");
        }
    }
}

void DoAdvanceCommand(Model& model, View& view) {
    cout << "Advancing one tick" << endl;

    // Run update
    model.Update();
}

void DoRunCommand(Model& model, View& view) {
    cout << "Advancing to next event" << endl;

    // Runs until 5 ticks, or until next event
    for (int i = 0; i < 5; i++) {
        if (model.Update()) {
            break;
        }
    }
}

void HandleCommand(Model& model, View& view, const char command) {
    // variables for IDs, x and y coordinates, etc. Max of 3 arguments per command
    double arg1;
    double arg2;
    double arg3;

    // variable to store Point2D
    Point2D point;

    try {
        switch (command) {
            case 'm':
                arg1 = read_double(); arg2 = read_double(); arg3 = read_double();
                point = Point2D(arg2, arg3);
                DoMoveCommand(model, arg1, point);
                break;
            case 's':
                arg1 = read_double(); arg2 = read_double();
                DoMoveToSpireCommand(model, arg1, arg2);
                break;
            case 'd':
                arg1 = read_double(); arg2 = read_double();
                DoMoveToHideoutCommand(model, arg1, arg2);
                break;
            case 'h':
                arg1 = read_double();
                DoStopCommand(model, arg1);
                break;
            case 'c':
                arg1 = read_double(); arg2 = read_double();
                DoRecoverInSpireCommand(model, arg1, arg2);
                break;
            case 'b':
                arg1 = read_double(); arg2 = read_double();
                DoBattleCommand(model, arg1, arg2);
                break;
            case 'n':
                model.NewCommand();
                break;
            case 'S': {
                string filename; cin >> filename;
                ofstream outfile(filename);
                model.save(outfile);
                outfile.close();
                cout << "Game saved to " << filename << endl;
                break;
            }
            case 'R': {
                string filename; cin >> filename;
                ifstream infile(filename);
                if (!infile.is_open())
                    throw Invalid_Input("Cannot open save file: " + filename);
                model.restore(infile);
                infile.close();
                cout << "Game restored from " << filename << endl;
                break;
            }
            case 'p':
                model.ShowStatus();
                break;
            case 'a':
                DoAdvanceCommand(model, view);
                break;
            case 'r':
                DoRunCommand(model, view);
                break;
            case 'q':
                exit(0);
                break;
            default:
                throw Invalid_Input("Try a different command");
        }
    }

    catch (Invalid_Input& except) {
        cout << "Invalid input - " << except.msg_ptr << endl;
    }
}