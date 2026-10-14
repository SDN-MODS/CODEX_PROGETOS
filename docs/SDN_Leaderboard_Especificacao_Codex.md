# SDN_Leaderboard — Especificação para desenvolvimento no Codex

**Projeto:** ESTUDO ENFORCE  
**Versão do documento:** 1.0 — 13/10/2026  
**Status:** planejamento funcional e técnico, sem código de implementação.

## 1. Instrução ao Codex

Desenvolver futuramente um mod DayZ PC chamado **SDN_Leaderboard**, seguindo esta especificação. O servidor é PvP, com experiência próxima dos servidores oficiais, sem mods de economia, clãs ou zonas seguras. Free kill significa kill combinada para manipular classificação e prêmios.

Este documento descreve o resultado desejado, não demonstra que alguma API específica já foi validada. Antes de implementar, inspecionar os scripts e exemplos da versão instalada do DayZ, confirmar callbacks, formatos de layouts, identidade persistente, entradas e comportamento de persistência. Não inventar métodos ou copiar exemplos ilustrativos sem validação.

Separar decisões confirmadas dos valores iniciais sugeridos. Informar limites técnicos reais antes de prometer autoria perfeita de mortes ou entrega exatamente uma vez. Não gerar um anticheat genérico nem funcionalidades administrativas de teleportar, invulnerabilidade ou spawn livre: acesso total significa administração completa deste mod.

## 2. Objetivo e escopo

Criar ranking PvP profissional, funcional, leve, auditável e resistente à manipulação. Preservar o combate: não impedir dano, criar zonas protegidas ou conceder invulnerabilidade de respawn. Proteções invalidam pontuação e podem afetar elegibilidade para premiações.

Primeira versão: ranking por temporada, perfil individual, histórico, prevenção de free kill e spawn kill, recompensas em itens vanilla, painel dos jogadores, painel administrativo completo, persistência, backups e logs. Sem dependência obrigatória de CF, Expansion, VPP, economia, Discord ou banco externo.

Não incluir inicialmente rankings PvE, clãs, loja, moeda, recompensas por tempo online ou punições por combat logging. São expansões possíveis, com especificação própria.

## 3. Linguagem e tecnologias do DayZ

A lógica de mods de DayZ Standalone usa **Enforce Script**, linguagem orientada a objetos, com tipagem estática e sintaxe semelhante a C#/C++, mas regras próprias. Arquivos de script têm extensão `.c`; isso não significa que sejam programas C compilados convencionalmente. Não confundir com SQF do antigo DayZ Mod de Arma.

| Recurso | Finalidade |
| --- | --- |
| Enforce Script `.c` | Lógica, eventos, dados, RPC e controle dos menus |
| `config.cpp` | Registro do addon, dependências e módulos de script |
| `mod.cpp` | Metadados de apresentação do mod |
| `.layout` | Hierarquia e propriedades dos widgets da interface DayZ |
| `.xml` | Entradas de teclado e textos localizados, quando necessário |
| `.json` | Configurações e dados persistentes no servidor |
| PBO e assinatura | Distribuição do addon e verificação dos arquivos |

O `.layout` é um recurso nativo de GUI do DayZ, não HTML/CSS nem uma página web. Usar o formato real aceito pelo editor/ferramentas da versão instalada; não presumir que seja XML. Evitar confundir os recursos do DayZ com os do Arma Reforger apenas por ambos utilizarem Enforce Script.

Os módulos carregam em ordem: `1_Core`, `2_GameLib`, `3_Game`, `4_World`, `5_Mission`. Este projeto deve usar principalmente os três últimos. Um módulo inferior não pode depender de tipos definidos em módulo superior.

| Módulo | Responsabilidade proposta |
| --- | --- |
| `3_Game` | Modelos de dados, constantes, contratos de rede e configurações |
| `4_World` | Captura e classificação de eventos de combate e nascimento |
| `5_Mission` | Inicialização/orquestração da sessão e interfaces do cliente |

Validar o posicionamento de cada classe conforme suas dependências. Ao estender classes vanilla, preservar a lógica original e chamadas a `super` adequadas ao método. Controlar referências, evitar ciclos e liberar históricos temporários.

## 4. Estrutura de desenvolvimento e distribuição

Todos os scripts próprios terão prefixo `SDN_`, assim como classes, layouts e recursos próprios quando aplicável. Usar extensão convencional `.c`, equivalente ao padrão de nomes solicitado `SDN_NOME_DO_ARQUIVO.C`. Arquivos obrigatórios como `config.cpp` e `mod.cpp` conservam os nomes do motor.

A tabela é uma proposta de organização; não contém arquivos implementados.

| Caminho relativo à raiz `SDN_Leaderboard/` | Conteúdo previsto |
| --- | --- |
| `config.cpp` | Manifesto e caminhos dos módulos |
| `mod.cpp` | Nome, versão e autoria |
| `scripts/3_Game/SDN_LeaderboardConstants.c` | Constantes e identificadores |
| `scripts/3_Game/SDN_LeaderboardModels.c` | Dados e respostas públicas |
| `scripts/3_Game/SDN_LeaderboardConfig.c` | Configuração e validação |
| `scripts/3_Game/SDN_LeaderboardRPC.c` | Contratos de mensagens |
| `scripts/4_World/SDN_LeaderboardCombat.c` | Histórico de dano e atribuição |
| `scripts/4_World/SDN_LeaderboardPlayer.c` | Integração de ciclo de vida |
| `scripts/4_World/SDN_LeaderboardProtection.c` | Regras de invalidação |
| `scripts/5_Mission/SDN_LeaderboardServer.c` | Orquestração no servidor |
| `scripts/5_Mission/SDN_LeaderboardPersistence.c` | Persistência e recuperação |
| `scripts/5_Mission/SDN_LeaderboardSeasons.c` | Calendário e fechamento |
| `scripts/5_Mission/SDN_LeaderboardRewards.c` | Concessões e entregas |
| `scripts/5_Mission/SDN_LeaderboardPermissions.c` | Autorização administrativa |
| `scripts/5_Mission/SDN_LeaderboardMenu.c` | Painel do jogador |
| `scripts/5_Mission/SDN_LeaderboardAdminMenu.c` | Painel administrativo |
| `scripts/5_Mission/SDN_LeaderboardMission.c` | Inicialização e entradas |
| `gui/layouts/SDN_Leaderboard.layout` | Janela principal |
| `gui/layouts/SDN_LeaderboardAdmin.layout` | Janela administrativa |
| `gui/layouts/SDN_LeaderboardRow.layout` | Linha reutilizável do ranking |
| `gui/layouts/SDN_LeaderboardDialog.layout` | Confirmações e mensagens |
| `data/SDN_LeaderboardInputs.xml` | Ações remapeáveis |
| `stringtable.xml` | Textos localizados, se exigido pelo formato utilizado |
| `docs/` | Instalação, regras, configurações e operação |

Se uma divisão criar dependência inválida, reorganizar preservando responsabilidades e prefixos. Não implementar classes monolíticas apenas para manter esta lista literal.

Distribuição proposta: addon instalado no servidor e nos clientes, porque há interface nativa. PBO(s) em `@SDN_Leaderboard/addons/`; chave pública em `@SDN_Leaderboard/keys/`. Chave privada permanece fora do pacote. Confirmar parâmetros de carregamento, assinatura e dependências na instalação real.

## 5. Estrutura no profiles

Resolver a raiz através do diretório de perfil do servidor, sem caminhos absolutos fixos de sistema operacional.

| Pasta | Finalidade |
| --- | --- |
| `profiles\SDN_MODS\SDN_Leaderboard\Config\` | Configurações por assunto |
| `profiles\SDN_MODS\SDN_Leaderboard\Players\` | Estatísticas por identidade persistente |
| `profiles\SDN_MODS\SDN_Leaderboard\Seasons\` | Temporadas e classificações congeladas |
| `profiles\SDN_MODS\SDN_Leaderboard\Rewards\` | Concessões, pendências e entregas |
| `profiles\SDN_MODS\SDN_Leaderboard\State\` | Limites entre duplas e estado operacional |
| `profiles\SDN_MODS\SDN_Leaderboard\Backups\` | Cópias recuperáveis e migrações |
| `profiles\SDN_MODS\SDN_Logs\SDN_Leaderboard_Logs\` | Logs exclusivos do mod |

Configurações propostas: `SDN_General.json`, `SDN_Scoring.json`, `SDN_Protections.json`, `SDN_Seasons.json`, `SDN_Rewards.json`, `SDN_Admins.json` e `SDN_Interface.json`.

Separar logs de combate, proteções, administração, premiações e erros. Rotacionar por data/tamanho e limitar retenção. Não criar pastas de clãs ou zonas não utilizadas.

## 6. Pontuação e estatísticas

Valores abaixo são defaults propostos, editáveis pelo administrador e sujeitos a calibração.

| Regra | Valor inicial |
| --- | --- |
| Kill PvP válida | 100 pontos |
| Assistência válida | 25 pontos |
| Bônus por headshot, distância ou sequência | Desabilitados |
| Kill invalidada | 0 pontos |
| Suicídio ou morte ambiental | 0 pontos |
| Penalidade por morrer | Desabilitada |
| Janela da mesma dupla | 30 minutos |
| Proteção de pontuação após nascimento | Até 120 segundos |
| Duração de temporada | 30 dias |
| Elegibilidade para prêmio | 20 kills válidas contra 10 vítimas distintas |
| Ranking por página | 25 jogadores |

Somente a primeira kill da mesma dupla dentro da janela pode pontuar, independentemente da direção. Persistir a janela após reconexão/reinício. Assistências vinculadas a uma kill inválida também não pontuam.

Assistências exigem dano confirmado pelo servidor, janela temporal e contribuição mínima configuráveis. Limitar o total distribuído por morte; proposta inicial: até 50 pontos de assistências, rateados conforme contribuição elegível. Confirmar tratamento de cura e dano repetido para impedir inflação da contribuição.

Classificação: pontos, depois vítimas distintas válidas, depois instante em que atingiu a pontuação final; usar identificador estável como último desempate técnico, sem conceder vantagem pela alteração do apelido.

Exibir kills válidas e inválidas separadas, mortes PvP, ambientais e suicídios, assistências, streak válida e dados verificáveis de combate. K/D competitivo deve usar kills válidas divididas pelas mortes PvP contabilizadas; com zero mortes, exibir “—” e manter os números absolutos. Invalidar pontos do atacante não apaga automaticamente a morte PvP da vítima.

Descrever essa definição no painel. Não misturar estatísticas históricas com pontos da temporada. Contas de administradores podem participar sem benefícios, com opção de exclusão de premiações.

## 7. Autoria de mortes

Registrar dano recente com agressor, vítima, horário, origem e informações verificáveis. Definir janela de atribuição e prioridades antes da implementação. Uma morte gera um evento único, imutável na origem, sujeito a correções auditadas.

Tratar: dano direto, sangramento após ataque, atacante desconectado, troca de arma, múltiplos atacantes, queda após combate, explosivos, veículos, armadilhas, suicídio e origem desconhecida. Só creditar dano indireto quando a autoria for demonstrável. Eventos ambíguos ficam sem crédito competitivo e com motivo registrado.

Desconexão não é morte. Não atribuir pontos porque alguém saiu durante combate. Mudança de nome ou novo personagem não cria nova identidade competitiva.

## 8. Free kill e spawn kill

Free kill: mortes combinadas para ganho artificial de pontos. Regras objetivas podem negar pontuação imediatamente; heurísticas produzem suspeitas, não prova automática de fraude.

Sinais: reciprocidade frequente, muitas mortes entre poucas contas, repetição de recém-nascidos e concentração anormal de kills. Limiares, janela, razões e severidade devem ser configuráveis. Nunca banir automaticamente por esses sinais nem usar IP compartilhado como prova.

Proteção de spawn vale somente para novo nascimento. Reconectar não concede proteção. Durante até 120 segundos, a morte não pontua se a vítima não realizou ação ofensiva reconhecida. Definir a lista de ações ofensivas e validar sua detecção no servidor; dano causado pela vítima deve encerrar a proteção. Estar sem arma nas mãos não é critério suficiente.

Não bloquear dano. A proteção afeta ranking, assistências e bônus. Quando várias regras coincidirem, registrar todas e escolher motivo principal por prioridade documentada. Jogadores veem justificativa simples sobre seus próprios eventos; evidências detalhadas ficam com a administração.

## 9. Temporadas

Calendário configurável, com início e fim explícitos e fuso apresentado. Proposta de exibição: America/Sao_Paulo; persistir tempos em referência consistente e validar recursos reais de conversão/calendário disponíveis. Não depender do relógio do cliente.

Encerramento deve congelar uma classificação, validar elegibilidade, criar concessões únicas e iniciar próxima temporada conforme política configurada. Se o servidor estava desligado na data de encerramento, recuperar a transição ao iniciar sem executar prêmios repetidos nem criar uma sequência ilimitada de temporadas vazias.

Alterações de pontuação valem preferencialmente na próxima temporada. Alteração imediata exige ação explícita, versão de regras e auditoria. Suspensão competitiva é distinta de banimento do servidor.

## 10. Premiações automáticas

Sem moeda externa: pacotes de itens vanilla configuráveis para Top 1, Top 2 e Top 3, além de título no histórico. Usar consumíveis moderados inicialmente. Validar classes, quantidades, estado dos itens e limites de cada pacote antes de ativar.

A concessão ocorre automaticamente ao encerrar a temporada. O jogador recebe pelo painel mediante solicitação validada, estando conectado, vivo e em condição segura. Proposta: impedir recebimento enquanto houver dano recente, com intervalo configurável. Concessão automática não significa inserção inesperada durante combate.

Estados conceituais: pendente, em entrega, entregue, retida, cancelada e revisão necessária. Usar identificador de concessão vinculado à temporada, categoria e conta. Preferir validar capacidade do pacote antes de iniciar; se entrega parcial for inevitável, registrar cada unidade ou etapa.

Inventário cheio mantém pendência. Não despejar itens no chão como fallback. Jogador offline mantém direito. Não repetir pacote completo após entrega parcial. Não expirar automaticamente por padrão.

Um crash entre criação de item e gravação do recibo pode produzir incerteza. Exigir estratégia de reconciliação validada; quando não for seguro concluir, bloquear nova entrega e encaminhar ao administrador. Não prometer atomicidade entre inventário e JSON sem suporte demonstrado.

Casos com suspeita acima de limiar publicado podem ter prêmio retido. A retenção requer motivo e revisão; não equivale a condenação pública.

## 11. Layout e painel dos jogadores

Criar `SDN_Leaderboard.layout` no formato nativo do DayZ, com hierarquia de widgets e nomes únicos. Controle em Enforce Script no cliente; dados e decisões no servidor.

Estilo: fundo escuro, alto contraste, detalhes discretos, tipografia legível e destaque da própria linha. Janela central adaptável à resolução, com rolagem onde necessário. Não usar animações contínuas, vídeos ou texturas grandes.

| Área visual | Conteúdo/comportamento |
| --- | --- |
| Cabeçalho | Nome do servidor, SDN Leaderboard, temporada e fechar |
| Navegação | Ranking, Meu Perfil, Temporada, Recompensas e Histórico |
| Ranking | Posição, nome, pontos, kills válidas, mortes PvP e K/D |
| Barra de consulta | Pesquisa por nome, atualizar e paginação |
| Rodapé | Própria posição, horário da atualização e status |
| Meu Perfil | Estatísticas, motivos resumidos de invalidação e progressão |
| Temporada | Prazo, regras de pontuação, elegibilidade e prêmios |
| Recompensas | Pacotes, estado, condições e botão Receber |
| Histórico | Temporadas anteriores e vencedores |

Widgets conceituais: `SDN_Root`, `SDN_Header`, `SDN_Tabs`, `SDN_Content`, `SDN_RankingList`, `SDN_Search`, `SDN_PreviousPage`, `SDN_NextPage`, `SDN_Refresh`, `SDN_Status` e `SDN_Close`. São nomes planejados, não declaração de classes de widgets existentes.

Prever carregando, lista vazia, erro de conexão, dados desatualizados, acesso negado e operação em andamento. Não confundir falha de consulta com ranking vazio. Limitar e tratar nomes de jogadores como texto, sem permitir marcação arbitrária ou conteúdo que quebre a interface.

Não publicar identidade completa da conta, localização, inventário, regras internas de detecção detalhadas ou evidências administrativas. Interface não pausa o mundo nem fornece proteção contra combate.

## 12. Acesso dos jogadores

Registrar uma ação de entrada chamada “Abrir SDN Leaderboard” em controles, remapeável. Tecla inicial sugerida: F7, sujeita a conferência de conflitos. Registrar entrada pelo mecanismo suportado do DayZ, não por captura fixa de teclado.

Abrir apenas quando houver sessão válida e contexto de interface compatível. Não abrir sobre chat digitando, menus incompatíveis ou tela de carregamento. Ao fechar por botão, ESC ou tecla, restaurar corretamente foco, cursor e controles. Morte, desconexão e troca de sessão devem limpar a interface.

Ao abrir, consultar primeira página e resumo próprio. Paginar e pesquisar no servidor, com limite de tamanho e frequência. Cache sugerido: 10 segundos; intervalo mínimo de atualização manual: 5 segundos. Valores configuráveis. Não enviar todas as contas a todos os clientes nem atualizar a interface a cada kill.

## 13. Acesso administrativo e permissões

Painel administrativo próprio em `SDN_LeaderboardAdmin.layout`, acessível pelo botão “Administração” dentro do painel quando o servidor confirmar permissão. Atalho separado opcional e remapeável. Ocultar botão melhora a interface, mas não constitui autorização.

Administrador proprietário possui acesso total aos recursos do SDN_Leaderboard. Cadastrar inicialmente a conta proprietária em `Config\SDN_Admins.json` no servidor. O mod não concede administração ao primeiro jogador conectado. Confirmar o identificador persistente confiável usado pela versão instalada; não autorizar por apelido.

Papéis sugeridos: proprietário, administrador, moderador e auditor. Verificar permissão no servidor a cada consulta ou alteração, com princípio de menor privilégio. Não enviar configuração privada de administradores aos jogadores.

| Recurso | Proprietário | Administrador | Moderador | Auditor |
| --- | --- | --- | --- | --- |
| Consultar estatísticas e evidências | Sim | Sim | Sim | Sim |
| Revisar suspeitas | Sim | Sim | Sim | Leitura |
| Ajustar pontos/reverter eventos | Sim | Sim | Não | Não |
| Suspender elegibilidade | Sim | Sim | Conforme permissão | Não |
| Gerenciar recompensas | Sim | Sim | Não | Leitura |
| Configurar regras e temporadas | Sim | Conforme permissão | Não | Leitura |
| Gerenciar administradores | Sim | Não | Não | Não |
| Restaurar backup/resetar temporada | Sim | Não por padrão | Não | Não |

O proprietário pode delegar permissões granulares. Não permitir remover o último proprietário ativo. Autorizações não são senhas compartilhadas e não podem ser enviadas como comandos livres pelo cliente.

## 14. Painel administrativo completo em jogo

| Aba | Funções |
| --- | --- |
| Visão geral | Temporada, participantes, pendências, erros e saúde do mod |
| Jogadores | Buscar online/offline, consultar perfil e eventos, ajustar pontos com motivo |
| Combate | Filtrar eventos, consultar autoria e causas de invalidação |
| Proteções | Consultar suspeitas, revisar, confirmar/descartar e reprocessar eventos elegíveis |
| Temporadas | Planejar, abrir, encerrar, arquivar e consultar classificação congelada |
| Recompensas | Configurar pacotes, consultar fila, reter/liberar/cancelar e reconciliar entregas |
| Configurações | Editar parâmetros tipados, validar e aplicar conforme política |
| Administração | Gerenciar contas e permissões, exclusivo do proprietário |
| Auditoria | Consultar ações e seus valores anteriores/posteriores |
| Manutenção | Backup, diagnóstico e restauração controlada |

Editor de configurações com campos tipados, limites, explicações e prévia das alterações. Não aceitar caminho de arquivo arbitrário, código Enforce, RPC arbitrária ou comando de sistema.

Alterações exigem motivo e resposta do servidor. Operações destrutivas, encerramento antecipado, restauração e alterações relevantes de prêmios exigem confirmação dentro do painel mostrando escopo e consequências. Desabilitar duplo clique durante execução e tornar operações repetidas reconhecíveis.

Aplicar versão esperada para alterações: se outro administrador já mudou o registro, retornar conflito e pedir recarga; não sobrescrever silenciosamente. Reverter evento já premiado requer fluxo próprio de conciliação; não remover itens aleatoriamente do inventário.

Logs imutáveis no fluxo normal: correção gera novo registro vinculado ao original. Administradores não podem apagar evidências pela interface. Auditoria registra identidade, horário, ação, alvo, motivo, valores e resultado.

Restauração não deve manipular arquivos vivos sem proteção: preparar backup e validação pelo painel e aplicar em modo de manutenção/reinício quando a tecnologia exigir. O painel deve explicar ações que precisam reiniciar. Não prometer recarregamento seguro de qualquer mudança.

## 15. Segurança de rede

Cliente só pede consultas/ações. O servidor calcula pontos e recompensa. Validar remetente real, alvo autorizado, papel, valores, tamanho, paginação, estado da temporada e frequência. Nunca confiar em SteamID/conta declarado no payload como identidade de quem enviou.

Usar contratos explícitos de mensagens, limites por conta/operação e identificadores sem conflito verificados. Separar respostas públicas das administrativas. Não aceitar nomes de arquivos do cliente para leitura/gravação.

Respostas devem ter resultado claro e versão. Falha ou timeout não autoriza repetir uma entrega como se nunca tivesse ocorrido. Consultar estado da operação antes de retry.

## 16. Persistência, desempenho e falhas

Processar por eventos. Não varrer todos os jogadores a cada frame. Mapear contas em memória, limitar histórico de dano, limpar referências de entidades e atualizar classificação em lotes ou sob demanda. Cache de ranking público não deve atrasar registros críticos de prêmios.

Persistir estados sujos em lotes com limites; proposta inicial de intervalo: 60 segundos. Informar a janela de perda possível de estatísticas não críticas e reduzir por checkpoints/journal quando necessário. Recompensas e transições exigem tratamento mais forte que esse salvamento periódico.

Incluir versão de esquema, validação, migração e backup antes de alteração incompatível. Arquivo corrupto não deve ser sobrescrito por defaults: preservar cópia, registrar erro e usar recuperação segura. Validar escrita temporária/substituição se houver suporte; documentar alternativa se não houver.

Limitar páginas, filtros, logs e retenção. Desabilitar debug verboso por padrão. Caso armazenamento fique indisponível, suspender entregas e operações que dependam de persistência, sem travar combate ou missão. Exibir status ao administrador.

Medir custo com lotação-alvo a informar pelo proprietário: CPU de processamento, volume RPC, memória, tamanho de dados e duração de salvamento. Não declarar “leve” sem medidas comparativas em servidor real.

## 17. Critérios de aceitação

1. Compila e inicia em cliente e servidor da versão-alvo, sem APIs fictícias.
2. Ranking, paginação, busca e perfil apresentam dados coerentes.
3. Uma morte gera um único evento e uma única aplicação de pontos.
4. Mesma dupla não contorna janela trocando direção, nome ou reconectando.
5. Novo nascimento recebe regra de spawn; reconexão não renova proteção.
6. Ação ofensiva encerra proteção conforme regra validada.
7. Kill inválida não gera assistência, bônus ou streak competitiva.
8. Mortes ambíguas não recebem autoria inventada.
9. Temporada encerra e recupera após reinício sem concessões duplicadas.
10. Conta offline e inventário cheio mantêm prêmio pendente.
11. Entrega parcial/crash não repete pacote; incerteza vai para revisão.
12. Cliente sem permissão não consulta evidências nem executa ações administrativas, mesmo forjando pedidos.
13. Remoção de permissão tem efeito nas próximas operações da sessão.
14. Dois administradores não sobrescrevem alterações sem detectar conflito.
15. Alterações críticas têm confirmação e auditoria completa.
16. Corrupção/falha de escrita preserva dados recuperáveis e não interrompe o jogo.
17. Fechar menus restaura controles nas resoluções e situações testadas.
18. Teste de carga documenta custos e limites da lotação-alvo.

## 18. Entregáveis do futuro desenvolvimento

Código-fonte organizado, recursos `.layout`, entradas, configurações padrão, addon empacotado quando houver ferramentas disponíveis, instruções de instalação, guia de administração, exemplos de configurações, regras públicas e relatório de testes. Não entregar apenas scripts sem interface funcional ou deixar premiações como simulação.

Ordem recomendada: validar versão/APIs; captura e autoria; persistência; ranking e proteção; temporadas; recompensas; UI pública; UI administrativa; recuperação e teste de carga. A primeira versão somente está completa com todos os itens essenciais, mesmo que construídos por etapas.

## 19. Decisões e pendências

Confirmado pelo proprietário: PvP próximo dos oficiais; nenhum mod de economia/clãs/zonas; free kill é kill combinada; padrão SDN; este estágio produz especificação, sem implementação.

Ainda sugeridos: nome SDN_Leaderboard, valores de pontuação, assistências, janela de 30 minutos, proteção de 120 segundos, temporada de 30 dias, F7, elegibilidade, pacotes e papéis administrativos. Manter configuráveis e não tratá-los como valores aprovados irrevogavelmente.

Informações necessárias antes da implantação: versão do DayZ, mapa, lotação-alvo, identidade da conta proprietária, composição dos pacotes, calendário inicial e lista completa de mods existentes. Não preencher identidade administrativa com dados fictícios.

## 20. Referências e uso do material anexado

Referência fornecida pelo usuário: **Criação de Mods DayZ com EnforceScript.md**. Usar como orientação de arquitetura; exemplos ilustrativos e afirmações sobre API, coleções, memória e layouts precisam de conferência na versão real. Este documento não reproduz seus exemplos de código.

Fontes oficiais consultadas para linguagem e organização:

- [Bohemia — Enforce Script Syntax](https://community.bistudio.com/wiki/Enscript)
- [Bohemia — DayZ Modding Basics](https://community.bistudio.com/wiki/DayZ:Modding_Basics)
- [Bohemia — DayZ Modding Structure](https://community.bistudio.com/wiki/DayZ:Modding_Structure)
- [Bohemia — DayZ Modding Samples](https://community.bistudio.com/wiki/DayZ:Modding_Samples)

As fontes descrevem fundamentos técnicos; as regras competitivas, telas e políticas de recompensa deste documento são propostas do projeto SDN.
