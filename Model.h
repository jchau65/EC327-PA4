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
         * @brief Reads TYPE, ID, X, Y from cin and dynamically creates a new game object
         * of the requested type at location (X, Y) with the given ID, adding it to
         * object_ptrs, active_ptrs, and the appropriate type-specific list.
         *
         * TYPE codes: g = Mage, s = ManaSpire, d = DemonHideout, o = RoamingDemon.
         *
         * Throws Invalid_Input if: TYPE is unrecognized, any numeric input is invalid,
         * or an object with the same ID already exists within the same type group.
         */
        void NewCommand();

        /**
         * @brief Saves the current game state to the given file.
         *
         * Writes, in order:
         *   1. The current simulation time.
         *   2. The catalog: count of active objects, followed by one type-code and id
         *      per active object (type codes: g/s/d/o matching NewCommand).
         *   3. The full member-variable data of every active object, in the same order,
         *      by calling each object's save() function.
         *
         * Only active (living) objects are saved; dead objects are omitted.
         *
         * @param file Open output file stream to write the save data to.
         */
        void save(ofstream& file);

        /**
         * @brief Restores game state from the given file, replacing all current objects.
         *
         * Steps:
         *   1. Deletes every existing object and clears all pointer lists.
         *   2. Reads the simulation time from the file.
         *   3. Reads the catalog and creates skeleton objects of the correct types and
         *      IDs (constructors fire here, producing their usual output messages).
         *   4. Calls restore() on each object in catalog order to fill in full state;
         *      cross-object pointer fields are resolved via the Get*Ptr() functions,
         *      which work correctly because all skeletons are created before any
         *      restore() call begins.
         *
         * @param file Open input file stream to read the save data from.
         */
        void restore(ifstream& file);
};

#endif
