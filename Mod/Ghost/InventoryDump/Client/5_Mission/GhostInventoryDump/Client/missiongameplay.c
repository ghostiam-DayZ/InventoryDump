modded class MissionGameplay {
    void GLog(string msg) {
        Print("[Ghost_MissionGameplay] " + msg);
    }

    ref InventoryDumpNetwork_Client _invDumpClient;
    ref InventoryDumpNetwork_Server _invDumpServer;
    void MissionGameplay() {
        GLog("Started");

        _invDumpClient = InventoryDumpNetwork_Client();

        if(!GetGame().IsMultiplayer()) {
            GLog("Started Server");
            _invDumpServer = InventoryDumpNetwork_Server();
        }
    }

    override void OnKeyPress(int key) {
        super.OnKeyPress(key);

        switch(key) {
            case KeyCode.KC_ESCAPE: {
                if(_invDumpClient) {
                    _invDumpClient.CloseUI();
                }
                break;
            }
        }
    }

    override void OnUpdate(float timeslice) {
        super.OnUpdate(timeslice);

        Input input = GetGame().GetInput();
        if(input.LocalPress("GhostOpenInventoryDump", false)) {
            _invDumpClient.ToogleUI();
        }
    }
};

