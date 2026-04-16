// ============================================================
//  QuestTrader | QT_AdminPanel.c  (v1.3 - single-line calls)
// ============================================================

class QT_AdminPlayerEntry
{
    string uid;
    string name;
    int    activeQuests;
    int    totalCompleted;
}

class QT_AdminPanel : UIScriptedMenu
{
    private ref array<ref QT_AdminPlayerEntry> m_players;
    private ref array<string>                  m_logLines;
    private int m_selectedPlayer;

    private TextListboxWidget   m_playerList;
    private MultilineTextWidget m_playerDetail;
    private MultilineTextWidget m_logText;
    private ButtonWidget        m_btnResetQuest;
    private ButtonWidget        m_btnWipePlayer;
    private ButtonWidget        m_btnReloadConfig;
    private ButtonWidget        m_btnRespawnNPCs;
    private ButtonWidget        m_btnRefreshLog;
    private ButtonWidget        m_btnClose;
    private Widget              m_tabPlayers;
    private Widget              m_tabLogs;
    private ButtonWidget        m_tabBtnPlayers;
    private ButtonWidget        m_tabBtnLogs;
    private TextWidget          m_statusLabel;

    void QT_AdminPanel()
    {
        m_players        = new array<ref QT_AdminPlayerEntry>();
        m_logLines       = new array<string>();
        m_selectedPlayer = -1;
    }

    override Widget Init()
    {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("QuestTrader/gui/layouts/AdminPanel.layout");

        if (!layoutRoot)
        {
            Print("[QuestTrader] AdminPanel.layout not found, UI disabled.");
            return null;
        }

        m_playerList      = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("PlayerList"));
        m_playerDetail    = MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("PlayerDetail"));
        m_logText         = MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("LogText"));
        m_btnResetQuest   = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnResetQuest"));
        m_btnWipePlayer   = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnWipePlayer"));
        m_btnReloadConfig = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnReloadConfig"));
        m_btnRespawnNPCs  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnRespawnNPCs"));
        m_btnRefreshLog   = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnRefreshLog"));
        m_btnClose        = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BtnClose"));
        m_tabPlayers      = layoutRoot.FindAnyWidget("TabPlayers");
        m_tabLogs         = layoutRoot.FindAnyWidget("TabLogs");
        m_tabBtnPlayers   = ButtonWidget.Cast(layoutRoot.FindAnyWidget("TabBtnPlayers"));
        m_tabBtnLogs      = ButtonWidget.Cast(layoutRoot.FindAnyWidget("TabBtnLogs"));
        m_statusLabel     = TextWidget.Cast(layoutRoot.FindAnyWidget("StatusLabel"));

        ShowTab(0);
        QT_RPCManager.RequestAdminData();
        SetStatus("Admin panel loaded. Requesting data...");
        return layoutRoot;
    }

    void SetPlayerData(array<ref QT_AdminPlayerEntry> players)
    {
        m_players = players;
        m_playerList.ClearItems();
        foreach (QT_AdminPlayerEntry p : players)
            m_playerList.AddItem(p.name + "  [" + p.activeQuests + " active / " + p.totalCompleted + " done]", null, 0);
        SetStatus("Loaded " + players.Count() + " online player(s).");
    }

    void SetLogLines(array<string> lines)
    {
        m_logLines = lines;
        string txt = "";
        foreach (string line : lines) txt = txt + line + "\n";
        if (m_logText) m_logText.SetText(txt);
        SetStatus("Log refreshed (" + lines.Count() + " lines).");
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == m_btnClose)       { Close(); return true; }
        if (w == m_tabBtnPlayers)  { ShowTab(0); return true; }
        if (w == m_tabBtnLogs)     { ShowTab(1); QT_RPCManager.RequestAdminLog(); return true; }

        if (w == m_btnRefreshLog)
        {
            QT_RPCManager.RequestAdminLog();
            SetStatus("Refreshing log...");
            return true;
        }
        if (w == m_btnReloadConfig)
        {
            QT_RPCManager.SendAdminCommand("RELOAD_CONFIG", "", "");
            SetStatus("Config reload requested...");
            return true;
        }
        if (w == m_btnRespawnNPCs)
        {
            QT_RPCManager.SendAdminCommand("RESPAWN_NPCS", "", "");
            SetStatus("NPC respawn requested...");
            return true;
        }
        if (w == m_btnResetQuest && m_selectedPlayer >= 0)
        {
            QT_AdminPlayerEntry resetPlayer = m_players[m_selectedPlayer];
            string questId = GetQuestIdFromInput();
            if (questId == "")
            {
                SetStatus("Enter a Quest ID in the input field first.");
                return true;
            }
            QT_RPCManager.SendAdminCommand("RESET_QUEST", resetPlayer.uid, questId);
            SetStatus("Reset quest done for " + resetPlayer.name);
            return true;
        }
        if (w == m_btnWipePlayer && m_selectedPlayer >= 0)
        {
            QT_AdminPlayerEntry wipePlayer = m_players[m_selectedPlayer];
            QT_RPCManager.SendAdminCommand("WIPE_PLAYER", wipePlayer.uid, "");
            SetStatus("Wiped quests for " + wipePlayer.name);
            return true;
        }
        return false;
    }

    override bool OnChange(Widget w, int x, int y, bool finished)
    {
        if (w == m_playerList)
        {
            m_selectedPlayer = m_playerList.GetSelectedRow();
            if (m_selectedPlayer >= 0 && m_selectedPlayer < m_players.Count())
            {
                QT_AdminPlayerEntry p = m_players[m_selectedPlayer];
                if (m_playerDetail)
                    m_playerDetail.SetText("Name: " + p.name + "\nUID: " + p.uid + "\nActive: " + p.activeQuests.ToString() + "\nCompleted: " + p.totalCompleted.ToString());
            }
        }
        return false;
    }

    private void ShowTab(int tab)
    {
        if (m_tabPlayers) m_tabPlayers.Show(tab == 0);
        if (m_tabLogs)    m_tabLogs.Show(tab == 1);
    }

    private void SetStatus(string msg)
    {
        if (m_statusLabel) m_statusLabel.SetText(msg);
    }

    private string GetQuestIdFromInput()
    {
        Widget input = layoutRoot.FindAnyWidget("QuestIdInput");
        if (!input) return "";
        EditBoxWidget eb = EditBoxWidget.Cast(input);
        if (!eb) return "";
        return eb.GetText();
    }
}
