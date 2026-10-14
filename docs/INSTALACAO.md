# Instalação para desenvolvimento

## Pré-requisitos externos

- DayZ PC e DayZ Server da **mesma versão estável**.
- DayZ Tools da Steam, incluindo Workbench e Addon Builder, em uma máquina compatível com essas ferramentas (normalmente Windows).
- Arquivos de jogo extraídos pelo fluxo oficial do DayZ Tools para conferir scripts e recursos reais.
- Servidor de testes isolado e dois clientes/contas para os cenários PvP; três para a futura assistência.

Não há cliente, servidor ou ferramenta de compilação DayZ instalados neste ambiente Linux. Não é necessário colocar senha Steam ou chave privada no repositório ou no chat. A compilação e o teste visual continuam externos até disponibilizar o runtime apropriado.

## Primeira compilação

1. Conferir a build instalada na Steam contra a referência `1.29.163709` e rever assinaturas se houver atualização.
2. Configurar a unidade de projeto com DayZ Tools e copiar `SDN_Leaderboard/` para `P:\SDN_Leaderboard\`.
3. Abrir o Script Editor/Workbench com os recursos do jogo e do mod. Corrigir erros reais de compilação antes de empacotar; os checks Python não aprovam esta etapa.
4. Abrir `gui/layouts/SDN_Leaderboard.layout` no editor nativo. Validar propriedades, estilos, fontes e dimensões; o arquivo é um protótipo ainda não aberto no editor. Ajustar no editor se a build exigir outro formato.
5. No Addon Builder, usar `P:\SDN_Leaderboard\` como diretório de origem e um diretório de saída separado. Preservar o prefixo `SDN_Leaderboard` definido por `$PBOPREFIX$` e incluir scripts `.c`, XML e layout no PBO. Conferir a lista de arquivos resultante.
6. Montar a distribuição abaixo. Copiar `mod.cpp` também para a raiz de `@SDN_Leaderboard`, onde o launcher lê os metadados.

```text
@SDN_Leaderboard/
  mod.cpp
  addons/
    SDN_Leaderboard.pbo
    SDN_Leaderboard.pbo.<chave>.bisign
  keys/
    <chave>.bikey
```

Assinar usando as ferramentas oficiais. A chave privada fica fora da distribuição e do Git. A chave pública deve estar também no diretório de chaves aceitas pelo servidor. Não desativar verificação de assinaturas para contornar erros do pacote.

## Carregamento

Carregar o addon no servidor **e** nos clientes usando `-mod=@SDN_Leaderboard`, junto aos demais argumentos usuais da instalação. Não usar apenas `-serverMod`: a interface e os contratos também existem no cliente.

Exemplo de fragmento de argumentos do servidor, relativo à pasta da instalação:

```text
-config=serverDZ.cfg -profiles=profiles -mod=@SDN_Leaderboard
```

O servidor precisa de sua configuração normal, missão/mapa e porta, conforme a instalação existente. Este repositório não altera a missão para spawn livre nem concede privilégios administrativos.

O primeiro início cria `profiles/SDN_MODS/SDN_Leaderboard/Config/SDN_General.json` e o estado. Verificar os logs de scripts e a mensagem `[SDN_Leaderboard] Alpha 0.1.0 iniciado`. A mensagem é apenas diagnóstico de startup; executar os cenários de [validação](VALIDACAO.md) antes de considerar o mod funcional.

F7 é a tecla inicial de abertura, remapeável em controles. O fechamento disponível neste alpha é o botão Fechar/ESC. A alternância pela mesma ação remapeável ainda precisa implementação e teste com o foco de teclado do menu.
