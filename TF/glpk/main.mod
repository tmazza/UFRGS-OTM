param m, integer, >= 0; # qtd. de elementos
param n, integer, >= 0; # qtd. de subconjuntos
param V{1..n}; # custo de cada subconjunto
param R{1..m,1..n}, binary; # se elemento i está no subconjunto j
var S{1..n}, binary; # se subconjunto i faz parte da solução

s.t. map{i in 1..m}: sum{j in 1..n} S[j]*R[i,j] >= 1; # cada elemento pelo menos em um subcojunto
minimize obj: sum{j in 1..n} S[j]*V[j]; #minimiza o custo da cobertura

end;
