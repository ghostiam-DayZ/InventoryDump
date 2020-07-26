class InventoryDumpMenuGui extends UIScriptedMenu {
    ref ScriptInvoker Event_OnDump = new ScriptInvoker();
    ref ScriptInvoker Event_OnRestore = new ScriptInvoker();
    ref ScriptInvoker Event_OnDelete = new ScriptInvoker();

    void InventoryDumpMenuGui() {
    }

    void ~InventoryDumpMenuGui() {
        PlayerBase player = GetGame().GetPlayer();
        player.GetInputController().SetDisabled(false);
    }

    TextListboxWidget _listDumpNames;
    ButtonWidget _btnCreateDump;
    EditBoxWidget _editBoxDumpName;

    ref array <string> _dumpNames = new array <string>();
    override Widget Init() {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("Ghost\\InventoryDump\\Client\\Data\\GUI\\Layouts\\InventoryDump.layout", null);

        _listDumpNames = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("ListDumps"));
        _btnCreateDump = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonCreateDump"));
        _editBoxDumpName = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxDumpName"));

        SetDumpNamesList(_dumpNames);
        SetFocus(layoutRoot);
        return layoutRoot;
    }

    override void OnShow() {
        super.OnShow();
        PPEffects.SetBlurMenu(0.5);
        GetGame().GetInput().ChangeGameFocus(1);
        SetFocus(layoutRoot);
    }

    override void OnHide() {
        super.OnHide();
        PPEffects.SetBlurMenu(0);

        GetGame().GetInput().ResetGameFocus();
        Close();
    }

    override bool OnClick(Widget w, int x, int y, int button) {
        super.OnClick(w, x, y, button);

        if(w == _btnCreateDump) {
            Event_OnDump.Invoke(_editBoxDumpName.GetText());
            _editBoxDumpName.SetText("");
        }

        return false;
    }

    override bool OnDoubleClick(Widget w, int x, int y, int button) {
        super.OnDoubleClick(w, x, y, button);

        if(w == _listDumpNames) {
            if(button == 0) {
                Event_OnRestore.Invoke(_listDumpNames.GetSelectedRow());
            }

            if(button == 1) {
                Event_OnDelete.Invoke(_listDumpNames.GetSelectedRow());
            }
        }

        return false;
    }

    void SetDumpNamesList(array <string> dumpNames) {
        _dumpNames = dumpNames;

        if(_listDumpNames) {
            _listDumpNames.ClearItems();

            for(int i = 0; i < _dumpNames.Count(); i++) {
                array <string> split = new array <string>;
                _dumpNames.Get(i).Split("|", split);

                _listDumpNames.AddItem(split.Get(0), NULL, 0);
                _listDumpNames.SetItem(i, split.Get(1), NULL, 1);
            }

            _listDumpNames.Enable(true);
        }
    }
}
