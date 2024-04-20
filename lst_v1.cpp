// Trabalho 1 da disciplina de Sistemas de Tempo Real.
// Desenvolver o escalonador Least Slack Time fist (LST).
// Autor: Guilherme Martins Specht
// Última atualização: 12/04/2024

#include <iostream>
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

        string escalonamento;           // String que guarda todo escalonamento.
        int trocas_contexto = 0;        // Número de trocas de contexto.
        int preempcoes = 0;             // Número de preempções.
        char ultima_letra_tarefa = '.'; // Letra da ultima tarefa.
        bool foi_preemptado = false;    // Verifica se uma tarefa foi preemptada.
        int tempo_atual;                // Guarda o tempo atual do escalonamento.

        for(tempo_atual = 0; tempo_atual < tempo_execucao; tempo_atual++){
            for(Tarefa& Tarefa : Tarefas){ // Para cada tarefa do vetor Tarefas
                if(tempo_atual % Tarefa.periodo == 0){ // Verifica se um novo ciclo da tarefa está iniciando.
                    Tarefa.tempo_falta = Tarefa.computacao; // Então o tempo que falta é o tempo da computação.
                    Tarefa.inicio = tempo_atual; // Tempo em que a tarefa iniciou.
                }
            }

            int min_slack = INT_MAX; // Guarda a tarefa com o menor slack para poder ser escalonada.
            Tarefa* proxima_tarefa = nullptr; // Ponteiro para a próxima tarefa que será a com menor slack.
            for(Tarefa& Tarefa : Tarefas){
                if (Tarefa.tempo_falta > 0 && tempo_slack(Tarefa, tempo_atual) < min_slack){ // Verifica se ainda tem tempo de execução restante e se o slack da tarefa atual é menor do que o min_slack da tarefa anterior.
                    min_slack = tempo_slack(Tarefa, tempo_atual); // Atribui a min_slack o menos slack entre as tarefas.
                    proxima_tarefa = &Tarefa; // Proxima_tarefa aponta para a tarefa atual.
                }
            }

            if(proxima_tarefa){
                char exec_char = proxima_tarefa->letra;
                if (tempo_atual >= proxima_tarefa->inicio + proxima_tarefa->deadline) { // Verifica se está executando fora do deadline no período atual
                    exec_char = tolower(exec_char); // Converte a letra para minúscula se a tarefa executa fora do deadline.
                }
                escalonamento += exec_char; // A tarefa com menor slack é escalonada.
                
                if(ultima_letra_tarefa != proxima_tarefa->letra){ // Verifica se teve troca de contexto ou seja se a letra anterior não é a mesma da atual
                    //trocas_contexto++; // Houve uma troca de contexto
                    if(foi_preemptado){
                        preempcoes++; // A preempção ocorre se a tarefa anterior foi interrompida.
                        trocas_contexto++;
                    }
                    foi_preemptado = false; // Não houve preempção.
                }

                proxima_tarefa->tempo_falta--; // Subtrai o tempo que falta.
                if(proxima_tarefa->tempo_falta == 0){
                    foi_preemptado = false; // Se a tarefa terminou, não há preempção.trocas_contexto++;
                    trocas_contexto++;
                }
                else{
                    foi_preemptado = true; // Se a tarefa não terminou, ela pode ser preemptada na próxima troca.
                }

                ultima_letra_tarefa = proxima_tarefa->letra; // Atualiza a última letra que foi escalonada.
            }
            else{
                escalonamento += '.'; // Caso não tenho nenhuma tarefa sendo executada é idle.
                if(ultima_letra_tarefa != '.'){ // Se a última letra da tarefa for do alfabeto e aconteceu antes do idle.
                    trocas_contexto++; 
                    preempcoes++;
                }
                ultima_letra_tarefa = '.';
                foi_preemptado = false;
            }
        }

        cout << escalonamento << endl;
        cout << trocas_contexto << " " << preempcoes << endl;
    }

    return 0;
}
