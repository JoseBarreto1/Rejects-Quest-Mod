// ============================================================
//  QuestTrader | QT_QuestHistory.c  (v2.0)
//  Uses QT_JsonHelper. Leaderboard simplified - individual
//  history per player only. No directory scan needed.
// ============================================================

class QT_HistoryEntry
{
    string questId;
    string questTitle;
    string completedAt;
    int    completedEpoch;
    string rewardSummary;
    int    runNumber;
}

class QT_PlayerHistory
{
    string playerUID;
    string playerName;
    int    totalQuestsCompleted;
    ref array<ref QT_HistoryEntry> entries;

    void QT_PlayerHistory()
    {
        entries              = new array<ref QT_HistoryEntry>();
        totalQuestsCompleted = 0;
    }
}

class QT_QuestHistory
{
    private static ref QT_QuestHistory s_instance;
    private static const string HISTORY_DIR = "$profile:QuestTrader/history/";

    private ref map<string, ref QT_PlayerHistory> m_cache;

    static QT_QuestHistory GetInstance()
    {
        if (!s_instance) s_instance = new QT_QuestHistory();
        return s_instance;
    }

    void QT_QuestHistory()
    {
        m_cache = new map<string, ref QT_PlayerHistory>();
        if (!FileExist(HISTORY_DIR)) MakeDirectory(HISTORY_DIR);
    }

    void RecordCompletion(string uid, string playerName, QT_QuestDef def, int epochTime)
    {
        ref QT_PlayerHistory hist = GetOrLoad(uid, playerName);

        int runNumber = 0;
        foreach (QT_HistoryEntry e : hist.entries)
        {
            if (e.questId == def.id) runNumber++;
        }

        string rewardSummary = "";
        foreach (QT_Reward r : def.rewards)
        {
            if (rewardSummary != "") rewardSummary = rewardSummary + ", ";
            rewardSummary = rewardSummary + r.amount.ToString() + "x " + r.itemClassName;
        }

        ref QT_HistoryEntry entry = new QT_HistoryEntry();
        entry.questId        = def.id;
        entry.questTitle     = def.title;
        entry.completedEpoch = epochTime;
        entry.completedAt    = GetTimeStr();
        entry.rewardSummary  = rewardSummary;
        entry.runNumber      = runNumber + 1;

        hist.entries.Insert(entry);
        hist.totalQuestsCompleted++;
        hist.playerName = playerName;

        SaveHistory(uid, hist);
        QT_Logger.GetInstance().Info("HISTORY", "Completed: " + def.title + " run#" + entry.runNumber.ToString(), uid, playerName);
    }

    ref QT_PlayerHistory GetHistory(string uid, string playerName = "")
    {
        return GetOrLoad(uid, playerName);
    }

    // Leaderboard requires directory scanning which has unreliable APIs.
    // Returns empty - leaderboard tab will show "No data" gracefully.
    array<string> GetLeaderboardLines(int topN = 10)
    {
        return new array<string>();
    }

    private ref QT_PlayerHistory GetOrLoad(string uid, string playerName)
    {
        if (m_cache.Contains(uid)) return m_cache.Get(uid);

        string path = HISTORY_DIR + uid + "_history.json";
        ref QT_PlayerHistory hist;

        if (FileExist(path))
        {
            string json = QT_JsonHelper.ReadFileToString(path);
            if (json != "")
            {
                string err;
                JsonSerializer ser = new JsonSerializer();
                ser.ReadFromString(hist, json, err);
            }
            if (!hist) hist = new QT_PlayerHistory();
        }
        else
        {
            hist = new QT_PlayerHistory();
        }

        hist.playerUID = uid;
        if (playerName != "") hist.playerName = playerName;
        m_cache.Insert(uid, hist);
        return hist;
    }

    private void SaveHistory(string uid, QT_PlayerHistory hist)
    {
        QT_JsonHelper.SaveToFile(HISTORY_DIR + uid + "_history.json", hist);
    }

    private string PadTwo(int v)
    {
        if (v < 10) return "0" + v.ToString();
        return v.ToString();
    }

    private string GetTimeStr()
    {
        int yr, mo, dy, hr, mn, sc;
        GetYearMonthDay(yr, mo, dy);
        GetHourMinuteSecond(hr, mn, sc);
        return yr.ToString() + "-" + PadTwo(mo) + "-" + PadTwo(dy) + " " + PadTwo(hr) + ":" + PadTwo(mn) + ":" + PadTwo(sc);
    }
}
