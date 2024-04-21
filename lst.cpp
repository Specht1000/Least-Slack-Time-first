// Trabalho 1 da disciplina de Sistemas de Tempo Real.
// Desenvolver o escalonador Least Slack Time fist (LST).
// Autor: Guilherme Martins Specht
// Última atualização: 21/04/2024

#include <iostream>
#include <sstream>
#include <vector>
#include <climits> // INT_MAX = 2^31 − 1 = 2,147,483,647

using namespace std;

struct Tarefa{
    char letra;      // Letra correspondente à tarefa.
    int computacao;  // Tempo da computação.
    int periodo;     // Tempo do período.
    int deadline;    // Tempo de deadline.
    int tempo_falta; // Tempo que falta para a computação de uma tarefa terminar.
    int inicio;      // Tempo em que a tarefa inicia.
};

int tempo_slack(const Tarefa& Tarefa, int tempo_atual){
    int tempo_ate_deadline; // Tempo até o fim do deadline.
    int slack; // Tempo de slack.
    tempo_ate_deadline = Tarefa.inicio + Tarefa.deadline - tempo_atual; // Cálculo do tempo até o fim do deadline.
    slack = tempo_ate_deadline - Tarefa.tempo_falta; // Cálculo do slack.
    return slack;
}

int main(){
    
    int num_tarefas, tempo_execucao;
    
    while(cin >> num_tarefas >> tempo_execucao && (num_tarefas != 0 || tempo_execucao != 0)){ // Pega a linha que descreve o número de tarefas e o tempo de execução.
        
        vector<Tarefa> Tarefas(num_tarefas); // Vetor que guarda as structs de todas as tarefas.

        if(num_tarefas > 26 || num_tarefas < 1){ // Verifica que o numero de tarefas não ultrapasse 26.
            cout << "ERRO! número excede os limites de 1 <= X <= 26." << endl;
            break;
        }
        
        for(int i = 0; i < num_tarefas; i++){
            Tarefas[i].letra = 'A' + i; // Define a letra das tarefas, A = (65)ascii, B = (66)ascii, C = (67)ascii e assim por diante.
            cin >> Tarefas[i].computacao >> Tarefas[i].periodo >> Tarefas[i].deadline; // Pega a linha que descreve computação, período e deadline. 

            if(Tarefas[i].computacao > 2048 || Tarefas[i].periodo > 2048 || Tarefas[i].deadline > 2048 || Tarefas[i].computacao < 1 || Tarefas[i].periodo < 1 || Tarefas[i].deadline < 1){ // Verifica que o numero de atributos não ultrapasse 2048.
                cout << "ERRO! número excede os limites de 1 <= X <= 2048." << endl;
                break;
            }

            Tarefas[i].tempo_falta = Tarefas[i].computacao; // Inicializa o tempo que falta para terminar uma computação.
            Tarefas[i].inicio = 0; // Inicializa o tempo em que a tarefa começa.
        }

        stringstream escalonamento;         // String que guarda todo escalonamento.
        stringstream prmp;                  // String que guarda as preempções.
        int trocas_contexto = 0;            // Número de trocas de contexto.
        int preempcoes = 0;                 // Número de preempções.
        int tempo_atual;                    // Guarda o tempo atual do escalonamento.
        Tarefa *ultima_tarefa = nullptr;    // Ponteiro para a última tarefa.
        Tarefa *proxima_tarefa = nullptr;   // Ponteiro para a próxima tarefa que será a com menor slack.
        
        Tarefa *idle = new Tarefa; // Inicializa uma tarefa.
        idle->computacao = 1;
        idle->tempo_falta = 1;
        idle->letra = '.';

        for(tempo_atual = 0; tempo_atual < tempo_execucao + 1; tempo_atual++){
            bool foi_preemptado = false;    // Verifica se uma tarefa foi preemptada.
            for(Tarefa& Tarefa : Tarefas){  // Para cada tarefa do vetor Tarefas
                if(tempo_atual % Tarefa.periodo == 0){ // Verifica se um novo ciclo da tarefa está iniciando.
                    Tarefa.tempo_falta = Tarefa.computacao; // Então o tempo que falta é o tempo da computação.
                    Tarefa.inicio = tempo_atual; // Tempo em que a tarefa iniciou.
                }
            }

            int min_slack = INT_MAX; // Guarda a tarefa com o menor slack para poder ser escalonada.
            proxima_tarefa = idle; // Valor padrão é idle.
            for(Tarefa& Tarefa : Tarefas){
                if (Tarefa.tempo_falta > 0 && tempo_slack(Tarefa, tempo_atual) < min_slack){ // Verifica se ainda tem tempo de execução restante e se o slack da tarefa atual é menor do que o min_slack da tarefa anterior.
                    min_slack = tempo_slack(Tarefa, tempo_atual); // Atribui a min_slack o menos slack entre as tarefas.
                    proxima_tarefa = &Tarefa; // Proxima_tarefa aponta para a tarefa atual.
                }
            }

            char exec_char = '.';
            if(proxima_tarefa->letra != '.'){ // Se o próximo caracter não for idle e sim uma letra.
                exec_char = proxima_tarefa->letra;
                if(tempo_atual >= proxima_tarefa->inicio + proxima_tarefa->deadline){ // Verifica se está executando fora do deadline no período atual
                    exec_char = tolower(exec_char); // Converte a letra para minúscula se a tarefa executa fora do deadline.
                }
                proxima_tarefa->tempo_falta--; // Subtrai o tempo que falta.
            }

            if(tempo_atual != tempo_execucao) // Caso ainda há tempo para a execução, previne a a adição de um caracter extra.
                escalonamento << exec_char;

            if(tempo_atual == 0){
                ultima_tarefa = proxima_tarefa; // Caso o escalonamento esteja no início, ultima tarefa recebe próxima tarfea para fins de comparação. 
            }
            else{
                if(ultima_tarefa->letra != proxima_tarefa->letra){ // Verifica se são tarefas diferentes.
                    trocas_contexto++; 
                    if(ultima_tarefa->tempo_falta != 0 && (ultima_tarefa->tempo_falta != ultima_tarefa->computacao)){ // Verifica se a última tarefa tem tempo restante e se la foi preemptada.
                        foi_preemptado = true; // Se a tarefa não terminou, ela pode ser preemptada na próxima troca.
                    }
                    else if(exec_char == '.'){ // Se for um idle.
                        if (tempo_atual != tempo_execucao) { // E não atingiu o tempo total de execução.
                            foi_preemptado = true; // Houve preempção.
                        }
                    }
                }
                else {
                    if(proxima_tarefa->letra != '.' && (tempo_atual % proxima_tarefa->periodo == 0)){ // Se fou uma tarefa não ociosa e começou um novo período.
                        trocas_contexto++; // Mesma tarefa, mas em periodos diferentes
                    }
                }
                ultima_tarefa = proxima_tarefa;
            }
            if(foi_preemptado) {
                preempcoes++; // A preempção ocorre se a tarefa anterior foi interrompida.
                prmp << tempo_atual << " "; // Debug.
            }
        }

        cout << escalonamento.str() << endl;
        cout << trocas_contexto << " " << preempcoes << endl << endl;
        // cout << prmp.str() << endl; // Debug.
    }
    return 0;
}
