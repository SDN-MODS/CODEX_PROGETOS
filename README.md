# SDN Leaderboard

MOD DayZ PC em Enforce Script. **Alpha 0.1.0 em desenvolvimento, sem compilação ou validação no motor DayZ. Não é a versão 1 completa da especificação.**

Referência técnica: scripts oficiais **1.29.163709**, revisão `125372`, commit `86974a0f5bd16b1ee3e334ad828133c93dca80a1`. Essa foi a versão confirmada no repositório oficial; a versão atual da Steam ainda deve ser conferida antes dos testes.

Esta primeira implementação contém:

- Manifesto, módulos `3_Game`, `4_World` e `5_Mission`, prefixos SDN e entrada F7 remapeável.
- Captura de morte direta, identidade persistente, ranking e desempates estáveis.
- Regra de dupla nas duas direções, proteção de pontuação após nascimento e encerramento da proteção ao causar dano confirmado a outro jogador.
- Configuração JSON validada, snapshots alternados e suspensão da pontuação em falhas de persistência.
- RPC autenticada pelo remetente, limite de consultas, busca, paginação e protótipo de painel nativo.
- Ferramentas locais de verificação e inspeção de snapshots.

As funcionalidades acima **foram escritas, mas ainda precisam compilar e ser exercitadas no DayZ**. Interface, ordem dos callbacks e persistência sob crash são pontos de validação obrigatória.

Ainda faltam assistências, autoria indireta, temporadas, prêmios reais, painel administrativo, auditoria completa, backups operacionais, histórico de temporadas e testes de carga. O ranking atual é cumulativo e experimental: não iniciar uma temporada premiada com este alpha.

## Desenvolvimento na nuvem

Python 3.10+ para as ferramentas locais; nenhum pacote externo:

```sh
cd /workspace/CODEX_PROGETOS
python3 tools/verify_source.py
python3 -m unittest discover -s tests -v
```

Esses comandos verificam organização, XML, contratos de RPC e a ferramenta de inspeção; **não são um compilador Enforce nem testes de gameplay**. Arquivos `.c` não devem ser compilados com GCC/Clang.

Para inspecionar uma cópia de snapshot sem modificá-la:

```sh
python3 tools/check_snapshot.py /caminho/SDN_State_1.json
```

## Documentação

- [Especificação original](docs/SDN_Leaderboard_Especificacao_Codex.md)
- [Instalação e empacotamento](docs/INSTALACAO.md)
- [Configuração e persistência](docs/OPERACAO_ALPHA.md)
- [Evidências de API e limitações](docs/REFERENCIAS.md)
- [Roteiro e situação dos testes](docs/VALIDACAO.md)
- [Próximas etapas da versão completa](docs/ROADMAP.md)
