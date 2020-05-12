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
int RAM[MAX_SIZE] = {0};

// структура процесса
class Process{ //struct
public:
  string name = "";
  int mem = 0;
  int position = -1;
  int prio = 0;
  int time = 0;
  bool is_on_swap = false;
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
  for(int i = 9; i >= 0; i--){
    // buffer queue
      queue<Process> tmp;
      int j = 1;
      // going through queue
      while(!HQ[i].empty()){
        Process cur = HQ[i].front();
        // rewriting to make queue as before
        HQ[i].pop();tmp.push(cur);
        cout << endl << j++ <<") " << cur.name << " (priority is " << cur.prio << ") - memory: " << cur.mem << " - time: " << cur.lastA << endl;
      }
      // remaking
      while(!tmp.empty()){
        Process c = tmp.front();
        tmp.pop(); HQ[i].push(c);
      }
  }

}

int main(){
  //Process *pr;
  createProcess();
  createProcess();
  displayProcesses();
  return 0;
}
