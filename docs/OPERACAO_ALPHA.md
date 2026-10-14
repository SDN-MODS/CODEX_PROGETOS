# Operação do alpha

## Configuração

Editar somente com o servidor parado. Arquivo gerado em `$profile:SDN_MODS/SDN_Leaderboard/Config/SDN_General.json`:

```json
{
  "SchemaVersion": 1,
  "Enabled": true,
  "KillPoints": 100,
  "PairWindowSeconds": 1800,
  "SpawnProtectionSeconds": 120,
  "PageSize": 25,
  "QueryIntervalSeconds": 5,
  "RankingLabel": "Ranking experimental — sem temporada"
}
```

Valores são defaults sugeridos, não decisões permanentes de pontuação. Limites: pontos 1–10000, janela de dupla 1–86400s, nascimento 0–600s, página 1–25, consultas 1–60s. Alterar pontos não recalcula eventos passados. Não há reload nem editor administrativo neste alpha.

## Regras implementadas no código

- Autoria aceita somente quando o evento identifica diretamente um jogador ou objeto ligado à sua hierarquia. Não há atribuição por último agressor nem reconstrução de explosivos, carros ou armadilhas. Validar o significado de `killer` no motor antes de usar competitivamente.
- Sem autoria direta confirmada, a morte não pontua. Mortes ambientais, suicídios e indiretas ainda não têm classificação completa; o contador é `UncreditedDeaths`.
- Proteção começa em `OnClientNewEvent`, não em login normal. Causar dano de saúde, sangue ou choque a outro jogador encerra a proteção. Tiro sem acerto e outras ações ofensivas não estão implementados.
- A dupla é simétrica. A janela começa na última kill **creditada**; tentativas inválidas não prorrogam o prazo. As entradas ficam por até 24h para suportar os limites de configuração.
- Kill inválida soma em `InvalidKills`; a morte PvP da vítima continua sendo contada. Sem penalidade por morrer.
- Desempate: pontos, vítimas distintas, instante em que atingiu a pontuação e identidade estável. Apelido não desempata.
- K/D usa kills válidas/mortes PvP, com “—” quando não há mortes.
- Não modifica dano, inventário, invulnerabilidade, posição ou equipamentos.

## Persistência e recuperação

`State/SDN_State_0.json` e `SDN_State_1.json` alternam gerações. Cada gravação é relida e comparada com a serialização esperada. Pontos, janela da dupla e evento pertencem ao mesmo snapshot; não existem três gravações independentes para uma kill.

O servidor suspende a pontuação se encontrar arquivo inválido, erro de escrita ou relógio UTC voltando no tempo. Preserva os arquivos, em vez de substituí-los por defaults. Um marcador detecta ausência dos dois snapshots em uma instalação já inicializada. O estado em memória após uma falha deixa de ser servido como ranking válido.

Isso **não fornece transação com o Hive, fsync, substituição atômica ou garantia exatamente uma vez após crash**. A perda de um slot inteiro pode não ser distinguível de uma instalação com só uma geração; não apagar arquivos para tentar recuperar. Um crash entre a persistência do personagem e a do mod pode gerar divergência. Recuperação automática ainda não está implementada.

Para diagnóstico:

1. Parar o servidor e copiar a pasta inteira para um backup externo, preservando os dois slots e o marcador.
2. Executar `python3 tools/check_snapshot.py <copia-do-slot>` em cada cópia. A ferramenta só valida estrutura e imprime contagens; não repara nem escolhe uma geração.
3. Conferir logs do servidor e os eventos mais recentes com a situação no jogo.
4. Restaurar somente um conjunto conhecido e consistente em manutenção; preservar o conjunto com erro para investigar. Não liberar premiações com base em uma recuperação não reconciliada.

Limites explícitos do alpha: 2000 contas, 20000 duplas retidas, últimos 500 eventos em snapshot. Não há arquivo de auditoria completo: eventos mais antigos saem do anel. Contas e vítimas distintas permanecem no estado. Identidades ficam somente no servidor, nunca na resposta pública de ranking.

A gravação integral é síncrona por conexão, nascimento, primeira ação ofensiva da vida e morte. O custo ainda não foi medido. Não há promessa de desempenho em alta lotação; migrar para persistência incremental/journal após medir. Nenhuma varredura de ranking é feita por frame; a classificação é reconstruída sob demanda e cacheada até mudança de estado.

Horários internos são segundos desde 01/01/2020 UTC, com limite de ano 2087. Não há ainda calendário de temporada nem conversão de exibição para America/Sao_Paulo.
