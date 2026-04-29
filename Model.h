#ifndef MODEL_H
#define MODEL_H

#include <list>
#include <fstream>
#include "ManaSpire.h"
#include "DemonHideout.h"
#include "Mage.h"
#include "RoamingDemon.h"
#include "View.h"

class Model {
    private:
        int time;
        list<GameObject*> object_ptrs;    // All created objects (for status and deletion)
        list<GameObject*> active_ptrs;    // Living objects (updated and displayed)
        list<Mage*> mage_ptrs;
        list<ManaSpire*> spire_ptrs;
        list<DemonHideout*> hideout_ptrs;
        list<RoamingDemon*> roamingdemon_ptrs;

    public:
        /**
         * Default constructor for Model.
         *
         * Initializes the time to 0 and then creates new objects in the heap using new as follows:
         * - Mage 1 (5, 1), object_ptrs front, mage_ptrs front
         * - Mage 2 (10, 1)
         * - ManaSpire 1 (1, 20)
         * - ManaSpire 2 (10, 20)
         * - DemonHideout 1 (0, 0)
         * - DemonHideout 2 (5, 5)
         * - RoamingDemon 1 (10, 12)
         * - RoamingDemon 2 (15, 5)
         *
         * All objects are added to object_ptrs and active_ptrs.
         * Finally, outputs a message "Model default constructed."
         */
        Model();

        /**
         * Destructor for Model.
         *
         * Deletes each object in object_ptrs and outputs a message "Model destructed."
         */
        ~Model();

        /**
         * Searches for a Mage of a given ID, and returns its pointer if found. Returns 0 if not.
         */
        Mage* GetMagePtr(const int id);

        /**
         * Searches for a ManaSpire of a given ID, and returns its pointer if found. Returns 0 if not.
         */
        ManaSpire* GetManaSpirePtr(const int id);

        /**
         * Searches for a DemonHideout of a given ID, and returns its pointer if found. Returns 0 if not.
         */
        DemonHideout* GetDemonHideoutPtr(const int id);

        /**
         * Searches for a RoamingDemon of a given ID, and returns its pointer if found. Returns 0 if not.
         */
        RoamingDemon* GetRoamingDemonPtr(const int id);

        /**
         * Updates every object in active_ptrs, removes dead objects from active_ptrs,
         * and checks win/loss conditions.
         *
         * @return True if any one of the objects returns true from Update().
         */
        bool Update();

        /**
         * Clears the view, plots all objects in active_ptrs, and draws.
         */
        void Display(View& view);

        /**
         * Outputs the time and status of all objects in object_ptrs.
         */
        void ShowStatus();

        /**
         * Reads TYPE, ID, X, Y from cin and creates a new object of the given type at (X, Y)
         * with the given ID, then adds it to the appropriate lists.
         * TYPE: g=Mage, s=ManaSpire, d=DemonHideout, o=RoamingDemon
         * Throws Invalid_Input for unknown TYPE, bad numeric input, or duplicate ID within a type group.
         */
        void NewCommand();

        /**
         * Writes time, catalog (count + type+id per active object), then each object's data.
         */
        void save(ofstream& file);

        /**
         * Deletes all existing objects, reads time and catalog from file to recreate objects,
         * then calls restore on each. Throws Invalid_Input if the file cannot be opened.
         */
        void restore(ifstream& file);
};

#endif
