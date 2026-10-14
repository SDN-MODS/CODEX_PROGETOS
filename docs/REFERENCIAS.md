# Referências e evidências de API

Consulta realizada em 13/10/2026, horário America/Sao_Paulo. A consulta ao repositório oficial funcionou; Steam, dayz.com e fóruns tiveram acesso bloqueado pela política de rede. Não afirmar que o HEAD de scripts equivale necessariamente ao último hotfix distribuído na Steam.

Fonte principal: [BohemiaInteractive/DayZ-Script-Diff, commit 86974a0](https://github.com/BohemiaInteractive/DayZ-Script-Diff/tree/86974a0f5bd16b1ee3e334ad828133c93dca80a1), mensagem `Build 1.29.163709, Scripts Rev. 125372`, de 12/08/2026.

| API conferida | Arquivo oficial | Uso neste alpha |
| --- | --- | --- |
| `PlayerBase.EEHitBy(...)`, `EEKilled(Object)` e `OnRPC(...)` | `scripts/4_world/entities/manbase/playerbase.c` | Observação de combate e mensagens; preserva `super` |
| `MissionServer.OnClientNewEvent(...)`, `InvokeOnConnect(...)` | `scripts/5_mission/mission/missionserver.c` | Distinguir nascimento de reconexão |
| `PlayerIdentity.GetId()`, `GetPlayer()` | `scripts/3_game/gameplay.c` | Identidade persistente e vínculo do remetente |
| `ScriptRPC.Send(...)` | `scripts/3_game/gameplay.c` | Resposta direcionada ao solicitante |
| `JsonFileLoader<T>.LoadFile/SaveFile/MakeData` | `scripts/3_game/tools/jsonfileloader.c` | Gravação com retorno de erro e leitura de conferência |
| `GetYearMonthDayUTC`, `GetHourMinuteSecondUTC`, `MakeDirectory` | `scripts/1_core/proto/ensystem.c` | Relógio e diretórios |
| `GetHierarchyRootPlayer()` | `scripts/3_game/entities/entityai.c` | Resolução restrita a hierarquia presente |
| `UAInputAPI.GetInputByName()`, `UAInput.LocalPress()` | `scripts/3_game/inputapi/uainput.c` | Ação remapeável |
| `UIScriptedMenu.OnShow/OnHide/OnPlayerDeath` | `scripts/3_game/tools/uiscriptedmenu.c` | Foco/cursor e fechamento por morte |
| `TextListboxWidget.AddItem`, `ClearItems` | `scripts/1_core/proto/enwidgets.c` | Linhas públicas de ranking |

Manifesto e XML de entrada conferidos com [DayZ-Samples/Test_Inputs](https://github.com/BohemiaInteractive/DayZ-Samples/tree/da5e5437c9502620d9853fb6eed14701135ab2ea/Test_Inputs).

Os identificadores 7342101–7342103 não apareceram nos scripts vanilla e exemplos consultados. Isso não prova ausência de colisão com outros mods; conferir a lista real de addons.

O formato do layout foi proposto, mas **não validado com recurso vanilla extraído ou editor nativo**. A existência de APIs e assinaturas não prova ordem de eventos, compatibilidade de tipos em todas as chamadas ou compilação do mod. Essas verificações exigem o motor.

O material oficial de referência está sujeito à [DayZ Public License](https://www.bohemia.net/community/licenses/dayz-public-license-dpl). Os dumps e assets oficiais não foram copiados para este repositório; consulte a licença antes de redistribuí-los. A especificação anexada foi preservada em `docs/` como referência do projeto.
