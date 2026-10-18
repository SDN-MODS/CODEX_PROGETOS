# SDN RaG CodeLoke Fix 1.30

Este é um patch de compatibilidade para o conflito entre `DigitalCodeLock`
do DayZ 1.30 e `CodeLock` do mod Code Lock. Os tipos são independentes.

## Aplicação

1. Faça uma cópia do arquivo original de `RaG_BaseBuilding`.
2. Em `scripts/4_World/rag_basebuilding/actions/vanilla/actiondismantlepart.c`,
   substitua a chamada `CodeLock codelock = fence.GetCodeLock();` pela busca
   explícita `CodeLock.Cast(fence.FindAttachmentBySlotName("Att_CodeLock"))`.
3. Não altere o acessador vanilla `DigitalCodeLock` e não use cast entre os tipos.
4. Compile no Workbench com DayZ 1.30, Code Lock, RaG BaseBuilding e este mod.

O arquivo `patches/actiondismantlepart.c` mostra a alteração mínima. O código
RaG original não está neste repositório, então o mod não inventa uma assinatura
de classe ou método que possa divergir da versão instalada.

## Carregamento

Use `-mod=@CodeLock;@RaG_BaseBuilding;@SDN_RaG_CodeLoke_Fix_1.30` conforme a
ordem suportada pelo servidor. O patch deve ser aplicado ao fonte do RaG antes
do empacotamento; apenas carregar um PBO adicional não corrige um erro de
compilação no fonte original.
