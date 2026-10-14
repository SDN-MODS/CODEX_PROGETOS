modded class MissionServer
{
    protected ref SDN_LeaderboardServer m_SDN_Server;

    override void OnInit()
    {
        super.OnInit();
        m_SDN_Server = new SDN_LeaderboardServer;
        SDN_LeaderboardBridge.Active = m_SDN_Server;
    }

    override PlayerBase OnClientNewEvent(PlayerIdentity identity, vector pos, ParamsReadContext ctx)
    {
        PlayerBase player = super.OnClientNewEvent(identity, pos, ctx);
        if (player && m_SDN_Server) m_SDN_Server.Connected(identity, true);
        return player;
    }

    override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
    {
        super.InvokeOnConnect(player, identity);
        if (m_SDN_Server) m_SDN_Server.Connected(identity, false);
    }

    override void OnMissionFinish()
    {
        SDN_LeaderboardBridge.Active = null;
        m_SDN_Server = null;
        super.OnMissionFinish();
    }
}

modded class MissionBase
{
    override UIScriptedMenu CreateScriptedMenu(int id)
    {
        if (id == SDN_LeaderboardConstants.MENU_ID)
        {
            SDN_LeaderboardMenu menu = new SDN_LeaderboardMenu;
            menu.SetID(id);
            return menu;
        }
        return super.CreateScriptedMenu(id);
    }
}

modded class MissionGameplay
{
    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (!player || !player.GetIdentity() || !player.IsAlive() || player.IsUnconscious() || IsPaused()) return;
        if (GetGame().GetUIManager().GetMenu() || IsControlDisabled()) return;
        UAInput input = GetUApi().GetInputByName("UASDNLeaderboard");
        if (input && input.LocalPress())
            GetGame().GetUIManager().EnterScriptedMenu(SDN_LeaderboardConstants.MENU_ID, null);
    }

    override void OnMissionFinish()
    {
        SDN_LeaderboardMenu menu = SDN_LeaderboardMenu.Cast(GetGame().GetUIManager().GetMenu());
        if (menu) menu.Close();
        SDN_LeaderboardClient.LastPage = null;
        super.OnMissionFinish();
    }
}
