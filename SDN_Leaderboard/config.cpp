class CfgPatches
{
    class SDN_Leaderboard
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
    };
};

class CfgMods
{
    class SDN_Leaderboard
    {
        dir = "SDN_Leaderboard";
        name = "SDN Leaderboard — Alpha";
        type = "mod";
        dependencies[] = {"Game", "World", "Mission"};
        inputs = "SDN_Leaderboard/data/SDN_LeaderboardInputs.xml";
        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"SDN_Leaderboard/scripts/3_Game"};
            };
            class worldScriptModule
            {
                value = "";
                files[] = {"SDN_Leaderboard/scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"SDN_Leaderboard/scripts/5_Mission"};
            };
        };
    };
};
