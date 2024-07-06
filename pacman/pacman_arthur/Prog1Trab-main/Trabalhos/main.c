#include <stdio.h>
#include <stdlib.h>

#define MAX_DIRETORIO 1007 //Tamano máximo + '\0' + /saida (será implementado logo no início do código)

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
    int morreu;
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


//FUNÇÕES tMAPA
tMapa IniciaJogo (char diret[MAX_DIRETORIO], int argc); //Cria o inicalizacao.txt e lê o mapa
tMapa LeMapa (char diret[MAX_DIRETORIO]);
tMapa ResetaTrilha (tMapa mapa); //Torna a trilha toda como -1
tMapa AtualizaTrilha (tMapa mapa, tPacMan pac); //Conforme PacMan anda, atualiza a trilha com sua posição
tMapa FazJogo (char diret[MAX_DIRETORIO], tMapa mapa);
tMapa Movimentos (tMapa mapa, FILE *arq_resumo);
tMapa MoveFantasmas (tMapa mapa); //Move todos os fantasmas
tMapa RodadaPacMan (tPacMan pac, tMapa mapa, char jogada, FILE *arq_resumo);

//FUNÇÕES tFANTASMA
tFantasma CriaFantasma (tFantasma fant, int linha, int coluna, int diferenciacao); //Zera seus valores e lê sua posição
tFantasma VerificaProxPosicaoFant (char matriz[40][100], tFantasma fant); //Realiza as condições se a próxima casa for * ou #
tFantasma MoveFantasmaAtual (tFantasma fant);
tFantasma InverteDirecaoEAndaFant (tFantasma fant);

//FUNÇÕES tPACMAN
tPacMan CriaPac (tPacMan pac, int linha, int coluna); //Zera seus valores e lê sua posição
tPacMan MovePacMan (tPacMan pac, char jogada);
tPacMan ResetaPosPacMan (tPacMan pac);
tPacMan ContaMorte (tPacMan pac);

//FUNÇÕES tPOSICAO
tPosicao GeraPosicao (int linha, int coluna); //Recebe um ponto (x, y) e retorna seu valor
tPosicao MoveEsquerda (tPosicao posicao);
tPosicao MoveDireita (tPosicao posicao);
tPosicao MoveCima (tPosicao posicao);
tPosicao MoveBaixo (tPosicao posicao);
tPosicao ParaPacMan (tPosicao posicao, char direcao); //Retorna PacMan para seu lugar anterior
tPosicao EntraTunel (tPosicao posicao_pac, tPosicao posicao_tunel_entrada, tPosicao posicao_tunel_saida);

//FUNÇÕES tMOV
tMov Reseta_tMov (tMov mov, int identificador);
tMov ContaMovimento (tMov mov);
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
void PrintaRanking (tPacMan pac, char diretorio[MAX_DIRETORIO]); //Faz ranking dos movimentos
void PrintaEstatisticas (tPacMan pac, char diretorio[MAX_DIRETORIO]);
void PrintaResumoComida (tPacMan pac, tMov mov, FILE *arq_resumo);
void PrintaResumoColisao (tPacMan pac, tMov mov, FILE *arq_resumo);
void PrintaResumoMorreu (tPacMan pac, tMov mov, FILE *arq_resumo);
void PrintaSaidaAtual (tMapa mapa);

//FUNÇÕES CHAR
char VerificaProxPosicaoPac (char matriz[40][100], tPosicao posicao, tPacMan pac); //Retorna o símbolo da próxima casa ('*', '#')

//FUNÇÕES INT
int PosicaoEhIgual (tPosicao pos1, tPosicao pos2);
int ComparaPosMatriz (tPosicao posicao, int linha, int coluna);
int GeraLinha (tPosicao posicao);
int GeraColuna (tPosicao posicao);
int PerdeuJogo (tMapa mapa);
int GanhouJogo (tMapa mapa);
int PacManMorreu (tFantasma fant, tPacMan pac);


int main(int argc, char **argv)
{
    tMapa mapa;
    char diretorio[MAX_DIRETORIO];
    sprintf (diretorio, "%s", argv[1]);

    mapa = IniciaJogo (diretorio, argc); 

    mapa = FazJogo (diretorio, mapa);
    ImprimeSaidas (mapa, diretorio);
    return 0;
}


//Funções Mapa
tMapa LeMapa (char diret[MAX_DIRETORIO])
{
    char diret_mapa [MAX_DIRETORIO +8]; //diret + mapa.txt
    int linha, coluna;
    int tunel1_ja_criado = 0; //Verifica se o tunel1 ja foi criado;

    tMapa mapa;
    mapa.qtd_comida = 0; //Reseta essa parte do mapa
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
            switch (mapa.matriz[linha][coluna]) //Gravar localizações de: túneis, fantasmas e pacman e conta quantidade de comida do mapa
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
                {
                    mapa.tunel1 = GeraPosicao (linha, coluna);
                    tunel1_ja_criado = 1;
                }
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
tMapa ResetaTrilha (tMapa mapa)
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
tMapa IniciaJogo (char diret[MAX_DIRETORIO], int argc)
{
    if (argc < 2) //Caso não digite o diretório, código termina
    { 
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        exit(0);
    }

    tMapa mapa;
    char diret_inicio[MAX_DIRETORIO+18]; //Para não alterar o diretório (diret + /inicializacao.txt);

    mapa = LeMapa(diret);

    sprintf (diret, "%s/saida", diret); //Tudo vai ser implementado no /saida, então está ok mudar o vetor diretório

    sprintf (diret_inicio, "%s/inicializacao.txt", diret);
    PrintaInicializacao (mapa, diret_inicio);

    return mapa;
}
tMapa FazJogo (char diret[MAX_DIRETORIO], tMapa mapa)
{
    char diret_resumo[MAX_DIRETORIO+10];
    sprintf (diret_resumo, "%s/resumo.txt", diret);
    FILE *arq_resumo = fopen (diret_resumo, "w"); //Cria e abre o arquivo de resumo para criar/limpar o existente
    //Como existem 3 funções de imprimir resumo, criei aqui para apenas usar o fprintf em cada

    mapa = AtualizaTrilha (mapa, mapa.pac); //Colocar o primeiro movimento como '0'

    while (1) //Looping de movimentos até Perder ou Ganhar, nessa ordem
    {
        mapa = Movimentos (mapa, arq_resumo);

        if (PerdeuJogo(mapa) == 2) //Caso tenha morrido, marca o pac como morto a fim de não ser impresso no "PrintaSaida"
            mapa.pac = ContaMorte (mapa.pac);

        PrintaSaidaAtual (mapa);
        if (PerdeuJogo(mapa))
        {
            if (PerdeuJogo(mapa) == 1)
                mapa = AtualizaTrilha (mapa, mapa.pac); //Se perdeu o jogo por falta de movimento, atualiza a última posição, se morreu, não atualiza
            break;
        }
        else if (GanhouJogo (mapa))
        {
            mapa = AtualizaTrilha (mapa, mapa.pac); //Se ganhou, atualiza o último passo antes de parar
            break;
        }

        mapa = AtualizaTrilha (mapa, mapa.pac);
    }
    fclose (arq_resumo);
    
    return mapa;
}
tMapa Movimentos (tMapa mapa, FILE *arq_resumo)
{
    mapa = MoveFantasmas (mapa);
    char jogada;
    scanf ("%c%*c", &jogada);
    mapa.mov_atual = jogada; //Iguala para não passar o jogada como parâmetro na hora de printar a saída
    mapa = RodadaPacMan (mapa.pac, mapa, mapa.mov_atual, arq_resumo);
    return mapa;
}
tMapa MoveFantasmas (tMapa mapa)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        mapa.fant[i] = MoveFantasmaAtual (mapa.fant[i]);
        mapa.fant[i] = VerificaProxPosicaoFant (mapa.matriz, mapa.fant[i]);
    }
    return mapa;
}
tMapa RodadaPacMan (tPacMan pac, tMapa mapa, char jogada, FILE *arq_resumo)
{
    int i, fim = 0;
    char verifica;
    pac.passada = pac.atual;
    for (i = 0; i < 4; i++)
    {
        if (jogada == pac.mov[i].letra)
        {
            pac.mov[i] = ContaMovimento (pac.mov[i]);
            pac = MovePacMan (pac, jogada);

            verifica = (VerificaProxPosicaoPac(mapa.matriz, pac.atual, pac)); //Verifica se a próxima casa é comida, túnel ou parede
            
            if (verifica == PAREDE) //Parar o PacMan antes de analisar as condições
                pac.atual = ParaPacMan(pac.atual, jogada);
            
            mapa.pac = pac; //Para carregar a função de PerdeuJogo
            if (PerdeuJogo(mapa) == 2) //Caso PacMan morra
            {
                PrintaResumoMorreu (pac, pac.mov[i], arq_resumo);
                if (verifica == PAREDE) //Caso morra batendo na parede, imprime a colisão
                {
                    pac.mov[i] = ContaColisao(pac.mov[i]);
                    PrintaResumoColisao(pac, pac.mov[i], arq_resumo);
                }
            }
            else
            {
                if (verifica == PAREDE)
                {
                    if (PosicaoEhIgual (pac.atual, mapa.tunel1)) //Caso esteja em cima do túnel e queira entrar nele denovo:
                    {
                        pac.atual = EntraTunel (pac.atual, mapa.tunel1, mapa.tunel2); 
                        break;
                    }
                    else if (PosicaoEhIgual (pac.atual, mapa.tunel2))
                    {
                        pac.atual = EntraTunel (pac.atual, mapa.tunel2, mapa.tunel1);
                        break;
                    }

                    pac.mov[i] = ContaColisao(pac.mov[i]);
                    PrintaResumoColisao(pac, pac.mov[i], arq_resumo);
                }
                else if (verifica == COMIDA)
                {
                    pac.mov[i] = ContaComida(pac.mov[i]);
                    PrintaResumoComida(pac, pac.mov[i], arq_resumo);
                    pac.comidas++;
                    mapa.matriz[GeraLinha(pac.atual)][GeraColuna(pac.atual)] = ' '; // Tira a comida daquela posição
                    mapa.qtd_comida--;
                }
                else if (verifica == TUNEL)
                {
                    if (PosicaoEhIgual (pac.atual, mapa.tunel1))
                        pac.atual = EntraTunel (pac.atual, mapa.tunel1, mapa.tunel2); 
                    else //Entrou no túnel 2
                        pac.atual = EntraTunel (pac.atual, mapa.tunel2, mapa.tunel1);
                }
            }
            break;  //Sai do loop para economizar tempo
        }
    }
    mapa.pac = pac;
    mapa.mov_restante--;
    return mapa;
}

//Funções Fantasma
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

//Funções PacMan
tPacMan CriaPac (tPacMan pac, int linha, int coluna)
{
    int i;
    pac.comidas = 0;
    pac.atual = GeraPosicao (linha, coluna);
    pac.mov_total = 0;
    pac.morreu = 0;
    for (i = 0; i < 4; i++)
    {
        pac.mov[i] = Reseta_tMov (pac.mov[i], i);
    }
    return pac;
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
tPacMan ContaMorte (tPacMan pac)
{
    pac.morreu = 1;
    return pac;
}

//Funções Posição
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
tPosicao ParaPacMan (tPosicao posicao, char direcao)
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
tPosicao EntraTunel (tPosicao posicao_pac, tPosicao posicao_tunel_entrada, tPosicao posicao_tunel_saida)
{
    posicao_pac = posicao_tunel_saida;
    return posicao_pac;
}

//Funções Movimento (Ex: 'w')
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
tMov ContaMovimento (tMov mov)
{
    mov.usou++;
    return mov;
}
tMov ContaColisao (tMov mov)
{
    mov.colisoes++;
    return mov;
}
tMov ContaComida (tMov mov)
{
    mov.comida++;
    return mov;
}


//Funções int
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
    if (PosicaoEhIgual (fant.atual, pac.atual) ||
        (PosicaoEhIgual (fant.passada, pac.atual) && 
        PosicaoEhIgual (fant.atual, pac.passada)))
        return 1;
    
    return 0;
}
int PosicaoEhIgual (tPosicao pos1, tPosicao pos2)
{
    if (pos1.linha == pos2.linha && pos1.coluna == pos2.coluna)
        return 1;
    return 0;
}
int GeraLinha (tPosicao posicao)
{
    return posicao.linha;
}
int GeraColuna (tPosicao posicao)
{
    return posicao.coluna;
}
int ComparaPosMatriz (tPosicao posicao, int linha, int coluna)
{
    if (posicao.linha == linha && posicao.coluna == coluna)
        return 1;
    return 0;
}

//Função char
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

//Funções Void
void ImprimeSaidas (tMapa mapa, char diretorio[MAX_DIRETORIO])
{
    PrintaEstatisticas (mapa.pac, diretorio);
    PrintaRanking (mapa.pac, diretorio);
    PrintaTrilha (mapa.trilha, mapa.linha, mapa.coluna, diretorio);
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
void PrintaInicializacao (tMapa mapa, char diret[MAX_DIRETORIO])
{

    int linha, coluna, ja_printou_aqui, i;
    FILE * file;
    file = fopen (diret, "w");
    PrintaMapaFILE (mapa, file);
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
void PrintaSaidaAtual (tMapa mapa)
{
    tPacMan pac = mapa.pac;
    printf ("Estado do jogo apos o movimento '%c':\n", mapa.mov_atual);
    PrintaMapaTerminal (mapa);
    printf ("Pontuacao: %d\n\n", pac.comidas);
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
                if (!mapa.pac.morreu)
                {
                    printf (">");
                    ja_printou_aqui = 1;
                }

            }
            if (!ja_printou_aqui)
                printf ("%c", mapa.matriz[linha][coluna]);
        }
        printf ("\n");
    }
}
void PrintaDerrota (tPacMan pac)
{
    printf ("Game over!\nPontuacao final: %d\n", pac.comidas);
}
void PrintaVitoria (tPacMan pac)
{
    printf ("Voce venceu!\nPontuacao final: %d\n", pac.comidas);
}
void PrintaRanking (tPacMan pac, char diretorio[MAX_DIRETORIO])
{
    char ranking[MAX_DIRETORIO+16];
    sprintf (ranking, "%s/ranking.txt", diretorio);
    FILE *file = fopen (ranking, "w");

    int i, j;
    tMov mov_aux; //Usado apenas para facilitar as trocas de posições
    for (i = 0; i < 4; i++)
    {
        for (j = i+1; j < 4; j++)
        {
            if (pac.mov[i].comida < pac.mov[j].comida)
            {
                mov_aux = pac.mov[i];
                pac.mov[i] = pac.mov[j];
                pac.mov[j] = mov_aux;
            }
            else if (pac.mov[i].comida == pac.mov[j].comida)
            {
                if (pac.mov[i].colisoes > pac.mov[j].colisoes)
                {
                    mov_aux = pac.mov[i];
                    pac.mov[i] = pac.mov[j];
                    pac.mov[j] = mov_aux;
                }
                else if (pac.mov[i].colisoes == pac.mov[j].colisoes)
                {
                    if (pac.mov[i].usou < pac.mov[j].usou)
                    {
                        mov_aux = pac.mov[i];
                        pac.mov[i] = pac.mov[j];
                        pac.mov[j] = mov_aux;
                    }
                    else if (pac.mov[i].usou == pac.mov[j].usou)
                    {
                        if (pac.mov[i].letra > pac.mov[j].letra)
                        {
                            mov_aux = pac.mov[i];
                            pac.mov[i] = pac.mov[j];
                            pac.mov[j] = mov_aux;
                        }
                    }
                }
            }
        }
        
    }

    for (i = 0; i < 4; i++)
    {
        tMov mov = pac.mov[i]; //Apenas para facilitar na impressão
        fprintf (file, "%c,%d,%d,%d\n", mov.letra, mov.comida, mov.colisoes, mov.usou);
    }
    
}
void PrintaEstatisticas (tPacMan pac, char diretorio[MAX_DIRETORIO])
{
    int colisoes = 0, i;
    for (i = 0; i < 4; i++)
    {
        colisoes += pac.mov[i].colisoes;
    }

    char estatistica[MAX_DIRETORIO+16];
    sprintf (estatistica, "%s/estatisticas.txt", diretorio);
    FILE *file = fopen (estatistica, "w");

    fprintf (file, "Numero de movimentos: %d\n", pac.mov_total);
    fprintf (file, "Numero de movimentos sem pontuar: %d\n", pac.mov_total - pac.comidas);
    fprintf (file, "Numero de colisoes com parede: %d\n", colisoes);
    fprintf (file, "Numero de movimentos para baixo: %d\n", pac.mov[2].usou); //Movimento 's'
    fprintf (file, "Numero de movimentos para cima: %d\n", pac.mov[0].usou); //Movimento 'w'
    fprintf (file, "Numero de movimentos para esquerda: %d\n", pac.mov[1].usou); //Movimento 'a'
    fprintf (file, "Numero de movimentos para direita: %d\n", pac.mov[3].usou); //Movimento 'd'

}
void PrintaResumoComida (tPacMan pac, tMov mov, FILE *arq_resumo)
{
    fprintf (arq_resumo, "Movimento %d (%c) pegou comida\n", pac.mov_total, mov.letra);
}
void PrintaResumoColisao (tPacMan pac, tMov mov, FILE *arq_resumo)
{
    fprintf (arq_resumo, "Movimento %d (%c) colidiu na parede\n", pac.mov_total, mov.letra);
}
void PrintaResumoMorreu (tPacMan pac, tMov mov, FILE *arq_resumo)
{
    fprintf (arq_resumo, "Movimento %d (%c) fim de jogo por encostar em um fantasma\n", pac.mov_total, mov.letra);
}