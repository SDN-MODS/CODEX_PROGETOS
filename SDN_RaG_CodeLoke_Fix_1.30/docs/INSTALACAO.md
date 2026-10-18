# SDN RaG CodeLoke Fix 1.30

Este PBO é independente e não contém RaG BaseBuilding nem Code Lock.

## O que ele corrige

Em DayZ 1.30, `Fence.GetCodeLock()` é o acessador vanilla e retorna
`DigitalCodeLock`. O Code Lock usa a entidade `CodeLock`. O mod fornece um
acessador separado:

```c
CodeLock codelock = fence.SDN_GetCodeLockAttachment();
```

Esse método lê o attachment `Att_CodeLock` e não altera `DigitalCodeLock`.

## Instalação

1. Compile este PBO como `@SDN_RaG_CodeLoke_Fix_1.30`.
2. Carregue-o depois de Code Lock e RaG BaseBuilding:
   `-mod=@CodeLock;@RaG_BaseBuilding;@SDN_RaG_CodeLoke_Fix_1.30`.
3. O RaG precisa usar `SDN_GetCodeLockAttachment()` na ação de desmontagem.

## Restrição do compilador

Se o RaG distribuído ainda contém `CodeLock codelock = fence.GetCodeLock();`,
o erro ocorre antes do carregamento de qualquer PBO e este mod não consegue
interceptar ou substituir essa linha. Nesse caso é necessária uma atualização
do RaG/Code Lock pelo autor ou um patch de uma linha no fonte do RaG; os
arquivos originais não precisam ser empacotados neste projeto.
