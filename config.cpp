// ============================================================
//  QuestTrader | config.cpp  (v2.1)
//  No custom NPC classes. No CfgActions - interaction is
//  handled via proximity + input in MissionGameplay.OnUpdate.
// ============================================================

class CfgPatches
{
    class QuestTrader
    {
        units[]          = {};
        weapons[]        = {};
        requiredVersion  = 0.1;
        requiredAddons[] = { "DZ_Data", "DZ_Scripts" };
    };
};

class CfgMods
{
    class QuestTrader
    {
        dir         = "QuestTrader";
        picture     = "";
        action      = "";
        hideName    = 0;
        hidePicture = 0;
        name        = "Quest Trader";
        credits     = "";
        author      = "Taco Donkey";
        authorID    = "0";
        version     = "2.1.0";
        extra       = 0;
        type        = "mod";

        dependencies[] = { "Game", "World", "Mission" };

        class defs
        {
            class gameScriptModule
            {
                value  = "";
                files[] = { "QuestTrader/scripts/3_Game" };
            };
            class worldScriptModule
            {
                value  = "";
                files[] = { "QuestTrader/scripts/4_World" };
            };
            class missionScriptModule
            {
                value  = "";
                files[] = { "QuestTrader/scripts/5_Mission" };
            };
        };
    };
};
