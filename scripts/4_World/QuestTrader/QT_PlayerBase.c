// ============================================================
//  QuestTrader | QT_PlayerBase.c  (v3.2)
//  Server-side only: persistence hooks + HUD push on connect.
//  Interaction is handled entirely client-side via
//  MissionGameplay.OnUpdate proximity + F key detection.
//  Trader invulnerability handled via SetAllowDamage(false)
//  in QT_TraderSpawner.c on spawn.
// ============================================================

modded class PlayerBase
{
    private bool m_qt_hudScheduled = false;

    override void OnConnect()
    {
        super.OnConnect();
        if (GetGame().IsServer() && GetIdentity())
        {
            QT_QuestManager.GetInstance().OnPlayerConnected(GetIdentity().GetId());
            m_qt_hudScheduled = true;
            // Both HUD update and trader positions are deferred to OnScheduledTick
            // so the client RPC handler is ready before we send anything
        }
    }

    override void OnDisconnect()
    {
        if (GetGame().IsServer() && GetIdentity())
            QT_QuestManager.GetInstance().OnPlayerDisconnected(GetIdentity().GetId());
        super.OnDisconnect();
    }

    override void OnScheduledTick(float deltaTime)
    {
        super.OnScheduledTick(deltaTime);
        if (m_qt_hudScheduled && GetGame().IsServer() && GetIdentity())
        {
            m_qt_hudScheduled = false;
            QT_RPCManager.SendHUDUpdate(this);
            QT_RPCManager.SendTraderPositions(this);
        }
    }

}
