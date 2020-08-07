class CfgPatches {
    class Ghost_InventoryDump_Server {
        units[]= {};
        weapons[]= {};
        requiredVersion=0.1;
        requiredAddons[]= {
            "JM_CF_Scripts", "DZ_Data"
        };
    };
};

class CfgMods {
    class Ghost_InventoryDump_Server {
        dir = "Ghost/InventoryDump";
        picture = "";
        actionName = "GitHub";
        action = "https://github.com/ghostiam-DayZ";
        hideName = 1;
        hidePicture = 1;
        name = "InventoryDump";
        credits = "GhostIAm";
        author = "GhostIAm";
        authorID = "76561197993153055";
        version = "0.1.0";
        extra = 0;
        type = "mod";

        dependencies[] = {
            "Game",
            "World",
            "Mission"
        };

        class defs {
            class missionScriptModule {
                value = "";
                files[] = {"Ghost/InventoryDump/Server/5_Mission"};
            };
        };
    };
};
