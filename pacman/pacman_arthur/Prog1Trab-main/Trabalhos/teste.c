#include <stdio.h>
#include <stdlib.h>
#define MAX_DIRETORIO 1001

const char DIREITA = 'd';
const char ESQUERDA = 'a';
const char CIMA = 'w';
const char BAIXO = 's';
const char PacMan = '>';
const char PAREDE = '#';
const char COMIDA = '*';
const char TUNEL = '@';


typedef struct
{
    int comida; //Quantas comidas aquele movimento gerou
    int colisoes; //Quantas colisões aquele movimento gerou
    int usou; //Quantas vezes aquele movimento foi usado
    char letra;
} tMov; //Gerar ranking dos movimentos

typedef struct
{
    int linha, coluna;
} tPosicao;

typedef struct
{
    tPosicao atual, passada; //Posição atual de cada fantasma
    int existe;
    char direcao, letra;
    int tampa_comida;
} tFantasma;

typedef struct
{
    tMov mov[4]; //'w', 'a', 's', 'd' (Nessa Ordem)
    tPosicao atual, passada; //Posição atual do PacMan
    int comidas;
    int mov_total;
} tPacMan;


typedef struct
{
    int linha, coluna;
    char matriz[40][100];
    int trilha[40][100];
    char mov_atual;
    int mov_restante;
    tFantasma fant[4]; //0 = B;; 1 = P;; 2 = I;; 3 = C
    tPacMan pac;
    int qtd_comida;
    tPosicao tunel1, tunel2;
} tMapa;

typedef struct
{
    tMapa mapa, trilha;
} tJogo;


//FUNÇÕES tJOGO
tJogo IniciaJogo (char diret[MAX_DIRETORIO], int argc);
tJogo GeraRanking (tJogo jogo);

//FUNÇÕES tMAPA
tMapa LeMapa (char diret[MAX_DIRETORIO]);
tMapa ResetaTrilha (tMapa mapa);
tMapa AtualizaTrilha (tMapa mapa, tPacMan pac);
tMapa FazJogo (char diret[MAX_DIRETORIO], tMapa mapa);
tMapa Movimentos (tMapa mapa);
tMapa MoveFantasmas (tMapa mapa);
tMapa RodadaPacMan (tPacMan pac, tMapa mapa, char jogada);
tMapa VoltaComida (tMapa mapa, tFantasma fant);

//FUNÇÕES tFANTASMA
tFantasma CriaFantasma (tFantasma fant, int linha, int coluna, int diferenciacao); //Zera seus valores e lê sua posição
tFantasma VerificaProxPosicaoFant (char matriz[40][100], tFantasma fant); //Realiza as condições se a próxima casa for * ou #
tFantasma MoveFantasmaAtual (tFantasma fant);
tFantasma ResetaComida (tFantasma fant);
tFantasma InverteDirecaoEAndaFant (tFantasma fant);

//FUNÇÕES tPACMAN
tPacMan CriaPac (tPacMan pac, int linha, int coluna); //Zera seus valores e lê sua posição
tPacMan MovePacMan (tPacMan pac, char jogada);
tPacMan ResetaPosPacMan (tPacMan pac);

//FUNÇÕES tPOSICAO
tPosicao GeraPosicao (int linha, int coluna); //Recebe um ponto (x, y) e retorna seu valor
tPosicao MoveEsquerda (tPosicao posicao);
tPosicao MoveDireita (tPosicao posicao);
tPosicao MoveCima (tPosicao posicao);
tPosicao MoveBaixo (tPosicao posicao);
tPosicao ParaPacMan (tPosicao posicao, char direcao);
tPosicao EntraTunel (tPosicao posicao_pac, tPosicao posicao_tunel_entrada, tPosicao posicao_tunel_saida);

//FUNÇÕES tMOV
tMov Reseta_tMov (tMov mov, int identificador);
tMov ContaColisao (tMov mov);
tMov ContaComida (tMov mov);

//FUNÇÕES VOID
void PrintaInicializacao (tMapa mapa, char diret[MAX_DIRETORIO]);
void PrintaMapaFILE (tMapa mapa, FILE *file);
void PrintaMapaTerminal (tMapa mapa);
void PrintaDerrota (tPacMan pac);
void PrintaVitoria (tPacMan pac);
void ImprimeSaidas (tMapa mapa, char diretorio[MAX_DIRETORIO]);
void PrintaTrilha (int trilha[40][100], int linha, int coluna, char diretorio[MAX_DIRETORIO]);
void PrintaRanking (tMapa mapa, char diretorio[MAX_DIRETORIO]); //Faz ranking dos movimentos
void PrintaEstatisticas (tMapa mapa, char diretorio[MAX_DIRETORIO]);
void PrintaResumo (tMapa mapa, char diretorio[MAX_DIRETORIO]);
void PrintaSaidaAtual (tMapa mapa);

//FUNÇÕES CHAR
char VerificaProxPosicaoPac (char matriz[40][100], tPosicao posicao, tPacMan pac); //Retorna o símbolo da próxima casa ('*', '#')


//FUNÇÕES INT
int PosicaoEhIgual (tPosicao pos1, tPosicao pos2);
int ComparaPosMatriz (tPosicao posicao, int linha, int coluna);
int GeraLinha (tPosicao posicao);
int GeraColuna (tPosicao posicao);
int EscondeComida (tFantasma fant);
int PerdeuJogo (tMapa mapa);
int GanhouJogo (tMapa mapa);
int PacManMorreu (tFantasma fant, tPacMan pac);


int main(int argc, char **argv)
{
    tJogo jogo;
    char diretorio[MAX_DIRETORIO];
    sprintf (diretorio, "%s", argv[1]);

    jogo = IniciaJogo (diretorio, argc); //Cria o inicalizacao.txt e lê o mapa

    jogo.mapa = FazJogo (diretorio, jogo.mapa);
    ImprimeSaidas (jogo.mapa, diretorio);
    return 0;
}


tMapa LeMapa (char diret[MAX_DIRETORIO])
{
    char diret_mapa [MAX_DIRETORIO +8];
    int linha, coluna, tunel1_ja_criado = 0; //Verifica se o tunel1 ja foi criado;
    tMapa mapa;
    mapa.qtd_comida = 0;
    FILE * file;

    sprintf (diret_mapa, "%s/mapa.txt", diret);
    file = fopen (diret_mapa, "r");
    if (file == NULL){
        printf ("ERRO: Não foi possível ler o arquivo\nO arquivo selecionado foi: %s\n", diret);
        exit(0);
    }
    
    fscanf(file, "%d %d %d%*c", &mapa.linha, &mapa.coluna, &mapa.mov_restante);

    mapa = ResetaTrilha (mapa);

    for (linha = 0; linha < mapa.linha; linha++)
    {
        for (coluna = 0; coluna < mapa.coluna; coluna++)
        {
            fscanf(file, "%c", &mapa.matriz[linha][coluna]);
            switch (mapa.matriz[linha][coluna]) //Se for fantasma ou o PacMan, grave sua localização.
            {
            case 'B':
                mapa.matriz[linha][coluna] = ' ';
                mapa.fant[0] = CriaFantasma (mapa.fant[0], linha, coluna, 0);
                break;

            case 'P':
                mapa.matriz[linha][coluna] = ' ';
                mapa.fant[1] = CriaFantasma (mapa.fant[1], linha, coluna, 1);
                break;

            case 'I':
                mapa.matriz[linha][coluna] = ' ';
                mapa.fant[2] = CriaFantasma (mapa.fant[2], linha, coluna, 2);
                break;

            case 'C':
                mapa.matriz[linha][coluna] = ' ';
                mapa.fant[3] = CriaFantasma (mapa.fant[3], linha, coluna, 3);
                break;

            case '>':
                mapa.matriz[linha][coluna] = ' ';
                mapa.pac = CriaPac (mapa.pac, linha, coluna);
                break;

            case '*':
                mapa.qtd_comida++;
                break;

            case '@':
                if (!tunel1_ja_criado)
                    mapa.tunel1 = GeraPosicao (linha, coluna);
                else
                    mapa.tunel2 = GeraPosicao (linha, coluna);

            default:
                break;
            }
        }
        fscanf(file, "%*c");
    }
    fclose (file);

    return mapa;
}

tMapa ResetaTrilha (tMapa mapa)//Torna a trilha toda como -1, que será lido no PrintaTrilha
{
    int i, j;
    for (i = 0; i < mapa.linha; i++)
    {
        for (j = 0; j < mapa.coluna; j++)
        {
            mapa.trilha[i][j] = -1;
        } 
    }
    return mapa;
}

tMapa AtualizaTrilha (tMapa mapa, tPacMan pac)
{
    int i, j;
    for (i = 0; i < mapa.linha; i++)
    {
        for (j = 0; j < mapa.coluna; j++)
        {
            if (ComparaPosMatriz (pac.atual, i, j))
                mapa.trilha[i][j] = pac.mov_total;
        }
    }
    return mapa;
}

void PrintaTrilha (int trilha[40][100], int linha, int coluna, char diretorio[MAX_DIRETORIO])
{
    FILE *file;
    char diret_trilha[MAX_DIRETORIO+10];
    sprintf (diret_trilha, "%s/trilha.txt", diretorio);

    file = fopen (diret_trilha, "w");
    int i, j;
    for (i = 0; i < linha; i++)
    {
        for (j = 0; j < coluna; j++)
        {
            if (trilha[i][j] == -1)
                fprintf (file, "# ");
            else
                fprintf (file, "%d ", trilha[i][j]);
        }
        fprintf (file, "\n");
    }
    fclose (file);
}

tJogo IniciaJogo (char diret[MAX_DIRETORIO], int argc)
{
    //Caso não digite o diretório, código termina
    if (argc < 2)
    { 
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        exit(0);
    }

    tJogo jogo;
    char diret_inicio[MAX_DIRETORIO]; //Para não alterar o diretório;

    jogo.mapa = LeMapa(diret);

    sprintf (diret, "%s/saida", diret); //Tudo vai ser implementado no /saida, então está ok mudar o vetor diretório
    sprintf (diret_inicio, "%s/inicializacao.txt", diret);
    PrintaTrilha (jogo.mapa.trilha, jogo.mapa.linha, jogo.mapa.coluna, diret);
    PrintaInicializacao (jogo.mapa, diret_inicio);

    return jogo;
}

void PrintaInicializacao (tMapa mapa, char diret[MAX_DIRETORIO])
{

    int linha, coluna, ja_printou_aqui, i;
    FILE * file;
    file = fopen (diret, "w");
    PrintaMapaFILE (mapa, file);
    /*for (int i = 0; i < mapa.linha; i++)
    {
        for (int j = 0; j < mapa.coluna; j++)
        {
            fprintf (file, "%c", mapa.matriz[i][j]);
        }
        fprintf (file, "\n");
    }*/
    fprintf (file, "Pac-Man comecara o jogo na linha %d e coluna %d", mapa.pac.atual.linha+1, mapa.pac.atual.coluna+1);
    fclose (file);
}

void PrintaMapaFILE (tMapa mapa, FILE *file)
{
    int linha, coluna, i, ja_printou_aqui;

    for (linha = 0; linha < mapa.linha; linha++)
    {
        for (coluna = 0; coluna < mapa.coluna; coluna++)
        {
            ja_printou_aqui = 0;
            if (ComparaPosMatriz (mapa.pac.atual, linha, coluna))
            {
                fprintf (file, ">");
                ja_printou_aqui = 1;
            }
            for (i = 0; i < 4; i++)
            {
                tFantasma fant = mapa.fant[i];
                if (fant.existe)
                {
                    if (ComparaPosMatriz (fant.atual, linha, coluna))
                    {  
                        fprintf (file, "%c", fant.letra);
                        ja_printou_aqui = 1; //Garante que o que quer que esteja embaixo do fantasma não seja printado
                        break;
                    }
                }
            }
            if (!ja_printou_aqui)
                fprintf (file, "%c", mapa.matriz[linha][coluna]);
        }
        fprintf (file, "\n");
    }
}

tMapa FazJogo (char diret[MAX_DIRETORIO], tMapa mapa)
{
    while (1)
    {
        mapa = Movimentos (mapa);
        PrintaSaidaAtual (mapa);
        if (PerdeuJogo(mapa))
        {
            if (PerdeuJogo(mapa) == 1)
                break;
            else if (PerdeuJogo(mapa) == 2)
                break;
        }
        else if (GanhouJogo (mapa))
            break;
        mapa = AtualizaTrilha (mapa, mapa.pac);
    }

    return mapa;
}

void PrintaSaidaAtual (tMapa mapa)
{
    tPacMan pac = mapa.pac;
    printf ("Estado do jogo apos o movimento '%c':\n", mapa.mov_atual);
    PrintaMapaTerminal (mapa);
    printf ("Pontuacao: %d\n", pac.comidas);
    if (GanhouJogo (mapa))
        PrintaVitoria (pac);
    else if (PerdeuJogo (mapa))
        PrintaDerrota (pac);
}

void PrintaMapaTerminal (tMapa mapa)
{
    int linha, coluna, i, ja_printou_aqui;

    for (linha = 0; linha < mapa.linha; linha++)
    {
        for (coluna = 0; coluna < mapa.coluna; coluna++)
        {
            ja_printou_aqui = 0;
            for (i = 0; i < 4; i++)
            {
                tFantasma fant = mapa.fant[i];
                if (fant.existe)
                {
                    if (ComparaPosMatriz (fant.atual, linha, coluna))
                    {  
                        printf ("%c", fant.letra);
                        ja_printou_aqui = 1; //Garante que o que quer que esteja embaixo do fantasma não seja printado
                        break;
                    }
                }
            }
            if (!ja_printou_aqui && ComparaPosMatriz (mapa.pac.atual, linha, coluna))
            {
                printf (">");
                ja_printou_aqui = 1;
            }
            if (!ja_printou_aqui)
                printf ("%c", mapa.matriz[linha][coluna]);
        }
        printf ("\n");
    }
}

tFantasma CriaFantasma (tFantasma fant, int linha, int coluna, int diferenciacaco)
{
    fant.existe = 1;
    fant.tampa_comida = 0; //Não está em cima de nenhuma comida no início do jogo
    switch (diferenciacaco)
    {
    case 0: //'B'
        fant.direcao = ESQUERDA;
        fant.letra = 'B';
        break;
    
    case 1: //'P'
        fant.direcao = CIMA;
        fant.letra = 'P';
        break;

    case 2: //'I'
        fant.direcao = BAIXO;
        fant.letra = 'I';
        break;

    case 3: //'C'
        fant.direcao = DIREITA;
        fant.letra = 'C';
        break;
    
    default:
        break;
    }
    fant.atual = GeraPosicao (linha, coluna);
    return fant;
}

tPacMan CriaPac (tPacMan pac, int linha, int coluna)
{
    int i;
    pac.comidas = 0;
    pac.atual = GeraPosicao (linha, coluna);
    pac.mov_total = 0;
    for (i = 0; i < 4; i++)
    {
        pac.mov[i] = Reseta_tMov (pac.mov[i], i);
    }
    return pac;
}

tPosicao GeraPosicao (int linha, int coluna)
{
    tPosicao posicao;
    posicao.linha = linha;
    posicao.coluna = coluna;
    return posicao;
}

tPosicao MoveEsquerda (tPosicao posicao)
{
    posicao.coluna = posicao.coluna - 1;
    return posicao;
}
tPosicao MoveDireita (tPosicao posicao)
{
    posicao.coluna = posicao.coluna + 1;
    return posicao;
}
tPosicao MoveCima (tPosicao posicao)
{
    posicao.linha = posicao.linha - 1;
    return posicao;
}
tPosicao MoveBaixo (tPosicao posicao)
{
    posicao.linha = posicao.linha + 1;
    return posicao;
}

tMov Reseta_tMov (tMov mov, int identificador)
{
    mov.colisoes = 0;
    mov.comida = 0;
    mov.usou = 0;
    switch (identificador)
    {
    case 0:
        mov.letra = CIMA;
        break;
    
    case 1:
        mov.letra = ESQUERDA;
        break;

    case 2:
        mov.letra = BAIXO;
        break;

    case 3:
        mov.letra = DIREITA;
        break;

    default:
        break;
    }
    return mov;
}

int PerdeuJogo (tMapa mapa)
{
    int i;

    if (!mapa.mov_restante)
        return 1;
    for (i = 0; i < 4; i++)
    {
        if (PacManMorreu (mapa.fant[i], mapa.pac))
            return 2;
    }

    return 0;
}
int GanhouJogo (tMapa mapa)
{
    if (!mapa.qtd_comida) //Se for nula, isto é, sem comida
    {
        return 1;
    }
    return 0;
}

int PacManMorreu (tFantasma fant, tPacMan pac)
{
    if (PosicaoEhIgual (fant.atual, pac.atual))
        return 1;
    return 0;
}

int PosicaoEhIgual (tPosicao pos1, tPosicao pos2)
{
    if (pos1.linha == pos2.linha && pos1.coluna == pos2.coluna)
        return 1;
    return 0;
}

void PrintaDerrota (tPacMan pac)
{
    printf ("Game over!\nPontuacao final: %d\n", pac.comidas);
}
void PrintaVitoria (tPacMan pac)
{
    printf ("Voce venceu!\nPontuacao final: %d\n", pac.comidas);
}

tMapa Movimentos (tMapa mapa)
{
    mapa = MoveFantasmas (mapa);
    char jogada;
    scanf ("%c%*c", &jogada);
    mapa.mov_atual = jogada;
    mapa = RodadaPacMan (mapa.pac, mapa, mapa.mov_atual);
    return mapa;
}

tMapa MoveFantasmas (tMapa mapa)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        /*if (EscondeComida (mapa.fant[i])) //Se tiver tapando uma comida, faça isso
        {
            mapa = VoltaComida (mapa, mapa.fant[i]); //Retorna a comida escondida para seu lugar
            mapa.fant[i] = ResetaComida (mapa.fant[i]);
        } ACHO QUE NÃO VAI PRECISAR DE NADA DISSO*/
        mapa.fant[i] = MoveFantasmaAtual (mapa.fant[i]);
        mapa.fant[i] = VerificaProxPosicaoFant (mapa.matriz, mapa.fant[i]);
    }
    return mapa;
}
tFantasma MoveFantasmaAtual (tFantasma fant)
{
    fant.passada = fant.atual; //Iguala para não perder esse dado
    if (fant.direcao == DIREITA)
    {
        fant.atual = MoveDireita (fant.atual);
    }
    else if (fant.direcao == ESQUERDA)
    {
        fant.atual = MoveEsquerda (fant.atual);
    }
    else if (fant.direcao == CIMA)
    {
        fant.atual = MoveCima (fant.atual);
    }
    else if (fant.direcao == BAIXO)
    {
        fant.atual = MoveBaixo (fant.atual);
    }
    return fant;
}

tFantasma VerificaProxPosicaoFant (char matriz[40][100], tFantasma fant)
{
    if (matriz[GeraLinha(fant.atual)][GeraColuna(fant.atual)] == COMIDA)
    {
        fant.tampa_comida++;
    }
    if (matriz[GeraLinha(fant.atual)][GeraColuna(fant.atual)] == PAREDE
        || matriz[GeraLinha(fant.atual)][GeraColuna(fant.atual)] == TUNEL)
    {
        fant = InverteDirecaoEAndaFant (fant);
    }
    return fant;
}

int GeraLinha (tPosicao posicao)
{
    return posicao.linha;
}
int GeraColuna (tPosicao posicao)
{
    return posicao.coluna;
}

int EscondeComida (tFantasma fant)
{
    if (fant.tampa_comida)
        return 1;
    return 0;
}

tMapa VoltaComida (tMapa mapa, tFantasma fant)
{
    mapa.matriz[GeraLinha(fant.atual)][GeraColuna(fant.atual)] = COMIDA;
    return mapa;
}

tFantasma ResetaComida (tFantasma fant)
{
    fant.tampa_comida = 0;
    return fant;
}

int ComparaPosMatriz (tPosicao posicao, int linha, int coluna)
{
    if (posicao.linha == linha && posicao.coluna == coluna)
        return 1;
    return 0;
}

tFantasma InverteDirecaoEAndaFant (tFantasma fant)
{
    if (fant.direcao == DIREITA)
    {
        fant.direcao = ESQUERDA;
        fant.atual = MoveEsquerda (fant.atual);
        fant.atual = MoveEsquerda (fant.atual); //2 vezes pois seria uma para sair da parede e outra para de fato andar
    }
    else if (fant.direcao == ESQUERDA)
    {
        fant.direcao = DIREITA;
        fant.atual = MoveDireita (fant.atual);
        fant.atual = MoveDireita (fant.atual);
    }
    else if (fant.direcao == CIMA)
    {
        fant.direcao = BAIXO;
        fant.atual = MoveBaixo (fant.atual);
        fant.atual = MoveBaixo (fant.atual);
    }
    else if (fant.direcao == BAIXO)
    {
        fant.direcao = CIMA;
        fant.atual = MoveCima (fant.atual);
        fant.atual = MoveCima (fant.atual);
    }

    return fant;
}

tMapa RodadaPacMan (tPacMan pac, tMapa mapa, char jogada)
{
    int i, fim = 0;
    char verifica;
    for (i = 0; i < 4; i++)
    {
        if (jogada == pac.mov[i].letra)
        {
            pac = MovePacMan (pac, jogada);
    
            verifica = (VerificaProxPosicaoPac(mapa.matriz, pac.atual, pac));
            if (verifica == PAREDE)
            {
                pac.mov[i] = ContaColisao (pac.mov[i]);
                pac.atual = ParaPacMan (pac.atual, jogada);
            }
            else if (verifica == COMIDA)
            {
                mapa.pac = pac; 
                if (!PerdeuJogo(mapa))
                {
                    pac.mov[i] = ContaComida (pac.mov[i]);
                    pac.comidas++;
                    mapa.matriz[GeraLinha(pac.atual)][GeraColuna(pac.atual)] = ' '; //Tira a comida daquela posição
                    mapa.qtd_comida--;
                }
            }
            //else if (verifica == TUNEL)
                //pac.atual = EntraTunel (pac.atual, mapa.tunel); //Função que entra no túnel e conta ++ para o movimento
            break;
        }
    }
    mapa.pac = pac;
    mapa.mov_restante--;
    return mapa;
}

tPacMan MovePacMan (tPacMan pac, char jogada)
{
    pac.passada = pac.atual;
    if (jogada == DIREITA)
        pac.atual = MoveDireita (pac.atual);
    else if (jogada == ESQUERDA)
        pac.atual = MoveEsquerda (pac.atual);
    else if (jogada == CIMA)
        pac.atual = MoveCima (pac.atual);
    else if (jogada == BAIXO)
        pac.atual = MoveBaixo (pac.atual);
    pac.mov_total++;
    
    return pac;
}

char VerificaProxPosicaoPac (char matriz[40][100], tPosicao posicao, tPacMan pac)
{
    if (matriz[GeraLinha(pac.atual)][GeraColuna(pac.atual)] == COMIDA)
        return COMIDA;
    else if (matriz[GeraLinha(pac.atual)][GeraColuna(pac.atual)] == PAREDE)
        return PAREDE;
    else if (matriz[GeraLinha(pac.atual)][GeraColuna(pac.atual)] == TUNEL)
        return TUNEL;
    else
        return ' ';
}

tMov ContaColisao (tMov mov)
{
    mov.colisoes++;
    mov.usou++;
    return mov;
}
tMov ContaComida (tMov mov)
{
    mov.comida++;
    mov.usou++;
    return mov;
}

tPosicao ParaPacMan (tPosicao posicao, char direcao) //Retorna PacMan para seu lugar anterior
{
    if (direcao == DIREITA)
        posicao = MoveEsquerda (posicao);
    else if (direcao == ESQUERDA)
        posicao = MoveDireita (posicao);
    else if (direcao == CIMA)
        posicao = MoveBaixo (posicao);
    else if (direcao == BAIXO)
        posicao = MoveCima (posicao);
    
    return posicao;
}

void ImprimeSaidas (tMapa mapa, char diretorio[MAX_DIRETORIO])
{
    //PrintaEstatisticas (mapa, diretorio);
    //PrintaRanking (mapa, diretorio);
    //PrintaResumo (mapa, diretorio);
    PrintaTrilha (mapa.trilha, mapa.linha, mapa.coluna, diretorio);
}
