# SDN RaG CodeLoke Fix 1.30

Correção de compatibilidade para o erro:

`Types 'DigitalCodeLock' and 'CodeLock' are unrelated`

A correção separa os nomes: `DigitalCodeLock` continua sendo o tipo vanilla do
DayZ 1.30 e o auxiliar do o `CodeLock` é obtido diretamente do attachment `Att_CodeLock`, sem chamar
`Fence.GetCodeLock()`. Consulte `patches/actiondismantlepart.c` e
`docs/INSTALACAO.md`.
