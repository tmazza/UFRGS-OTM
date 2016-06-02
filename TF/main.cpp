#include <iostream>
#include <malloc.h>
#include <cstdlib>
#include <time.h>

using namespace std;


void init();
void end();
bool** alocaMatriz();

int getCustoRemocao(int i);
int valorSolucao(bool* Sol);
int busca_local(bool* Sol,int sAtual);
bool* perturbar(bool* Sol);
bool* copiaSolucao(bool* S_);

void atualizaContagemDeElementos(bool* S);

int ILS(bool *S);

int m,n; // quantidade de elementos, quantidade de subconjunos,solução atual,incice para flip
bool **R; // Define se conjunto i+1 possui elemento j+1
bool *S,*S_; //   Solução corrente, solução sendo avaliada
int *V; // Custo de cada subconjunto
int *C; // Conta em quantos subconjutos da solução atual o elemento j está

int seed;
int penalizacaoPorElemento = 10;
float kFlips=0;

bool isSolucao(){
  for(int j=0;j<m;j++)
    if(C[j]<=0) return false;
  return true;
}

int main(){
  seed = 123456;
  cin >> m >> n;
  init();
  cout << ILS(S) << endl;
  end();
  return 0;
}

/**
 * Para após limiteSemMelhora iterações.
 * Pertubação: kFlips bits de indice aleatório alterados
 * Buca local: best improvement
 */
int ILS(bool *S){
  int limiteSemMelhora = 100, countSemMelhora=0,custoAtual,novoCusto;

  custoAtual = valorSolucao(S); // Todo os bits são 1
  custoAtual = busca_local(S,custoAtual);

  while(countSemMelhora < limiteSemMelhora){
    S_ = perturbar(S);
    novoCusto = valorSolucao(S_);

    novoCusto = busca_local(S_,novoCusto);

    countSemMelhora++;
    if(novoCusto < custoAtual){
      S = copiaSolucao(S_);
      custoAtual = novoCusto;
      cout << countSemMelhora << " | NC: " << custoAtual << " | SOL: " << isSolucao() << endl;
      countSemMelhora = 0;
    }
  }
  return custoAtual;
}

/**
 * Best improvement
 */
int busca_local(bool *Sol,int sAtual){
  int melhor,nova;
  int indice=-1;
  melhor = sAtual;
  for(int i=0;i<n;i++){
    if(Sol[i]){ // após flip subconjunto será removido
      nova = sAtual - V[i] + getCustoRemocao(i);
    } else { // após flip subconjunto será adicionado
      nova = sAtual + V[i];
    }
    if(nova < melhor){
      melhor = nova;
      indice = i;
    }
  }
  if(indice != -1) // Aplica flip
    Sol[indice]=!Sol[indice];

  return melhor;
}

/**
 * Calcula o custo/valor de uma solução
 * Soma dos custos dos subconjuntos contidos na solução
 * + penalização por elemento faltando na solução
 */
int valorSolucao(bool *Sol){
  atualizaContagemDeElementos(Sol);

  int soma=0;
  for(int i=0;i<n;i++) // Soma custo dos subconjuntos da solução
    if(Sol[i])
      soma+=V[i];
  for(int j=0;j<m;j++){ // Inclui penalização para cada elemento que não está na solução
    if(C[j] <= 0){
      soma+=penalizacaoPorElemento;
    }
  }
  return soma;
}

/**
 * Mantém contador da quantidade de subconjuntos em que um elemento está presente
 */
void atualizaContagemDeElementos(bool* Sol){
  int qtd;
  for(int j=0;j<m;j++){
    qtd=0;
    for(int i=0;i<n;i++){
      if(Sol[i] && R[i][j]){ // subconjunto i é parte da solução e contém elemento j
        qtd++;
      }
    }
    C[j]=qtd;
  }
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
  int flips = (int) n * kFlips;
  for(int i=0;i<flips;i++){
    int indice = (int) (drand48() * n);
    S_[indice] = !S_[indice];
  }
  return S_;
}

/**
 * Copia S_ para S atualizando o contador de elementos por subconnjunto para
 * cada alteração ocorrida em S_
 */
bool* copiaSolucao(bool* Sol){
  for(int i=0;i<n;i++)
    S[i] = Sol[i];
  return S;
}

/**
 * Calcula o custo da remoção do subconjunto incluindo uma penalização(por elemento) caso
 * algum elemento deixe de ser coberto.
 */
int getCustoRemocao(int i){
  int custo = 0;
  for(int j=0;j<m;j++){
    if(R[i][j] && (C[j]-1 <= 0) ){ // Após remoção S deixa de ser solução então penaliza operação
      custo+=penalizacaoPorElemento;
    }
  }
  return custo;
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
    S[i]=1; // Solução inicial contém todos os subconjunto
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
