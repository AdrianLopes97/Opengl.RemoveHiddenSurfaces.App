#include <GL/glut.h> 
#include <iostream>
#include <vector>
#include <algorithm> // Necessário para std::sort
#include <string>    // Necessário para std::string e std::to_string

// Variáveis globais
float rotationAngle = 0.0f; // Ângulo de rotação da cena
int activeMethod = 0; // 0: Nenhum, 1: Back-Face Culling, 2: Z-Buffer, 3: Algoritmo do Pintor

// Estrutura para armazenar dados dos objetos para o Algoritmo do Pintor
struct SceneObject {
    int id;
    float depth; // Profundidade aproximada para ordenação
    void (*drawFunc)(); // Ponteiro de função para a função de desenho do objeto
    float x, y, z; // Posição do objeto
};

std::vector<SceneObject> objects; // Vetor para armazenar os objetos da cena

// --- Funções de Desenho dos Objetos ---
void drawRedCube() {
    glColor3f(1.0f, 0.0f, 0.0f); // Cor vermelha
    glutSolidCube(0.5); // Desenha um cubo sólido
}

void drawGreenSphere() {
    glColor3f(0.0f, 1.0f, 0.0f); // Cor verde
    glutSolidSphere(0.3, 20, 20); // Desenha uma esfera sólida
}

void drawBlueTorus() {
    glColor3f(0.0f, 0.0f, 1.0f); // Cor azul
    glutSolidTorus(0.1, 0.25, 20, 20); // Desenha um toroide sólido
}

void drawYellowCone() {
    glColor3f(1.0f, 1.0f, 0.0f); // Cor amarela
    // A base do glutSolidCone está em z=0 e a ponta em z=altura.
    // Para fazê-lo aparecer em uma certa profundidade.
    glPushMatrix();
    glTranslatef(0, 0, -0.25f); // Centraliza o cone visualmente
    glutSolidCone(0.3, 0.5, 20, 20); // Desenha um cone sólido
    glPopMatrix();
}
// --- Fim das Funções de Desenho dos Objetos ---

void init() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // Cor de fundo cinza escuro

    // Habilita iluminação para efeito 3D
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // Habilita a fonte de luz 0
    GLfloat light_position[] = { 2.0, 2.0, 5.0, 0.0 }; // Posição da luz
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    GLfloat ambient_light[] = { 0.3, 0.3, 0.3, 1.0 }; // Luz ambiente
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);
    glEnable(GL_COLOR_MATERIAL); // Habilita o rastreamento de cor
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); // Define o material para rastrear cor ambiente e difusa

    // Define os objetos com suas funções de desenho e posições/profundidades iniciais
    // As profundidades são relativas à visão inicial da câmera.
    // O algoritmo do pintor ordenará com base nesses valores de 'depth'.
    // Para outros métodos, esses valores de 'depth' são usados para o posicionamento inicial.
    objects.push_back({1, -2.0f, drawRedCube, -1.5f, 0.0f, -2.0f});  // Mais distante
    objects.push_back({2, -1.0f, drawGreenSphere, -0.5f, 0.0f, -1.0f});
    objects.push_back({3, 0.0f, drawBlueTorus, 0.5f, 0.0f, 0.0f});
    objects.push_back({4, 1.0f, drawYellowCone, 1.5f, 0.0f, 1.0f});   // Mais próximo
}

// Função para desenhar texto na tela
void drawText(float x, float y, const std::string &text) {
    glMatrixMode(GL_PROJECTION); // Muda para a matriz de projeção
    glPushMatrix(); // Salva a matriz de projeção atual
    glLoadIdentity(); // Carrega a matriz identidade
    // Assumindo que o tamanho da janela é conhecido ou pode ser recuperado se dinâmico
    // Para simplificar está sendo utilizado 800x600.
    gluOrtho2D(0, 800, 0, 600); // Define uma projeção ortográfica 2D
    glMatrixMode(GL_MODELVIEW); // Muda de volta para a matriz de modelview
    glPushMatrix(); // Salva a matriz de modelview atual
    glLoadIdentity(); // Carrega a matriz identidade

    glDisable(GL_LIGHTING); // Desabilita iluminação para texto 2D
    glDisable(GL_DEPTH_TEST); // Garante que o texto seja desenhado por cima de tudo

    glColor3f(1.0f, 1.0f, 1.0f); // Cor do texto: branco
    glRasterPos2f(x, y); // Define a posição do raster para o texto
    for (char c : text) { // Itera sobre cada caractere no texto
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c); // Desenha o caractere
    }

    glEnable(GL_LIGHTING); // Reabilita a iluminação se estava ligada
    // O teste de profundidade será reabilitado ou desabilitado pela lógica do método ativo

    glPopMatrix(); // Restaura a matriz de modelview anterior
    glMatrixMode(GL_PROJECTION); // Muda para a matriz de projeção
    glPopMatrix(); // Restaura a matriz de projeção anterior
    glMatrixMode(GL_MODELVIEW); // Muda de volta para a matriz de modelview
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa os buffers de cor e profundidade
    glMatrixMode(GL_MODELVIEW); // Muda para a matriz de modelview
    glLoadIdentity(); // Carrega a matriz identidade

    // Define a posição e orientação da câmera
    gluLookAt(0.0, 0.5, 5.0,  // Posição da câmera (ligeiramente elevada)
              0.0, 0.0, 0.0,  // Ponto para o qual a câmera olha (origem)
              0.0, 1.0, 0.0); // Vetor "para cima"

    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f); // Rotaciona a cena inteira em torno do eixo Y

    std::string activeMethodText = "Metodo Ativo: "; // Texto para exibir o método ativo

    // Desabilita todas as técnicas por padrão antes de aplicar a selecionada
    glDisable(GL_CULL_FACE); // Desabilita o back-face culling
    glDisable(GL_DEPTH_TEST); // Desabilita o Z-buffer por padrão

    if (activeMethod == 1) { // Back-Face Culling
        glEnable(GL_CULL_FACE); // Habilita o back-face culling
        glCullFace(GL_BACK); // Define para remover as faces traseiras
        activeMethodText += "Back-Face Culling (1)";
    } else if (activeMethod == 2) { // Z-Buffer
        glEnable(GL_DEPTH_TEST); // Habilita o teste de profundidade (Z-buffer)
        glDepthFunc(GL_LESS);    // Passa se a profundidade de entrada for menor que a profundidade armazenada
        activeMethodText += "Z-Buffer (2)";
    } else if (activeMethod == 3) { // Algoritmo do Pintor
        // Para o Algoritmo do Pintor, o teste de profundidade idealmente deve estar desligado, ou pode interferir
        // Ordena os objetos por profundidade (do mais distante para o mais próximo)
        std::sort(objects.begin(), objects.end(), [](const SceneObject& a, const SceneObject& b) {
            // Esta ordenação é baseada na 'profundidade' predefinida que é o seu Z inicial.
            // Em uma cena dinâmica, essa profundidade precisaria ser recalculada com base na visão.
            return a.z < b.z; // Ordena por Z crescente (objetos mais distantes primeiro)
        });
        activeMethodText += "Algoritmo do Pintor (3)";
    } else {
        activeMethodText += "Nenhum (0)";
    }

    // --- Desenha os Objetos ---
    if (activeMethod == 3) { // Algoritmo do Pintor: Desenha os objetos ordenados
        for (const auto& obj : objects) { // Itera sobre os objetos ordenados
            glPushMatrix(); // Salva a matriz de modelview atual
            glTranslatef(obj.x, obj.y, obj.z); // Posiciona o objeto
            obj.drawFunc(); // Chama a função de desenho específica do objeto
            glPopMatrix(); // Restaura a matriz de modelview anterior
        }
    } else { // Para Nenhum, Back-Face Culling, Z-Buffer - desenha em ordem fixa (usando o vetor de objetos original)
        for (const auto& obj : objects) { // Itera sobre os objetos originais, não ordenados
            glPushMatrix(); // Salva a matriz de modelview atual
            glTranslatef(obj.x, obj.y, obj.z); // Posiciona o objeto
            obj.drawFunc(); // Chama a função de desenho específica do objeto
            glPopMatrix(); // Restaura a matriz de modelview anterior
        }
    }
    // --- Fim do Desenho dos Objetos ---

    drawText(10, 570, activeMethodText); // Exibe o método ativo no canto superior esquerdo

    glutSwapBuffers(); // Troca os buffers frontal e traseiro para animação suave
}

void reshape(int w, int h) {
    if (h == 0) h = 1; // Previne divisão por zero
    float ratio = w * 1.0 / h; // Calcula a proporção da janela

    glMatrixMode(GL_PROJECTION); // Muda para a matriz de projeção
    glLoadIdentity(); // Carrega a matriz identidade
    glViewport(0, 0, w, h); // Define a viewport para a janela inteira
    gluPerspective(45.0f, ratio, 0.1f, 100.0f); // Define a projeção perspectiva
    glMatrixMode(GL_MODELVIEW); // Muda de volta para a matriz de modelview
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case '1':
            activeMethod = 1; // Ativa o Back-Face Culling
            std::cout << "Back-Face Culling ativado." << std::endl;
            break;
        case '2':
            activeMethod = 2; // Ativa o Z-Buffer
            std::cout << "Z-Buffer ativado." << std::endl;
            break;
        case '3':
            activeMethod = 3; // Ativa o Algoritmo do Pintor
            std::cout << "Algoritmo do Pintor ativado." << std::endl;
            break;
        case '0':
            activeMethod = 0; // Desativa todos os métodos
            std::cout << "Nenhum metodo de remocao de superficies ocultas ativado." << std::endl;
            break;
        case 27: // Tecla ESC
            exit(0); // Fecha a aplicação
            break;
    }
    glutPostRedisplay(); // Solicita um redesenho da cena
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT: // Seta para a esquerda
            rotationAngle -= 5.0f; // Diminui o ângulo de rotação
            break;
        case GLUT_KEY_RIGHT: // Seta para a direita
            rotationAngle += 5.0f; // Aumenta o ângulo de rotação
            break;
    }
    if (rotationAngle > 360.0f) rotationAngle -= 360.0f; // Mantém o ângulo entre 0 e 360
    if (rotationAngle < 0.0f) rotationAngle += 360.0f; // Mantém o ângulo entre 0 e 360
    glutPostRedisplay(); // Solicita um redesenho da cena
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv); // Inicializa o GLUT
    // Habilita buffer duplo, cor RGB e um buffer de profundidade
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600); // Define o tamanho da janela
    glutInitWindowPosition(100, 100); // Define a posição inicial da janela
    glutCreateWindow("OpenGL Remocao de Superficies Ocultas - CG UBLA"); // Cria a janela com um título

    init(); // Chama a função de inicialização

    glutDisplayFunc(display);       // Registra a função de callback de display
    glutReshapeFunc(reshape);       // Registra a função de callback de redimensionamento
    glutKeyboardFunc(keyboard);     // Registra a função de callback do teclado
    glutSpecialFunc(specialKeys);   // Registra a função de callback de teclas especiais (para setas)

    glutMainLoop(); // Entra no loop de processamento de eventos do GLUT
}
