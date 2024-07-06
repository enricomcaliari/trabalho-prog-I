#include <stdio.h>
#include <stdlib.h>

// definicao do tamanho maximo das constantes
#define TAM_MAX_LINHA_MAPA 42
#define TAM_MAX_COLUNA_MAPA 102
#define TAM_MAX_DIRETORIO 1000

// definicao do tipo tMapa com suas variaveis
typedef struct {
  char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA];
  int qntdLinhas;
  int qntdColunas;
  int qntdMovimentos;
} tMapa;

// definicao do tipo tPacMan com suas variaveis
typedef struct {
  int pos_xInicial;
  int pos_yInicial;
  int pos_xAtual;
  int pos_yAtual;
  char jogada;
  int movimentou;
  int comidaspacman;
  int pos_xDiferente;
  int pos_yDiferente;
  int jogadasDiferentes;
} tPacman;

// definicao do tipo tFantasmas com suas variaveis
typedef struct {
  int posfanXB;
  int posfanYB;
  int posfanXP;
  int posfanYP;
  int posfanXI;
  int posfanYI;
  int posfanXC;
  int posfanYC;
  int flagB;
  int flagP;
  int flagI;
  int flagC;
  int qntdfantasmas;
} tFantasmas;

// definicao do tipo tTrilha com suas variaveis
typedef struct {
  int mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA];
  int qntdLinhas;
  int qntdColunas;
} tTrilha;

typedef struct{
  int numMovimentos;
  int numMovimentosSemPontuar;
  int colisoesParede;
  int moveCima;
  int moveBaixo;
  int moveDireita;
  int moveEsquerda;
}tEstatisticas;

// definicao do tipo tJogo com suas variaveis
typedef struct {
  tMapa mapa;
  tPacman pacman;
  tFantasmas fantasmas;
  int qntdComidas;
  int movimentosJogados;
  tTrilha trilha;
  tEstatisticas estatisticas;
} tJogo;

// funcoes do tipo tPacman
tPacman EncontrarPacman(char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA],
                        int qntdLinhas, int qntdColunas);
tPacman DeslocaPacman(tPacman pacman,
                      char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA]);
int DescobreDirecaoPacman(tPacman pacman);
tPacman MovimentaPacmanCima(tPacman pacman,
                            char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA]);
tPacman
MovimentaPacmanBaixo(tPacman pacman,
                     char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA]);
tPacman
MovimentaPacmanDireita(tPacman pacman,
                       char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA]);
tPacman
MovimentaPacmanEsquerda(tPacman pacman,
                        char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA]);

// funcoes do tipo tJogo
tJogo LeJogo(char caminho[1000]);
void ArquivoInicializacao(tJogo jogo, char caminho[1000]);
int VerificaMortePacman(tJogo jogo);
tJogo ContaComidasMapa(tJogo jogo);
int VerificaSeJogoAcabou(tJogo jogo);
void ImprimeJogo(tJogo jogo);
tJogo PacmanComeComidas(tJogo jogo);
void ArquivoSaida(tJogo jogo, char caminho[1000]);
void ResultadoArquivoSaida(tJogo jogo, char caminho[1000]);
void ArquivoResumo(tJogo jogo, char caminho[1000]);
void ArquivoRanking(tJogo jogo, char caminho[1000]);
void ArquivoEstatistica(tJogo jogo, char caminho[1000]);
void ArquivoTrilha(tJogo jogo, char caminho[1000]);
tJogo IncrementaEstatistica(tJogo jogo);

// funcoes do tipo tMapa
tMapa LimpaFantasmasDoMapa(tMapa mapa);

// funcoes do tipo tFantasmas
tFantasmas
EncontrarFantasmas(char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA],
                   int qntdLinhas, int qntdColunas);
tFantasmas MoveFantasmas(tFantasmas fantasmas,
                         char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA],
                         int qntdLinhas, int qntdColunas);

// funcoes do tipo tTrilha
tTrilha InicializaTrilha(int qntdLinhas, int qntdColunas);
tTrilha IncrementaTrilha(tJogo jogo, tTrilha trilha, int posXPacman,
                         int posYPacman, int numeroDaJogada);

// funcao que procura o pacman no mapa e salva a posicao inicial dele, e tambem
// a atual pra quando for mexer com o pacman usar a atual
tPacman EncontrarPacman(char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA],
                        int qntdLinhas, int qntdColunas) {
  int i, j;
  tPacman pacman;
  for (i = 0; i < qntdLinhas; i++) {
    for (j = 0; j < qntdColunas; j++) {
      if (mapa[i][j] == '>') {
        pacman.pos_xInicial = i + 1;
        pacman.pos_yInicial = j + 1;
        pacman.pos_xAtual = j;
        pacman.pos_yAtual = i;
      }
    }
  }
  return pacman;
}

// Funcao que descobre qual a jogada e retorna um valor pra posteriormente em
// outra função deslocar ele na direção desejada
int DescobreDirecaoPacman(tPacman pacman) {
  char jogada;
  scanf("%c\n", &jogada);
  if (jogada == 'w') { // CIMA
    return 1;
  }
  if (jogada == 's') { // BAIXO
    return 2;
  }
  if (jogada == 'd') { // DIREITA
    return 3;
  }
  if (jogada == 'a') { // ESQUERDA
    return 4;
  }
  return 0;
}

// Funcao que depois de descobrir qual a direção do pacman, desloca ele de
// acordo com a jogada
tPacman DeslocaPacman(tPacman pacman,
                      char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA]) {
  int x, y, direcao;
  direcao = DescobreDirecaoPacman(pacman);
  if (direcao == 1) {
    pacman = MovimentaPacmanCima(pacman, mapa);
    pacman.jogada = 'w';
  }
  if (direcao == 2) {
    pacman = MovimentaPacmanBaixo(pacman, mapa);
    pacman.jogada = 's';
  }
  if (direcao == 3) {
    pacman = MovimentaPacmanDireita(pacman, mapa);
    pacman.jogada = 'd';
  }
  if (direcao == 4) {
    pacman = MovimentaPacmanEsquerda(pacman, mapa);
    pacman.jogada = 'a';
  }
  return pacman;
}

// Funcao que move o pacman para cima (w)
tPacman
MovimentaPacmanCima(tPacman pacman,
                    char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA]) {
  if (mapa[pacman.pos_yAtual - 1][pacman.pos_xAtual] !=
      '#') { // move pacman para cima desde que não haja uma parede na posição
             // acima
    pacman.pos_yAtual--;
    pacman.movimentou = 1;
  } 
  
  else {
    pacman.movimentou = 0;
  }
  return pacman;
}

// Funcao que move o pacman para baixo (s)
tPacman
MovimentaPacmanBaixo(tPacman pacman,
                     char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA]) {
  if (mapa[pacman.pos_yAtual + 1][pacman.pos_xAtual] !=
      '#') { // move pacman para baixo desde que não haja uma parede na posição
             // abaixo
    pacman.pos_yAtual++;
    pacman.movimentou = 1;
  }
  else {
    pacman.movimentou = 0;
  }
  return pacman;
}

// Funcao que move o pacman para direita (d)
tPacman
MovimentaPacmanDireita(tPacman pacman,
                       char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA]) {
  if (mapa[pacman.pos_yAtual][pacman.pos_xAtual + 1] !=
      '#') { // move pacman para direita desde que não haja uma parede na
             // posição posterior
    pacman.pos_xAtual++;
    pacman.movimentou = 1;
  }
  else {
    pacman.movimentou = 0;
  }
  return pacman;
}

// Funcao que move o pacman para esquerda (a)
tPacman
MovimentaPacmanEsquerda(tPacman pacman,
                        char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA]) {
  if (mapa[pacman.pos_yAtual][pacman.pos_xAtual - 1] !=
      '#') { // move pacman para esquerda desde que não haja uma parede na
             // posição posterior (sentido esquerda)
    pacman.pos_xAtual--;
    pacman.movimentou = 1;
  }

  else {
    pacman.movimentou = 0;
  }
  return pacman;
}

// funcao que procura os fantasmas no mapa e salva posicao de cada um deles
// as posicoes estao sendo salvas com numeros grandes e aleatorios para caso
// elas estejam com lixo, e o lixo pertencer ao parametro nao se confundir com
// um fantasma
tFantasmas
EncontrarFantasmas(char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA],
                   int qntdLinhas, int qntdColunas) {
  int i, j;
  tFantasmas fantasma;
  fantasma.posfanXB = 5200;
  fantasma.posfanXC = 5200;
  fantasma.posfanXI = 5200;
  fantasma.posfanXP = 5200;
  fantasma.posfanYB = 5200;
  fantasma.posfanYC = 5200;
  fantasma.posfanYI = 5200;
  fantasma.posfanYP = 5200;

  for (i = 0; i < qntdLinhas; i++) {
    for (j = 0; j < qntdColunas; j++) {
      if (mapa[i][j] == 'B') {
        fantasma.posfanXB = i;
        fantasma.posfanYB = j;
      }
      if (mapa[i][j] == 'P') {
        fantasma.posfanXP = i;
        fantasma.posfanYP = j;
      }
      if (mapa[i][j] == 'I') {
        fantasma.posfanXI = i;
        fantasma.posfanYI = j;
      }
      if (mapa[i][j] == 'C') {
        fantasma.posfanXC = i;
        fantasma.posfanYC = j;
      }
    }
  }
  return fantasma;
}

// funcao move os fantasma de acordo com a direção de cada um
tFantasmas MoveFantasmas(tFantasmas fantasmas,
                         char mapa[TAM_MAX_LINHA_MAPA][TAM_MAX_COLUNA_MAPA],
                         int qntdLinhas, int qntdColunas) {
  // FANTASMA C
  // move horizontalmente para direita até encontrar a parede, depois move
  // horizontalmente para esquerda até encontrar uma parede (faz isso
  // infinitamente a cada jogada)
  if (fantasmas.posfanXC <= TAM_MAX_LINHA_MAPA) {
    if (fantasmas.flagC == 0) {
      if (mapa[fantasmas.posfanXC][fantasmas.posfanYC - 1] == '#') {
        fantasmas.flagC = 1;
        fantasmas.posfanYC += 1;
      }

      else {
        fantasmas.posfanYC -= 1;
      }
    }

    else {
      if (mapa[fantasmas.posfanXC][fantasmas.posfanYC + 1] == '#') {
        fantasmas.flagC = 0;
        fantasmas.posfanYC -= 1;
      }

      else {
        fantasmas.posfanYC += 1;
      }
    }
  }

  // FANTASMA B
  // move horizontalmente para esquerda até encontrar a parede, depois move
  // horizontalmente para direita até encontrar uma parede (faz isso
  // infinitamente a cada jogada)
  if (fantasmas.posfanXB <= TAM_MAX_LINHA_MAPA) {
    if (fantasmas.flagB == 0) {
      if (mapa[fantasmas.posfanXB][fantasmas.posfanYB + 1] == '#') {
        fantasmas.flagB = 1;
        fantasmas.posfanYB -= 1;
      }

      else {
        fantasmas.posfanYB += 1;
      }
    }

    else {
      if (mapa[fantasmas.posfanXB][fantasmas.posfanYB - 1] == '#') {
        fantasmas.flagB = 0;
        fantasmas.posfanYB += 1;
      }

      else {
        fantasmas.posfanYB -= 1;
      }
    }
  }

  // FANTASMA P
  // move verticalmente para cima até encontrar a parede, depois move
  // verticalmente para baixo até encontrar uma parede (faz isso infinitamente a
  // cada jogada)
  if (fantasmas.posfanXP <= TAM_MAX_LINHA_MAPA) {
    if (fantasmas.flagP == 0) {
      if (mapa[fantasmas.posfanXP + 1][fantasmas.posfanYP] == '#') {
        fantasmas.flagP = 1;
        fantasmas.posfanXP -= 1;
      }

      else {
        fantasmas.posfanXP += 1;
      }
    }

    else {
      if (mapa[fantasmas.posfanXP - 1][fantasmas.posfanYP] == '#') {
        fantasmas.flagP = 0;
        fantasmas.posfanXP += 1;
      }

      else {
        fantasmas.posfanXP -= 1;
      }
    }
  }

  // FANTASMA I
  // move verticalmente para baixo até encontrar a parede, depois move
  // verticalmente para cima até encontrar uma parede (faz isso infinitamente a
  // cada jogada)
  if (fantasmas.posfanXI <= TAM_MAX_LINHA_MAPA) {
    if (fantasmas.flagI == 0) {
      if (mapa[fantasmas.posfanXI - 1][fantasmas.posfanYI] == '#') {
        fantasmas.flagI = 1;
        fantasmas.posfanXI += 1;
      }

      else {
        fantasmas.posfanXI -= 1;
      }
    }

    else {
      if (mapa[fantasmas.posfanXI + 1][fantasmas.posfanYI] == '#') {
        fantasmas.flagI = 0;
        fantasmas.posfanXI -= 1;
      }

      else {
        fantasmas.posfanXI += 1;
      }
    }
  }

  return fantasmas;
}

// funcao limpa posicao inicial do fantasma no mapa
tMapa LimpaFantasmasDoMapa(tMapa mapa) {
  tJogo jogo;
  for (int i = 0; i < mapa.qntdLinhas; i++) {
    for (int j = 0; j < mapa.qntdColunas; j++) {
      if (mapa.mapa[i][j] == 'C' || mapa.mapa[i][j] == 'B' ||
          mapa.mapa[i][j] == 'I' || mapa.mapa[i][j] == 'P') {
        mapa.mapa[i][j] = ' ';
      }
    }
  }

  return mapa;
}

tTrilha InicializaTrilha(int qntdLinhas, int qntdColunas) {
  tTrilha trilha;

  trilha.qntdLinhas = qntdLinhas;
  trilha.qntdColunas = qntdColunas;

  for (int i = 0; i < qntdLinhas; i++) {
    for (int j = 0; j < qntdColunas; j++) {
      trilha.mapa[i][j] = -1;
    }
  }

  return trilha;
}

tTrilha IncrementaTrilha(tJogo jogo, tTrilha trilha, int posXPacman,
                         int posYPacman, int numeroDaJogada) {
  if(!VerificaMortePacman(jogo)){
    trilha.mapa[posYPacman][posXPacman] = numeroDaJogada;
  }

  return trilha;
}

// a funcao le o arquivo do mapa do jogo
tJogo LeJogo(char caminho[1000]) {
  int i, j, contlinha = 0;
  tJogo jogo;
  char caminhofinal[2000];
  sprintf(caminhofinal, "%s/%s", caminho, "mapa.txt");
  FILE *pFile = fopen(caminhofinal, "r");
  if (pFile == NULL) {
    printf("ERRO: Nao foi possivel ler o arquivo %s no diretorio %s\n",
           "mapa.txt", caminhofinal);
    exit(1);
  } else {
    fscanf(pFile, "%d %d %d\n", &jogo.mapa.qntdLinhas, &jogo.mapa.qntdColunas,
           &jogo.mapa.qntdMovimentos);
    while (!feof(pFile)) {
      fscanf(pFile, "%[^\n]\n", jogo.mapa.mapa[contlinha]);
      contlinha++;
    }
  }
  fclose(pFile);

  jogo.estatisticas.colisoesParede = 0;
  jogo.estatisticas.moveBaixo = 0;
  jogo.estatisticas.moveCima = 0;
  jogo.estatisticas.moveDireita = 0;
  jogo.estatisticas.moveEsquerda = 0;
  jogo.estatisticas.numMovimentos = 0;
  jogo.estatisticas.numMovimentosSemPontuar = 0;

  return jogo;
}

// funcao cria um arquivo inicializacao com o mapa informando a posicaoinicial
// do pacman
void ArquivoInicializacao(tJogo jogo, char caminho[1000]) {
  char caminhofinal[2000];
  sprintf(caminhofinal, "%s/%s", caminho, "saida/inicializacao.txt");
  FILE *File = fopen(caminhofinal, "w");
  if (File == NULL) {
    printf("ERRO: Nao foi possivel criar o arquivo %s no diretorio %s\n",
           "inicializacao.txt", caminhofinal);
    exit(1);
  } else {
    for (int i = 0; i < jogo.mapa.qntdLinhas; i++) {
      fprintf(File, "%s\n", jogo.mapa.mapa[i]);
    }
    fprintf(File, "Pac-Man comecara o jogo na linha %d e coluna %d\n",
            jogo.pacman.pos_xInicial, jogo.pacman.pos_yInicial);
    fclose(File);
  }
}

// a funcao verifica se a posicao atual do fantasma é a mesma posicao atual do
// pacman, e se for, significa que o pacman morreu e retorna 1
int VerificaMortePacman(tJogo jogo) {
  if (jogo.pacman.pos_yAtual == jogo.fantasmas.posfanXB &&
      jogo.pacman.pos_xAtual == jogo.fantasmas.posfanYB) {
    return 1;
  }
  if (jogo.pacman.pos_yAtual == jogo.fantasmas.posfanXI &&
      jogo.pacman.pos_xAtual == jogo.fantasmas.posfanYI) {
    return 1;
  }
  if (jogo.pacman.pos_yAtual == jogo.fantasmas.posfanXC &&
      jogo.pacman.pos_xAtual == jogo.fantasmas.posfanYC) {
    return 1;
  }
  if (jogo.pacman.pos_yAtual == jogo.fantasmas.posfanXP &&
      jogo.pacman.pos_xAtual == jogo.fantasmas.posfanYP) {
    return 1;
  }
  return 0;
}

// a funcao varre o mapa e conta quantas comidas existem nele
tJogo ContaComidasMapa(tJogo jogo) {
  int i, j, contcomidas = 0;
  for (i = 0; i < jogo.mapa.qntdLinhas; i++) {
    for (j = 0; j < jogo.mapa.qntdColunas; j++) {
      if (jogo.mapa.mapa[i][j] == '*') {
        contcomidas++;
      }
    }
  }
  jogo.qntdComidas = contcomidas;
  return jogo;
}

// a funcao verifica se o jogo acabou, e se tiver acabado retorna 1
int VerificaSeJogoAcabou(tJogo jogo) {
  if (jogo.qntdComidas == 0) { // se a qntd de comidas for igual a 0
    return 1;
  } else if (VerificaMortePacman(jogo) ==
             1) { // se o pacman tiver morrido (colidido com um fantasma)
    return 1;
  } else if (jogo.movimentosJogados ==
             jogo.mapa
                 .qntdMovimentos) { // se chegar aomáximo de movimentos jogados
    return 1;
  }
  return 0;
}

// funcao imprime o jogo no terminal
void ImprimeJogo(tJogo jogo) {
  int i, j;
  if (jogo.movimentosJogados != 0) {
    printf("Estado do jogo apos o movimento '%c':\n", jogo.pacman.jogada);
  }
  for (i = 0; i < jogo.mapa.qntdLinhas; i++) {
    for (j = 0; j < jogo.mapa.qntdColunas; j++) {
      if (i == jogo.fantasmas.posfanXB && j == jogo.fantasmas.posfanYB) {
        printf("B");
      }

      else if (i == jogo.fantasmas.posfanXC && j == jogo.fantasmas.posfanYC) {
        printf("C");
      }

      else if (i == jogo.fantasmas.posfanXP && j == jogo.fantasmas.posfanYP) {
        printf("P");
      }

      else if (i == jogo.fantasmas.posfanXI && j == jogo.fantasmas.posfanYI) {
        printf("I");
      }

      else if (i == jogo.pacman.pos_yAtual && j == jogo.pacman.pos_xAtual) {
        printf(">");
      }

      else if (i == jogo.pacman.pos_yDiferente && j == jogo.pacman.pos_xDiferente){
      	printf("&");
      }
      
      else {
        printf("%c", jogo.mapa.mapa[i][j]);
      }
    }
    printf("\n");
  }
  printf("Pontuacao: %d", jogo.pacman.comidaspacman);
  printf("\n\n");
}

// funcao contabiliza toda vez que o pacman assume a posicao da comida e depois
// imprime um espaco em branco para indicar que a comida foi comida pelo pacman
tJogo PacmanComeComidas(tJogo jogo) {
  if (jogo.mapa.mapa[jogo.pacman.pos_yAtual][jogo.pacman.pos_xAtual] == '*' && !VerificaMortePacman(jogo)) {
    jogo.mapa.mapa[jogo.pacman.pos_yAtual][jogo.pacman.pos_xAtual] = ' ';
    jogo.pacman.comidaspacman++;
    jogo.qntdComidas--;
  }
  if (jogo.mapa.mapa[jogo.pacman.pos_yAtual][jogo.pacman.pos_xAtual] == '%' && !VerificaMortePacman(jogo)) {
    jogo.mapa.mapa[jogo.pacman.pos_yAtual][jogo.pacman.pos_xAtual] = ' ';
    jogo.pacman.comidaspacman++;
    jogo.pacman.comidaspacman++;
    jogo.pacman.comidaspacman++;
    jogo.pacman.comidaspacman++;
    jogo.pacman.comidaspacman++;
  }
  if (jogo.mapa.mapa[jogo.pacman.pos_yAtual][jogo.pacman.pos_xAtual] == '!' && !VerificaMortePacman(jogo)) {
    jogo.mapa.mapa[jogo.pacman.pos_yAtual][jogo.pacman.pos_xAtual] = ' ';
  }
  return jogo;
}

// funcao some com a posicao inicial do pacman
tJogo SumirPacmanInicial(tJogo jogo) {
  jogo.mapa.mapa[jogo.pacman.pos_xInicial - 1][jogo.pacman.pos_yInicial - 1] = ' ';
  
  return jogo;
}

tJogo IncrementaEstatistica(tJogo jogo){
  if(jogo.pacman.jogada == 'w'){
    jogo.estatisticas.moveCima++;
  }

  else if(jogo.pacman.jogada == 's'){
    jogo.estatisticas.moveBaixo++;
  }

  else if(jogo.pacman.jogada == 'a'){
    jogo.estatisticas.moveEsquerda++;
  }

  else if(jogo.pacman.jogada == 'd'){
    jogo.estatisticas.moveDireita++;
  }

  if (jogo.mapa.mapa[jogo.pacman.pos_yAtual][jogo.pacman.pos_xAtual] != '*') {
    jogo.estatisticas.numMovimentosSemPontuar++; 
  }

  if(jogo.pacman.movimentou == 0){
    jogo.estatisticas.colisoesParede++;
  }

  jogo.estatisticas.numMovimentos++;

  return jogo;
}

// cria o arquivo resumo e escreve o resumo do jogo
void ArquivoResumo(tJogo jogo, char caminho[1000]) {
  char caminhofinal[2000];
  sprintf(caminhofinal, "%s/%s", caminho, "saida/resumo.txt");
  FILE *reFile = fopen(caminhofinal, "a+");

  if (jogo.mapa.mapa[jogo.pacman.pos_yAtual][jogo.pacman.pos_xAtual] == '*' &&
      !VerificaMortePacman(jogo)) { // diz quando o pacman come a comida
    fprintf(reFile, "Movimento %d (%c) pegou comida\n",
            jogo.movimentosJogados, // e qual foi a jogada e o numero da jogada
            jogo.pacman.jogada);
  }
  if (((jogo.mapa.mapa[jogo.pacman.pos_yAtual - 1]
                      [jogo.pacman.pos_xAtual] == // diz quando o pacman colide
                                                  // com a parede
            '#' && // e qual foi a jogada e o numero da jogada
        jogo.pacman.jogada == 'w' &&
        jogo.pacman.movimentou == 0) ||
       (jogo.mapa.mapa[jogo.pacman.pos_yAtual + 1][jogo.pacman.pos_xAtual] ==
            '#' &&
        jogo.pacman.jogada == 's' && jogo.pacman.movimentou == 0) ||
       (jogo.mapa.mapa[jogo.pacman.pos_yAtual][jogo.pacman.pos_xAtual + 1] ==
            '#' &&
        jogo.pacman.jogada == 'd' && jogo.pacman.movimentou == 0) ||
       (jogo.mapa.mapa[jogo.pacman.pos_yAtual][jogo.pacman.pos_xAtual - 1] ==
            '#' &&
        jogo.pacman.jogada == 'a' && jogo.pacman.movimentou == 0)) &&
      VerificaMortePacman(jogo) == 0) {
    fprintf(reFile, "Movimento %d (%c) colidiu na parede\n",
            jogo.movimentosJogados, jogo.pacman.jogada);
  }
  if (VerificaMortePacman(jogo) ==
      1) { // diz quando o pacman colide com o fantasma, qual a jogada e o
           // numero dajogada
    fprintf(reFile,
            "Movimento %d (%c) fim de jogo por encostar em um fantasma\n",
            jogo.movimentosJogados, jogo.pacman.jogada);
  }
  fclose(reFile);
}

// cria o arquivo ranking
void ArquivoRanking(tJogo jogo, char caminho[1000]) {
  char caminhofinal[2000];
  sprintf(caminhofinal, "%s/%s", caminho, "saida/ranking.txt");
  FILE *raFile = fopen(caminhofinal, "w");

  fclose(raFile);
}

// cria o arquivo estatistica
void ArquivoEstatistica(tJogo jogo, char caminho[1000]) {
  char caminhofinal[2000];
  sprintf(caminhofinal, "%s/%s", caminho, "saida/estatisticas.txt");
  FILE *eFile = fopen(caminhofinal, "w");

  if(eFile == NULL){
    printf("Erro ao abrir arquivos de estatistica\n");
    exit(1);
  }

  else{
    fprintf(eFile, "Numero de movimentos: %d\n", jogo.estatisticas.numMovimentos);
    fprintf(eFile, "Numero de movimentos sem pontuar: %d\n", jogo.estatisticas.numMovimentosSemPontuar);
    fprintf(eFile, "Numero de colisoes com parede: %d\n", jogo.estatisticas.colisoesParede);
    fprintf(eFile, "Numero de movimentos para baixo: %d\n", jogo.estatisticas.moveBaixo);
    fprintf(eFile, "Numero de movimentos para cima: %d\n", jogo.estatisticas.moveCima);
    fprintf(eFile, "Numero de movimentos para esquerda: %d\n", jogo.estatisticas.moveEsquerda);
    fprintf(eFile, "Numero de movimentos para direita: %d\n", jogo.estatisticas.moveDireita);
  }

  fclose(eFile);
}

// cria o arquivo trilha
void ArquivoTrilha(tJogo jogo, char caminho[1000]) {
  char caminhofinal[2000];
  sprintf(caminhofinal, "%s/%s", caminho, "saida/trilha.txt");
  FILE *tFile = fopen(caminhofinal, "w+");

  if (tFile == NULL) {
    printf("Erro na abertura de arquivo (TRILHA)\n");
    exit(1);
  }

  else {
    for (int i = 0; i < jogo.trilha.qntdLinhas; i++) {
      for (int j = 0; j < jogo.trilha.qntdColunas; j++) {
        if (jogo.trilha.mapa[i][j] == -1 && j + 1 < jogo.trilha.qntdColunas) {
          fprintf(tFile, "# ");
        } else if (jogo.trilha.mapa[i][j] == -1 &&
                   j + 1 == jogo.trilha.qntdColunas) {
          fprintf(tFile, "#");
        }

        else if (jogo.trilha.mapa[i][j] != -1 &&
                 j + 1 < jogo.trilha.qntdColunas) {
          fprintf(tFile, "%d ", jogo.trilha.mapa[i][j]);
        }

        else {
          fprintf(tFile, "%d", jogo.trilha.mapa[i][j]);
        }
      }

      fprintf(tFile, "\n");
    }
  }
  fclose(tFile);
}

void ArquivoSaida(tJogo jogo, char caminho[1000]){
  char caminhofinal[2000];
  sprintf(caminhofinal, "%s/%s", caminho, "saida/saida.txt");
  FILE *tFile = fopen(caminhofinal, "a+");

  if (tFile == NULL) {
    printf("Erro na abertura de arquivo (TRILHA)\n");
    exit(1);
  }

  else {
    int i, j;
  if (jogo.movimentosJogados != 0) {
    fprintf(tFile, "Estado do jogo apos o movimento '%c':\n", jogo.pacman.jogada);
  }
  for (i = 0; i < jogo.mapa.qntdLinhas; i++) {
    for (j = 0; j < jogo.mapa.qntdColunas; j++) {
      if (i == jogo.fantasmas.posfanXB && j == jogo.fantasmas.posfanYB) {
        fprintf(tFile, "B");
      }

      else if (i == jogo.fantasmas.posfanXC && j == jogo.fantasmas.posfanYC) {
        fprintf(tFile, "C");
      }

      else if (i == jogo.fantasmas.posfanXP && j == jogo.fantasmas.posfanYP) {
        fprintf(tFile, "P");
      }

      else if (i == jogo.fantasmas.posfanXI && j == jogo.fantasmas.posfanYI) {
        fprintf(tFile, "I");
      }

      else if (i == jogo.pacman.pos_yAtual && j == jogo.pacman.pos_xAtual) {
        fprintf(tFile, ">");
      }

      else {
        fprintf(tFile, "%c", jogo.mapa.mapa[i][j]);
      }
    }
    fprintf(tFile, "\n");
  }
  fprintf(tFile, "Pontuacao: %d\n\n", jogo.pacman.comidaspacman);
  }

  fclose(tFile);
}

void ResultadoArquivoSaida(tJogo jogo, char caminho[1000]){
  char caminhofinal[2000];
  sprintf(caminhofinal, "%s/%s", caminho, "saida/saida.txt");
  FILE *tFile = fopen(caminhofinal, "a+");

  if (tFile == NULL) {
    printf("Erro na abertura de arquivo (SAIDA)\n");
    exit(1);
  }

  else {
    if (jogo.qntdComidas == 0) { // se a qntd de comidas for igual a 0
    fprintf(tFile, "Voce venceu!\n");
    fprintf(tFile, "Pontuacao final: %d\n", jogo.pacman.comidaspacman);
  } else if (VerificaMortePacman(jogo) ==
             1) { // se o pacman tiver morrido (colidido com um fantasma)
    fprintf(tFile, "Game over!\n");
    fprintf(tFile, "Pontuacao final: %d\n", jogo.pacman.comidaspacman);
  } else if (jogo.movimentosJogados ==
             jogo.mapa
                 .qntdMovimentos) { // se chegar aomáximo de movimentos jogados
    fprintf(tFile, "Game over!\n");
    fprintf(tFile, "Pontuacao final: %d\n", jogo.pacman.comidaspacman);
  }
  }

  fclose(tFile);
}

// funcao principal
int main(int argc, char **argv) {
  // verifica se algum diretorio foi informado, em caso negativo, o programa e
  // finalizado, caso contrario, e inicializado
  if (argc <= 1) {
    printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
    return 1;
  }

  tJogo jogo = LeJogo(argv[1]);
  jogo.pacman = EncontrarPacman(jogo.mapa.mapa, jogo.mapa.qntdLinhas,
                                jogo.mapa.qntdColunas);

  jogo.fantasmas = EncontrarFantasmas(jogo.mapa.mapa, jogo.mapa.qntdLinhas,
                                      jogo.mapa.qntdColunas);
  jogo = ContaComidasMapa(jogo);
  jogo.movimentosJogados = 0;

  ArquivoInicializacao(jogo, argv[1]);
  jogo.trilha = InicializaTrilha(jogo.mapa.qntdLinhas, jogo.mapa.qntdColunas);
  jogo.trilha =
      IncrementaTrilha(jogo, jogo.trilha, jogo.pacman.pos_xAtual,
                       jogo.pacman.pos_yAtual, jogo.movimentosJogados);
  jogo.mapa = LimpaFantasmasDoMapa(jogo.mapa);
  jogo = SumirPacmanInicial(jogo);
  jogo.pacman.comidaspacman = 0;

  while (!VerificaSeJogoAcabou(jogo)) {
    jogo.pacman = DeslocaPacman(jogo.pacman, jogo.mapa.mapa);
    jogo.fantasmas = MoveFantasmas(jogo.fantasmas, jogo.mapa.mapa,
                                   jogo.mapa.qntdLinhas, jogo.mapa.qntdColunas);
    jogo = IncrementaEstatistica(jogo);
    jogo.movimentosJogados++;
    jogo.trilha =
        IncrementaTrilha(jogo, jogo.trilha, jogo.pacman.pos_xAtual,
                         jogo.pacman.pos_yAtual, jogo.movimentosJogados);
    ArquivoResumo(jogo, argv[1]);

    jogo = PacmanComeComidas(jogo);
    ArquivoTrilha(jogo, argv[1]);
    //ImprimeJogo(jogo);
    ArquivoSaida(jogo, argv[1]);
  }

  if (jogo.mapa.mapa[jogo.pacman.pos_yAtual][jogo.pacman.pos_xAtual] == '*' && VerificaMortePacman(jogo)) {
    jogo.estatisticas.numMovimentosSemPontuar++;
  }

  ResultadoArquivoSaida(jogo, argv[1]);
  ArquivoRanking(jogo, argv[1]);
  ArquivoEstatistica(jogo, argv[1]);
  return 0;
}
