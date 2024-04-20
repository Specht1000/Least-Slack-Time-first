#include <iostream>
#include <vector>
#include <climits>

using namespace std;

struct Tarefa {
    char letra;
    int computacao;
    int periodo;
    int deadline;
    int tempo_falta;
    int inicio;
    int deadline_atual;

    Tarefa(char l = ' ', int c = 1, int p = 1, int d = 1)
        : letra(l), computacao(c), periodo(p), deadline(d), tempo_falta(c), inicio(0), deadline_atual(d) {}

    void update(int tempo_atual) {
        if (tempo_atual % periodo == 0) { // Início de um novo período
            tempo_falta = computacao;
            inicio = tempo_atual;
            deadline_atual = inicio + deadline;
        }
    }

    int getSlack(int tempo_atual) const {
        if (tempo_falta == 0) return INT_MAX; // Se não está ativa, retorna slack máximo
        return deadline_atual - tempo_atual - tempo_falta;
    }
};

int main() {
    int num_tarefas, tempo_execucao;

    while (cin >> num_tarefas >> tempo_execucao && (num_tarefas != 0 || tempo_execucao != 0)) {
        vector<Tarefa> tarefas(num_tarefas);
        char letra = 'A';

        for (int i = 0; i < num_tarefas; i++, letra++) {
            int computacao, periodo, deadline;
            cin >> computacao >> periodo >> deadline;
            tarefas[i] = Tarefa(letra, computacao, periodo, deadline);
        }

        string escalonamento;
        int trocas_contexto = 0, preempcoes = 0;
        Tarefa* ultima_tarefa = nullptr;

        for (int tempo_atual = 0; tempo_atual < tempo_execucao; tempo_atual++) {
            Tarefa* tarefa_selecionada = nullptr;
            int min_slack = INT_MAX;

            for (Tarefa& tarefa : tarefas) {
                tarefa.update(tempo_atual);
                if (tarefa.getSlack(tempo_atual) < min_slack) {
                    min_slack = tarefa.getSlack(tempo_atual);
                    tarefa_selecionada = &tarefa;
                }
            }

            if (ultima_tarefa && tarefa_selecionada && ultima_tarefa != tarefa_selecionada) {
                if (ultima_tarefa->tempo_falta > 0) preempcoes++;
                trocas_contexto++;
            }

            if (tarefa_selecionada) {
                escalonamento += (tempo_atual >= tarefa_selecionada->deadline_atual) ? tolower(tarefa_selecionada->letra) : tarefa_selecionada->letra;
                tarefa_selecionada->tempo_falta--;
                ultima_tarefa = tarefa_selecionada;
            } else {
                escalonamento += '.';
                ultima_tarefa = nullptr;
            }
        }

        cout << escalonamento << endl;
        cout << trocas_contexto << " " << preempcoes << endl;
        cout << endl; // Separa cada conjunto de saídas com uma linha em branco
    }

    return 0;
}
