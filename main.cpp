#include <iostream>
#include <math.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <queue>
#include <iomanip>
#include <set>
#include <map>
#include <list>
#include <algorithm> 

using namespace std;
using std::setprecision;

typedef struct aresta {
    int destino; // destino
    int origem; // origem
    float peso; //peso
    bool removed;
} aresta;

typedef struct vertice {
  int classe;
  float x;
  float y;
  bool isVisitado;
} vertice;

typedef struct grafo {
  int m;
  int n;
  vector<vertice> vertices;
  vector<vector<aresta >> arestas;
} grafo;

class OPF {
    public:
        // Funcoes para manipulacao do arquivo
        string lerArquivo(string filename);
        vector<string> processaEntrada(string input, const char *delim);
        vector<vertice> processaLinhas(vector<string> all_lines);

        // Fucoes grafo
        grafo criaGrafo(int m, int n, vector<vertice> vertices);
        vertice criaVertice(float classfic, float x, float y);
        aresta criaAresta(vertice v1, vertice v2, int destino, int des);
        vector<aresta> getVector(grafo g);

        vector<aresta> prim(grafo g);

        // Calculo da distancia euclideana
        float distanciaEuclideana(vertice v1, vertice v2);


        // Funcoes para imprimir arestas e vertices e o grafo completo
        void imprimeGrafo(grafo g);
        void imprimeArestas(aresta e);
        void imprimeVertices(vertice v);

        // Funcao para predizer
        int classifica(vector<aresta> prim_mst, vector<vertice> vertices, vertice v);

};


int main () {

    OPF model;
    vertice v;

    string data = model.lerArquivo("spirals.txt");
    vector<string> all_lines = model.processaEntrada(data, "\r\n");
    vector<vertice> vertices = model.processaLinhas(all_lines);

    // Cria grafo
    grafo g = model.criaGrafo(vertices.size(), vertices.size(), vertices);


    vector<aresta> prim_mst = model.prim(g);

    // model.imprimeArestas(g);
    model.imprimeGrafo(g);

    cout << endl;

    v.x = 2.583113;
    v.y = 6.236187;
    
    cout << "X = " << v.x << "\nY = " << v.y << "\nClasse predita: " <<  model.classifica(prim_mst, vertices, v) << endl << endl;

}

int OPF::classifica(vector<aresta> prim_mst, vector<vertice> vertices, vertice v) {
    set<pair<double, float>> dist_individuos;
    vector <double> mst;
    vector <int> salva;

    float distance = 0;
    int predicao;
    int cont=0;

    // Calculando a distancia com base nas vertices passadas
    // A vertice e percorrida no sentido i na matriz, e adicionado o indice e a distancia em um set para matermos ordenado
    for (int i = 0; i < prim_mst.size(); i++) {
        distance = distanciaEuclideana(vertices.at(i), v);
        dist_individuos.insert(make_pair(distance, i));
    }
    
    // Percorrendo as vertices para pegar as classes possiveis no dataset
    for (auto x : vertices) 
        salva.push_back(x.classe);
    
    // Adicionando todos os custos das arestas da MST
    for (auto x: prim_mst) 
        mst.push_back(x.peso);
    
    // ordenando a mst
    sort(mst.begin(), mst.end()); 

     // Prevendo a classe
    for (auto x: dist_individuos) {
        // Verifico se o menor valor do set calculado, e menor que a do indice da MST denotado pelo set, cruzando as informacoes
           if (x.first <= mst[x.second]) {  
            // Se for menor pega o indice da classe com o indice armazenado no set
            predicao = salva[x.second];
            cont++;
            // Para garantir que pegarar sempre a primeira previsao, que e a de menor distancia do set
            if (cont == 1)
                break;
        }
    }

    // Retorna a predicao
    return predicao;
}

// Funcao retorna um novo grafo do tipo grafo
grafo OPF::criaGrafo(int m, int n, vector<vertice> vertices) {
    grafo g;

    g.m = m;
    g.n = n;

    g.vertices = vertices;

    for(int i = 0 ; i < g.m ; i++) {
        vector<aresta> row;
        for(int j = 0 ; j < g.n ; j++) {
        row.push_back(criaAresta(vertices.at(i), vertices.at(j), i, j));
        }
        g.arestas.push_back(row);
    }

    return g;
}

// Funcao retorna uma nova vertice do tipo vertice
vertice OPF::criaVertice(float classfic, float x, float y) {
    vertice v;
    v.classe = classfic;
    v.x = x;
    v.y = y;
    v.isVisitado = false;
    return v;
}

// Funcao retorna uma nova aresta do tipo aresta
aresta OPF::criaAresta(vertice v1, vertice v2, int destino, int origem) {
    aresta e;

    e.peso = distanciaEuclideana(v1, v2);
    e.destino = destino;
    e.origem = origem;
    e.removed = false;
    return e;
}

// Calcula distancia euclideana para as combinacoes das features
float OPF::distanciaEuclideana(vertice v1, vertice v2) {
    float x = ((v1.x - v2.x) * (v1.x - v2.x));
    float y = ((v1.y - v2.y) * (v1.y - v2.y));

    float xy = x + y;

    return sqrt(xy);
}

// retorna uma string de leitura do arquivo
string OPF::lerArquivo(string filename) {
    string buffer;
    // abre o arquivo
    ifstream fin(filename, fstream::in);
    if(!fin.is_open()) {
        cout << "Erro!";
        return 0;
    } else {
        char c;
        // Ler os dados do arquivo e adiciona na string buffer
        while(fin.get(c)) buffer.push_back(c);
        // Apos terminar o while, o arquivo e fechado
        fin.close();
    }
    // Retorna todos os dados lidos do arquivo
    return buffer;
}

// Retorna um vetor de strings contendo cada linha separada
vector<string> OPF::processaEntrada(string input, const char *delim) {
    // String auxliar do tamanho do buffer retornado
    char *str = new char[input.length() + 1];

    // Copia para a string str os dados do buffer de entrada
    strcpy(str, input.c_str());

    // Cria um vetor para armazenar as strings
    vector<string> all;

    // Separa todos os dados por um delimitador por meio do strtok
    char *token = strtok(str, delim);

    // Ler todos os dados do token e adiciona no vetor
    while(token != NULL) {
        all.push_back(string(token));
        token = strtok(NULL, delim);
    }
    // Deleta o ponteiro com a string auxiliar
    delete [] str;

    // Retorna o vetor com os dados
    return all;
}

// Retorna as vertices das linhas processadas
vector<vertice> OPF::processaLinhas(vector<string> all_lines) {
    vector<vertice> vertices;
    // Percorre todas as linhas
    for(string s : all_lines) {
        // Adiciona em uma variavel auxilar usando delimitador com texto
        vector<string> line = processaEntrada(s, " ");
        string::size_type sz;
        // Adiciona uma nova vertice, passando as colunas do arquivo, sendo primera {classe}, segunda {variaveis} e terceira {variaveis}
        // referencia funcao {at} https://www.cplusplus.com/reference/vector/vector/at/
        vertice v = criaVertice(stof(line.at(2).c_str(), &sz), stof(line.at(0).c_str(), &sz), stof(line.at(1).c_str(), &sz));
        // Adiciona a vertice
        vertices.push_back(v);
    }
    // retorna a vertice
    return vertices;
}

// Algoritmo de prim, aplicando a MST/Recorte de arestas
vector<aresta> OPF::prim(grafo g) {
    // A funcao getVector devolve um vetor de areas
    vector<aresta> arestas = getVector(g);

    // Funcao lambda
    // Escolhendo qualquer vértice do grafo como vértice inicial/de partida
    // A funcao auto gera um loop, oonde e adicionado em um vetor o retorno da instancia
    // se a o peso da aresta esquerda e maior que a da direita
    auto cmp_pq = [](aresta left, aresta right) { return (left.peso) > (right.peso); };

    // Fila de prioridade do tipo aresta
    // As filas de prioridade sao projetados especificamente para que seu primeiro elemento seja sempre o maior dos elementos que contém,
    priority_queue<aresta, vector<aresta>, decltype(cmp_pq)> pq(cmp_pq);

    // Itera em cada aresta e adiciona na fila
    for (aresta e : arestas)
        pq.push(e);

    // Declara um vetor do tipo aresta para a MST
    vector<aresta> mst;

    // Enquanto a fila for maior que 0
    while (pq.size() > 0) {
        // Extraior o valor do topo da fila, como e uma fila de prioridade, e extraido o maior valor
        aresta e = pq.top();
        // remove o maior valor
        pq.pop();
        aresta min_aresta;
        do {
            // se a vertice de destino nao foi visitada, adiciono esta vertice a MST, junto da sua origem e destino, e a marco como visitada
            if (!g.vertices.at(e.destino).isVisitado) {
                mst.push_back(g.arestas.at(e.destino).at(e.origem));
                g.vertices.at(e.destino).isVisitado = true;
            }

            // Vetor de arestas com as arestas de origem do grafo
            vector<aresta> adjs = g.arestas.at(e.origem);

            // Se o valor da aresta de origem for menor que o tamanho contido no vetor de arestas de destino, entao esta e uma aresta minima
            if ((int)(e.origem + 1) <= (adjs.size() - 1))
                min_aresta = adjs.at(e.origem + 1);
            else
                // Caso contrario nao e uma aresta minima, quebra o loop
                break;
            // Por fim percorrer cada aresta adjacente
            for (int i = (e.origem + 2); i <= (adjs.size() - 1); i++) {
                // atualiza a aresta com o valor 
                aresta adj = adjs.at(i);
                // Verifica se a vertice do indice do vetor de adjacencia ja foi visitado
                if (!g.vertices.at(adj.destino).isVisitado) {
                    // Se o peso da aresta de adjacencia for menor, faz uma troca dos valores
                    if (adj.peso < min_aresta.peso)
                        min_aresta = adj;
                }
            }
            // Por fim adiciona a aresta recebe a sua nova posicao baseada no seu destino, e sua origem
            e = g.arestas.at(min_aresta.destino).at(min_aresta.origem);
            // Percorrer enquanto todas as vertices nao forem visitadas
        } while (!g.vertices.at(e.destino).isVisitado);
    }

    // Retorna o grafo com o recorte de arestas
    return mst;
}


void OPF::imprimeGrafo(grafo g) {

    // Cria um vetor do tipo vertice para adicionar as vertices do grafo
    vector<vertice> row = g.vertices;
    

    // Para cada vertice em row imprime a vertice
    for(vertice v : row) {
        imprimeVertices(v);
        cout << " ";
    }
    cout << "\n";
    cout << "\n";

    // percorre todo o vetor das vertices imprimindo cada combinacao de arestas i e j
    for(int i = 0 ; i < row.size() ; i++) {
        cout << " ";
        for(int j = 0 ; j < row.size() ; j++) {
        imprimeArestas(g.arestas.at(i).at(j));
        cout << " |";
        }
        cout << "\n";
    }
}

// Funcao auxiliar para imprimir as vertices
void OPF::imprimeVertices(vertice v) {
    // Setando a precisao para 4 casas decimais
    cout << fixed << setprecision(4);
    cout << v.classe << "-(" << v.x << ", " << v.y << ")";
}

// Funcao auxiliar para imprimir as arestas
void OPF::imprimeArestas(aresta e) {
    cout << fixed << setprecision(4);
    cout << e.destino << "->" << e.origem << ": "  << e.peso;
   
}

// Funcao para retornar um vetor separado com as arestas do grafo
vector<aresta> OPF::getVector(grafo g) {
    vector<aresta> v;
    for (vector<aresta> row : g.arestas) {
        for (aresta col : row)
            if (col.destino < col.origem)
                v.push_back(col);
    }
    return v;
}