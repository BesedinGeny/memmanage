#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <time.h>
using namespace std;
//константы
const int MAX_SIZE = 1000;
bool need_upd = false;
int RAM[MAX_SIZE + 1] = {0};
// структура процесса
class Process{ //struct
public:
  string name = "";
  int mem = 0;
  int position = -1;
  int prio = 0;
  int time = 0;
  bool is_on_swap = true;
  clock_t lastA;

  //constructor
  Process(string n, int m, int pr, int t){
    name = n; mem = m; prio = pr; time = t;
    lastA = clock() / CLOCKS_PER_SEC;
  }



};

// создаем очередь очередей.
// 0 - наименьший приоритет, 9 - наибольший
vector<queue<Process>> HQ(10);

// UI creation
void createProcess(){
  // вводим какие-то корректные данные
  cout << "Enter a name of new process: ";
  string name;
  cin >> name;
  cout << "Enter a priority of process (0 - 9): ";
  int prior; cin >> prior;
  while (prior < 0 || prior > 9){
      cout << "\nWrong number, try again!\t";
      cin >> prior;
  }
  cout << "How long the process will be able (in sec)? ";
  int timeLife; cin >> timeLife;
  cout << "How much memory is needed (in blocks)? ";
  int mem; cin >> mem;
  while(mem < 0){
      cout << "\nMemory mustn't be negative, try again!\t";
      cin >> mem;
  }
  // создаем сам процесс через конструктор
  Process q(name, mem, prior, timeLife);
  // добавляем в очнередь приоритетов (по массив очередей)
  HQ[prior].push(q);
  cout << "Process successfully created!\n";
}

// show queues
void displayProcesses(){
  cout << "\n\nCurrent working processes:\n";
  int j = 1;
  for(int i = 9; i >= 0; i--){
    // buffer queue
      queue<Process> tmp;

      // going through queue
      while(!HQ[i].empty()){
        Process cur = HQ[i].front();
        // rewriting to make queue as before
        HQ[i].pop();tmp.push(cur);
        cout << endl << j <<") " << cur.name << " (priority is " << cur.prio;
        if (cur.is_on_swap) cout << " and is on swap file";
        else cout << " and is not on a swap file";
        cout << ") - memory: " << cur.mem << " - time: " << cur.lastA << endl;
        j++;
      }
      // remaking
      while(!tmp.empty()){
        Process c = tmp.front();
        tmp.pop(); HQ[i].push(c);
      }
  }

}

// ckack RAM for process`s mem
bool isMemEnough(Process &exe){
  int memory = exe.mem;
  int begin = 0; // begin of a proj
  while (begin + memory <= MAX_SIZE){
    bool isOk = true; // enough from HERE or not
    int needmem = memory;
    int ind = 0;
    while(isOk && needmem ){
      if (RAM[begin + ind]){
        //zanyato
        begin += ind; // pos of last mem sector
        isOk = false;
      }
      needmem--;ind++;
    }
    if (isOk){
      exe.position = begin;
      return isOk;//true
    }
    begin++; // going ro NEXT mem sector.
  }
  return false;
}

void pushProcToSWAP(Process &exe){
  if (exe.is_on_swap) return; // если процесс уже в свопе, то ничего делать не надо
  for(int i = exe.position; i < exe.mem + exe.position; i++){
    //clear RAM
    RAM[i] = 0;
  }
  // update execute proc
  exe.positon = -1; exe.is_on_swap = true;
}

void updateRAM(){
  for(int i = 9; i >= 0; i--){
    queue<Process> tmp;

    // going through queue
    while(!HQ[i].empty()){
      Process cur = HQ[i].front();
      // rewriting to make queue as before
      HQ[i].pop();tmp.push(cur);
      if (cur.is_on_swap) {// либо новый процесс либо грузим из свопа, пока нет разницы
        // try to place process
        if (isMemEnough(cur)){ // просто нашли место в памяти
          //refilling RAM
          for(int j = 0; j < cur.mem; j++){
            // get RAM
            RAM[j + cur.position] = 1;
          }
        } else{ /* сейчас оперативка забита
           надо убрать менее приоритетные процессы
           (начиная с самых неприоритетных)
           попробовать поставить данный процесс
           если получится, то мы будем считать, что
           что это успех. Не получится, ну ШТОШ буду продолжать все
           скидывать в своп, что могу и восстанавливать
           мнее приоритетные процессы при следующих итерация i
           */
          int j = 0;
          bool done = false; // эвристика
          while (j < i && !done){ // less prio processes
            queue<Process> tmp1;
            // going through queue
            while(!HQ[j].empty() && !done){
              Process cur1 = HQ[j].front();
              // rewriting to make queue as before
              HQ[j].pop();tmp1.push(cur1);

              // удаление процесса
              int now = cur1.position;
              pushProcToSWAP(cur1);


              if (isMemEnough(cur)){ //  нашли место после удаления
                // занимаем место в памяти
                for(int t = 0; t < cur.mem; t++){
                  RAM[t + now] = 1;
                }
                done = true; // завершаем геноцид процессов
            }
          }
            // восстановление
            while(!tmp1.empty()){
              Process c = tmp1.front();
              tmp1.pop(); HQ[j].push(c);
            }

            j++;
        }
      }
    }
    // remaking queue
    while(!tmp.empty()){
      Process c = tmp.front();
      tmp.pop(); HQ[i].push(c);
    }

  }

}

int main(){
  //Process *pr;
  createProcess();
  cout << "";
  displayProcesses();
  createProcess();
  displayProcesses();
  return 0;
}
