# Validação

## Executado na nuvem

- `python3 tools/verify_source.py`: passou nas verificações estáticas de caminhos, delimitadores, XML, widgets, dependências de módulos, presença de chamadas originais e ordem dos campos de resposta RPC.
- `python3 -m unittest discover -s tests -v`: **13 testes passaram**. Eles exercitam a ferramenta Python de inspeção de snapshots e verificam que mudanças incompatíveis de RPC/widgets são detectadas.

Não foi executado: compilação Enforce, parser nativo de layouts, startup de cliente/servidor, teste PvP, recuperação após crash, empacotamento PBO, assinatura ou teste de carga. Os 13 testes **não testam o algoritmo de pontuação em Enforce**.

## Sequência obrigatória em ambiente DayZ de testes

Registrar build de cliente/servidor, mods, mapa, horário, arquivos de log e resultado real de cada cenário. Não marcar como aprovado sem execução.

| Cenário | Resultado esperado | Situação |
| --- | --- | --- |
| Compilar módulos e abrir layout no Workbench | Sem erro de script/layout | Pendente |
| Iniciar servidor e cliente com o PBO | Sem erro, configuração gerada | Pendente |
| Nova vida, morrer antes de 120s sem atacar | Zero pontos, motivo de nascimento | Pendente |
| Nova vida causa dano e depois morre | Proteção encerrada; outras regras continuam | Pendente |
| Reconectar durante/depois da proteção | Prazo original preservado | Pendente |
| A mata B elegível | A +100, uma kill, B +1 morte PvP, um evento | Pendente |
| Repetir callback de morte | Um evento por entidade, sem nova pontuação | Pendente |
| B mata A dentro de 30min | Zero pontos, dupla repetida | Pendente |
| Reiniciar e repetir dupla dentro do prazo | Regra persistida | Pendente |
| Repetir após exatamente 30min | Pode pontuar se demais regras permitirem | Pendente |
| Autoria ambígua/ambiente | Sem crédito competitivo | Pendente |
| Alterar apelido | Mesma identidade, sem escapar da regra | Pendente |
| Abrir painel, buscar e trocar páginas | Dados coerentes, máximo 25 linhas | Pendente |
| Consultar RPC de outro jogador | Sem dados retornados | Pendente |
| Exceder frequência/formato de consulta | Sem processamento competitivo | Pendente |
| Desconectar servidor durante consulta | Timeout explícito, não ranking vazio | Pendente |
| ESC, Fechar, morrer, desconectar com menu | Foco/cursor/controles restaurados | Pendente |
| Testar chat, inventário e carregamento | Não abrir sobre contexto incompatível | Pendente |
| 1280×720, 1920×1080 e ultrawide | Texto/rolagem utilizáveis | Pendente |
| Corromper uma cópia de snapshot no servidor de teste | Pontuação suspensa, sem apagar arquivo | Pendente |
| Bloquear gravação na pasta de teste | Combate vanilla continua; ranking indisponível | Pendente |
| Reiniciar entre as gravações do mod e do Hive | Diagnosticar divergências, sem promessa de atomicidade | Pendente |

Os cenários de recompensa e painel administrativo da especificação permanecem fora deste alpha e precisam implementação antes de teste. A versão 1 não pode ser aprovada enquanto esses itens faltarem.

Ao reportar erro, fornecer a primeira mensagem do log de script e seu contexto, build exata, ação que reproduz e mods carregados. Remover dados privados antes de compartilhar logs.
