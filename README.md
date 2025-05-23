# Projeto de Computação Gráfica: Remoção de Superfícies Ocultas em OpenGL

## Objetivo

O objetivo deste trabalho é compreender, aplicar e comparar os principais algoritmos de remoção de elementos ocultos utilizados em computação gráfica. Foi desenvolvida uma aplicação interativa usando OpenGL com GLUT, implementando diferentes técnicas para demonstrar visualmente seus efeitos sobre uma cena 3D.

## Conteúdo Abordado

O trabalho contempla os seguintes conceitos de remoção de superfícies ocultas:

1.  **Back-Face Culling**: Remoção de faces traseiras com base no vetor normal da face e na direção da câmera. Polígonos que não estão voltados para a câmera não são renderizados.
2.  **Painter's Algorithm (Algoritmo do Pintor)**: Desenho dos objetos da cena em uma ordem específica, geralmente do mais distante para o mais próximo da câmera. Objetos mais próximos são pintados sobre os mais distantes.
3.  **Z-Buffer (Buffer de Profundidade)**: Utilização de um buffer adicional (o Z-buffer) que armazena a informação de profundidade de cada pixel. Um novo pixel só é desenhado se estiver mais próximo da câmera do que o pixel anteriormente desenhado naquela posição.

Além disso, o projeto inclui:

*   **Movimentação de Câmera**: Controle de rotação da cena (em torno do eixo Y global) utilizando as teclas direcionais do teclado (setas ← →).
*   **Comparação Visual entre os Métodos**: A aplicação permite alternar entre os diferentes algoritmos de remoção de superfícies ocultas (e nenhum algoritmo) para evidenciar visualmente as diferenças e artefatos de cada técnica.

## Requisitos Técnicos Atendidos

O programa final:

*   Renderiza 4 objetos 3D distintos (um cubo vermelho, uma esfera verde, um toroide azul e um cone amarelo) em profundidades diferentes.
*   Permite rotacionar a cena com as teclas direcionais (esquerda e direita).
*   Permite ativar/desativar cada técnica individualmente:
    *   **Tecla '0'**: Nenhum método de remoção de superfície oculta.
    *   **Tecla '1'**: Ativa o Back-Face Culling.
    *   **Tecla '2'**: Ativa o Z-Buffer.
    *   **Tecla '3'**: Ativa o Algoritmo do Pintor.
*   Exibe uma legenda na tela indicando o método de remoção de superfície oculta ativo no momento.
*   Utiliza código C++ comentado.

## Implementação

*   **Objetos**: Foram utilizados `glutSolidCube`, `glutSolidSphere`, `glutSolidTorus`, e `glutSolidCone` para criar os objetos 3D.
*   **Estrutura**: O código foi organizado em funções para inicialização (`init`), desenho da cena (`display`), tratamento de redimensionamento da janela (`reshape`), e callbacks de teclado (`keyboard`, `specialKeys`).
*   **Técnicas**:
    *   Back-Face Culling: Habilitado com `glEnable(GL_CULL_FACE)` e `glCullFace(GL_BACK)`.
    *   Z-Buffer: Habilitado com `glEnable(GL_DEPTH_TEST)` e `glDepthFunc(GL_LESS)`. O `GLUT_DEPTH` é solicitado na inicialização do modo de display.
    *   Algoritmo do Pintor: Implementado ordenando os objetos com base em sua coordenada Z (profundidade) antes de desenhá-los. A ordenação é feita do mais distante para o mais próximo.
*   **Interação**: A troca entre os métodos é feita através da função `glutKeyboardFunc`, e a rotação da cena pela `glutSpecialFunc`.

## Como Compilar e Executar

### Pré-requisitos

*   Um compilador C++ (como g++, MinGW, MSVC).
*   GLUT instalado e configurado para o seu ambiente de desenvolvimento.
    *   Para Windows, geralmente envolve ter `glut32.dll` acessível (no diretório do executável ou no sistema), `glut.h` no diretório de includes do compilador, e `libglut32.a` (ou `glut32.lib`) no diretório de bibliotecas do compilador.

### Compilação (Exemplo com g++)

```bash
g++ main.cpp -o HiddenSurfaceRemoval -lglut32 -lopengl32 -lglu32
```

(Os nomes das bibliotecas e flags podem variar ligeiramente dependendo do sistema e compilador).

Se estiver usando um IDE como Code::Blocks, geralmente basta abrir o projeto e compilá-lo/executá-lo a partir do IDE, assumindo que o GLUT está corretamente configurado nas opções do compilador/linker do projeto.

### Execução

Após a compilação bem-sucedida, um arquivo executável (e.g., `HiddenSurfaceRemoval.exe` ou `HiddenSurfaceRemoval`) será gerado. Execute-o para iniciar a aplicação.

## Como Usar a Aplicação

*   **Rotacionar a Cena**:
    *   Use a **seta para a esquerda** para rotacionar a cena para a esquerda.
    *   Use a **seta para a direita** para rotacionar a cena para a direita.
*   **Alternar Métodos de Remoção de Superfície Oculta**:
    *   **Tecla '0'**: Desativa todos os métodos. Você poderá ver objetos se sobrepondo incorretamente.
    *   **Tecla '1'**: Ativa o **Back-Face Culling**. Faces traseiras dos objetos não serão renderizadas.
    *   **Tecla '2'**: Ativa o **Z-Buffer**. A visibilidade é determinada pixel a pixel com base na profundidade.
    *   **Tecla '3'**: Ativa o **Algoritmo do Pintor**. Objetos são desenhados do mais distante para o mais próximo.
*   **Sair da Aplicação**:
    *   Pressione a tecla **ESC**.

Uma legenda no canto superior esquerdo da janela indicará qual método está ativo.

### Observando o Comportamento dos Algoritmos

Ao executar a aplicação, você pode observar os seguintes comportamentos ao alternar entre os métodos:

*   **Tecla '0' (Nenhum método):**
    *   **Observação**: Objetos serão desenhados na ordem em que aparecem no código, sem considerar a profundidade. Isso resultará em artefatos visuais onde objetos mais distantes podem aparecer na frente de objetos mais próximos se forem desenhados depois. Gire a cena para ver como a sobreposição incorreta muda.

*   **Tecla '1' (Back-Face Culling):**
    *   **Observação**: As faces dos objetos que não estão voltadas para a câmera não serão renderizadas. Isso é mais perceptível em objetos "abertos" ou se caso nós pudessemos entrar dentro de um objeto. Para os sólidos `glutSolid*`, o efeito principal é uma otimização (menos polígonos para processar), mas visualmente pode não ser diferente do Z-Buffer para objetos fechados. Se os objetos fossem, por exemplo, caixas sem tampa, você veria o interior ao invés da face traseira ser desenhada incorretamente sobre a face frontal.

*   **Tecla '2' (Z-Buffer):**
    *   **Observação**: Este é geralmente o método mais robusto visualmente. Cada pixel é verificado contra o buffer de profundidade. Objetos mais próximos corretamente ocluem objetos mais distantes, independentemente da ordem de desenho. Gire a cena; a visualização deve permanecer correta em todos os ângulos.

*   **Tecla '3' (Algoritmo do Pintor):**
    *   **Observação**: Os objetos são desenhados do mais distante para o mais próximo. Para a configuração atual (objetos separados e sem intersecção complexa), ele deve funcionar bem e parecer similar ao Z-Buffer. No entanto, o Algoritmo do Pintor falha em casos de:
        *   **Objetos que se Interpenetram**: Se dois objetos se cruzarem, a ordenação simples por um ponto de profundidade pode não ser suficiente para resolver qual parte de qual objeto está na frente.
        *   **Dependência Cíclica de Profundidade**: Três ou mais objetos que se sobrepõem de forma cíclica (A sobre B, B sobre C, e C sobre A) não podem ser ordenados corretamente por este método simples.
        *   **Objetos Muito Grandes**: Um único polígono muito grande que se estende por várias profundidades pode ser difícil de ordenar corretamente contra outros objetos.
    *   Na implementação atual, a "profundidade" é baseada na posição Z inicial dos objetos. Se os objetos fossem mais complexos ou se movessem dinamicamente de forma a interpenetrarem-se significativamente, você poderia começar a ver artefatos onde a ordenação falha.

## Estrutura do Projeto

*   `main.cpp`: Contém todo o código-fonte da aplicação.
*   `Opengl.RemoveHiddenSurfaces.App.cbp`: Arquivo de projeto do Code::Blocks (opcional, para quem usa este IDE).
*   `Opengl.RemoveHiddenSurfaces.App.depend`: Arquivo de dependências do Code::Blocks.
*   `README.md`: Este arquivo.

---
Trabalho de Computação Gráfica
