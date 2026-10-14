class SDN_LeaderboardPersistence
{
    string Error;

    bool PrepareDirectories()
    {
        array<string> paths = {"$profile:SDN_MODS", SDN_LeaderboardConstants.ROOT, SDN_LeaderboardConstants.ROOT + "/Config", SDN_LeaderboardConstants.ROOT + "/State"};
        foreach (string path : paths)
        {
            if (!FileExist(path) && !MakeDirectory(path))
            {
                Error = "Nao foi possivel criar " + path;
                return false;
            }
        }
        return true;
    }

    bool LoadConfig(out SDN_LeaderboardConfig config)
    {
        string path = SDN_LeaderboardConstants.ROOT + "/Config/SDN_General.json";
        config = new SDN_LeaderboardConfig;
        if (FileExist(path))
        {
            if (!JsonFileLoader<SDN_LeaderboardConfig>.LoadFile(path, config, Error)) return false;
        }
        else if (!JsonFileLoader<SDN_LeaderboardConfig>.SaveFile(path, config, Error)) return false;
        if (!config || !config.IsValid())
        {
            Error = "Configuracao invalida; arquivo preservado. Corrija com o servidor parado.";
            return false;
        }
        return true;
    }

    string Slot(int index)
    {
        return SDN_LeaderboardConstants.ROOT + "/State/SDN_State_" + index.ToString() + ".json";
    }

    bool Valid(SDN_LeaderboardState state)
    {
        if (!state || state.SchemaVersion != SDN_LeaderboardConstants.SCHEMA || state.Generation < 0 || state.LastClock < 0 || state.LastEvent < 0) return false;
        if (!state.Players || !state.Pairs || !state.Events) return false;
        if (state.PlayerCount != state.Players.Count() || state.PairCount != state.Pairs.Count() || state.EventCount != state.Events.Count()) return false;
        if (state.Players.Count() > SDN_LeaderboardConstants.MAX_PLAYERS || state.Pairs.Count() > SDN_LeaderboardConstants.MAX_PAIRS || state.Events.Count() > SDN_LeaderboardConstants.MAX_EVENTS) return false;
        map<string, bool> ids = new map<string, bool>;
        foreach (SDN_LeaderboardPlayerData player : state.Players)
        {
            if (!player || player.Id == "" || player.Id.Length() > 128 || ids.Contains(player.Id) || !player.Victims) return false;
            if (player.Points < 0 || player.ValidKills < 0 || player.InvalidKills < 0 || player.PvpDeaths < 0 || player.UncreditedDeaths < 0 || player.Streak < 0 || player.SpawnAt < 0 || player.PointsReachedAt < 0) return false;
            if (player.Name.LengthUtf8() > 48 || player.Victims.Count() > SDN_LeaderboardConstants.MAX_PLAYERS) return false;
            ids.Insert(player.Id, true);
        }
        foreach (SDN_LeaderboardPlayerData record : state.Players)
        {
            map<string, bool> victims = new map<string, bool>;
            foreach (string victim : record.Victims)
            {
                if (!ids.Contains(victim) || victim == record.Id || victims.Contains(victim)) return false;
                victims.Insert(victim, true);
            }
        }
        map<string, bool> pairs = new map<string, bool>;
        foreach (SDN_LeaderboardPair pair : state.Pairs)
        {
            if (!pair || pair.A == pair.B || !ids.Contains(pair.A) || !ids.Contains(pair.B) || pair.CreditedAt < 0 || pair.CreditedAt > state.LastClock) return false;
            string key = pair.A + ":" + pair.B;
            string reverse = pair.B + ":" + pair.A;
            if (pairs.Contains(key) || pairs.Contains(reverse)) return false;
            pairs.Insert(key, true);
        }
        int previous;
        foreach (SDN_LeaderboardEvent evt : state.Events)
        {
            if (!evt || evt.Sequence <= previous || evt.Sequence > state.LastEvent || evt.At < 0 || evt.At > state.LastClock || evt.Points < 0 || !ids.Contains(evt.Victim)) return false;
            if (evt.Attacker != "" && !ids.Contains(evt.Attacker)) return false;
            previous = evt.Sequence;
        }
        return true;
    }

    bool Load(out SDN_LeaderboardState state)
    {
        bool found;
        for (int i = 0; i < 2; i++)
        {
            if (!FileExist(Slot(i))) continue;
            SDN_LeaderboardState candidate;
            if (!JsonFileLoader<SDN_LeaderboardState>.LoadFile(Slot(i), candidate, Error) || !Valid(candidate) || candidate.Generation < 1)
            {
                Error = "Snapshot invalido. Pontuacao suspensa; preserve os dois slots antes de recuperar.";
                return false; // Never silently roll back to an older snapshot.
            }
            if (!found || candidate.Generation > state.Generation) state = candidate;
            found = true;
        }
        if (!found)
        {
            if (FileExist(SDN_LeaderboardConstants.ROOT + "/State/SDN_Initialized.txt"))
            {
                Error = "Snapshots ausentes em instalacao ja inicializada.";
                return false;
            }
            state = new SDN_LeaderboardState;
            state.SchemaVersion = SDN_LeaderboardConstants.SCHEMA;
            state.Generation = 0;
            state.LastClock = 0;
            state.LastEvent = 0;
            if (!Save(state)) return false;
        }
        string marker = SDN_LeaderboardConstants.ROOT + "/State/SDN_Initialized.txt";
        if (!FileExist(marker))
        {
            FileHandle handle = OpenFile(marker, FileMode.WRITE);
            if (!handle)
            {
                Error = "Nao foi possivel criar marcador de inicializacao.";
                return false;
            }
            FPrint(handle, "SDN Leaderboard schema 1\n");
            CloseFile(handle);
        }
        return true;
    }

    bool Save(SDN_LeaderboardState state)
    {
        state.PlayerCount = state.Players.Count();
        state.PairCount = state.Pairs.Count();
        state.EventCount = state.Events.Count();
        if (!Valid(state) || state.Generation >= 2000000000)
        {
            Error = "Estado invalido ou limite de geracoes atingido.";
            return false;
        }
        state.Generation++;
        string path = Slot(state.Generation % 2);
        if (!JsonFileLoader<SDN_LeaderboardState>.SaveFile(path, state, Error)) return false;
        SDN_LeaderboardState readback;
        if (!JsonFileLoader<SDN_LeaderboardState>.LoadFile(path, readback, Error) || !Valid(readback)) return false;
        string expected, actual;
        if (!JsonFileLoader<SDN_LeaderboardState>.MakeData(state, expected, Error, false)) return false;
        if (!JsonFileLoader<SDN_LeaderboardState>.MakeData(readback, actual, Error, false)) return false;
        if (expected != actual)
        {
            Error = "Leitura apos gravacao divergiu do estado esperado.";
            return false;
        }
        return true;
    }
}
