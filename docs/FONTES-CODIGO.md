# Referências públicas de código

Consultadas em 13/10/2026 por indicação de Davy. Cópias para leitura em build,
ignoradas pelo Git; nenhum jogo desses projetos foi iniciado ou baixado.
Manter a cópia fixa Godzilla inalterada. Mudanças do Kyoei ficam em src/ e CMake.

| Projeto | Versão consultada | Licença declarada |
| --- | --- | --- |
| [FelipePrado0/gow3_pc](https://github.com/FelipePrado0/gow3_pc) | ec9b416f2415fc5a0469076aecc230c6d6027ac0 | GPL-2.0, cabeçalhos pertinentes GPL-2.0-or-later |
| [Supermedo/bloodborne_pc](https://github.com/Supermedo/bloodborne_pc) | e5d04e2ba4864ccf40de6c3cf6aa532855320b0d | GPL-2.0, cabeçalhos pertinentes GPL-2.0-or-later |
| [ocornut/imgui](https://github.com/ocornut/imgui) | f1cc2ae15e53a861a874c3034aae6798fde194ab, 1.92.9b | MIT |

## Aproveitamento nesta sessão

GoW3: `gpu/shim/gow3_overlay.cpp` e integração em
`gpu/shadps4/video_core/renderer_vulkan/vk_presenter.cpp` demonstram desenho
ImGui sobre o swapchain, depois do frame, com dynamic rendering e loadOp=Load.
Inicialização Vulkan, font upload com mutex da fila e padrão de attachment
foram adaptados em src/kyoei_overlay_gpu.cpp, que conserva indicação da origem.
Estado dos diálogos, editor, contratos ABI e atalhos são próprios do Kyoei.
Não copiar confirmação Cross/volta Circle: aqui Circle confirma e Cross volta.

Bloodborne: examinados `gpu/shim/bbport_overlay.cpp` e estrutura de runtime,
entrada e save/load. Referência útil para encaminhamento de eventos e separação
de apresentação. Não inserido seu runtime inteiro nem aplicado seu modelo de
memória/upscaling como se fosse compatível automaticamente com Kyoei.

ImGui: snapshot mínimo em third_party/imgui, com LICENSE.txt e UPSTREAM.md.
Sem alterar o upstream. Core e backend Vulkan na mesma versão, evitando misturar
APIs de fontes/texturas de versões diferentes.

## Candidatos posteriores

- Cache de shaders em background e métricas de compilação, após conferir
  contratos/cache/chaves na base usada pelo Kyoei e testes de sincronização.
- Miniaturas dos saves, preservando formatos e ponteiros da ABI local.
- Backups antes de uma partida, sem depender de alterar os saves originais.
- Perfis/remapeamento de controles; conservar a convenção japonesa de botões.

Esses candidatos não foram implementados por aparecerem na referência. Não
transferir patches/endereço de GoW3 ou Bloodborne ao eboot do Kyoei. Resultados
de FPS e gameplay publicados pelos autores não medem nosso port.
