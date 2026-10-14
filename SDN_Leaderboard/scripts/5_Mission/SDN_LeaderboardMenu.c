class SDN_LeaderboardMenu : UIScriptedMenu
{
    protected TextListboxWidget m_List;
    protected TextWidget m_Status;
    protected TextWidget m_Title;
    protected TextWidget m_Profile;
    protected EditBoxWidget m_Search;
    protected int m_Page;
    protected int m_Pages = 1;
    protected int m_Request;
    protected int m_Revision;
    protected float m_Elapsed;
    protected float m_SinceRequest = 60;
    protected int m_RefreshSeconds = 5;
    protected bool m_Waiting;
    protected bool m_ValidLayout;

    override Widget Init()
    {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("SDN_Leaderboard/gui/layouts/SDN_Leaderboard.layout");
        if (!layoutRoot) return null;
        m_List = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("SDN_RankingList"));
        m_Status = TextWidget.Cast(layoutRoot.FindAnyWidget("SDN_Status"));
        m_Title = TextWidget.Cast(layoutRoot.FindAnyWidget("SDN_Title"));
        m_Profile = TextWidget.Cast(layoutRoot.FindAnyWidget("SDN_Profile"));
        m_Search = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("SDN_Search"));
        m_ValidLayout = m_List && m_Status && m_Title && m_Profile && m_Search;
        return layoutRoot;
    }

    override void OnShow()
    {
        super.OnShow();
        if (!m_ValidLayout)
        {
            Print("[SDN_Leaderboard] Layout incompleto ou incompativel.");
            Close();
            return;
        }
        GetGame().GetMission().AddActiveInputExcludes({"menu"});
        m_Revision = SDN_LeaderboardClient.Revision;
        Request(0);
    }

    override void OnHide()
    {
        if (m_ValidLayout && GetGame().GetMission())
            GetGame().GetMission().RemoveActiveInputExcludes({"menu"}, true);
        super.OnHide();
    }

    protected void Request(int page)
    {
        if (m_Waiting) return;
        if (m_SinceRequest < m_RefreshSeconds)
        {
            m_Status.SetText("Aguarde o intervalo de consulta: " + m_RefreshSeconds.ToString() + " segundos.");
            return;
        }
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (!player || !player.IsAlive()) return;
        string search = m_Search.GetText();
        if (search.Length() > 64)
        {
            m_Status.SetText("Pesquisa muito longa: use ate 64 bytes de texto.");
            return;
        }
        SDN_LeaderboardClient.NextRequest++;
        m_Request = SDN_LeaderboardClient.NextRequest;
        m_Elapsed = 0;
        m_SinceRequest = 0;
        m_Waiting = true;
        m_Status.SetText("Consultando servidor... dados anteriores podem estar desatualizados.");
        ScriptRPC rpc = new ScriptRPC;
        rpc.Write(SDN_LeaderboardConstants.PROTOCOL);
        rpc.Write(m_Request);
        rpc.Write(page);
        rpc.Write(search);
        rpc.Send(player, SDN_LeaderboardConstants.RPC_QUERY, true);
    }

    override void Update(float timeslice)
    {
        super.Update(timeslice);
        if (!m_ValidLayout) return;
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (!player || !player.GetIdentity() || !player.IsAlive() || player.IsUnconscious())
        {
            Close();
            return;
        }
        m_SinceRequest += timeslice;
        m_Elapsed += timeslice;
        if (m_Revision != SDN_LeaderboardClient.Revision)
        {
            m_Revision = SDN_LeaderboardClient.Revision;
            SDN_LeaderboardPage response = SDN_LeaderboardClient.LastPage;
            if (response && response.Request == m_Request)
            {
                m_Waiting = false;
                m_RefreshSeconds = response.RefreshSeconds;
                Render(response);
            }
        }
        if (m_Waiting && m_Elapsed >= 10)
        {
            m_Waiting = false;
            m_Status.SetText("Sem resposta. Dados anteriores desatualizados; tente Atualizar apos aguardar.");
        }
    }

    protected void Render(SDN_LeaderboardPage response)
    {
        m_Status.SetText(response.Message);
        if (!response.Success)
        {
            m_List.ClearItems();
            m_Profile.SetText("Perfil indisponivel.");
            return;
        }
        m_Page = response.Page;
        m_Pages = response.Pages;
        m_Title.SetText(response.Label + " | pagina " + (m_Page + 1).ToString() + "/" + m_Pages.ToString());
        m_Profile.SetText("Sua posicao: " + response.OwnRank.ToString() + " | Pontos: " + response.OwnPoints.ToString() + " | Kills invalidadas: " + response.OwnInvalidKills.ToString());
        m_List.ClearItems();
        foreach (SDN_LeaderboardRow row : response.Rows)
        {
            string kd = "—";
            if (row.Deaths > 0)
            {
                float ratio = row.Kills;
                ratio = Math.Round(ratio / row.Deaths * 100) / 100;
                kd = ratio.ToString();
            }
            string marker = "";
            if (row.Own) marker = "> ";
            string line = marker + row.Rank.ToString() + ". " + SDN_LeaderboardText.Clean(row.Name) + " | " + row.Points.ToString() + " pts | " + row.Kills.ToString() + " kills | " + row.Deaths.ToString() + " mortes | K/D " + kd;
            m_List.AddItem(line, null, 0);
        }
        if (response.Rows.Count() == 0) m_Status.SetText("Nenhum jogador encontrado para esta consulta.");
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w.GetName() == "SDN_Close")
        {
            Close();
            return true;
        }
        if (w.GetName() == "SDN_Refresh") { Request(0); return true; }
        if (w.GetName() == "SDN_PreviousPage") { Request(Math.Max(0, m_Page - 1)); return true; }
        if (w.GetName() == "SDN_NextPage") { Request(Math.Min(m_Pages - 1, m_Page + 1)); return true; }
        return super.OnClick(w, x, y, button);
    }
}
