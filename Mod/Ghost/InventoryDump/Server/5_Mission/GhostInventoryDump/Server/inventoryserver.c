class InventoryDumpNetwork_Server {
    void GLog(string msg) {
        Print("[Ghost_InventoryDumpNetwork_Server] " + msg);
    }

    void GError(string msg) {
        Error("[Ghost_InventoryDumpNetwork_Server] " + msg);
    }

    ref InventoryDump _invDump;
    void InventoryDumpNetwork_Server() {
        GLog("Init");

        _invDump = InventoryDump();
        GetRPCManager().AddRPC("Ghost_InventoryDump", "RPC_Dump", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("Ghost_InventoryDump", "RPC_Restore", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("Ghost_InventoryDump", "RPC_Delete", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("Ghost_InventoryDump", "RPC_ListRequest", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("Ghost_InventoryDump", "RPC_CheckPermissionsRequest", this, SingleplayerExecutionType.Server);
    }

    void RPC_Dump(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target) {
        Param1<string> data;
        if(!ctx.Read(data)) {
            return;
        }

        PlayerBase player = GetPlayerFromIdentity(sender);
        _invDump.Dump(player, data.param1);
    }

    void RPC_Restore(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target) {
        Param1<int> data;
        if(!ctx.Read(data)) {
            return;
        }

        PlayerBase player = GetPlayerFromIdentity(sender);
        _invDump.Restore(player, data.param1);
    }

    void RPC_Delete(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target) {
        Param1<int> data;
        if(!ctx.Read(data)) {
            return;
        }

        PlayerBase player = GetPlayerFromIdentity(sender);
        _invDump.Delete(player, data.param1);
    }

    void RPC_ListRequest(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target) {
        PlayerBase player = GetPlayerFromIdentity(sender);

        array<string> list = _invDump.GetList(player);
        GetRPCManager().SendRPC("Ghost_InventoryDump", "RPC_ListResponseFromServer", new Param1<array<string>>(list), true);
    }

    void RPC_CheckPermissionsRequest(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target) {
        PlayerBase player = GetPlayerFromIdentity(sender);

        bool allow = _invDump.CheckPermissions(player);
        GetRPCManager().SendRPC("Ghost_InventoryDump", "RPC_CheckPermissionsResponseFromServer", new Param1<bool>(allow), true);
    }

    PlayerBase GetPlayerFromIdentity(ref PlayerIdentity sender) {
        if(!GetGame().IsMultiplayer()) {
            return GetGame().GetPlayer();
        }

        string senderSteamid = sender.GetPlainId();

        array<Man> mans = new array<Man>;
        GetGame().GetPlayers(mans);

        Man senderMan;
        for(int i = 0; i < mans.Count(); i++) {
            Man man = mans.Get(i);
            if(man.GetIdentity().GetPlainId() == senderSteamid) {
                senderMan = man;
                break;
            }
        }

        if(!senderMan) {
            GError("Empty senderMan");
            return null;
        }

        return PlayerBase.Cast(senderMan);
    }
}

class InventoryDump {
    void GLog(string msg) {
        Print("[Ghost_InventoryDump] " + msg);
    }

    void GError(string msg) {
        Error("[Ghost_InventoryDump] " + msg);
    }

    void InventoryDump() {
        GLog("Init");

        if(!FileExist("$profile:InventoryDump")) {
            MakeDirectory("$profile:InventoryDump");
        }

        if(!FileExist("$profile:InventoryDump/permissions.txt")) {
            FileHandle file = OpenFile("$profile:InventoryDump/permissions.txt", FileMode.WRITE);
            if(file != 0) {
                FPrintln(file, "# Ghost (Mod developer)");
                FPrintln(file, "76561197993153055");
                CloseFile(file);
            }
        }
    }

    bool CheckPermissions(PlayerBase player) {
        if(!GetGame().IsMultiplayer()) {
            return true;
        }

        string steamID = "local";
        PlayerIdentity id = player.GetIdentity();
        if(id) {
            steamID = id.GetPlainId();
        }

        FileHandle file_handle = OpenFile("$profile:InventoryDump/permissions.txt", FileMode.READ);
        string line_content;

        while(FGets(file_handle,  line_content) > 0) {
            if(line_content.Trim() == steamID) {
                CloseFile(file_handle);
                return true;
            }
        }

        CloseFile(file_handle);

        return false;
    }

    array <string> GetList(PlayerBase player) {
        if(!CheckPermissions(player)) {
            return new array <string>();
        }

        string steamID = "local";
        PlayerIdentity id = player.GetIdentity();
        if(id) {
            steamID = id.GetPlainId();
        }

        InventoryDumpData dump = new InventoryDumpData();
        dump.inventory = new array<ref InventoryDumpHead>();

        if(FileExist("$profile:InventoryDump/" + steamID + ".json")) {
            JsonFileLoader<ref InventoryDumpData>.JsonLoadFile("$profile:InventoryDump/" + steamID + ".json", dump);
        }

        array <string> list = new array <string>;
        for(int i = 0; i < dump.inventory.Count(); i++) {
            list.Insert(dump.inventory.Get(i).name + "|" + dump.inventory.Get(i).datetime);
        }

        return list;
    }

    void Delete(PlayerBase player, int index) {
        if(!CheckPermissions(player)) {
            return;
        }

        string steamID = "local";
        PlayerIdentity id = player.GetIdentity();
        if(id) {
            steamID = id.GetPlainId();
        }

        ref InventoryDumpData dump;
        JsonFileLoader<ref InventoryDumpData>.JsonLoadFile("$profile:InventoryDump/" + steamID + ".json", dump);

        if(!dump || dump.inventory.Count() <= index) {
            GError("Restore out of range");
            return;
        }

        dump.inventory.Remove(index);

        JsonFileLoader<ref InventoryDumpData>.JsonSaveFile("$profile:InventoryDump/" + steamID + ".json", dump);
    }

    /*
    void DumpToLog(PlayerBase player) {
    	string steamID = "local";
    	PlayerIdentity id = player.GetIdentity();
    	if (id) {
    		steamID = id.GetPlainId();
    	}
    	GLog("Player: " + steamID);

    	HumanInventory inv = player.GetHumanInventory();
    	GLog("CountInventory: " +inv.CountInventory().ToString());
    	GLog("GetSlotIdCount: "+ inv.GetSlotIdCount().ToString());
    	GLog("AttachmentCount: "+ inv.AttachmentCount().ToString());
    	GLog("GetAttachmentSlotsCount: "+ inv.GetAttachmentSlotsCount().ToString());

    	int i;
    	EntityAI child;
    	ItemBase itemBs;
    	Magazine itemMag;

    	GLog("-------------------------------");

    	for (i = 0; i < inv.AttachmentCount(); i++)
    	{
    		GLog(inv.GetAttachmentFromIndex(i).ToString());
    	}

    	GLog("-------------------------------");

    	EntityAI hand = inv.GetEntityInHands();
    	if (hand)
    	{
    		GLog(hand.ToString());
    	}

    	GLog("-------------------------------");

    	array<EntityAI> children = new array<EntityAI>;
    	int count;


    	inv.EnumerateInventory(InventoryTraversalType.PREORDER, children);
    	count = children.Count();
    	for (i = 0; i < count; i++)
    	{
    		child = children.Get(i);
    		if (!child)
    		{
    			continue;
    		}

    		InventoryLocation child_src = new InventoryLocation;
    		child.GetInventory().GetCurrentInventoryLocation(child_src);
    		if (!child_src) {
    			continue;
    		}

    		GLog(child.ToString() + ": " + InventoryLocation.DumpToStringNullSafe(child_src));

    		if (Class.CastTo(itemBs, child)) {
    			GLog(child.ToString() + ": " + itemBs.GetQuantity().ToString() + " - " +itemBs.GetHealth().ToString() + " - " + itemBs.GetCompEM().GetEnergy0To1());
    		}
    		if (Class.CastTo(itemMag, child))
    			GLog(child.ToString() + ": " + itemMag.GetAmmoCount().ToString());


    		GLog("-");
    	}

    }
    */

    void Dump(PlayerBase player, string name) {
        if(!CheckPermissions(player)) {
            return;
        }

        string steamID = "local";
        PlayerIdentity id = player.GetIdentity();
        if(id) {
            steamID = id.GetPlainId();
        }

        // DumpToLog(player);

        // Load old data
        InventoryDumpData dump = new InventoryDumpData();
        dump.inventory = new array<ref InventoryDumpHead>();

        if(FileExist("$profile:InventoryDump/" + steamID + ".json")) {
            JsonFileLoader<ref InventoryDumpData>.JsonLoadFile("$profile:InventoryDump/" + steamID + ".json", dump);
        }

        // Dump
        InventoryDumpHead head = new InventoryDumpHead();
        InventoryDumpChild data = GetInventoryDumpChild(player);

        head.name = name;
        head.datetime = JMDate.Now(false).ToString("YYYY-MM-DD hh:mm:ss");
        head.data = data;

        // Append new item
        dump.inventory.Insert(head);

        if(!FileExist("$profile:InventoryDump")) {
            MakeDirectory("$profile:InventoryDump");
        }

        JsonFileLoader<ref InventoryDumpData>.JsonSaveFile("$profile:InventoryDump/" + steamID + ".json", dump);
    }

    ref InventoryDumpChild GetInventoryDumpChild(EntityAI item) {
        InventoryDumpChild ii = new InventoryDumpChild();
        ii.item = item.GetType();
        ii.pos = item.GetPosition();

        ItemBase itemBs;
        if(Class.CastTo(itemBs, item)) {
            ii.quantity = itemBs.GetQuantity();
            ii.health = itemBs.GetHealth();

            ComponentEnergyManager em = itemBs.GetCompEM();
            if(em) {
                ii.energy = em.GetEnergy();
            }
        }

        Magazine itemMag;
        if(Class.CastTo(itemMag, item)) {
            ii.magAmmoCount = itemMag.GetAmmoCount();
        }

        GameInventory inv = item.GetInventory();

        InventoryLocation item_src = new InventoryLocation;
        inv.GetCurrentInventoryLocation(item_src);
        if(item_src) {
            ii.slot = item_src.GetSlot();
            ii.locType = item_src.GetType();
            ii.locIdx = item_src.GetIdx();
            ii.locRow = item_src.GetRow();
            ii.locCol = item_src.GetCol();
            ii.locFlip = item_src.GetFlip();
        }

        ii.childrens = new array<ref InventoryDumpChild>();
        ref InventoryDumpChild cc;
        int i;

        CargoBase cargo = inv.GetCargo();
        if(cargo) {
            for(i = 0; i < cargo.GetItemCount(); i++) {
                GLog(cargo.GetItem(i).ToString());
                cc = GetInventoryDumpChild(cargo.GetItem(i));
                ii.childrens.Insert(cc);
            }
        }

        for(i = 0; i < inv.AttachmentCount(); i++) {
            GLog(inv.GetAttachmentFromIndex(i).ToString());
            cc = GetInventoryDumpChild(inv.GetAttachmentFromIndex(i));
            ii.childrens.Insert(cc);
        }

        HumanInventory hInv;
        if(Class.CastTo(hInv, inv)) {
            EntityAI hand = hInv.GetEntityInHands();
            if(hand) {
                GLog(hand.ToString());
                cc = GetInventoryDumpChild(hand);
                ii.childrens.Insert(cc);
            }
        }

        return ii;
    }

    void Restore(PlayerBase player, int index) {
        if(!CheckPermissions(player)) {
            return;
        }

        string steamID = "local";
        PlayerIdentity id = player.GetIdentity();
        if(id) {
            steamID = id.GetPlainId();
        }

        ref InventoryDumpData dump;
        JsonFileLoader<ref InventoryDumpData>.JsonLoadFile("$profile:InventoryDump/" + steamID + ".json", dump);

        if(!dump || dump.inventory.Count() <= index) {
            GError("Restore out of range");
            return;
        }

        InventoryDumpHead head = dump.inventory.Get(index);
        InventoryDumpChild data = head.data;

        GetGame().ObjectDelete(player);
        player = PlayerBase.Cast(GetGame().CreatePlayer(id, data.item, data.pos, 0, "NONE"));
        RestoreChilds(player, data.childrens);

        GetGame().SelectPlayer(id, player);
    }

    void RestoreChilds(EntityAI entity, array<ref InventoryDumpChild> childrens) {
        GameInventory inv = entity.GetInventory();
        if(!inv) {
            return;
        }

        InventoryDumpChild c;
        for(int i = 0; i < childrens.Count(); i++) {
            c = childrens.Get(i);
            if(!c) {
                continue;
            }

            EntityAI item = CreateItem(inv, entity, c);

            if(item) {
                ItemBase itemBs;
                if(Class.CastTo(itemBs, item)) {
                    itemBs.SetQuantity(c.quantity);
                    itemBs.SetHealth(c.health);

                    ComponentEnergyManager em = itemBs.GetCompEM();
                    if(em) {
                        em.SetEnergy(c.energy);
                    }
                }

                Magazine itemMag;
                if(Class.CastTo(itemMag, item)) {
                    itemMag.ServerSetAmmoCount(c.magAmmoCount);
                }

                RestoreChilds2(item, c.childrens);
            }
        }
    }

    void RestoreChilds2(EntityAI entity, array<ref InventoryDumpChild> childrens) {
        RestoreChilds(entity, childrens);
    }

    EntityAI CreateItem(GameInventory inv, EntityAI entity, InventoryDumpChild c) {
        EntityAI item;
        switch(c.locType) {
            case InventoryLocationType.ATTACHMENT: {
                GLog("CreateAttachmentEx: "+c.item);
                item = inv.CreateAttachmentEx(c.item, c.slot);
                break;
            }
            case InventoryLocationType.CARGO: {
                GLog("CreateEntityInCargoEx: "+c.item);
                item = inv.CreateEntityInCargoEx(c.item, c.locIdx, c.locRow, c.locCol, c.locFlip);
                break;
            }
            case InventoryLocationType.HANDS: {
                HumanInventory hInv;
                if(Class.CastTo(hInv, inv)) {
                    GLog("CreateInHands: "+c.item);
                    item = hInv.CreateInHands(c.item);
                    if(!item) {
                        break;
                    }
                }
                break;
            }
        }

        return item;
    }
}

class InventoryDumpHead {
    string name;
    string datetime;
    InventoryDumpChild data;
}

class InventoryDumpChild {
    string item;
    vector pos;
    int slot;
    int quantity;
    float health;
    float energy;
    int magAmmoCount;

    InventoryLocationType locType;
    int locIdx;
    int locRow;
    int locCol;
    bool locFlip;

    ref array<ref InventoryDumpChild> childrens;
}

class InventoryDumpData {
    ref array<ref InventoryDumpHead> inventory;
}

