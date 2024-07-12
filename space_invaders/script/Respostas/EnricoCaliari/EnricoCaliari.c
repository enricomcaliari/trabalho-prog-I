#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// definicao do tamanho maximo do mapa e do diretorio
#define MAX_LINHA_MAPA 200
#define MAX_COLUNA_MAPA 200
#define MAX_DIRETORIO 1100

// definicao do tipo tMapa
typedef struct
{
  char mapa[MAX_LINHA_MAPA][MAX_COLUNA_MAPA];
  int qtd_linhas;
  int qtd_colunas;
} tMapa;

// definicao do tipo tNave
typedef struct
{
  int linha, coluna;
} tNave;

// definicao do tipo tInimigo
typedef struct
{
  int linha, coluna;
  int morreu, iteracao_morte;
  int fileira, indice;
} tInimigo;

// definicao do tipo tMovimento
typedef struct
{
  char jogada;
  int pontos;
  int iteracao;
  int total_movimentos_AD;
} tMovimento;

// definicao do tipo tTiro
typedef struct
{
  int linha, coluna;
  int ativo;
  int tiros_efetivos;
  int tiros_nao_efetivos;
} tTiro;

// definicao do tipo tRanking
typedef struct
{
  int linha;
  int iteracao;
  int fileira, indice;
} tRanking;

// definicao do tipo tJogo
typedef struct
{
  tMapa mapa;
  tNave nave;
  tInimigo inimigo[20];
  tRanking ranking[20];
  int inimigosDescidas;
  int inimigosRestantes;
  int nInimigos;
  int bonus;
  int terminou;
  int descer;     // 1 se os inimigos vao se mover verticalmente, 0 caso contrario
  int horizontal; // 0 para direita, 1 para esquerda
  char inimigoPadrao[3][3];
  char inimigoAnimacao1[3][3];
  char inimigoAnimacao2[3][3];
  tMovimento movimento;
  tTiro tiro;
  int heatmap[MAX_LINHA_MAPA][MAX_COLUNA_MAPA];
} tJogo;

// FUNCOES

/**
 * @brief Atribui cada coordenada do mapa a um caractere: bordas do mapa, espacos vazios,
 * inimigos (com ou sem animacao), nave do jogador e o tiro da nave.
 * @param jogo recebe o jogo para atualizar o mapa
 * @return tJogo retorna o jogo com o mapa atualizado
 */
tJogo DefineMapa(tJogo jogo);

/**
 * @brief Move todos os inimigos vivos uma posicao para direita, para esquerda ou para baixo a
 * depender dos inteiros i e j.
 * @param i Valor somado a linha: 0 ou 1.
 * @param j Valor somado a coluna: 0, 1, ou -1.
 * @param jogo recebe o jogo para atualizar a posicao dos inimigos vivos
 * @return tJogo retorna o jogo com a posicao dos inimigos vivos atualziada
 */
tJogo MoveInimigo(int i, int j, tJogo jogo);

/**
 * @brief Verifica se algum inimigo vivo esta em uma das laterais do mapa, podendo alterar seu
 * movimento. Se algum inimigo estiver na lateral esquerda ou na lateral direita, o movimento
 * horizontal de todos os inimigo vivos eh invertido e o vertical eh ativado.
 * @param jogo recebe o jogo para atualizar o tipo de movimento dos inimigos
 * @return tJogo retorna o jogo com o tipo de movimento dos inimigos atualizado
 */
tJogo VerificaParedeInimigo(tJogo jogo);

/**
 * @brief Verifica se algum inimigo vivo ultrapassou a linha limite.
 * @param jogo recebe o jogo para a verificacao
 * @return int retorna 1 se verdadeiro, 0 se falso
 */
int InimigoCruzouLinha(tJogo jogo);

/**
 * @brief Imprime o mapa no arquivo saida.txt.
 * @param jogo recebe o mapa do jogo
 * @param saida_txt onde o mapa sera impresso
 */
void PrintaMapa(tJogo jogo, FILE *saida_txt);

/**
 * @brief Move a nave do jogador ou atira a depender do movimento lido. Tambem verifica se o
 * jogador colidiu em uma lateral do mapa e, caso verdadeiro, imprime a colisao no arquivo
 * resumo.txt.
 * @param jogo recebe o jogo para atualizar a posicao ou o tiro da nave.
 * @param resumo_txt caso exista colisao, onde ela sera impressa.
 * @return tJogo retorna o jogo com a posicao ou o tiro da nave atualizado.
 */
tJogo MoveNave(tJogo jogo, FILE *resumo_txt);

/**
 * @brief A nave do jogador atira caso nao haja um tiro ativo, ativando o tiro e atualizando
 * o numero de tiros efetivos.
 * @param jogo recebe o jogo para atualizar o tiro.
 * @return tJogo retorna o jogo com o tiro atualizado.
 */
tJogo NaveAtira(tJogo jogo);

/**
 * @brief Se houver um tiro ativo, move-o uma posicao acima no mapa. Se o tiro atingir o topo
 * do mapa, o tiro eh inativado e atualiza-se o numero de tiros nao efetivos.
 * @param jogo recebe o jogo para atualizar a posicao do tiro.
 * @return tJogo retorna o jogo com a posicao do tiro atualizada.
 */
tJogo MoveTiro(tJogo jogo);

/**
 * @brief Se houver um tiro ativo, verifica se o tiro atingiu algum inimigo vivo. Se for o caso,
 * mata o inimigo, diminui a quantidade de inimigos restantes, inativa o tiro, soma os pontos e
 * imprime as informacoes no arquivo resumo.txt.
 * @param jogo recebe o jogo para a verificacao.
 * @param resumo_txt onde a morte do inimigo sera impressa.
 * @return tJogo retorna o jogo com os inimigos atualizados.
 */
tJogo MatouInimigo(tJogo jogo, FILE *resumo_txt);

/**
 * @brief Atualiza o heatmap de acordo com a movimentacao da nave do jogador e dos tiros.
 * @param jogo recebe o jogo para atualizar o heatmap.
 * @return tJogo retorna o jogo com o heatmap atualizado.
 */
tJogo AtualizaHeatmap(tJogo jogo);

/**
 * @brief Realiza a leitura das informacoes dos arquivos mapa.txt e inimigo.txt localizados
 * no diretorio argv fornecido e as atribui as respectivas variaveis. Tambem gera o arquivo
 * inicializacao.txt.
 * @param argv diretorio fornecido ao executar o programa.
 * @return tJogo retorna o jogo inicializado.
 */
tJogo InicializaJogo(char **argv);

/**
 * @brief Realiza o jogo efetivamente. Le as informacoes do arquivo entrada.txt localizado no
 * diretorio argv fornecido e as atribui as respectivas variaveis. Tambem gera os arquivos
 * estatisticas.txt, heatmap.txt, ranking.txt, resumo.txt e saida.txt.
 * @param jogo recebe o jogo inicializado.
 * @param argv diretorio fornecido ao executar o programa.
 */
void RealizaJogo(tJogo jogo, char **argv);

/**
 * @brief Imprime as estatisticas no arquivo estatisticas.txt.
 * @param jogo recebe as estatisticas do jogo.
 * @param estatisticas_txt onde as estatisticas serao impressas.
 */
void PrintaEstatisticas(tJogo jogo, FILE *estatisticas_txt);

/**
 * @brief Imprime o ranking no arquivo ranking.txt.
 * @param jogo recebe o ranking do jogo.
 * @param ranking_txt onde o ranking sera impresso.
 */
void PrintaRanking(tJogo jogo, FILE *ranking_txt);

/**
 * @brief Imprime o heatmap no arquivo heatmap.txt.
 * @param jogo recebe o heatmap do jogo.
 * @param heatmap_txt onde o heatmap sera impresso.
 */
void PrintaHeatmap(tJogo jogo, FILE *heatmap_txt);

tJogo DefineMapa(tJogo jogo)
{
  for (int i = 0; i < jogo.mapa.qtd_linhas; i++)
  {
    for (int j = 0; j < jogo.mapa.qtd_colunas; j++)
    {
      if ((i == 0 || i == jogo.mapa.qtd_linhas - 1) && !(j == 0 || j == jogo.mapa.qtd_colunas - 1))
      {
        jogo.mapa.mapa[i][j] = '-';
      }

      else if ((i == 0 || i == jogo.mapa.qtd_linhas - 1) && (j == 0 || j == jogo.mapa.qtd_colunas - 1))
      {
        jogo.mapa.mapa[i][j] = '+';
      }

      else if (!(i == 0 || i == jogo.mapa.qtd_linhas - 1) && (j == 0 || j == jogo.mapa.qtd_colunas - 1))
      {
        jogo.mapa.mapa[i][j] = '|';
      }
      else
        jogo.mapa.mapa[i][j] = ' ';
    }
  }
  jogo.mapa.mapa[jogo.nave.linha - 3][0] = '-';
  jogo.mapa.mapa[jogo.nave.linha - 3][jogo.mapa.qtd_colunas - 1] = '-';

  for (int i = -1; i < 2; i++)
  {
    for (int j = -1; j < 2; j++)
    {
      if (i == -1 && j == 0)
      {
        continue;
      }
      else
      {
        jogo.mapa.mapa[jogo.nave.linha + i - 1][jogo.nave.coluna + j - 1] = 'M';
      }
    }
  }

  if (jogo.bonus == 0)
  {
    for (int k = 0; k < jogo.nInimigos; k++)
    {
      if (jogo.inimigo[k].morreu == 0)
      {
        for (int i = -1; i < 2; i++)
        {
          for (int j = -1; j < 2; j++)
          {
            jogo.mapa.mapa[jogo.inimigo[k].linha + i - 1][jogo.inimigo[k].coluna + j - 1] = jogo.inimigoPadrao[i + 1][j + 1];
          }
        }
      }
    }
  }
  else
  {
    if (jogo.movimento.iteracao % 3 == 0)
    {
      for (int k = 0; k < jogo.nInimigos; k++)
      {
        if (jogo.inimigo[k].morreu == 0)
        {
          for (int i = -1; i < 2; i++)
          {
            for (int j = -1; j < 2; j++)
            {
              jogo.mapa.mapa[jogo.inimigo[k].linha + i - 1][jogo.inimigo[k].coluna + j - 1] = jogo.inimigoPadrao[i + 1][j + 1];
            }
          }
        }
      }
    }
    else if (jogo.movimento.iteracao % 3 == 1)
    {
      for (int k = 0; k < jogo.nInimigos; k++)
      {
        if (jogo.inimigo[k].morreu == 0)
        {
          for (int i = -1; i < 2; i++)
          {
            for (int j = -1; j < 2; j++)
            {
              jogo.mapa.mapa[jogo.inimigo[k].linha + i - 1][jogo.inimigo[k].coluna + j - 1] = jogo.inimigoAnimacao1[i + 1][j + 1];
            }
          }
        }
      }
    }
    else
    {
      for (int k = 0; k < jogo.nInimigos; k++)
      {
        if (jogo.inimigo[k].morreu == 0)
        {
          for (int i = -1; i < 2; i++)
          {
            for (int j = -1; j < 2; j++)
            {
              jogo.mapa.mapa[jogo.inimigo[k].linha + i - 1][jogo.inimigo[k].coluna + j - 1] = jogo.inimigoAnimacao2[i + 1][j + 1];
            }
          }
        }
      }
    }
  }

  if (jogo.tiro.ativo == 1)
  {
    jogo.mapa.mapa[jogo.tiro.linha][jogo.tiro.coluna] = 'o';
  }

  return jogo;
}

tJogo MoveInimigo(int i, int j, tJogo jogo)
{
  for (int k = 0; k < jogo.nInimigos; k++)
  {
    if (jogo.inimigo[k].morreu)
    {
      continue;
    }
    jogo.inimigo[k].linha += i;
    jogo.inimigo[k].coluna += j;
  }
  return jogo;
}

tJogo VerificaParedeInimigo(tJogo jogo)
{
  for (int k = 0; k < jogo.nInimigos; k++)
  {
    if (jogo.inimigo[k].morreu)
    {
      continue;
    }
    else if (jogo.horizontal == 0 && jogo.inimigo[k].coluna + 2 == jogo.mapa.qtd_colunas)
    {
      jogo.horizontal = 1;
      jogo.descer = 1;
      return jogo;
    }
    else if (jogo.horizontal == 1 && jogo.inimigo[k].coluna - 2 == 1)
    {
      jogo.horizontal = 0;
      jogo.descer = 1;
      return jogo;
    }
  }
  return jogo;
}

int InimigoCruzouLinha(tJogo jogo)
{
  for (int k = 0; k < jogo.nInimigos; k++)
  {
    if (jogo.inimigo[k].morreu == 0 && jogo.inimigo[k].linha == jogo.nave.linha - 2)
    {
      return 1;
    }
  }
  return 0;
}

void PrintaMapa(tJogo jogo, FILE *saida_txt)
{
  fprintf(saida_txt, "Pontos: %d | Iteracoes: %d\n", jogo.movimento.pontos, jogo.movimento.iteracao);
  for (int i = 0; i < jogo.mapa.qtd_linhas; i++)
  {
    for (int j = 0; j < jogo.mapa.qtd_colunas; j++)
    {
      fprintf(saida_txt, "%c", jogo.mapa.mapa[i][j]);
    }
    fprintf(saida_txt, "\n");
  }
}

tJogo MoveNave(tJogo jogo, FILE *resumo_txt)
{
  if (jogo.movimento.jogada == 'a')
  {
    if (jogo.nave.coluna - 2 == 1)
    {
      fprintf(resumo_txt, "[%d] Jogador colidiu na lateral esquerda.\n", jogo.movimento.iteracao);
      return jogo;
    }
    else
    {
      jogo.movimento.total_movimentos_AD++;
      jogo.nave.coluna--;
      return jogo;
    }
  }
  else if (jogo.movimento.jogada == 'd')
  {
    if (jogo.nave.coluna + 2 == jogo.mapa.qtd_colunas)
    {
      fprintf(resumo_txt, "[%d] Jogador colidiu na lateral direita.\n", jogo.movimento.iteracao);
      return jogo;
    }
    else
    {
      jogo.movimento.total_movimentos_AD++;
      jogo.nave.coluna++;
      return jogo;
    }
  }
  else if (jogo.movimento.jogada == 's')
  {
    return jogo;
  }
  else
  {
    jogo = NaveAtira(jogo);
    return jogo;
  }
}

tJogo NaveAtira(tJogo jogo)
{
  if (jogo.tiro.ativo == 1)
  {
    return jogo;
  }
  else
  {
    jogo.tiro.coluna = jogo.nave.coluna - 1;
    jogo.tiro.linha = jogo.nave.linha - 3;
    jogo.tiro.ativo = 1;
    jogo.tiro.tiros_efetivos++;
    return jogo;
  }
}

tJogo MoveTiro(tJogo jogo)
{
  if (jogo.tiro.ativo == 1)
  {
    jogo.tiro.linha--;
    if (jogo.tiro.linha == 0)
    {
      jogo.tiro.ativo = 0;
      jogo.tiro.tiros_nao_efetivos++;
      return jogo;
    }
    return jogo;
  }
  else
    return jogo;
}

tJogo MatouInimigo(tJogo jogo, FILE *resumo_txt)
{
  if (jogo.tiro.ativo)
  {
    for (int k = 0; k < jogo.nInimigos; k++)
    {
      for (int i = -1; i < 2; i++)
      {
        for (int j = -1; j < 2; j++)
        {
          if (jogo.tiro.linha == jogo.inimigo[k].linha + i - 1 &&
              jogo.tiro.coluna == jogo.inimigo[k].coluna + j - 1 && jogo.inimigo[k].morreu == 0)
          {
            jogo.inimigo[k].morreu = 1;
            jogo.inimigosRestantes--;
            jogo.tiro.ativo = 0;
            jogo.movimento.pontos += (jogo.inimigo[k].coluna - 1) * (jogo.mapa.qtd_linhas - jogo.inimigo[k].linha - 1);
            jogo.inimigo[k].iteracao_morte = jogo.movimento.iteracao;
            fprintf(resumo_txt, "[%d] Inimigo de indice %d da fileira %d foi atingido na posicao (%d %d).\n", jogo.movimento.iteracao, jogo.inimigo[k].indice + 1, jogo.inimigo[k].fileira + 1, jogo.inimigo[k].coluna + j - 1, jogo.inimigo[k].linha + i - 1);
            jogo.inimigo[k].linha = jogo.tiro.linha;
            jogo.tiro.linha = 0;
            jogo.tiro.coluna = 0;
            return jogo;
          }
        }
      }
    }
  }
  return jogo;
}

tJogo AtualizaHeatmap(tJogo jogo)
{
  for (int i = -1; i < 2; i++)
  {
    for (int j = -1; j < 2; j++)
    {
      jogo.heatmap[jogo.nave.linha + i - 1][jogo.nave.coluna + j - 1] += 1;
    }
  }
  if (jogo.tiro.linha != 0 && jogo.tiro.coluna != 0)
  {
    jogo.heatmap[jogo.tiro.linha][jogo.tiro.coluna] += 1;
  }
  return jogo;
}

void PrintaEstatisticas(tJogo jogo, FILE *estatisticas_txt)
{
  fprintf(estatisticas_txt, "Numero total de movimentos (A ou D): %d;\n", jogo.movimento.total_movimentos_AD);
  fprintf(estatisticas_txt, "Numero de tiros efetivos: %d;\n", jogo.tiro.tiros_efetivos);
  fprintf(estatisticas_txt, "Numero de tiros que nao acertaram: %d;\n", jogo.tiro.tiros_nao_efetivos);
  fprintf(estatisticas_txt, "Numero de descidas dos inimigos: %d;\n", jogo.inimigosDescidas);
  return;
}

void PrintaRanking(tJogo jogo, FILE *ranking_txt)
{
  int count = 0;
  for (int k = 0; k < jogo.nInimigos; k++)
  {
    if (jogo.inimigo[k].morreu)
    {
      jogo.ranking[count].indice = jogo.inimigo[k].indice;
      jogo.ranking[count].fileira = jogo.inimigo[k].fileira;
      jogo.ranking[count].linha = jogo.inimigo[k].linha;
      jogo.ranking[count].iteracao = jogo.inimigo[k].iteracao_morte;
      count++;
    }
  }
  for (int i = 0; i < count; i++)
  {
    for (int j = 0; j < count; j++)
    {
      if (jogo.ranking[i].linha > jogo.ranking[j].linha)
      {
        tRanking temp = jogo.ranking[j];
        jogo.ranking[j] = jogo.ranking[i];
        jogo.ranking[i] = temp;
      }
      else if (jogo.ranking[i].linha == jogo.ranking[j].linha && jogo.ranking[i].iteracao < jogo.ranking[j].iteracao)
      {
        tRanking temp = jogo.ranking[j];
        jogo.ranking[j] = jogo.ranking[i];
        jogo.ranking[i] = temp;
      }
    }
  }
  for (int k = 0; k < count; k++)
  {
    fprintf(ranking_txt, "%d,%d,%d,%d\n", jogo.ranking[k].indice + 1, jogo.ranking[k].fileira + 1, jogo.mapa.qtd_linhas - jogo.ranking[k].linha - 1, jogo.ranking[k].iteracao);
  }
  return;
}

void PrintaHeatmap(tJogo jogo, FILE *heatmap_txt)
{
  for (int i = 1; i < jogo.mapa.qtd_linhas - 1; i++)
  {
    for (int j = 1; j < jogo.mapa.qtd_colunas - 1; j++)
    {
      fprintf(heatmap_txt, " %3d", jogo.heatmap[i][j]);
    }
    fprintf(heatmap_txt, " \n");
  }
  return;
}

tJogo InicializaJogo(char **argv)
{
  // LEITURA DO mapa.txt
  FILE *mapa_txt;
  char erro_diretorio[MAX_DIRETORIO];
  sprintf(erro_diretorio, "%s", argv[1]);
  char diretorio_mapa[MAX_DIRETORIO];
  sprintf(diretorio_mapa, "%s/mapa.txt", argv[1]);
  mapa_txt = fopen(diretorio_mapa, "r");
  if (mapa_txt == NULL)
  {
    printf("ERRO: nao foi possivel ler o arquivo mapa.txt localizado no diretorio %s\n", erro_diretorio);
    exit(1);
  }

  // inicializacao de variaveis
  tJogo jogo;
  jogo.terminou = 0;
  int linha, coluna;
  jogo.nInimigos = 0;

  // leitura do tamanho do mapa
  fscanf(mapa_txt, "%d %d%*c", &coluna, &linha);
  jogo.mapa.qtd_colunas = coluna + 2;
  jogo.mapa.qtd_linhas = linha + 2;

  // leitura da coordenada da nave
  fscanf(mapa_txt, "(%d %d)%*c", &coluna, &linha);
  jogo.nave.coluna = coluna + 1;
  jogo.nave.linha = linha + 1;

  // leitura da coordenada dos inimigos(se tiver)
  char x, string[10];
  int j;
  for (int i = 0; i < 3; i++)
  {
    fscanf(mapa_txt, "%c", &x);
    if (x == '\n')
    {
      continue;
    }
    fscanf(mapa_txt, "%d %d)", &coluna, &linha);
    jogo.inimigo[jogo.nInimigos].linha = linha + 1;
    jogo.inimigo[jogo.nInimigos].coluna = coluna + 1;
    jogo.inimigo[jogo.nInimigos].fileira = i;
    j = 0;
    jogo.inimigo[jogo.nInimigos].indice = j;
    j++;
    jogo.nInimigos++;
    while (fscanf(mapa_txt, "%c", &x))
    {
      if (x == '\n')
      {
        break;
      }
      fscanf(mapa_txt, "%[^)]%*c", string);
      sscanf(string, "(%d %d", &coluna, &linha);
      jogo.inimigo[jogo.nInimigos].linha = linha + 1;
      jogo.inimigo[jogo.nInimigos].coluna = coluna + 1;
      jogo.inimigo[jogo.nInimigos].fileira = i;
      jogo.inimigo[jogo.nInimigos].indice = j;
      j++;
      jogo.nInimigos++;
    }
  }
  fclose(mapa_txt);

  // se nao tiver inimigos, o jogo termina
  if (jogo.nInimigos == 0)
  {
    jogo.terminou = 1;
  }

  // LEITURA DO inimigo.txt
  FILE *inimigo_txt;
  char diretorio_inimigo[MAX_DIRETORIO];
  sprintf(diretorio_inimigo, "%s/inimigo.txt", argv[1]);
  inimigo_txt = fopen(diretorio_inimigo, "r");
  if (inimigo_txt == NULL)
  {
    printf("ERRO: nao foi possivel ler o arquivo inimigo.txt localizado no diretorio %s\n", erro_diretorio);
    exit(1);
  }

  // leitura da flag (com ou sem animacao)
  int bonus;
  fscanf(inimigo_txt, "%d%*c", &bonus);
  jogo.bonus = bonus;

  // leitura do desenho dos inimigos
  for (int j = 0; j < 3; j++)
  {
    for (int k = 0; k < 3; k++)
    {
      fscanf(inimigo_txt, "%c", &jogo.inimigoPadrao[j][k]);
    }
    fscanf(inimigo_txt, "%*c");
  }
  for (int j = 0; j < 3; j++)
  {
    for (int k = 0; k < 3; k++)
    {
      fscanf(inimigo_txt, "%c", &jogo.inimigoAnimacao1[j][k]);
    }
    fscanf(inimigo_txt, "%*c");
  }
  for (int j = 0; j < 3; j++)
  {
    for (int k = 0; k < 3; k++)
    {
      fscanf(inimigo_txt, "%c", &jogo.inimigoAnimacao2[j][k]);
    }
    fscanf(inimigo_txt, "%*c");
  }
  fclose(inimigo_txt);

  // CRIACAO DO inicializacao.txt
  char diretorio_inicializacao[MAX_DIRETORIO];
  sprintf(diretorio_inicializacao, "%s/saida/inicializacao.txt", argv[1]);
  FILE *inicializao_txt = fopen(diretorio_inicializacao, "w");
  if (inicializao_txt == NULL)
  {
    printf("Erro ao gerar o arquivo inicializacao.txt\n");
    exit(1);
  }

  // define o mapa
  jogo = DefineMapa(jogo);

  // printando o mapa no inicializacao.txt
  for (int i = 0; i < jogo.mapa.qtd_linhas; i++)
  {
    for (int j = 0; j < jogo.mapa.qtd_colunas; j++)
    {
      fprintf(inicializao_txt, "%c", jogo.mapa.mapa[i][j]);
    }
    fprintf(inicializao_txt, "\n");
  }
  fprintf(inicializao_txt, "A posicao central do jogador iniciara em (%d %d).", jogo.nave.coluna - 1, jogo.nave.linha - 1);

  // inicializacao de mais variaveis
  jogo.movimento.iteracao = 0;
  jogo.movimento.pontos = 0;
  jogo.descer = 0;
  jogo.horizontal = 0;
  jogo.tiro.ativo = 0;
  jogo.inimigosRestantes = jogo.nInimigos;
  for (int k = 0; k < jogo.nInimigos; k++)
  {
    jogo.inimigo[k].morreu = 0;
  }
  jogo.movimento.total_movimentos_AD = 0;
  jogo.tiro.tiros_efetivos = 0;
  jogo.tiro.tiros_nao_efetivos = 0;
  jogo.inimigosDescidas = 0;
  for (int i = 0; i < jogo.mapa.qtd_linhas; i++)
  {
    for (int j = 0; j < jogo.mapa.qtd_colunas; j++)
    {
      jogo.heatmap[i][j] = 0;
    }
  }

  fclose(inicializao_txt);

  return jogo;
}

void RealizaJogo(tJogo jogo, char **argv)
{
  // CRIACAO DO saida.txt
  char diretorio_saida[MAX_DIRETORIO];
  sprintf(diretorio_saida, "%s/saida/saida.txt", argv[1]);
  FILE *saida_txt = fopen(diretorio_saida, "w");
  if (saida_txt == NULL)
  {
    printf("Erro ao gerar o arquivo saida.txt\n");
    exit(1);
  }

  // LEITURA DO entrada.txt
  FILE *entrada_txt;
  char erro_diretorio[MAX_DIRETORIO];
  sprintf(erro_diretorio, "%s", argv[1]);
  char diretorio_entrada[MAX_DIRETORIO];
  sprintf(diretorio_entrada, "%s/entrada.txt", argv[1]);
  entrada_txt = fopen(diretorio_entrada, "r");
  if (entrada_txt == NULL)
  {
    printf("ERRO: nao foi possivel ler o arquivo entrada.txt localizado no diretorio %s\n", erro_diretorio);
    exit(1);
  }

  // CRIACAO DO estatisticas.txt
  FILE *estatisticas_txt;
  char diretorio_estatisticas[MAX_DIRETORIO];
  sprintf(diretorio_estatisticas, "%s/saida/estatisticas.txt", argv[1]);
  estatisticas_txt = fopen(diretorio_estatisticas, "w");
  if (estatisticas_txt == NULL)
  {
    printf("Erro ao gerar o arquivo estatisticas.txt\n");
    exit(1);
  }

  // CRIACAO DO ranking.txt
  FILE *ranking_txt;
  char diretorio_ranking[MAX_DIRETORIO];
  sprintf(diretorio_ranking, "%s/saida/ranking.txt", argv[1]);
  ranking_txt = fopen(diretorio_ranking, "w");
  if (ranking_txt == NULL)
  {
    printf("Erro ao gerar o arquivo ranking.txt\n");
    exit(1);
  }
  fprintf(ranking_txt, "indice,fileira,linha,iteracao\n");

  // CRIACAO DO resumo.txt
  FILE *resumo_txt;
  char diretorio_resumo[MAX_DIRETORIO];
  sprintf(diretorio_resumo, "%s/saida/resumo.txt", argv[1]);
  resumo_txt = fopen(diretorio_resumo, "w");
  if (resumo_txt == NULL)
  {
    printf("Erro ao gerar o arquivo resumo.txt\n");
    exit(1);
  }

  // CRIACAO DO heatmap.txt
  FILE *heatmap_txt;
  char diretorio_heatmap[MAX_DIRETORIO];
  sprintf(diretorio_heatmap, "%s/saida/heatmap.txt", argv[1]);
  heatmap_txt = fopen(diretorio_heatmap, "w");
  if (heatmap_txt == NULL)
  {
    printf("Erro ao gerar o arquivo heatmap.txt\n");
    exit(1);
  }

  jogo = AtualizaHeatmap(jogo); // atualiza o heatmap com a posicao inicial da nave

  if (jogo.terminou == 1) // se o jogo nao tiver inimigos, ganha jogo
  {
    PrintaMapa(jogo, saida_txt);
    fprintf(saida_txt, "Parabéns, você ganhou!\n");
    PrintaEstatisticas(jogo, estatisticas_txt);
    PrintaHeatmap(jogo, heatmap_txt);
    fclose(entrada_txt);
    fclose(estatisticas_txt);
    fclose(ranking_txt);
    fclose(saida_txt);
    fclose(resumo_txt);
    return;
  }

  jogo = VerificaParedeInimigo(jogo); // verifica se algum inimigo inicia em alguma lateral do mapa

  PrintaMapa(jogo, saida_txt); // printa o jogo na iteracao 0

  while (jogo.terminou == 0) // loop do jogo
  {
    if (InimigoCruzouLinha(jogo)) // perde jogo
    {
      jogo.terminou = 1;
      fprintf(saida_txt, "Você perdeu, tente novamente!\n");
      break;
    }

    if (jogo.inimigosRestantes == 0) // ganha jogo
    {
      jogo.terminou = 1;
      fprintf(saida_txt, "Parabéns, você ganhou!\n");
      break;
    }

    jogo = MatouInimigo(jogo, resumo_txt); // verifica se algum inimigo vivo tomou tiro

    if (jogo.descer == 1) // se algum inimigo vivo colidiu em alguma lateral
    {
      for (int k = 0; k < jogo.nInimigos; k++) // imprime a colisao no resumo.txt
      {
        if (!jogo.horizontal && !jogo.inimigo[k].morreu && jogo.inimigo[k].coluna - 2 == 1)
        {
          fprintf(resumo_txt, "[%d] Inimigo de indice %d da fileira %d colidiu na lateral esquerda.\n", jogo.movimento.iteracao, jogo.inimigo[k].indice + 1, jogo.inimigo[k].fileira + 1);
        }
        else if (jogo.horizontal && !jogo.inimigo[k].morreu && jogo.inimigo[k].coluna + 2 == jogo.mapa.qtd_colunas)
        {
          fprintf(resumo_txt, "[%d] Inimigo de indice %d da fileira %d colidiu na lateral direita.\n", jogo.movimento.iteracao, jogo.inimigo[k].indice + 1, jogo.inimigo[k].fileira + 1);
        }
      }
      jogo = MoveInimigo(1, 0, jogo); // move os inimigos vivos para baixo no mapa
      if (jogo.inimigosRestantes > 0)
      {
        jogo.inimigosDescidas++; // atualiza as descidas dos inimigos
      }
      jogo.descer = 0;
    }
    else
    {
      if (jogo.horizontal == 0) // movimento para direita
      {
        jogo = MoveInimigo(0, 1, jogo);
      }
      else // movimento para esquerda
      {
        jogo = MoveInimigo(0, -1, jogo);
      }
    }
    jogo = VerificaParedeInimigo(jogo); // verifica se algum inimigo vivo colidiu em uma lateral

    jogo = MoveTiro(jogo); // move o tiro se estiver ativo

    jogo.movimento.iteracao++; // atualiza a iteracao

    fscanf(entrada_txt, "%c", &jogo.movimento.jogada); // le a jogada da nave
    fscanf(entrada_txt, "%*c");
    jogo = MoveNave(jogo, resumo_txt); // move a nave ou atira

    jogo = DefineMapa(jogo); // atualiza o mapa

    PrintaMapa(jogo, saida_txt); // printa o mapa

    jogo = AtualizaHeatmap(jogo); // atualiza o heatmap
  }

  // imprime as estatisticas
  PrintaEstatisticas(jogo, estatisticas_txt);

  // imprime o ranking
  PrintaRanking(jogo, ranking_txt);

  // imprime o heatmap
  PrintaHeatmap(jogo, heatmap_txt);

  fclose(heatmap_txt);
  fclose(entrada_txt);
  fclose(resumo_txt);
  fclose(estatisticas_txt);
  fclose(ranking_txt);
  fclose(saida_txt);
}

int main(int argc, char **argv)
{
  if (argc < 2) // verifica se foi fornecido o diretorio ao executar o programa
  {
    printf("ERRO: Informe o diretorio com os arquivos de configuracao.\n");
    exit(1);
  }
  tJogo jogo = InicializaJogo(argv); // inicializa o jogo
  RealizaJogo(jogo, argv);           // realiza o jogo
  return 0;
}