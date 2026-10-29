# Arquivos do Outro Lado — protótipo nativo 0.1

Jogo de investigação e terror paranormal em primeira pessoa para Windows, com Unreal Engine 5. Este é um projeto novo, sem código de Turno da Meia-Noite. O alvo inicial é UE 5.6; não representa uma versão do motor detectada no computador do usuário.

**Estado real:** regras portáveis compiladas e testadas em Linux. Código de integração Unreal e scripts Windows ainda não compilados/executados. Não existe aplicativo `.exe` gerado. Cenário de primitivas, interface provisória e som original de teste; sem arte final ou desempenho medido.

## Caso 001 — A Casa Sem Registro

O DOI envia um investigador a uma casa ausente dos registros imobiliários. Uma família desaparecida aparece numa fotografia de 1978. O relatório lacrado contém o nome do investigador, décadas antes de sua contratação.

O recorte tem três salas e um corredor. Não pretende entregar nesta etapa cidade, hospitais, floresta ou a campanha completa. O loop é recolher evidências, confrontar um depoimento, escolher uma hipótese, recuperar o relatório e decidir o destino da casa.

### O que existe em código

- Personagem em primeira pessoa com movimento, visão e lanterna com bateria.
- Casa construída em runtime, com passagens abertas e abrigo iluminado junto à saída.
- Cinco pistas: registro, fotografia, fita, depoimento pelo interfone e relatório.
- Quadro textual com vínculo entre pistas, confronto e duas hipóteses; hipótese errada aumenta medo e permite reavaliar.
- Entidade geométrica que patrulha, investiga movimento/interação e persegue alvos visíveis próximos. Usa rotas simples pelas portas e testes de linha; não é uma IA de navegação final.
- Sinal sonoro espacial de proximidade, seis segundos de exposição contínua para derrota e recuperação ao se afastar. O abrigo protege da entidade e reduz medo.
- Uma estante muda de posição quando fica atrás do jogador após registrar a fotografia; não fecha a saída. Não há ainda arquitetura procedural mutável.
- Ritual de três etapas, erro com custo de medo e finais de contenção, liberação, extração/retirada e desaparecimento.
- Quadro e pausa congelam a simulação e a entidade. F8 alterna a pulsação lenta da lanterna sob ameaça. Suave é o padrão; não há flashes rápidos ou jumpscares.

C fotografando e G gravando registram **pistas predefinidas apontadas**, sem capturar arquivos de imagem ou áudio da cena. O depoimento é uma interação textual, não conversa por IA. A entidade detecta deslocamento e ações como eventos de ruído, sem ouvir microfone. Equipamentos não têm inventário visual. Não há combate, saves, progressão entre casos, gamepad, remapeamento, DLSS ou assets licenciados das referências. As consequências são locais aos finais deste caso. O medidor de medo ainda não distorce as pistas.

## Windows / Unreal

1. Instale/reutilize Unreal Engine 5.6 e Visual Studio 2022 com Desenvolvimento de jogos com C++, MSVC e Windows SDK compatíveis com esse motor. Use os instaladores oficiais; os scripts não instalam ferramentas.
2. Execute `VerificarInstalacao.cmd`. Ele verifica caminhos e lista SDKs, mas a compilação é que confirma compatibilidade. Usa `UE_ENGINE_ROOT` ou o caminho padrão `C:\Program Files\Epic Games\UE_5.6`. Para outro caminho, passe `-EngineRoot "D:\Epic\UE_5.6"` ao atalho.
3. Execute `PrepararProjeto.cmd`: compila o módulo Editor e cria `/Game/Maps/Case001` e o som através do Python do Unreal. As duas etapas precisam terminar antes de abrir o jogo.
4. Execute `AbrirEditor.cmd` e pressione Play em `/Game/Maps/Case001`. O mapa salvo é vazio: a casa é criada pelo GameMode em execução.
5. Valide o caso e feche o Editor antes de `GerarAplicativo.cmd`. A saída prevista é `Builds\Windows`; o script imprime os caminhos dos executáveis somente após o empacotamento. **Distribua a pasta completa**, incluindo dados e pré-requisitos.

Logs em `Logs`. Os atalhos usam `ExecutionPolicy Bypass` somente no processo; não alteram a política permanente. Respeitam bloqueios de organização. Não execute arquivos de origem desconhecida.

Os modos Prepare, Editor e Package recompilam o módulo e verificam os assets; a geração preserva mapa/áudio já existentes. Não execute duas preparações em paralelo e não substitua mapas editados para contornar falhas.

### Controles

| Tecla | Ação |
| --- | --- |
| WASD / mouse | Mover / olhar |
| E | Examinar, interfone, cofre, saída |
| C / G | Registrar fotografia / ouvir fita apontada |
| F | Lanterna |
| Tab | Quadro, com tempo suspenso |
| 1 / 2 no quadro | Confrontar depoimento; depois escolher hipótese temporal / fraude |
| 3 no quadro | Abrir o limiar após recuperar documento, decisão irreversível |
| 1, 2, 3 junto ao selo | Ritual, na ordem revelada pela fita |
| Esc | Fechar quadro / pausar |
| F8 | Atmosfera suave / intensa |
| R após o final | Reiniciar o caso |

## Validação e próximos passos

Em Linux: `bash Scripts/test-core.sh`. Requer g++ e C++17; usa warnings como erros e AddressSanitizer/UndefinedBehaviorSanitizer. O binário temporário é removido ao terminar.

`Tests/VALIDATION.md` separa resultados reais das verificações pendentes. Primeiro objetivo no Windows: UHT e compilação, corrigir falhas reais, preparar assets, testar Play e empacotar. Depois: avaliar legibilidade, enquadramento, fluxo das salas e perseguição antes de investir em arte e campanha. Não tratar compilações portáveis como validação das APIs do Unreal.
