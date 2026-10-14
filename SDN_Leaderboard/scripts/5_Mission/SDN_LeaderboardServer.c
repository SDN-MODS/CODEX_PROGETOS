class SDN_LeaderboardServer : SDN_LeaderboardBridge
{
    protected ref SDN_LeaderboardPersistence m_Store = new SDN_LeaderboardPersistence;
    protected ref SDN_LeaderboardConfig m_Config;
    protected ref SDN_LeaderboardState m_State;
    protected ref map<string, SDN_LeaderboardPlayerData> m_Players = new map<string, SDN_LeaderboardPlayerData>;
    protected ref map<string, int> m_NextQuery = new map<string, int>;
    protected ref array<SDN_LeaderboardPlayerData> m_Ranking = new array<SDN_LeaderboardPlayerData>;
    protected bool m_Ready;
    protected bool m_RankingDirty = true;

    void SDN_LeaderboardServer()
    {
        if (!m_Store.PrepareDirectories() || !m_Store.LoadConfig(m_Config) || !m_Store.Load(m_State))
        {
            Stop(m_Store.Error);
            return;
        }
        foreach (SDN_LeaderboardPlayerData player : m_State.Players) m_Players.Insert(player.Id, player);
        m_Ready = m_Config.Enabled;
        if (m_Ready) Print("[SDN_Leaderboard] Alpha 0.1.0 iniciado; ranking sem temporadas e sem premios.");
    }

    void Stop(string reason)
    {
        m_Ready = false;
        Print("[SDN_Leaderboard] Pontuacao indisponivel: " + reason);
    }

    protected int Time()
    {
        int now = SDN_LeaderboardClock.Now();
        if (now < 0) return -1;
        if (now < m_State.LastClock)
        {
            Stop("Relogio UTC retrocedeu. Corrija antes de reiniciar o mod.");
            return -1;
        }
        m_State.LastClock = now;
        return now;
    }

    protected bool Commit()
    {
        m_RankingDirty = true;
        if (m_Store.Save(m_State)) return true;
        Stop(m_Store.Error);
        return false;
    }

    protected SDN_LeaderboardPlayerData Ensure(PlayerIdentity identity)
    {
        if (!identity || identity.GetId() == "") return null;
        string id = identity.GetId();
        SDN_LeaderboardPlayerData result;
        if (!m_Players.Find(id, result))
        {
            if (m_State.Players.Count() >= SDN_LeaderboardConstants.MAX_PLAYERS)
            {
                Stop("Limite de contas do alpha atingido.");
                return null;
            }
            result = new SDN_LeaderboardPlayerData;
            result.Id = id;
            m_State.Players.Insert(result);
            m_Players.Insert(id, result);
        }
        result.Name = SDN_LeaderboardText.Clean(identity.GetName());
        return result;
    }

    void Connected(PlayerIdentity identity, bool newborn)
    {
        if (!m_Ready) return;
        int now = Time();
        if (now < 0) return;
        SDN_LeaderboardPlayerData player = Ensure(identity);
        if (!player) return;
        if (newborn)
        {
            player.SpawnAt = now;
            player.Offensive = false;
            player.Streak = 0;
        }
        Commit();
    }

    override void Hit(PlayerBase victim, PlayerBase attacker)
    {
        if (!m_Ready || !attacker || attacker == victim) return;
        SDN_LeaderboardPlayerData player = Ensure(attacker.GetIdentity());
        if (!player || player.Offensive) return;
        if (Time() < 0) return;
        player.Offensive = true;
        Commit(); // Persist once per life, not once per hit.
    }

    protected void PrunePairs(int now)
    {
        // Keep 24h even if configured window is shorter, so changing settings
        // cannot resurrect an already forgotten pair during the supported window.
        for (int i = m_State.Pairs.Count() - 1; i >= 0; i--)
        {
            if (now - m_State.Pairs[i].CreditedAt >= 86400) m_State.Pairs.Remove(i);
        }
    }

    protected SDN_LeaderboardPair FindPair(string a, string b)
    {
        foreach (SDN_LeaderboardPair pair : m_State.Pairs)
        {
            if (pair.Matches(a, b)) return pair;
        }
        return null;
    }

    override void Death(PlayerBase victim, PlayerBase attacker)
    {
        if (!m_Ready || !victim) return;
        int now = Time();
        if (now < 0) return;
        SDN_LeaderboardPlayerData dead = Ensure(victim.GetIdentity());
        if (!dead) return;
        SDN_LeaderboardPlayerData killer;
        if (attacker && attacker != victim) killer = Ensure(attacker.GetIdentity());
        if (!m_Ready) return;
        if (m_State.LastEvent >= 2000000000)
        {
            Stop("Limite de eventos atingido.");
            return;
        }
        PrunePairs(now);
        SDN_LeaderboardEvent evt = new SDN_LeaderboardEvent;
        m_State.LastEvent++;
        evt.Sequence = m_State.LastEvent;
        evt.At = now;
        evt.Victim = dead.Id;
        dead.Streak = 0;
        evt.Reason = "sem_autoria_direta_confirmada";
        if (!killer || killer.Id == dead.Id)
        {
            dead.UncreditedDeaths++;
            if (attacker == victim) evt.Reason = "origem_propria_sem_credito";
        }
        else
        {
            evt.Attacker = killer.Id;
            dead.PvpDeaths++;
            SDN_LeaderboardPair pair = FindPair(killer.Id, dead.Id);
            bool spawn = dead.SpawnAt > 0 && now - dead.SpawnAt < m_Config.SpawnProtectionSeconds && !dead.Offensive;
            bool repeated = pair && now - pair.CreditedAt < m_Config.PairWindowSeconds;
            if (spawn || repeated)
            {
                killer.InvalidKills++;
                evt.Reason = "";
                if (spawn) evt.Reason = "protecao_nascimento";
                if (repeated) evt.Reason += "|dupla_repetida";
            }
            else
            {
                if ((!pair && m_State.Pairs.Count() >= SDN_LeaderboardConstants.MAX_PAIRS) || killer.Points > 2000000000 - m_Config.KillPoints)
                {
                    Stop("Limite de estado competitivo atingido; pontuacao suspensa.");
                    return;
                }
                if (!pair)
                {
                    pair = new SDN_LeaderboardPair;
                    pair.A = killer.Id;
                    pair.B = dead.Id;
                    m_State.Pairs.Insert(pair);
                }
                pair.CreditedAt = now;
                killer.Points += m_Config.KillPoints;
                killer.ValidKills++;
                killer.Streak++;
                killer.PointsReachedAt = now;
                if (killer.Victims.Find(dead.Id) == -1) killer.Victims.Insert(dead.Id);
                evt.Points = m_Config.KillPoints;
                evt.Reason = "kill_direta_valida";
            }
        }
        m_State.Events.Insert(evt);
        if (m_State.Events.Count() > SDN_LeaderboardConstants.MAX_EVENTS) m_State.Events.Remove(0);
        Commit(); // Stats, event and pair guard are one generation.
    }

    protected bool Before(SDN_LeaderboardPlayerData a, SDN_LeaderboardPlayerData b)
    {
        if (a.Points != b.Points) return a.Points > b.Points;
        if (a.Victims.Count() != b.Victims.Count()) return a.Victims.Count() > b.Victims.Count();
        if (a.PointsReachedAt != b.PointsReachedAt) return a.PointsReachedAt < b.PointsReachedAt;
        return SDN_LeaderboardText.Before(a.Id, b.Id);
    }

    protected void BuildRanking()
    {
        if (!m_RankingDirty) return;
        m_Ranking.Clear();
        foreach (SDN_LeaderboardPlayerData player : m_State.Players) m_Ranking.Insert(player);
        // Shell sort; no per-frame ranking work and no dependency on a custom comparator API.
        for (int gap = m_Ranking.Count() / 2; gap > 0; gap = gap / 2)
        {
            for (int i = gap; i < m_Ranking.Count(); i++)
            {
                SDN_LeaderboardPlayerData item = m_Ranking[i];
                int j = i;
                while (j >= gap && Before(item, m_Ranking[j - gap]))
                {
                    m_Ranking[j] = m_Ranking[j - gap];
                    j -= gap;
                }
                m_Ranking[j] = item;
            }
        }
        m_RankingDirty = false;
    }

    override void Query(PlayerBase target, PlayerIdentity sender, ParamsReadContext ctx)
    {
        // Authenticate actual transport sender before parsing the request.
        if (!sender || !target || !target.GetIdentity() || target.GetIdentity().GetId() != sender.GetId() || sender.GetPlayer() != target) return;
        string id = sender.GetId();
        int now = SDN_LeaderboardClock.Now();
        if (now < 0) return;
        int next;
        if (m_NextQuery.Find(id, next) && now < next) return;
        if (!m_NextQuery.Contains(id) && m_NextQuery.Count() >= SDN_LeaderboardConstants.MAX_PLAYERS) return;
        m_NextQuery.Set(id, now + 5);
        if (m_Config) m_NextQuery.Set(id, now + m_Config.QueryIntervalSeconds);
        int protocol, request, page;
        string search;
        if (!ctx.Read(protocol) || protocol != SDN_LeaderboardConstants.PROTOCOL || !ctx.Read(request) || !ctx.Read(page) || !ctx.Read(search)) return;
        if (page < 0 || page > SDN_LeaderboardConstants.MAX_PLAYERS || search.Length() > 64) return;
        SDN_LeaderboardPage result = new SDN_LeaderboardPage;
        result.Request = request;
        if (m_Config) result.RefreshSeconds = m_Config.QueryIntervalSeconds;
        result.Message = "Ranking indisponivel. Consulte o administrador.";
        if (m_Ready)
        {
            BuildRanking();
            result.Success = true;
            result.Label = m_Config.RankingLabel;
            result.Message = "Atualizado. K/D = kills validas / mortes PvP; sem mortes: —";
            array<int> matches = new array<int>;
            search.ToLower();
            for (int i = 0; i < m_Ranking.Count(); i++)
            {
                SDN_LeaderboardPlayerData player = m_Ranking[i];
                if (player.Id == id)
                {
                    result.OwnRank = i + 1;
                    result.OwnPoints = player.Points;
                    result.OwnInvalidKills = player.InvalidKills;
                }
                string name = player.Name;
                name.ToLower();
                if (search == "" || name.Contains(search)) matches.Insert(i);
            }
            result.Pages = Math.Max(1, (matches.Count() + m_Config.PageSize - 1) / m_Config.PageSize);
            result.Page = Math.Min(page, result.Pages - 1);
            int end = Math.Min(matches.Count(), (result.Page + 1) * m_Config.PageSize);
            for (int index = result.Page * m_Config.PageSize; index < end; index++)
            {
                int position = matches[index];
                SDN_LeaderboardPlayerData ranked = m_Ranking[position];
                SDN_LeaderboardRow row = new SDN_LeaderboardRow;
                row.Rank = position + 1;
                row.Name = ranked.Name;
                row.Points = ranked.Points;
                row.Kills = ranked.ValidKills;
                row.Deaths = ranked.PvpDeaths;
                row.Own = ranked.Id == id;
                result.Rows.Insert(row);
            }
        }
        Send(target, sender, result);
    }

    protected void Send(PlayerBase target, PlayerIdentity recipient, SDN_LeaderboardPage page)
    {
        ScriptRPC rpc = new ScriptRPC;
        rpc.Write(SDN_LeaderboardConstants.PROTOCOL);
        rpc.Write(page.Request);
        rpc.Write(page.Success);
        rpc.Write(page.Message);
        rpc.Write(page.Label);
        rpc.Write(page.Page);
        rpc.Write(page.Pages);
        rpc.Write(page.OwnRank);
        rpc.Write(page.OwnPoints);
        rpc.Write(page.OwnInvalidKills);
        rpc.Write(page.RefreshSeconds);
        rpc.Write(page.Rows.Count());
        foreach (SDN_LeaderboardRow row : page.Rows)
        {
            rpc.Write(row.Rank);
            rpc.Write(row.Name);
            rpc.Write(row.Points);
            rpc.Write(row.Kills);
            rpc.Write(row.Deaths);
            rpc.Write(row.Own);
        }
        rpc.Send(target, SDN_LeaderboardConstants.RPC_RESULT, true, recipient);
    }
}
