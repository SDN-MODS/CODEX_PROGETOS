# SDN RaG CodeLoke Fix 1.30

PBO independente de compatibilidade para DayZ 1.30. Ele adiciona
`Fence.SDN_GetCodeLockAttachment()`, que recupera o `CodeLock` do attachment
`Att_CodeLock` sem conflitar com o acessador vanilla `Fence.GetCodeLock()`, cujo
retorno é `DigitalCodeLock`.

## Limitação necessária

O erro original acontece durante a compilação do fonte do RaG:

```c
CodeLock codelock = fence.GetCodeLock();
```

Um PBO separado não pode substituir uma linha de outro PBO antes que o
compilador a processe. Portanto, este mod é carregável independentemente, mas o
RaG precisa receber a atualização do autor ou uma única alteração de fonte para
usar `fence.SDN_GetCodeLockAttachment()`. Não é necessário empacotar RaG nem
Code Lock dentro deste mod.
