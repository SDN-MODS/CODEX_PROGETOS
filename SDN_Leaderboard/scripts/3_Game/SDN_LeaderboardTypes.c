class SDN_LeaderboardConstants
{
    // Private IDs checked against vanilla 1.29; third-party collisions still need testing.
    static const int RPC_QUERY = 7342101;
    static const int RPC_RESULT = 7342102;
    static const int MENU_ID = 7342103;
    static const int PROTOCOL = 1;
    static const int SCHEMA = 1;
    static const int MAX_PLAYERS = 2000;
    static const int MAX_PAIRS = 20000;
    static const int MAX_EVENTS = 500;
    static const string ROOT = "$profile:SDN_MODS/SDN_Leaderboard";
}

class SDN_LeaderboardConfig
{
    int SchemaVersion = 1;
    bool Enabled = true;
    int KillPoints = 100;
    int PairWindowSeconds = 1800;
    int SpawnProtectionSeconds = 120;
    int PageSize = 25;
    int QueryIntervalSeconds = 5;
    string RankingLabel = "Ranking experimental — sem temporada";

    bool IsValid()
    {
        return SchemaVersion == 1 && KillPoints >= 1 && KillPoints <= 10000 && PairWindowSeconds >= 1 && PairWindowSeconds <= 86400 && SpawnProtectionSeconds >= 0 && SpawnProtectionSeconds <= 600 && PageSize >= 1 && PageSize <= 25 && QueryIntervalSeconds >= 1 && QueryIntervalSeconds <= 60 && RankingLabel.Length() <= 100;
    }
}

class SDN_LeaderboardPlayerData
{
    string Id;
    string Name;
    int Points;
    int ValidKills;
    int InvalidKills;
    int PvpDeaths;
    int UncreditedDeaths;
    int Streak;
    int PointsReachedAt;
    int SpawnAt;
    bool Offensive;
    ref array<string> Victims = new array<string>;
}

class SDN_LeaderboardPair
{
    string A;
    string B;
    int CreditedAt;

    bool Matches(string first, string second)
    {
        return (A == first && B == second) || (A == second && B == first);
    }
}

class SDN_LeaderboardEvent
{
    int Sequence;
    int At;
    string Victim;
    string Attacker;
    string Reason;
    int Points;
}

class SDN_LeaderboardState
{
    int SchemaVersion = -1;
    int Generation = -1;
    int LastClock = -1;
    int LastEvent = -1;
    int PlayerCount = -1;
    int PairCount = -1;
    int EventCount = -1;
    ref array<ref SDN_LeaderboardPlayerData> Players = new array<ref SDN_LeaderboardPlayerData>;
    ref array<ref SDN_LeaderboardPair> Pairs = new array<ref SDN_LeaderboardPair>;
    ref array<ref SDN_LeaderboardEvent> Events = new array<ref SDN_LeaderboardEvent>;
}

class SDN_LeaderboardRow
{
    int Rank;
    string Name;
    int Points;
    int Kills;
    int Deaths;
    bool Own;
}

class SDN_LeaderboardPage
{
    int Request;
    bool Success;
    string Message;
    string Label;
    int Page;
    int Pages;
    int OwnRank;
    int OwnPoints;
    int OwnInvalidKills;
    int RefreshSeconds = 5;
    ref array<ref SDN_LeaderboardRow> Rows = new array<ref SDN_LeaderboardRow>;
}

class SDN_LeaderboardClient
{
    static ref SDN_LeaderboardPage LastPage;
    static int Revision;
    static int NextRequest;
}

class SDN_LeaderboardText
{
    static string Clean(string value, int maxLength = 48)
    {
        value.Replace("\n", " ");
        value.Replace("\r", " ");
        value.Replace("\t", " ");
        value.Replace("<", "(");
        value.Replace(">", ")");
        if (value.LengthUtf8() > maxLength)
            value = value.SubstringUtf8(0, maxLength);
        return value;
    }

    static bool Before(string a, string b)
    {
        int count = Math.Min(a.Length(), b.Length());
        for (int i = 0; i < count; i++)
        {
            int left = a.Get(i).ToAscii();
            int right = b.Get(i).ToAscii();
            if (left != right)
                return left < right;
        }
        return a.Length() < b.Length();
    }
}
