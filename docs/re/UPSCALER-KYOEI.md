# Upscaler temporal (FSR 3/4, DLSS) no Kyoei — pesquisa

O núcleo GoW3 traz FSR 3.1, FSR 4 e DLSS (vk_temporal_upscaler, vk_camera_motion, vk_dlss),
mas detecta cena e câmera do God of War III (G-buffer com 5+ alvos, constantes de 864 bytes).
Para o Kyoei é preciso dizer onde estão cor da cena, profundidade e câmera.

Captura de quadro (24/10/2026, rua do início, GOW3_DRAW_PIPE=0, GOW3_CAPTURE_TRIGGER;
patch frame_capture_whole_frame_cpp: captura de flip a flip, o Kyoei escreve o buffer de
exibição em mais de um passe):
- Sombras: D32 4096x4096 (#19-22, ~700 desenhos).
- Cena: o mesmo conjunto de 778 desenhos duas vezes: #25 escreve profundidade
  (0x1049ca0000 D32S8 1920x1080) e RGBA16F 0x104b680000; #39 desenha a cor em
  RGBA16F 0x1048cb0000 com a profundidade só leitura. Depois transparências (#40-62) no
  mesmo alvo, pós-processo em 960x540/1920x540 e a composição sRGB final.
- Projeção: constantes do VS (slot 1, 1616-1728 bytes) começam com a matriz 4x4 em linhas:
  P00 1.5455, P11 2.7475, P22 -1.0001, P23 -0.10001, P32 -1 (perto 0.1, longe 1000,
  convenção OpenGL; profundidade 0..1 = 1.0001 - 0.10001/z_frente).
  Para o shader camera_motion.comp (z para frente positivo): proj = (P00, P11, -P22, P23)
  e matrizes de vista com a 3ª linha negada.
- Vista: os 12 primeiros floats das constantes do PS (slot 0) são iguais em todos os passes da
  cena (rotação + translação em linhas, [R|t]); os floats 16-27 do VS mudam por objeto
  (mundo-vista). A confirmar com a sobreposição de vetores (GOW3_DEBUG_MOTION) antes de ligar.

Próximos passos: gancho Kyoei no rasterizador (passe com RGBA16F 1920x1080 + D32 1920x1080),
câmera do PS slot 0 + projeção do VS slot 1, validar vetores com GOW3_DEBUG_MOTION, depois
GOW3_UPSCALER=fsr3/dlss e opção no launcher (qualidade/equilíbrio/desempenho, nativo AA).
