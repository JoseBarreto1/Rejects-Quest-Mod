// ============================================================
//  QuestTrader | QT_PersistenceManager.c  (v3.0)
//  Avoids "out" keyword for complex types (map) - returns
//  the loaded map directly instead of via out parameter.
// ============================================================

class QT_SavedObjective
{
    int currentAmount;
}

class QT_SavedQuestState
{
    string questId;
    int    state;
    ref array<ref QT_SavedObjective> objectives;
    int    completedTimestamp;

    void QT_SavedQuestState() { objectives = new array<ref QT_SavedObjective>(); }
}

class QT_SavedPlayerData
{
    string playerUID;
    ref array<ref QT_SavedQuestState> questStates;

    void QT_SavedPlayerData() { questStates = new array<ref QT_SavedQuestState>(); }
}

class QT_PersistenceManager
{
    private static const string SAVE_DIR = "$profile:QuestTrader/players/";

    static void SavePlayer(string uid, map<string, ref QT_PlayerQuestState> questMap)
    {
        if (!FileExist(SAVE_DIR)) MakeDirectory(SAVE_DIR);

        ref QT_SavedPlayerData saveData = new QT_SavedPlayerData();
        saveData.playerUID = uid;

        foreach (string questId, QT_PlayerQuestState qs : questMap)
        {
            ref QT_SavedQuestState saved = new QT_SavedQuestState();
            saved.questId            = questId;
            saved.state              = qs.state;
            saved.completedTimestamp = qs.completedTimestamp;

            foreach (int progress : qs.objectiveProgress)
            {
                ref QT_SavedObjective so = new QT_SavedObjective();
                so.currentAmount = progress;
                saved.objectives.Insert(so);
            }
            saveData.questStates.Insert(saved);
        }

        string filePath = SAVE_DIR + uid + ".json";
        string json = "";
        JsonSerializer ser = new JsonSerializer();
        bool ok = ser.WriteToString(saveData, false, json);
        if (!ok || json == "" || json.Length() < 5)
        {
            Print("[QuestTrader] SAVE FAILED for: " + uid + " ok=" + ok + " len=" + json.Length());
            return;
        }

        FileHandle fh = OpenFile(filePath, FileMode.WRITE);
        if (fh == 0)
        {
            Print("[QuestTrader] Cannot open file for write: " + filePath);
            return;
        }
        FPrint(fh, json);
        CloseFile(fh);
        Print("[QuestTrader] Saved " + questMap.Count() + " quests for: " + uid + " (" + json.Length() + "B)");
    }

    // Returns loaded map, or null on failure - avoids "out" for complex types
    static ref map<string, ref QT_PlayerQuestState> LoadPlayer(string uid)
    {
        string filePath = SAVE_DIR + uid + ".json";
        if (!FileExist(filePath)) return null;

        string json = "";
        FileHandle fh = OpenFile(filePath, FileMode.READ);
        if (fh == 0) return null;
        string line;
        while (FGets(fh, line) >= 0)
            json = json + line + "\n";
        CloseFile(fh);

        if (json == "" || json.Length() < 5)
        {
            Print("[QuestTrader] Empty save file for: " + uid);
            return null;
        }

        ref QT_SavedPlayerData saveData = new QT_SavedPlayerData();
        string err;
        JsonSerializer ser = new JsonSerializer();
        if (!ser.ReadFromString(saveData, json, err))
        {
            Print("[QuestTrader] LOAD PARSE FAILED for: " + uid + " - " + err);
            return null;
        }
        if (!saveData) return null;

        ref map<string, ref QT_PlayerQuestState> questMap = new map<string, ref QT_PlayerQuestState>();
        foreach (QT_SavedQuestState saved : saveData.questStates)
        {
            ref QT_PlayerQuestState qs = new QT_PlayerQuestState();
            qs.questId            = saved.questId;
            qs.state              = saved.state;
            qs.completedTimestamp = saved.completedTimestamp;
            foreach (QT_SavedObjective so : saved.objectives)
                qs.objectiveProgress.Insert(so.currentAmount);
            questMap.Insert(saved.questId, qs);
        }

        Print("[QuestTrader] Loaded " + questMap.Count() + " quests for: " + uid);
        return questMap;
    }

    static void DeletePlayer(string uid)
    {
        string filePath = SAVE_DIR + uid + ".json";
        if (FileExist(filePath)) DeleteFile(filePath);
    }
}
