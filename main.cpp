#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <conio.h>
#include <math.h>
#include <queue>
#include <stack>
#include <cstdlib> // Para funciones de generación aleatoria
#include <ctime> 
using namespace sf;
using namespace std;

int contador = 0;

RenderWindow window(VideoMode(800, 700), "SFML Circle Example");
Event event;
template <class T>
struct node;

struct Circulo {
    CircleShape cir;
    pair<int, int> cord;
    Circulo(int x = 0, int y = 0) {
        cir.setRadius(2.5);
        cir.setFillColor(Color::Red);
        cord = { x,y };
    }
    void set(int x, int y) {
        cord = { x,y };
        cir.setPosition(x, y);
    }
    void draw() {
        window.draw(cir);
    }
    bool clicked(int x, int y) {
        FloatRect bounds = cir.getGlobalBounds();
        return bounds.contains(x, y);
    }
};


template <class T>
struct edge {
    float v;
    vector<node<T>*> nodes;
    edge(float _v, node<T>* n1, node<T>* n2) {
        nodes.push_back(n1);
        nodes.push_back(n2);
        v = _v;
    }
    int get_v() {
        return v;
    }
    void set_v(int _v) {
        v = _v;
    }
};

template <class T>
struct node {
    T v;
    vector<edge<T>*> edges;
    Circulo cir;
    pair<int, int> cord;
    node(T _v, int x = 0, int y = 0) {
        v = _v;
    }
    void set(int x, int y) {
        cord = { x,y };
        cir.set(cord.first, cord.second);
    }
    void set_v(T _v) {
        v = _v;
    }
    T get_v() {
        return v;
    }
};

int activador = 0;
template <class T>
struct graph {
    vector<edge<T>*> edges;
    vector<node<T>*> nodes;
    vector<node<T>*> pares;
    graph() {
        pares.resize(2, nullptr);
    }
    void ins_node(T v) {
        node<T>* nod = new node<T>(v);
        nodes.push_back(nod);
    }

    void ins_edge(T a, T b, int v = 1) {
        edge<T>* tmp = new edge<T>(v, find(a), find(b));
        edges.push_back(tmp);
        find(a)->edges.push_back(tmp);
        find(b)->edges.push_back(tmp);
    }
    void del_edge(T a, T b) {
        node<T>* n1 = find(a);
        node<T>* n2 = find(b);

        for (auto it = edges.begin(); it != edges.end();) {
            if (((*it)->nodes[0] == n1 && (*it)->nodes[1] == n2) ||
                ((*it)->nodes[0] == n2 && (*it)->nodes[1] == n1)) {
                // Eliminar la arista y liberar la memoria
                delete* it;
                it = edges.erase(it);
            }
            else {
                ++it;
            }
        }

        // Actualizar las listas de aristas de los nodos
        for (auto it = n1->edges.begin(); it != n1->edges.end();) {
            if ((*it)->nodes[0] == n2 || (*it)->nodes[1] == n2) {
                delete* it;
                it = n1->edges.erase(it);
            }
            else {
                ++it;
            }
        }

        for (auto it = n2->edges.begin(); it != n2->edges.end();) {
            if ((*it)->nodes[0] == n1 || (*it)->nodes[1] == n1) {
                delete* it;
                it = n2->edges.erase(it);
            }
            else {
                ++it;
            }
        }
    }
    void del_node(T a) {
        node<T>* tmp = find(a);
        if (tmp != nullptr) {
            // Eliminar todas las aristas que conectan con el nodo a eliminar
            for (int i = 0; i < edges.size(); i++) {
                if (edges[i]->nodes[0] == tmp || edges[i]->nodes[1] == tmp) {
                    edges.erase(edges.begin() + i);
                    i--;
                }
            }
            // Eliminar el nodo del vector de nodos
            for (int i = 0; i < nodes.size(); i++) {
                if (nodes[i] == tmp) {
                    nodes.erase(nodes.begin() + i);
                    break;
                }
            }
            // Liberar la memoria del nodo eliminado
            delete tmp;
        }
        for (int i = 0; i < nodes.size(); i++) {
            for (auto it = nodes[i]->edges.begin(); it != nodes[i]->edges.end();) {
                if ((*it)->nodes[0]->v < 0 || (*it)->nodes[1]->v < 0) {

                    delete* it; // Libera la memoria de la arista antes de eliminarla
                    it = nodes[i]->edges.erase(it);
                }
                else {
                    ++it;
                }
            }
        }

    }
    void del_random(int r) {
        srand(time(NULL));
        for (int i = 0; i < r; i++) {
            int n = rand() % nodes.size();
            del_node(nodes[n]->v);
        }
    }
    void print() {
        cout << "Nodos" << endl;
        for (int i = 0; i < nodes.size(); i++) {
            cout << nodes[i]->get_v() << " ";
        }
        cout << endl << "Vertices" << endl;
        for (int i = 0; i < edges.size(); i++) {
            cout << edges[i]->nodes[0]->v << " " << edges[i]->get_v() << " " << edges[i]->nodes[1]->v << endl;
        }
    }
    node<T>* find(T v) {
        for (int i = 0; i < nodes.size(); i++) {
            if (nodes[i]->v == v) {
                return nodes[i];
            }
        }
    }


    bool ciclo(vector<node<T>*>& v, node<T>* n) {
        for (int i = 0; i < v.size(); i++) {
            if (v[i] == n) {
                return true;
            }
        }
        return false;
    }

    void prints(stack<vector<node<T>*>> s) {
        while (!s.empty()) {
            std::vector<node<T>*> vec = s.top();
            s.pop();
            cout << "{";
            for (const node<T>* n : vec) {
                std::cout << n->v << " ";
            }
            cout << "} ";
        }
        cout << endl << endl << endl;
    }

    node<T>* near_p(node<T>* n, node<T>* fin) {
        node<T>* tmp = nullptr;
        int menor = 10000;
        for (int i = 0; i < n->edges.size(); i++) {
            if (n->edges[i]->nodes[0] == n) {
                float distancia = sqrt(pow((fin->cord.first - n->edges[i]->nodes[1]->cord.first), 2) + pow((fin->cord.second - n->edges[i]->nodes[1]->cord.second), 2));
                if (distancia < menor) {
                    tmp = n->edges[i]->nodes[1];
                    menor = distancia;
                }
            }
            else if (n->edges[i]->nodes[1] == n) {
                float distancia = sqrt(pow((fin->cord.first - n->edges[i]->nodes[0]->cord.first), 2) + pow((fin->cord.second - n->edges[i]->nodes[0]->cord.second), 2));
                if (distancia < menor) {
                    tmp = n->edges[i]->nodes[0];
                    menor = distancia;
                }
            }
        }
        return tmp;
    }

    node<T>* near_p_modify(node<T>* n, node<T>* fin) {
        node<T>* tmp = nullptr;
        int menor = 10000;
        for (int i = 0; i < n->edges.size(); i++) {
            if (n->edges[i]->nodes[0] == n) {
                float distancia = n->edges[i]->v + sqrt(pow((fin->cord.first - n->edges[i]->nodes[1]->cord.first), 2) + pow((fin->cord.second - n->edges[i]->nodes[1]->cord.second), 2));
                if (distancia < menor) {
                    tmp = n->edges[i]->nodes[1];
                    menor = distancia;
                }
            }
            else if (n->edges[i]->nodes[1] == n) {
                float distancia = n->edges[i]->v + sqrt(pow((fin->cord.first - n->edges[i]->nodes[0]->cord.first), 2) + pow((fin->cord.second - n->edges[i]->nodes[0]->cord.second), 2));
                if (distancia < menor) {
                    tmp = n->edges[i]->nodes[0];
                    menor = distancia;
                }
            }
        }
        return tmp;
    }

    vector<node<T>*> Hill(T a, T b, int max_iterations) {
        node<T>* ini = find(a);
        node<T>* fin = find(b);
        vector<node<T>*> path;
        path.push_back(ini);

        int iterations = 0;
        while (iterations < max_iterations) {
            node<T>* current = path.back();

            if (current == fin) {
                // Hemos encontrado el nodo de destino, terminamos
                return path;
            }

            // Buscamos el nodo más cercano al nodo de destino
            node<T>* next = near_p(current, fin);

            if (next != nullptr) {
                // Si encontramos un nodo cercano, lo agregamos al camino actual
                path.push_back(next);
            }
            else {
                // Si no hay nodo cercano, retrocedemos eliminando el último nodo del camino actual
                path.pop_back();
            }

            iterations++;
        }

        // Si llegamos al límite de iteraciones, devolvemos el camino actual (que podría ser parcial)
        return path;
    }


    vector<node<T>*> Astar(T a, T b, int max_iterations) {
        node<T>* ini = find(a);
        node<T>* fin = find(b);
        vector<node<T>*> path;
        path.push_back(ini);

        int iterations = 0;
        while (iterations < max_iterations) {
            node<T>* current = path.back();

            if (current == fin) {
                // Hemos encontrado el nodo de destino, terminamos
                return path;
            }

            // Buscamos el nodo más cercano al nodo de destino
            node<T>* next = near_p_modify(current, fin);

            if (next != nullptr) {
                // Si encontramos un nodo cercano, lo agregamos al camino actual
                path.push_back(next);
            }
            else {
                // Si no hay nodo cercano, retrocedemos eliminando el último nodo del camino actual
                path.pop_back();
            }

            iterations++;
        }

        // Si llegamos al límite de iteraciones, devolvemos el camino actual (que podría ser parcial)
        return path;
    }

    vector<node<T>*> BP(T a, T b) {
        stack<vector<node<T>*>> s;
        node<T>* ini = find(a);
        node<T>* fin = find(b);
        s.push(vector<node<T>*>{ini});
        int limit = 0;
        while (s.top()[0] != fin) {
            //prints(s);
            vector<node<T>*> aux = s.top();
            s.pop();
            if (aux[0]->edges.size() >0)
            {
                for (auto& it : aux[0]->edges) {
                    vector<node<T>*> tmp = aux;
                    if (it->nodes[0] == aux[0] and !ciclo(tmp, it->nodes[1])) {
                        tmp.insert(tmp.begin(), it->nodes[1]);
                        s.push(tmp);
                    }
                    else if (it->nodes[1] == aux[0] and !ciclo(tmp, it->nodes[0])) {
                        tmp.insert(tmp.begin(), it->nodes[0]);
                        s.push(tmp);
                    }
                }
            }
        }
        return s.top();
    }

    
    vector<node<T>*> BFS(T a, T b) {
        queue<vector<node<T>*>> s;
        node<T>* ini = find(a);
        node<T>* fin = find(b);
        s.push(vector<node<T>*>{ini});
        int limit = 0;
        while (s.front()[0] != fin) {
            
            vector<node<T>*> aux = s.front();
            s.pop();
            if (aux[0]->edges.size() > 0)
            {
                for (auto& it : aux[0]->edges) {
                    vector<node<T>*> tmp = aux;
                    if (it->nodes[0] == aux[0] and !ciclo(tmp, it->nodes[1])) {
                        tmp.insert(tmp.begin(), it->nodes[1]);
                        s.push(tmp);
                    }
                    else if (it->nodes[1] == aux[0] and !ciclo(tmp, it->nodes[0])) {
                        tmp.insert(tmp.begin(), it->nodes[0]);
                        s.push(tmp);
                    }
                }
            }
        }
        return s.front();
    }


    void setpos(T a, int x, int y) {
        node<T>* tmp = find(a);
        tmp->set(x, y);
    }
    void drawe() {

        for (int i = 0; i < edges.size(); i++) {
            VertexArray lines(LineStrip); // Crea un conjunto de líneas
            lines.append(Vertex(Vector2f(edges[i]->nodes[0]->cord.first + 2.5, edges[i]->nodes[0]->cord.second + 2.5), Color::Red));   // Punto de inicio de la línea
            lines.append(Vertex(Vector2f(edges[i]->nodes[1]->cord.first + 2.5, edges[i]->nodes[1]->cord.second + 2.5), Color::Red));  // Punto de fin de la línea
            window.draw(lines);
        }
    }
    void drawn() {
        for (int i = 0; i < nodes.size(); i++) {
            nodes[i]->cir.draw();
        }
    }
    void clicked(node<T>*& n) {
        n = nullptr;
        static bool circleClicked = false; // Keep track of whether a circle has been clicked
        if (event.type == Event::MouseButtonPressed) {
            if (event.mouseButton.button == Mouse::Left) {
                int mouseX = event.mouseButton.x;
                int mouseY = event.mouseButton.y;
                for (int i = 0; i < nodes.size(); i++) {
                    if (nodes[i]->cir.clicked(mouseX, mouseY)) {
                        if (!circleClicked) { // Only print "circle" once
                            cout << "circle: " << nodes[i]->v << endl;
                            //cout << "Contador: " << contador;
                            n = nodes[i];
                            circleClicked = true;
                            if (contador == 0) {
                                nodes[i]->cir.cir.setFillColor(Color::Green);
                            } // Change the color of the clicked circle
                            else if (contador == 1) {
                                nodes[i]->cir.cir.setFillColor(Color::Blue);
                            }
                            contador++;
                            break; // Exit the loop since we only want to change the color of one circle
                        }
                    }
                    else {
                        //nodes[i]->cir.cir.setFillColor(Color::Red); // Reset the color of all other circles
                    }
                }
            }
        }
        else if (event.type == Event::MouseButtonReleased) {
            if (event.mouseButton.button == Mouse::Left) {
                circleClicked = false; // Reset the circleClicked flag when the left mouse button is released
            }
        }
    }

    void color_path(vector<node<T>*> v) {
        for (int i = 0; i < v.size() - 1; i++) {
            VertexArray lines(LineStrip); // Crea un conjunto de líneas
            lines.append(Vertex(Vector2f(v[i]->cord.first + 2.5, v[i]->cord.second + 2.5), Color::Green));   // Punto de inicio de la línea
            lines.append(Vertex(Vector2f(v[i + 1]->cord.first + 2.5, v[i + 1]->cord.second + 2.5), Color::Green));  // Punto de fin de la línea
            window.draw(lines);
        }
    }
};





int main() {
    int puntito = 0;
    graph<int> G;
    int cantidad = 20;
    for (int i = 0; i < cantidad*cantidad; i++) {
        G.ins_node(i);
    }
    for (int j = 0; j < cantidad; j++) {
        for (int i = 0; i < cantidad; i++) {
            G.setpos(i + (j * cantidad), 10 + (i * 10), 10 + (j * 10));
        }
    }
    for (int j = 0; j < cantidad-1; j++) {
        for (int i = 0; i < cantidad-1; i++) {
            G.ins_edge(i + (j * cantidad), (j * cantidad) + i + 1, 10);
            G.ins_edge(i + (j * cantidad), ((j + 1) * cantidad) + i, 10);
            if ((i + j) % 2 == 0) { G.ins_edge(i + (j * cantidad), ((j + 1) * cantidad) + i + 1, 14.1421); }
        }
    }
    G.del_random(100);
    cout << "-----------------------" << endl;
    //G.print();

    //auto x = G.BP(0, 1);
    vector<node<int>*> x;
    for (int i = x.size() - 1; i >= 0; --i) {
        cout << x[i]->v << "->";
    }


    //G.Hill(0, 1);
    bool waitForClick = true;
    node<char>* aux = nullptr;

    while (window.isOpen())
    {

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            if (waitForClick && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                // Detecta el clic y cambia el estado
                waitForClick = false;
            }
        }


        window.clear();
        G.drawn();
        G.drawe();
        G.clicked(G.pares[contador % 2]); // Pasa aux como parámetro
        if (activador == 1) {
            G.color_path(x);
        }
        if (contador == 0) {
            for (auto& it : G.nodes) {
                //it->cir.cir.setFillColor(Color::Red);
            }

        }
        if (!waitForClick and G.pares[0] and G.pares[1]) {
            x = G.Hill(G.pares[0]->v, G.pares[1]->v, 500);
            //x = G.Astar(G.pares[0]->v, G.pares[1]->v, 500);
            //x = G.BP(G.pares[0]->v, G.pares[1]->v);
            //x = G.BFS(G.pares[0]->v, G.pares[1]->v);
            // Dibuja el camino cuando aux no es nulo
            contador = 0;
            activador = 1;
            for (int i = x.size() - 1; i >= 0; --i) { cout << x[i]->v << "->"; }
            waitForClick = true;
            G.pares.clear();
            G.pares.resize(2);
        }

        window.display();
    }

    cout << "Tu puedeeees :)";

    return 0;
}