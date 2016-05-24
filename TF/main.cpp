#include <iostream>
#include <malloc.h>
#include <cstdlib>
#include <time.h>

using namespace std;


void init();
void end();
bool** alocaMatriz();

int valorSolucao(bool* Sol);
bool* busca_local(bool* Sol,int sAtual);
bool* perturbar(bool* Sol);
bool* copiaSolucao(bool* S_);

void updateCountElementos(int indice);
bool isUmaSolucao(int i);

int ILS(bool *S);

int m,n,sIndice; // quantidade de elementos, quantidade de subconjunos,solução atual,incice para flip
bool **R; // Define se conjunto i+1 possui elemento j+1
bool *S,*S_; // Solução corrente, solução sendo avaliada
int *V; // Custo de cada subconjunto
int *C; // Conta em quantos subconjutos da solução atual o elemento j está

int seed=123456;
int kFlips=10;

int main(){
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
  int limiteSemMelhora = 1, countSemMelhora=0,custoAtual,novoCusto;

  custoAtual = valorSolucao(S); // S é inicializado com todos bits == 1
  S = busca_local(S,custoAtual);
  updateCountElementos(sIndice);
  custoAtual = valorSolucao(S);

  while(countSemMelhora < limiteSemMelhora){
    S_ = perturbar(S);
    S_ = busca_local(S_,custoAtual);
    novoCusto = valorSolucao(S_);
    countSemMelhora++;
    if(novoCusto < custoAtual){
      S = copiaSolucao(S_);
      custoAtual = novoCusto;
      countSemMelhora = 0;
    }
  }
  return custoAtual;
}

/**
 * Copia S_ para S atualizando o contador de elementos por subconnjunto para
 * cada alteração ocorrida em S_
 */
bool* copiaSolucao(bool* Sol){
  for(int i=0;i<n;i++){
    if(Sol[i]==S[i]){
      S[i] = Sol[i];
    } else {
      S[i] = Sol[i];
      updateCountElementos(i);
    }
  }
  return S;
}

/**
 * Copia S para S_ com alguns bits alterados. Só altera o bit se S_ se mantém uma solução válida
 * Altera kFlips bits de S garantindo que o novo S gerado seja uma solução
 */
bool* perturbar(bool* Sol){
  for(int i=0;i<n;i++){
    S_[i] = Sol[i];
  }
  srand48(seed);
  seed+=100;
  seed=seed%999999;

  int count = 0;
  while(count < kFlips){
    int indice = (int) (drand48() * n);
    if(!S_[indice] || (S_[indice] && isUmaSolucao(indice))){
      S_[indice] = !S_[indice];
      count++;
    }
  }
  return S_;
}

/**
 * Testa se solução S com flip em i é valida
 * Testa se todos os elementos de S[i] estão contidos em algum
 * dos demias sub conjuntos de S
 */
bool isUmaSolucao(int i){
  for(int j=0;j<m;j++){ // Se j em SCi e SCi vai ser removido então j deve estar em algum outro SC de S
    if(R[i][j] && (C[j]-1 <= 0))
      return false;
  }
  return true;
}

/**
 * Best improvement
 */
bool* busca_local(bool *Sol,int sAtual){
  int sMelhor,sNova;
  sIndice=-1;
  sMelhor = sAtual;
  for(int i=0;i<n;i++){
    if(Sol[i]){ // após flip subconjunto será removido
      sNova = sAtual - V[i];
      if(!isUmaSolucao(i)){
        sNova += 10000;
      }
    } else { // após flip subconjunto será adicionado
      sNova = sAtual + V[i];
    }
    if(sNova < sMelhor){
      sMelhor = sNova;
      sIndice = i;
    }
  }
  if(sIndice != -1)
    Sol[sIndice]=!Sol[sIndice];
  return Sol;
}

/**
 * Considera estado final de S.
 * Se S[i]=0 ocorreu a troca 1 para 0 então decrementa-se o contador dos elementos que estão no subconjunto i.
 * Se S[i]=1 ocorreu a troca 0 para 1 então incrementa-se o contador dos elementos que estão no subconjunto i.
 */
void updateCountElementos(int indice){
  for(int j=0;j<m;j++){
    if(S[indice] && R[indice][j]){
      C[j]++;
    }
    if(!S[indice] && R[indice][j]){
      C[j]--;
    }
  }
}

int valorSolucao(bool *Sol){
  int soma=0;
  for(int i=0;i<n;i++){
    if(Sol[i]!=0){
      soma+=V[i];
    }
  }
  return soma;
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
