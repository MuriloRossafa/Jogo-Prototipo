# Validação — 29/10/2026

## Executado nesta sessão Linux

- `bash Scripts/test-core.sh`: PASS com C++17, -Wall -Wextra -Werror -pedantic, AddressSanitizer e UndefinedBehaviorSanitizer. Nenhuma opção de sanitizador desabilitada.
- Cobertura: pré-requisitos de evidências e cofre, confronto, hipótese errada/correta, coleta duplicada, sequência/erro de ritual, contenção, liberação, saída com/sem documento, imutabilidade após o final, exposição e recuperação, derrota, esgotamento da bateria e cem cenários de limites de recursos.
- Scripts Python: análise de sintaxe com ast.parse. Não executa import unreal.
- GenerateAudio.py executado: um WAV mono PCM de 16 bits, 22050 Hz, um segundo. Arquivo aberto e frames verificados. Não comprova importação ou espacialização no motor.
- OtherSide.uproject: JSON válido.

## Não executado / bloqueio de plataforma

Unreal Editor e PowerShell não disponíveis nesta sessão Linux; instalação Windows do usuário inacessível. Versão do Unreal/GPU/MSVC/SDK do usuário desconhecida.

Não executados: parsing PowerShell, Diagnose no Windows, UHT, compilação Editor/Game, Python no Unreal, materiais, geração do mapa, Play, testes visuais/sonoros, desempenho, cook, empacotamento ou abertura de executável. Não há `.exe` gerado.

## Checklist necessário no Windows

1. Executar VerificarInstalacao.cmd e inspecionar motor/toolchain/SDK reais no log.
2. PrepararProjeto.cmd deve terminar sem erro, com Content/Maps/Case001.umap e Content/Audio/Warning.uasset.
3. Play: spawn fora do piso, movimento/mouse, três salas e todos os objetos alcançáveis, luz e textos legíveis em 720p e 1080p. Ajustar HUD se houver sobreposição; ainda não avaliada visualmente.
4. Completar registro, foto, depoimento, confronto, fita, hipótese, cofre; verificar que cofre não abre antes da conclusão e erros não bloqueiam nova tentativa.
5. Quadro/Esc: posição, bateria, medo e entidade devem parar; fechar sem movimento residual.
6. Após 30 segundos e uma pista: ouvir aviso com direção, testar patrulha, sons, rotas nas portas, perda de contato e abrigo. Verificar que a entidade não atravessa paredes ou fica presa; IA ainda é provisória.
7. Estante só muda fora de vista e não fecha uma rota; referências de interação permanecem acessíveis.
8. Testar erro e acerto no ritual, abrir limiar, retirar com/sem documento e derrota. R deve reiniciar sem carregar estado anterior.
9. Testar F8 e lanterna descarregada; pistas devem permanecer legíveis sem depender da lanterna.
10. GerarAplicativo.cmd, verificar caminho real, executar fora do Editor e repetir um caso. Distribuir diretório completo.

Falhas da integração devem ser investigadas pelos logs, sem registrar como aprovados testes não executados.
