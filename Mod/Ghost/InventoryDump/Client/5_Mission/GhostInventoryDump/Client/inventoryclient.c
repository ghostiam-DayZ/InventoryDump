class InventoryDumpNetwork_Client {
    void GLog(string msg) {
        Print("[Ghost_InventoryDumpNetwork_Client] " + msg);
    }

    void GError(string msg) {
        Error("[Ghost_InventoryDumpNetwork_Client] " + msg);
    }

    void InventoryDumpNetwork_Client() {
        GLog("Init");

        GetRPCManager().AddRPC("Ghost_InventoryDump", "RPC_ListResponseFromServer", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("Ghost_InventoryDump", "RPC_CheckPermissionsResponseFromServer", this, SingleplayerExecutionType.Client);
        Event_OnCheckPermissionsSuccess.Insert(RealToogleUI);
    }

    void Dump(string name) {
        GetRPCManager().SendRPC("Ghost_InventoryDump", "RPC_Dump", new Param1<string>(name), true);

        GetList();
    }

    void Restore(int idx) {
        GetRPCManager().SendRPC("Ghost_InventoryDump", "RPC_Restore", new Param1<int>(idx), true);

        CloseUI();
    }

    void Delete(int idx) {
        GetRPCManager().SendRPC("Ghost_InventoryDump", "RPC_Delete", new Param1<int>(idx), true);

        GetList();
    }

    void GetList() {
        GetRPCManager().SendRPC("Ghost_InventoryDump", "RPC_ListRequest", NULL, true);
    }

    void CheckPermissions() {
        GetRPCManager().SendRPC("Ghost_InventoryDump", "RPC_CheckPermissionsRequest", NULL, true);
    }

    ref ScriptInvoker Event_OnListResponse = new ScriptInvoker();
    void RPC_ListResponseFromServer(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target) {
        Param1<array<string>> data;
        if(!ctx.Read(data)) {
            return;
        }

        Event_OnListResponse.Invoke(data.param1);
    }

    ref ScriptInvoker Event_OnCheckPermissionsSuccess = new ScriptInvoker();
    void RPC_CheckPermissionsResponseFromServer(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target) {
        Param1<bool> data;
        if(!ctx.Read(data)) {
            return;
        }

        if(data.param1) {
            Event_OnCheckPermissionsSuccess.Invoke();
        }
    }

    void ToogleUI() {
        CheckPermissions();
    }

    ref InventoryDumpMenuGui _inventoryDumpMenuGui;
    void RealToogleUI() {
        UIScriptedMenu menu = GetGame().GetUIManager().GetMenu();
        if(menu == NULL) {
            _inventoryDumpMenuGui = new ref InventoryDumpMenuGui();
            _inventoryDumpMenuGui.Event_OnDump.Insert(Dump);
            _inventoryDumpMenuGui.Event_OnRestore.Insert(Restore);
            _inventoryDumpMenuGui.Event_OnDelete.Insert(Delete);

            Event_OnListResponse.Insert(_inventoryDumpMenuGui.SetDumpNamesList);
            GetList();

            GetGame().GetUIManager().ShowScriptedMenu(_inventoryDumpMenuGui, NULL);
        } else if(_inventoryDumpMenuGui) {
            CloseUI();
        }
    }

    void CloseUI() {
        if(!_inventoryDumpMenuGui) {
            return;
        }

        _inventoryDumpMenuGui.Close();
        Event_OnListResponse.Clear();
    }
}
