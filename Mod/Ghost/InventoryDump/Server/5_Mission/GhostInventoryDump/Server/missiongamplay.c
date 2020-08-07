modded class MissionGameplay {
    void GLog(string msg) {
        Print("[Ghost_InventoryDump_MissionGameplay] " + msg);
    }

    ref InventoryDumpNetwork_Server _invDumpServer;
    void MissionGameplay() {
        if(!GetGame().IsMultiplayer()) {
            GLog("Init Local Server");
            _invDumpServer = InventoryDumpNetwork_Server();
        }
    }
};

