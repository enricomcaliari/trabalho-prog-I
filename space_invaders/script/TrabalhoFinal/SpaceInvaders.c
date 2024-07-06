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
  int morreu;
} tInimigo;

// definicao do tipo tMovimento
typedef struct
{
  char jogada;
  int pontos;
  int iteracao;
} tMovimento;

// definicao do tipo tTiro
typedef struct
{
  int linha, coluna;
  int ativo;
} tTiro;

// definicao do tipo tJogo
typedef struct
{
  tMapa mapa;
  tNave nave;
  tInimigo inimigo[20];
  int inimigosRestantes;
  int nInimigos;
  int bonus;
  int terminou;
  int descer;
  int horizontal;
  char inimigoDesenho[3][3];
  char inimigoAnimacao1[3][3];
  char inimigoAnimacao2[3][3];
  tMovimento movimento;
  tTiro tiro;
} tJogo;

// SUBFUNCOES
tJogo DefineMapa(tJogo jogo);
tJogo MoveInimigo(int i, int j, tJogo jogo);
tJogo VerificaParedeInimigo(tJogo jogo);
int InimigoCruzouLinha(tJogo jogo);
void PrintaMapa(tJogo jogo, FILE *saida_txt);
tJogo MoveNave(tJogo jogo);
tJogo NaveAtira(tJogo jogo);
tJogo MoveTiro(tJogo jogo);
tJogo MatouInimigo(tJogo jogo);

// FUNCOES PRINCIPAIS
tJogo InicializaJogo(char **argv);
void RealizaJogo(tJogo jogo, char **argv);

// SUBFUNCAO DEFINE MAPA
tJogo DefineMapa(tJogo jogo)
{
  // atribuicao de cada posicao do mapa a um caractere
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
            jogo.mapa.mapa[jogo.inimigo[k].linha + i - 1][jogo.inimigo[k].coluna + j - 1] = jogo.inimigoDesenho[i + 1][j + 1];
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
              jogo.mapa.mapa[jogo.inimigo[k].linha + i - 1][jogo.inimigo[k].coluna + j - 1] = jogo.inimigoDesenho[i + 1][j + 1];
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

// SUBFUNCAO MOVE INIMIGO
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

// SUBFUNCAO VERIFICA PAREDE INIMIGO
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

// SUBFUNCAO INIMIGO CRUZOU LINHA
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

// SUBFUNCAO PRINTA MAPA
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

// SUBFUNCAO MOVE NAVE
tJogo MoveNave(tJogo jogo)
{
  if (jogo.movimento.jogada == 'a')
  {
    if (jogo.nave.coluna - 2 == 1)
    {
      return jogo;
    }
    else
    {
      jogo.nave.coluna--;
      return jogo;
    }
  }
  else if (jogo.movimento.jogada == 'd')
  {
    if (jogo.nave.coluna + 2 == jogo.mapa.qtd_colunas)
    {
      return jogo;
    }
    else
    {
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

// SUBFUNCAO NAVE ATIRA
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
    return jogo;
  }
}

// SUBFUNCAO MOVE TIRO
tJogo MoveTiro(tJogo jogo)
{
  if (jogo.tiro.ativo == 1)
  {
    jogo.tiro.linha--;
    if (jogo.tiro.linha == 0)
    {
      jogo.tiro.ativo = 0;
      return jogo;
    }
    return jogo;
  }
  else
    return jogo;
}

// SUBFUNCAO MATOU INIMIGO
tJogo MatouInimigo(tJogo jogo)
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
            return jogo;
          }
        }
      }
    }
  }
  return jogo;
}

// FUNCAO INICIALIZA JOGO
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
    printf("ERRO: nao foi possivel ler o arquivo mapa.txt localizado no diretorio '%s'.\n", erro_diretorio);
    exit(1);
  }

  // inicializando variaveis
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

  char x, string[10];

  // leitura da coordenada dos inimigos(se tiver)
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
      jogo.nInimigos++;
    }
  }
  fclose(mapa_txt);

  // se nao tiver inimigos
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
    printf("ERRO: nao foi possivel ler o arquivo inimigo.txt localizado no diretorio '%s'.\n", erro_diretorio);
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
      fscanf(inimigo_txt, "%c", &jogo.inimigoDesenho[j][k]);
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

  // CRIACAO DO inicializao.txt
  char diretorio_inicializacao[MAX_DIRETORIO];
  sprintf(diretorio_inicializacao, "%s/saida/inicializacao.txt", argv[1]);
  FILE *inicializao_txt = fopen(diretorio_inicializacao, "w");
  if (inicializao_txt == NULL)
  {
    printf("Erro ao gerar o arquivo inicializacao.txt\n");
    exit(1);
  }

  char matriz[jogo.mapa.qtd_linhas][jogo.mapa.qtd_colunas];

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

  fclose(inicializao_txt);

  return jogo;
}

// FUNCAO REALIZA JOGO
void RealizaJogo(tJogo jogo, char **argv)
{
  char diretorio_saida[MAX_DIRETORIO];
  sprintf(diretorio_saida, "%s/saida/saida.txt", argv[1]);
  FILE *saida_txt = fopen(diretorio_saida, "w");
  if (saida_txt == NULL)
  {
    printf("Erro ao gerar o arquivo saida.txt\n");
    exit(1);
  }

  FILE *entrada_txt;
  char erro_diretorio[MAX_DIRETORIO];
  sprintf(erro_diretorio, "%s", argv[1]);
  char diretorio_entrada[MAX_DIRETORIO];
  sprintf(diretorio_entrada, "%s/entrada.txt", argv[1]);
  entrada_txt = fopen(diretorio_entrada, "r");
  if (entrada_txt == NULL)
  {
    printf("ERRO: nao foi possivel ler o arquivo entrada.txt localizado no diretorio '%s'.\n", erro_diretorio);
    exit(1);
  }

  if (jogo.terminou == 1)
  {
    PrintaMapa(jogo, saida_txt);
    fprintf(saida_txt, "Parabéns, você ganhou!\n");
    return;
  }

  jogo = VerificaParedeInimigo(jogo);

  PrintaMapa(jogo, saida_txt);

  while (jogo.terminou == 0)
  {
    if (InimigoCruzouLinha(jogo))
    {
      jogo.terminou = 1;
      fprintf(saida_txt, "Você perdeu, tente novamente!\n");
      break;
    }

    if (jogo.inimigosRestantes == 0)
    {
      jogo.terminou = 1;
      fprintf(saida_txt, "Parabéns, você ganhou!\n");
      break;
    }

    jogo = MatouInimigo(jogo);

    // move inimigo
    if (jogo.descer == 1)
    {
      jogo = MoveInimigo(1, 0, jogo);
      jogo.descer = 0;
    }
    else
    {
      if (jogo.horizontal == 0)
      {
        jogo = MoveInimigo(0, 1, jogo);
      }
      else
      {
        jogo = MoveInimigo(0, -1, jogo);
      }
    }
    jogo = VerificaParedeInimigo(jogo);

    // move tiro
    jogo = MoveTiro(jogo);

    jogo.movimento.iteracao++;

    // move nave
    fscanf(entrada_txt, "%c", &jogo.movimento.jogada);
    fscanf(entrada_txt, "%*c");
    jogo = MoveNave(jogo);

    jogo = DefineMapa(jogo);

    PrintaMapa(jogo, saida_txt);
  }

  fclose(entrada_txt);
  fclose(saida_txt);
}

// FUNCAO MAIN
int main(int argc, char **argv)
{
  if (argc < 2)
  {
    printf("ERRO: Informe o diretorio com os arquivos de configuracao.\n");
    exit(1);
  }
  tJogo jogo = InicializaJogo(argv);
  RealizaJogo(jogo, argv);
  return 0;
}