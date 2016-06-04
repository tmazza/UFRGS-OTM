#include <iostream>
#include <malloc.h>


using namespace std;

int m,n;
unsigned short int *V;
bool **R;

bool** alocaMatriz(){
  bool **M = (bool**) malloc(n * sizeof(bool *));
  for (int i=0; i<n;i++){
    M[i] = (bool*) malloc(m * sizeof(bool));
  }
  return M;
}

int main(){
  cin >> m >> n;

  R = alocaMatriz();
  V = (unsigned short int  *) malloc( sizeof(unsigned short int) * n );

  // Lê lista de valores e incializa lista de restrições e solução inicial
  for(int i=0;i<n;i++){
    cin >> V[i];
    for(int j=0;j<m;j++) { R[i][j]=false; }
  }

  int qtdSubConj; // Quantidade de subconjuntos que elemento aparece
  int temp; // Para leitura dos subconjuntos
  for(int i=0;i<m*2;i+=2) {
    cin >> qtdSubConj;
    for(int j=0;j<qtdSubConj;j++){
      cin >> temp;
      R[temp-1][i/2] = true;
    }
  }

  cout << "data;" << endl;
  cout << "param m := " << m << ";" << endl;
  cout << "param n := " << n << ";" << endl;

  // Param V
  cout << "param V :=" << endl;
  for(int i=0;i<n;i++){
    cout << i+1 << " " << V[i] << endl;
  }
  cout << ";" << endl;

  // Param R
  cout << "param R : ";
  for(int i=0;i<n;i++){ cout << i+1 << " "; }
  cout << " := " << endl;
  for(int j=0;j<m;j++){
    cout << j+1 << " ";
    for(int i=0;i<n;i++){
      cout << R[i][j] << " ";
    }
    cout << endl;
  }
  cout << ";" << endl;


  cout << "end;" << endl;


  free(V);


  return 0;
}
