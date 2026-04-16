// ============================================================
//  QuestTrader | QT_KillHooks.c  (4_World)
//  Single-shot kills: EEKilled fires before EEHitBy stores
//  the attacker. Fix: check killer param in EEKilled first,
//  then fall back to stored attacker from EEHitBy.
//  Also handle: DamageSystem passes weapon/projectile as killer.
// ============================================================

class QT_KillHelper
{
    static PlayerBase FindKillerPlayer(Object killer)
    {
        if (!killer) return null;

        // Direct player (melee)
        PlayerBase direct = PlayerBase.Cast(killer);
        if (direct) return direct;

        // Walk up parent chain: projectile -> weapon -> hands -> player
        EntityAI ent = EntityAI.Cast(killer);
        while (ent)
        {
            PlayerBase p = PlayerBase.Cast(ent);
            if (p) return p;
            EntityAI parent = EntityAI.Cast(ent.GetHierarchyParent());
            if (!parent) break;
            ent = parent;
        }
        return null;
    }

    // Try to find a player from a DamageSource / projectile info
    static PlayerBase FindFromDamageSource(EntityAI source)
    {
        if (!source) return null;
        PlayerBase p = PlayerBase.Cast(source);
        if (p) return p;
        EntityAI ent = source;
        while (ent)
        {
            p = PlayerBase.Cast(ent);
            if (p) return p;
            EntityAI parent = EntityAI.Cast(ent.GetHierarchyParent());
            if (!parent) break;
            ent = parent;
        }
        return null;
    }
}

modded class AnimalBase
{
    private PlayerBase m_qt_lastAttacker;

    override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source,
                           int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
    {
        // Store attacker BEFORE super call - super may trigger EEKilled for fatal hits
        if (GetGame().IsServer())
        {
            PlayerBase player = QT_KillHelper.FindFromDamageSource(source);
            if (player) m_qt_lastAttacker = player;
        }
        super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);
        if (!GetGame().IsServer()) return;

        // 1. Try to resolve player from the killer parameter directly
        PlayerBase player = QT_KillHelper.FindKillerPlayer(killer);

        // 2. Fall back to last recorded attacker from EEHitBy
        if (!player) player = m_qt_lastAttacker;

        // 3. Last resort: killer IS a projectile - get its owner via GetHierarchyRootPlayer
        if (!player)
        {
            EntityAI killerEnt = EntityAI.Cast(killer);
            if (killerEnt)
            {
                Object root = killerEnt.GetHierarchyRootPlayer();
                if (root) player = PlayerBase.Cast(root);
            }
        }

        if (player)
        {
            Print("[QuestTrader] Kill credited: " + GetType() + " -> " + player.GetIdentity().GetName());
            QT_QuestManager.GetInstance().OnEntityKilled(this, player);
        }
        else
        {
            Print("[QuestTrader] Kill NOT credited: " + GetType() + " killer=" + killer.GetType());
        }
        m_qt_lastAttacker = null;
    }
}

modded class ZombieBase
{
    private PlayerBase m_qt_lastAttacker;

    override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source,
                           int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
    {
        super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
        if (!GetGame().IsServer()) return;
        PlayerBase player = QT_KillHelper.FindFromDamageSource(source);
        if (player) m_qt_lastAttacker = player;
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);
        if (!GetGame().IsServer()) return;
        PlayerBase player = QT_KillHelper.FindKillerPlayer(killer);
        if (!player) player = m_qt_lastAttacker;
        if (player) QT_QuestManager.GetInstance().OnEntityKilled(this, player);
        m_qt_lastAttacker = null;
    }
}
