# Arquivos do Outro Lado

Responda em português brasileiro. Leia LEIA-ME.md e Tests/VALIDATION.md antes de alterar o projeto.

Projeto novo de investigação e terror, Unreal Engine 5.5 como alvo atual (5.5.4 mostrado pelo usuário), primeira pessoa e Windows. Preserve a distinção entre regras portáveis e integração não validada no Unreal. Não substitua o projeto por jogo de navegador.

Use o checkout existente; não crie worktree sem pedido. Testes portáveis: `bash Scripts/test-core.sh`. Scripts Windows: VerificarInstalacao.cmd, PrepararProjeto.cmd, AbrirEditor.cmd e GerarAplicativo.cmd. Detecte o motor real antes de escolher toolchain. Não prometa acesso ao PC do usuário.

Preserve assets/mapas editados e arquivos existentes. Não desative verificações de TLS, assinaturas ou assertions. Não declare executável, desempenho ou qualidade visual comprovados sem execução real. Modelo e cenários são proxies de primitivas. Não há DLSS nem combate implementados.

Prioridade: compilar UHT/Editor em Windows com UE instalado, corrigir logs reais, testar Play, empacotar e abrir o aplicativo. Depois avançar visual e jogabilidade. Distribuição exige a pasta completa do empacotamento.
