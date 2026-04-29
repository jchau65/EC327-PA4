#include <iostream>
#include <string>
#include <math.h>
#include <random>
#include <fstream>

#include "Mage.h"
#include "RoamingDemon.h"
#include "Input_Handling.h"
#include "Model.h"

using namespace std;

Mage::Mage() :
    GameObject('M'),
    speed(5), is_at_spire(false), is_in_hideout(false),
    mana(20), experience(0), gold_pieces(0),
    battles_to_buy(0), crystals_to_buy(0),
    current_spire(nullptr), current_hideout(nullptr),
    current_roaming_demon(nullptr)
{
    cout << "Mage default constructed." << endl;
}

Mage::Mage(const char in_code) :
    GameObject(in_code),
    speed(5), is_at_spire(false), is_in_hideout(false),
    mana(20), experience(0), gold_pieces(0),
    battles_to_buy(0), crystals_to_buy(0),
    current_spire(nullptr), current_hideout(nullptr),
    current_roaming_demon(nullptr)
{
    this->state = STOPPED;
    cout << "Mage constructed." << endl;
}

Mage::Mage(const string in_name, const int in_id, const char in_code, const unsigned int in_speed, const Point2D& in_loc) :
    GameObject(in_loc, in_id, in_code),
    speed(in_speed), is_at_spire(false), is_in_hideout(false),
    name(in_name),
    mana(20), experience(0), gold_pieces(0),
    battles_to_buy(0), crystals_to_buy(0),
    current_spire(nullptr), current_hideout(nullptr),
    current_roaming_demon(nullptr)
{
    cout << "Mage constructed." << endl;
}

void Mage::StartMoving(const Point2D& dest) {
    // Remove mages from buildings
    if (state == IN_HIDEOUT) {
        is_in_hideout = false;
        current_hideout->RemoveOneMage();
    }
    else if (state == AT_SPIRE) {
        is_at_spire = false;
        current_spire->RemoveOneMage();
    }

    // If this Mage already at the destination run this bit
    Point2D currentLocation = GetLocation();
    if (currentLocation.x == dest.x && currentLocation.y == dest.y) {
        throw Invalid_Input(name + " is already at this spot.");
    }

    // If this Mage is knocked out run this bit
    else if (IsKnockedOut()) {
        throw Invalid_Input(name + " is knocked out!");
    }

    // Otherwise
    else {
        cout << display_code << id_num << ": On my way." << endl;
        // Sets the state to MOVING
        this->state = MOVING;

        // Calls the SetupDestination() function (make the next step)
        SetupDestination(dest);
    }
}

void Mage::StartMovingToHideout(DemonHideout* hideout) {
    // Create a new variable for destination
    Point2D dest = hideout->GetLocation();

    // Remove mages from buildings
    if (state == IN_HIDEOUT) {
        is_in_hideout = false;
        current_hideout->RemoveOneMage();
    }
    else if (state == AT_SPIRE) {
        is_at_spire = false;
        current_spire->RemoveOneMage();
    }

    // Set new target hideout
    current_hideout = hideout;

    // Sets the state to MOVING_TO_HIDEOUT
    this->state = MOVING_TO_HIDEOUT;

    // If this Mage is knocked out run this bit
    if (IsKnockedOut()) {
        throw Invalid_Input(name + " is knocked out!");
    }
    
    // If this Mage is already at destination run this bit
    else if (GetLocation().x == dest.x && GetLocation().y == dest.y) {
        throw Invalid_Input(name + " is already at the hideout.");
    }

    // Otherwise
    else {
        cout << display_code << id_num << ": on my way to hideout " << hideout->GetId() << endl;
        
        // Calls the SetupDestination() function (make the next step)
        SetupDestination(dest);
    }
}

void Mage::StartMovingToSpire(ManaSpire* spire) {
    // Create a new variable for destination point
    Point2D dest = spire->GetLocation();

    // Remove mages from buildings
    if (state == IN_HIDEOUT) {
        is_in_hideout = false;
        current_hideout->RemoveOneMage();
    }
    else if (state == AT_SPIRE) {
        is_at_spire = false;
        current_spire->RemoveOneMage();
    }

    // Set new target spire
    current_spire = spire;

    // Sets the state to MOVING_TO_SPIRE
    this->state = MOVING_TO_SPIRE;

    // If this Mage has lost all mana run this bit
    if (IsKnockedOut()) {
        throw Invalid_Input(name + " is knocked out!");
    }

    // If the Mage is already there, run this bit
    else if (GetLocation().x == dest.x && GetLocation().y == dest.y) {
        throw Invalid_Input(name + " is already at the spire.");
    }

    // Otherwise, general case
    else {
        cout << display_code << id_num << ": On my way to Spire " << spire->GetId() << endl;
        // Calls the SetupDestination() function with spire's location (make the next step)
        SetupDestination(dest);
    }
}

void Mage::StartBattling(const unsigned int num_battles) {
    // if Mage is out of mana
    if (IsKnockedOut()) {
        throw Invalid_Input(name + " is knocked out!");
    }

    // If Mage is not in a hideout
    else if (!is_in_hideout) {
        throw Invalid_Input(name + " is not in a hideout.");
    }

    // If Mage cannot afford battle
    else if (current_hideout->GetGoldCost(num_battles) > gold_pieces) {
        throw Invalid_Input(name + " does not have enough gold.");
    }

    // If the currrent hideout is done
    else if (current_hideout->GetNumBattlesRemaining() == 0) {
        throw Invalid_Input("Hideout " + to_string(current_hideout->GetId()) + " has already been cleared.");
    }

    // Otherwise, general case
    else {
        // Print out information
        cout << display_code << id_num << ": Started to battle at the Demon Hideout " << current_hideout->GetId() << " with " << num_battles << " battles" << endl;

        // Set battles_to_buy
        battles_to_buy = min(num_battles, current_hideout->GetNumBattlesRemaining());
        current_hideout->DemonBattle(battles_to_buy);

        // Sets the state to BATTLING_IN_HIDEOUT
        this->state = BATTLING_IN_HIDEOUT;
    }
}

void Mage::StartRecoveringMana(const unsigned int num_crystals) {
    // If the mage does not have enough gold
    if (!current_spire->CanAffordCrystal(num_crystals, gold_pieces)) {
        throw Invalid_Input(name + " does not have enough money.");
    }

    // If the Mana Spire is out of crystals
    else if (!current_spire->HasCrystals()) {
        throw Invalid_Input("Spire " + to_string(current_spire->GetId()) + " is out of crystals.");
    }

    // If the mage is not at a Mana Spire
    else if (!is_at_spire) {
        throw Invalid_Input(name + " is not at a spire.");
    }

    // Otherwise, general case
    else {
        // Sets crystals to buy as the minimum of mana spire amount and requested amount so mana spire doesn't go negative
        crystals_to_buy = min(current_spire->GetNumCrystalsRemaining(), num_crystals);
        cout << display_code << id_num << ": Started recovering " << crystals_to_buy << " crystals at Mana Spire " << current_spire->GetId() << endl;

        // Reduce spire crystal count
        current_spire->DistributeCrystals(crystals_to_buy);

        // Set the state to recovering mana
        this->state = RECOVERING_MANA;
    }
}

void Mage::Stop() {
    this->state = STOPPED;

    cout << display_code << id_num << ": Stopping... " << endl;
}

bool Mage::IsKnockedOut() const {
    return (mana <= 0);
}

bool Mage::ShouldBeVisible() const {
    return (!IsKnockedOut());
}

void Mage::ShowStatus() const {
    // Header for status
    cout << name << " status:" << endl;
    GameObject::ShowStatus();

    switch(state) {
        // Print if stopped
        case STOPPED:
            cout << "Stopped" << endl;
            break;

        // Print if moving
        case MOVING:
            cout << "Moving at a speed of " << speed << " to destination " 
            << destination << " at each step of " << delta << "." << endl;
            break;

        // Print if moving to hideout
        case MOVING_TO_HIDEOUT:
            cout << "Heading to Demon Hideout " << current_hideout->GetId() << " at a speed of " 
            << speed << " at each step of " << delta << "." << endl;
            break;

        // Print if moving to spire
        case MOVING_TO_SPIRE:
            cout << "Heading to Mana Spire " << current_spire->GetId() << " at a speed of " 
            << speed << " at each step of " << delta << "." << endl;
            break;

        // Print if in hideout
        case IN_HIDEOUT:
            cout << "Inside Demon Hideout " << current_hideout->GetId() << endl;
            break;

        // Print if at spire
        case AT_SPIRE:
            cout << "Inside Mana Spire " << current_spire->GetId() << endl;
            break;

        // Print if battling in hideout
        case BATTLING_IN_HIDEOUT:
            cout << "battling in Demon Hideout " << current_hideout->GetId() << endl;
            break;

        // Print if recovering mana
        case RECOVERING_MANA:
            cout << "Recovering mana in Mana Spire " << current_spire->GetId() << endl;
            break;
    }

    // Print general information
    cout << "Mana: " << mana << endl;
    cout << "Gold Pieces: " << gold_pieces << endl;
    cout << "Experience: " << experience << endl;
}

bool Mage::Update() {
    switch (state) {
        // if the mage is stopped
        case STOPPED:
            return false;

        // If the mage is moving
        case MOVING:
            // Checks if the mage has arrived
            if (UpdateLocation()) {
                if (state != KNOCKED_OUT) { 
                    state = STOPPED;
                }
                return true;
            }
            return false;

        // If the mage is moving to hideout
        case MOVING_TO_HIDEOUT:
            // Checks if the mage has arrived
            if (UpdateLocation()) {
                state = IN_HIDEOUT;
                is_in_hideout = true;
                current_hideout->AddOneMage();
                return true;
            }
            return false;
            
        // If the mage is moving to spire
        case MOVING_TO_SPIRE:
            // Checks if the mage has arrived
            if (UpdateLocation()) {
                state = AT_SPIRE;
                is_at_spire = true;
                current_spire->AddOneMage();
                return true;
            }
            return false;
        
        // If the mage is in hideout
        case IN_HIDEOUT:
            return false;

        // If the mage is at spire
        case AT_SPIRE:
            return false;

        // If the mage is battling in a hideout
        case BATTLING_IN_HIDEOUT:
            // Reduce mana
            if (current_hideout->GetManaCost(battles_to_buy) <= mana) {
                mana -= current_hideout->GetManaCost(battles_to_buy);
            } else {
                mana = 0;
            }

            // Reduce gold
            gold_pieces -= current_hideout->GetGoldCost(battles_to_buy);

            // Increase experience (have to wrap the code in brackets to avoid jump to case label error)
            {
                unsigned int experienceGain = current_hideout->GetExperiencePerBattle() * battles_to_buy;
                experience += experienceGain;

                // Print out experience gain and battles completed
                cout << name << " completed " << battles_to_buy << " battle(s)!" << endl;
                cout << name << " gained " << experienceGain << " experience!" << endl;
            }

            // Set state to IN_HIDEOUT and return true
            state = IN_HIDEOUT;
            return true;

        // If the mage is recovering mana
        case RECOVERING_MANA:
            // Increase mana
            mana += 5 * crystals_to_buy;

            // Reduce gold
            gold_pieces -= current_spire->GetCrystalCost(crystals_to_buy);

            // Print out mana recovered and crystals bought
            cout << name << " recovered " << 5 * crystals_to_buy << " mana!" << endl;
            cout << name << " bought " << crystals_to_buy << " crystal(s)!" << endl;

            // Set state to at spire after mana is recovered
            state = AT_SPIRE;
            return true;

        case KNOCKED_OUT:
            return false;
    }
    return false;
}

string Mage::GetName() {
    return name;
}

Mage::~Mage() {
    cout << "Mage destructed." << endl;
}

bool Mage::UpdateLocation() {
    if (IsKnockedOut()) {
        cout << name << " is out of mana and can't move" << endl;
        state = KNOCKED_OUT;
        return true;
    }

    mana -= 1;
    gold_pieces += GetRandomAmountOfGP();

    if (current_roaming_demon != nullptr) {
        if (current_roaming_demon->get_attack() <= mana) {
            mana -= current_roaming_demon->get_attack();
        } else {
            mana = 0;
        }
    }

    // Check AGAIN after decrementing
    if (IsKnockedOut()) {
        cout << name << " is out of mana and can't move" << endl;
        state = KNOCKED_OUT;
        return true;
    }

    if (fabs((destination - location).x) <= fabs(delta.x) && 
        fabs((destination - location).y) <= fabs(delta.y)) 
    {
        location = destination;
        cout << display_code << id_num << ": Arrived at destination." << endl;
        return true;
    }
    else {
        location = location + delta;
        cout << display_code << id_num << ": Moved to " << location << endl;
        return false;
    }
}

void Mage::SetupDestination(const Point2D& dest) {
    // Save the value of destination
    destination = dest;

    // Calculate each "step"
    if (GetDistanceBetween(destination, location) != 0) {
        delta = (destination - location) * ((double) speed / GetDistanceBetween(destination, location));
    }

    else {
        delta = Vector2D();
    }
}

void Mage::Followed(RoamingDemon* demon) {
    this->current_roaming_demon = demon;
}

double GetRandomAmountOfGP() {
    // Obtain random seed
    static random_device rd;

    // Initialize generator engine
    static mt19937 gen(rd());

    // Define the range
    double min = 0;
    double max = 2;
    static uniform_real_distribution<double> dis(min, max);

    // Generate double
    return dis(gen);
}

void Mage::save(ofstream& file) {
    GameObject::save(file);
    file << speed << "\n";
    file << (int)is_at_spire << "\n";
    file << (int)is_in_hideout << "\n";
    file << mana << "\n";
    file << experience << "\n";
    file << gold_pieces << "\n";
    file << battles_to_buy << "\n";
    file << crystals_to_buy << "\n";
    // name: length on one line, name on the next (handles spaces)
    file << name.size() << "\n" << name << "\n";
    // pointer fields saved as id (-1 if null)
    file << (current_spire  ? current_spire->GetId()  : -1) << "\n";
    file << (current_hideout ? current_hideout->GetId() : -1) << "\n";
    file << destination.x << "\n" << destination.y << "\n";
    file << delta.x << "\n" << delta.y << "\n";
    file << (current_roaming_demon ? current_roaming_demon->GetId() : -1) << "\n";
}

void Mage::restore(ifstream& file, Model& model) {
    GameObject::restore(file, model);
    file >> speed;
    int iat, iih;
    file >> iat >> iih;
    is_at_spire = iat; is_in_hideout = iih;
    file >> mana >> experience >> gold_pieces >> battles_to_buy >> crystals_to_buy;
    // restore name
    size_t len; file >> len; file.ignore();
    name.resize(len);
    if (len > 0) file.read(&name[0], len);
    file.ignore(); // skip trailing newline
    // restore pointers
    int spire_id, hideout_id, demon_id;
    file >> spire_id >> hideout_id;
    current_spire   = (spire_id   == -1) ? nullptr : model.GetManaSpirePtr(spire_id);
    current_hideout = (hideout_id == -1) ? nullptr : model.GetDemonHideoutPtr(hideout_id);
    file >> destination.x >> destination.y;
    file >> delta.x >> delta.y;
    file >> demon_id;
    current_roaming_demon = (demon_id == -1) ? nullptr : model.GetRoamingDemonPtr(demon_id);
}