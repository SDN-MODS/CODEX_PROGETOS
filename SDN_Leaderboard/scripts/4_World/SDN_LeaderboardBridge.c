// The lower module knows only this interface. Implementation lives in 5_Mission.
class SDN_LeaderboardBridge
{
    static ref SDN_LeaderboardBridge Active;
    void Hit(PlayerBase victim, PlayerBase attacker) {}
    void Death(PlayerBase victim, PlayerBase attacker) {}
    void Query(PlayerBase target, PlayerIdentity sender, ParamsReadContext ctx) {}
}

modded class PlayerBase
{
    protected bool m_SDN_DeathProcessed;

    static PlayerBase SDN_DirectPlayer(Object source)
    {
        PlayerBase direct = PlayerBase.Cast(source);
        if (direct) return direct;
        EntityAI entity = EntityAI.Cast(source);
        // Attached/held objects only. No inferred ownership for traps, cars or explosives.
        if (entity && entity.GetHierarchyParent())
            return PlayerBase.Cast(entity.GetHierarchyRootPlayer());
        return null;
    }

    override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
    {
        // Observe before super: the original handler must always execute.
        if (GetGame().IsServer() && SDN_LeaderboardBridge.Active && damageResult)
        {
            if (damageResult.GetDamage(dmgZone, "Health") > 0 || damageResult.GetDamage(dmgZone, "Blood") > 0 || damageResult.GetDamage(dmgZone, "Shock") > 0)
                SDN_LeaderboardBridge.Active.Hit(this, SDN_DirectPlayer(source));
        }
        super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
    }

    override void EEKilled(Object killer)
    {
        if (GetGame().IsServer() && !m_SDN_DeathProcessed)
        {
            m_SDN_DeathProcessed = true;
            if (SDN_LeaderboardBridge.Active)
                SDN_LeaderboardBridge.Active.Death(this, SDN_DirectPlayer(killer));
        }
        super.EEKilled(killer);
    }

    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        if (rpc_type == SDN_LeaderboardConstants.RPC_QUERY)
        {
            if (GetGame().IsServer() && SDN_LeaderboardBridge.Active)
                SDN_LeaderboardBridge.Active.Query(this, sender, ctx);
            return;
        }
        if (rpc_type == SDN_LeaderboardConstants.RPC_RESULT)
        {
            if (!GetGame().IsServer() && this == GetGame().GetPlayer())
                SDN_ReadPage(ctx);
            return;
        }
        super.OnRPC(sender, rpc_type, ctx);
    }

    void SDN_ReadPage(ParamsReadContext ctx)
    {
        int protocol;
        SDN_LeaderboardPage page = new SDN_LeaderboardPage;
        if (!ctx.Read(protocol) || protocol != SDN_LeaderboardConstants.PROTOCOL) return;
        if (!ctx.Read(page.Request) || !ctx.Read(page.Success) || !ctx.Read(page.Message)) return;
        if (!ctx.Read(page.Label) || !ctx.Read(page.Page) || !ctx.Read(page.Pages)) return;
        if (!ctx.Read(page.OwnRank) || !ctx.Read(page.OwnPoints) || !ctx.Read(page.OwnInvalidKills)) return;
        if (!ctx.Read(page.RefreshSeconds) || page.RefreshSeconds < 1 || page.RefreshSeconds > 60) return;
        int count;
        if (!ctx.Read(count) || count < 0 || count > 25) return;
        for (int i = 0; i < count; i++)
        {
            SDN_LeaderboardRow row = new SDN_LeaderboardRow;
            if (!ctx.Read(row.Rank) || !ctx.Read(row.Name) || !ctx.Read(row.Points)) return;
            if (!ctx.Read(row.Kills) || !ctx.Read(row.Deaths) || !ctx.Read(row.Own)) return;
            page.Rows.Insert(row);
        }
        SDN_LeaderboardClient.LastPage = page;
        SDN_LeaderboardClient.Revision++;
    }
}
