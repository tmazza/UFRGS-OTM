#include <iostream>
#include <malloc.h>
#include <cstdlib>
#include <time.h>
#include <stdio.h>

using namespace std;


void init();
void end();
bool** alocaMatriz();

int ILS(bool *S);
int busca_local(bool* Sol,int sAtual);
int valorSolucao(bool* Sol);
bool* perturbar(bool* Sol);
bool* copiaSolucao(bool* S1,bool* S2);


int m,n; // quantidade de elementos, quantidade de subconjunos,solução atual,incice para flip
bool **R; // Define se conjunto i+1 possui elemento j+1
bool *S,*S_; //   Solução corrente, solução sendo avaliada
int *V; // Custo de cada subconjunto
int *C; // Conta em quantos subconjutos da solução atual o elemento j está

int seed;
int penalizacaoPorElemento = 2;
float kFlips;
int limiteSemMelhora = 1000;
int tempoLimite = 1800;
clock_t begin_time;
float tempoDecorrido;
bool flatExcedido;

void doAll(int k){
  kFlips = k;

  int seeds[10] = {123456,578459,543210,567891,987654,987515,412875,165873,985128,247551};
  int resultado;
  float tempo;
  int startSeed;

  for(int iSeed=0;iSeed<3;iSeed++){
    begin_time = clock();
    flatExcedido = false;

    for(int i=0;i<n;i++){  S[i]=0; }
    startSeed = seed = seeds[iSeed];
    resultado = ILS(S);

    tempo = int( clock () - begin_time ) /  CLOCKS_PER_SEC;
    printf("\n%d,%d,%d,%f,%c",k,startSeed,resultado,tempo,flatExcedido?'*':'N');
  }

  printf("\n\n");
}

int main(int argc, char* argv[]){
  cin >> m >> n;
  cout << "K,Seed,Melhor,Tempo,limite tempo execido? ";
  init();
  doAll(4);
  doAll(16);
  doAll(64);
  end();
  return 0;
}

int isSolucao(){
  int qtdElementos;
  for(int j=0;j<m;j++){
    qtdElementos=0;
    for(int i=0;i<n;i++){
      if(S[i] && R[i][j]){ // subconjunto i é parte da solução e contém elemento j
        qtdElementos++;
      }
    }
    if(qtdElementos <= 0)
      return 0;
  }
  return 1;
}

/**
 * Para após limiteSemMelhora iterações.
 * Pertubação: kFlips bits de indice aleatório alterados
 * Buca local: best improvement
 */
int ILS(bool *S){
  int countSemMelhora=0,custoAtual,novoCusto;

  cin >> m >> n;
  custoAtual = valorSolucao(S); // Todo os bits são 1
  if(custoAtual == -1){
    flatExcedido=true;
    return -1;
  }

  custoAtual = busca_local(S,custoAtual);

  while(countSemMelhora < limiteSemMelhora){

    // Controle de tempo
    if(countSemMelhora % 10 == 0){
      tempoDecorrido = (float( clock () - begin_time ) /  CLOCKS_PER_SEC);
      if(tempoDecorrido >= tempoLimite){
        flatExcedido=true;
        return custoAtual;
      }
    }

    S_ = perturbar(S);
    novoCusto = valorSolucao(S_);
    novoCusto = busca_local(S_,novoCusto);

    countSemMelhora++;
    // cout << countSemMelhora << "|" << custoAtual << "|" << novoCusto << " | Tempo: " << tempoDecorrido << endl ;
    if(novoCusto < custoAtual){
       copiaSolucao(S_,S);
       custoAtual = novoCusto;
       //cout << "       | NOVO: " << custoAtual  << endl;
       countSemMelhora = 0;
    }
  }
  return custoAtual;
}

/**
 * Best improvement
 */
int busca_local(bool *Sol,int sAtual){

  tempoDecorrido = (float( clock () - begin_time ) /  CLOCKS_PER_SEC);
  if(tempoDecorrido >= tempoLimite){
    flatExcedido=true;
    return -1;
  }

  int valorBase,nova;
  int indice=-1;
  valorBase = valorSolucao(Sol);
  for(int i=0;i<n;i++){ // Busca melhor vizinho
    if(S[i]){
      nova = valorBase - V[i];
      for(int j=0;j<m;j++) // verifica se algum elemento ficará descoberto e penaliza
        if(R[i][j] && C[j]-1 <= 0)
          nova += penalizacaoPorElemento;
    } else {
      nova = valorBase + V[i];
      for(int j=0;j<m;j++) // remove penalzação caso algum elemento passe a ser coberto
        if(R[i][j] && C[j] <= 0)
          nova -= penalizacaoPorElemento;
    }
    if(nova < sAtual){
      sAtual = nova;
      indice = i;
    }
  }
  if(indice == -1) { // Nenhum vizinho melhor
    return sAtual;
  } else {
    Sol[indice]=!Sol[indice];
    return busca_local(Sol,sAtual);
  }
}

/**
 * Calcula o custo/valor de uma soluç1ão:
 * Soma dos custos dos subconjuntos contidos na solução
 * + penalização por elemento faltando na solução
 */
int valorSolucao(bool *Sol){
  int soma=0;
  for(int i=0;i<n;i++) // Soma custo dos subconjuntos da solução
    if(Sol[i])
      soma+=V[i];

  int qtdCobertura; // Quantidade de subconjuntos que um elemento está sendo coberto
  for(int j=0;j<m;j++){
    qtdCobertura=0;
    for(int i=01;i<n;i++){
      if(Sol[i] && R[i][j]){ // subconjunto i é parte da solução e contém elemento j
        qtdCobertura++;
      }
    }
    C[j]=qtdCobertura;
    if(C[j] <= 0){
      soma+=penalizacaoPorElemento;
    }
  }
  return soma;
}

/**
 * Copia S para S_ com alguns bits alterados.
 */
bool* perturbar(bool* Sol){
  for(int i=0;i<n;i++)
    S_[i] = Sol[i];
  srand48(seed);
  seed+=10;
  seed=seed%999999;
  // int flips = (int) n * kFlips;
  int flips = kFlips;
  int i=0;
  while(i<flips){
    int indice = (int) (drand48() * n);
    if(S_[indice]){
      S_[indice] = S_[indice]?false:true;
    }
    i++;
  }
  return S_;
}

/**
 * Copia S1 para S2
 */
bool* copiaSolucao(bool* S1,bool* S2){
  for(int i=0;i<n;i++){
    S2[i] = S1[i];
  }
}

/**
 * Cria estruturas necessárias, lê arquivo e inicializa estrturas
 */
void init(){
  V = (int*) malloc(n * sizeof(int));
  S = (bool*) malloc(n * sizeof(bool));
  S_ = (bool*) malloc(n * sizeof(bool));
  C = (int*) malloc(m * sizeof(int));

  R = alocaMatriz();

  // Lê lista de valores e incializa lista de restrições e solução inicial
  for(int i=0;i<n;i++){
    cin >> V[i];
    S[i]=0; // Solução inicial contém todos os subconjunto
    for(int j=0;j<m;j++) { R[i][j]=false; }
  }

  int qtdSubConj; // Quantidade de subconjuntos que elemento aparece
  int temp; // Para leitura dos subconjuntos
  for(int i=0;i<m*2;i+=2) {
    cin >> qtdSubConj;
    C[i/2] = qtdSubConj;
    for(int j=0;j<qtdSubConj;j++){
      cin >> temp;
      R[temp-1][i/2] = true;
    }
  }
}

void end(){
  free(V);
  free(S);
  free(S_);
  free(C);
  free(R);
  cout << endl;
}

bool** alocaMatriz(){
  bool **M = (bool**) malloc(n * sizeof(bool *));
  for (int i=0; i<n;i++){
    M[i] = (bool*) malloc(m * sizeof(bool));
  }
  return M;
}
