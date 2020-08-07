class CfgPatches {
    class Ghost_InventoryDump_Client {
        units[]= {};
        weapons[]= {};
        requiredVersion=0.1;
        requiredAddons[]= {
            "JM_CF_Scripts", "DZ_Data"
        };
    };
};

class CfgMods {
    class Ghost_InventoryDump_Client {
        dir = "Ghost/InventoryDump";
        picture = "";
        actionName = "GitHub";
        action = "https://github.com/ghostiam-DayZ";
        hideName = 0;
        hidePicture = 1;
        name = "InventoryDump";
        credits = "GhostIAm";
        author = "GhostIAm";
        authorID = "76561197993153055";
        version = "0.1.0";
        extra = 0;
        type = "mod";
        inputs="Ghost/InventoryDump/Client/Data/modded_inputs.xml";

        dependencies[] = {
            "Game",
            "World",
            "Mission"
        };

        class defs {
            class missionScriptModule {
                value = "";
                files[] = {"Ghost/InventoryDump/Client/5_Mission"};
            };
        };
    };
};
