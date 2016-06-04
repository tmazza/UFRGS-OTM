#include <iostream>
#include <malloc.h>
#include <cstdlib>
#include <time.h>

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

int seed = 987654;
int penalizacaoPorElemento = 2;
float kFlips=1;
int limiteSemMelhora = 100;


int main(){
  const clock_t begin_time = clock();
  cin >> m >> n;
  init();
  cout << "Best find:" << ILS(S) << endl;
  end();
  cout << "Total time: " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << "s" << endl;
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
  custoAtual = busca_local(S,custoAtual);
  cout << countSemMelhora << " | -Current: " << custoAtual << endl;

  while(countSemMelhora < limiteSemMelhora){
    S_ = perturbar(S);
    novoCusto = valorSolucao(S_);
    cout << custoAtual << "|" << novoCusto << "  " << endl;
    novoCusto = busca_local(S_,novoCusto);

    countSemMelhora++;
    cout << countSemMelhora << "|";
    if(novoCusto < custoAtual){
       copiaSolucao(S_,S);
       custoAtual = novoCusto;
       cout << " | Current: " << custoAtual << endl;
       countSemMelhora = 0;
    }
  }
  return custoAtual;
}

/**
 * Best improvement
 */
int busca_local(bool *Sol,int sAtual){
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
 * Calcula o custo/valor de uma solução:
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
    for(int i=0;i<n;i++){
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
 * Copia S para S_ com alguns bits alterados. Só altera o bit se S_ se mantém uma solução válida
 * Altera kFlips bits de S garantindo que o novo S gerado seja uma solução
 */
bool* perturbar(bool* Sol){
  for(int i=0;i<n;i++)
    S_[i] = Sol[i];
  srand48(seed);
  seed+=100;
  seed=seed%999999;
  // int flips = (int) n * kFlips;
  int flips = kFlips;
  for(int i=0;i<flips;i++){
    int indice = (int) (drand48() * n);
    S_[indice] = !S_[indice];
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
}

bool** alocaMatriz(){
  bool **M = (bool**) malloc(n * sizeof(bool *));
  for (int i=0; i<n;i++){
    M[i] = (bool*) malloc(m * sizeof(bool));
  }
  return M;
}
