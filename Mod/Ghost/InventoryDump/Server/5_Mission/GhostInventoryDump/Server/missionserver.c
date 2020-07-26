modded class MissionServer {
    void GLog(string msg) {
        Print("[Ghost_MissionServer] " + msg);
    }

    ref InventoryDumpNetwork_Server _invDumpServer;
    void MissionServer() {
        GLog("Started");
        _invDumpServer = InventoryDumpNetwork_Server();
    }
};
