# QuestTrader – DayZ Mod

A fully configurable Quest system for DayZ. Players interact with Trader NPCs
to accept quests (collect items or kill animals/infected) and receive item rewards
upon completion. Everything is driven by a single JSON config file – no recompile needed.

---

## Features

- **Collect quests** – require the player to gather specific item types
- **Kill quests** – require the player to kill specific entity types (wolves, deer, boar, etc.)
- **Multiple traders** – place as many NPCs as you like, each with their own quest pool
- **Prerequisites** – chain quests so later ones unlock after earlier ones complete
- **Repeatable quests** with configurable cooldown (hours)
- **Live kill-progress notifications** in chat
- **Full JSON config** – add/edit traders and quests without touching a single script
- **Anti-damage NPCs** – traders cannot be killed

---

## Installation

1. Copy the `QuestTrader` folder into your server's `@QuestTrader` mod directory.
2. Add `@QuestTrader` to your server's `-mod=` launch parameter.
3. Copy `QuestTrader/config/QuestConfig.json` to your server profile directory:
   ```
   <ServerProfileDir>/QuestTrader/QuestConfig.json
   ```
   On first run the mod will do this automatically if the file is missing.

---

## File Structure

```
@QuestTrader/
├── config.cpp
├── ModInfo.c
├── config/
│   └── QuestConfig.json          ← Edit this!
├── gui/
│   └── layouts/
│       └── QuestMenu.layout
└── scripts/
    ├── 4_World/
    │   └── QuestTrader/
    │       ├── QuestData.c           (Data classes & enums)
    │       ├── QuestConfigLoader.c   (JSON parser)
    │       ├── QuestManager.c        (Server logic)
    │       ├── QT_TraderNPC.c        (NPC entity classes)
    │       ├── QT_TraderSpawner.c    (NPC world spawner)
    │       ├── QT_RPCManager.c       (Client↔Server RPC)
    │       ├── ActionInteractTrader.c (Player action)
    │       └── QT_PlayerBase.c       (Action registration)
    └── 5_Mission/
        └── QuestTrader/
            ├── QT_MissionHooks.c     (Mission lifecycle hooks)
            └── QT_QuestMenu.c        (Client UI)
```

---

## Configuring QuestConfig.json

### Trader NPC Positions

```json
"TraderNPCPositions": [
  {
    "id": "trader_001",           // Unique ID – referenced by quests
    "name": "Viktor the Questmaster",
    "position": [3692.0, 0.0, 5988.0],  // X, Y (auto-snapped), Z
    "orientation": [0.0, 180.0, 0.0],   // Pitch, Yaw, Roll
    "model": "SurvivorM_Mirek",          // See supported models below
    "greeting": "Zdravstvuyte, survivor.",
    "farewell": "Stay safe."
  }
]
```

**Supported models:**
`SurvivorM_Mirek`, `SurvivorF_Eva`, `SurvivorM_Denis`,
`SurvivorF_Irena`, `SurvivorF_Marta`, `SurvivorM_Roman`

---

### Quest Definitions

```json
"Quests": [
  {
    "id": "quest_001",              // Unique quest ID
    "traderId": "trader_001",       // Which trader offers this quest
    "title": "Scavenger's Duty",
    "description": "Find medical supplies.",
    "type": 0,                      // 0 = collect,  1 = kill,  2 = Deliver
    "repeatable": false,
    "cooldownHours": 0,             // Hours before repeatable quest resets
    "objectives": [
      // COLLECT objective:
      { "itemClassName": "Bandage", "requiredAmount": 5, "description": "Collect 5 Bandages" },
      // KILL objective:
      { "entityClassName": "Wolf",  "requiredAmount": 3, "description": "Kill 3 Wolves" }
    ],
    "rewards": [
      { "itemClassName": "AKM",        "amount": 1 },
      { "itemClassName": "AKMag30Rnd", "amount": 2 }
    ],
    "rewardMessage":  "Excellent work! Take this.",
    "acceptMessage":  "Bring me those supplies.",
    "prerequisiteQuestIds": ["quest_000"]   // Leave [] for no prerequisites
  }
]
```

---

### Global Settings

```json
"Settings": {
  "interactionDistance": 3.5,       // Metres the player must be within to interact
  "showQuestMarkersOnMap": true,     // (Future feature hook)
  "notifyOnKillProgress": true,      // Chat messages as you rack up kills
  "debugLogging": false              // Verbose server log output
}
```

---

## Finding Coordinates

Use the DayZ Workbench or connect to your server and run:
```
Print(GetGame().GetPlayer().GetPosition().ToString());
```
Copy the X and Z values into your config. Y is auto-snapped to ground level.

---

## Item / Entity Class Names

- **Items:** Use exact DayZ classnames e.g. `Bandage`, `AKM`, `Mosin9130`, `GoldBar`
- **Entities:** Use partial or exact classnames e.g. `Wolf`, `RoeDeer`, `Boar`, `ZmbM_PolicemanFat`
  - The kill tracker uses `String.Contains()` so `"Wolf"` will match `"Wolf"` and `"WolfBlack"`

---

## Adding More Quest Types (Modder Notes)

The `QT_QuestType` enum in `QuestData.c` can be extended:
```
enum QT_QuestType { COLLECT=0, KILL=1, DELIVER=2, REACH=3 }
```
Add handler logic in `QuestManager.c` and update `QT_RPCManager.c` for the new turn-in path.

---

## Compatibility

- Tested against DayZ 1.28+
- Compatible with **DrJones Trader**, **COT**, and other popular mods
  (no class conflicts – only modded classes are `MissionServer`, `MissionGameplay`, `PlayerBase`)
- Works on both vanilla and modded maps

---

## License

MIT – free to use, modify, and redistribute with attribution. Do whatever the fuck you want with it.
Just Credit the mod author, Taco Donkey, and link to the original mod. Please and Thank you.
