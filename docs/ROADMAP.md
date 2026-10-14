# Etapas até a versão completa

A especificação original continua sendo o escopo da versão 1. Este alpha entrega uma base para testes; não substitui requisitos faltantes por simulações.

1. **Validar o alpha no motor:** compilar, ajustar layout nativo, confirmar autoria/callbacks, foco de menus, persistência e contratos de rede. Testar ações ofensivas além de dano e alternância pela tecla remapeada.
2. **Combate e estatísticas completos:** histórico limitado de dano, autoria indireta demonstrável, suicídios/ambiente, assistências com limites e tratamento de cura, histórico individual e justificativas.
3. **Persistência operacional:** journal, backups, migrações, reconciliação com Hive, logs por categoria com retenção e recuperação controlada. Medir custo antes de aumentar limites.
4. **Temporadas:** calendário UTC com exibição explícita, transição recuperável, ranking congelado, versionamento de regras e histórico.
5. **Recompensas reais:** concessões únicas, validação de classes vanilla, capacidade de inventário, entrega parcial rastreável, bloqueio durante combate e revisão em caso de crash ambíguo. Não implementar entrega sem ensaiar reconciliação no motor.
6. **Administração:** cadastro explícito do proprietário, papéis e autorização por operação, painel nativo completo, auditoria, conflitos de versão, confirmações e restauração em manutenção. Não conceder acesso ao primeiro jogador.
7. **Interface completa e release:** abas públicas, painel administrativo, localização, resolução, carga na lotação-alvo, PBO assinado, operação e todos os critérios de aceitação.

Informações ainda necessárias antes de implantação: mapa, lotação-alvo, outros mods, identidade persistente do proprietário, pacotes e calendário. Nenhuma conta fictícia, segredo ou prêmio foi configurado.

Bloqueio atual de validação: este ambiente não possui DayZ, Workbench ou Addon Builder. A confirmação do alpha no runtime é necessária antes de tratar suas integrações como base estável para as etapas seguintes.
