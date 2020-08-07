modded class MissionGameplay {
    void GLog(string msg) {
        Print("[Ghost_InventoryDump_MissionGameplay] " + msg);
    }

    ref InventoryDumpNetwork_Client _invDumpClient;
    void MissionGameplay() {
        GLog("Init");

        _invDumpClient = InventoryDumpNetwork_Client();
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

