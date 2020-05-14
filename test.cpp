#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <time.h>
using namespace std;
//константы
const int MAX_SIZE = 100;
bool need_upd = false;
vector <int> RAM(MAX_SIZE + 1, 0);
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
  Process *q = new Process(name, mem, prior, timeLife);
  // добавляем в очнередь приоритетов (по массив очередей)
  HQ[prior].push(*q);
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
        else cout << " and is on memory";
        cout << ") - memory: " << cur.mem << " - time: " << cur.lastA;
        if (cur.position != -1) cout << " - position: " << cur.position << endl;
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
  bool answ = false;
  int begin = 0; // begin of a proj
  //while (begin + memory <= MAX_SIZE){
  for(begin = 0; begin <= MAX_SIZE - memory && !answ; begin++){
    //cout << "Start find place" << endl;
    bool isOk = true; // enough from HERE or not
    int needmem = memory;
    int ind = 0;
    while(isOk && needmem-- ){
      //cout << begin << " " <<needmem << " "<< ind <<  endl;
      //for(int i = 0 ; i < MAX_SIZE; i++) cout << RAM[i];
      if (RAM[begin + ind]){
        //zanyato

        begin += ind; // pos of last mem sector
        isOk = false;
      }
      ind++;
    }
    //cout << isOk<< "here\n";

    if (isOk){
      exe.position = begin;
      //cout << "OK " << begin  << " + " << ind << endl;
      answ = isOk;
      //return isOk;//true
      break;
    }
    //begin++; // going ro NEXT mem sector.
  }
  //cout << "quit the finding\n";
  return answ;
}

void pushProcToSWAP(Process &exe){
  if (exe.is_on_swap) return; // если процесс уже в свопе, то ничего делать не надо
  for(int i = exe.position; i < exe.mem + exe.position; i++){
    //clear RAM
    RAM[i] = 0;
  }
  // update execute proc
  exe.position = -1; exe.is_on_swap = true;
}

void updateRAM(){
  for(int i = 0 ; i < MAX_SIZE; i++) cout << RAM[i];
  for(int i = 9; i >= 0; i--){
      queue<Process> tmp;
      //cout << i << " prior)\n";
      // going through queue
      while(!HQ[i].empty()){
        Process cur = HQ[i].front();
        //cout << cur.name << endl;
        // rewriting to make queue as before
        HQ[i].pop();
        if (cur.is_on_swap) {// либо новый процесс либо грузим из свопа, пока нет разницы
          // try to place process
          if (isMemEnough(cur)){ // просто нашли место в памяти
            //refilling RAM
            //cout << "I WANT RAM\n";
            cur.is_on_swap = false;
            //cout << "here!\n";
            RAM[cur.position] = 2;
            for(int j = 1; j < cur.mem; j++){
              // get RAM
              RAM[j + cur.position] = 1;
            }
          }
          else
          { /* сейчас оперативка забита
             надо убрать менее приоритетные процессы
             (начиная с самых неприоритетных)
             попробовать поставить данный процесс
             если получится, то мы будем считать, что
             что это успех. Не получится, ну ШТОШ буду продолжать все
             скидывать в своп, что могу и восстанавливать
             мнее приоритетные процессы при следующих итерация i
             */
            int j = 0;
            //cout  << "I NEED RAM\n";
            bool done = false; // эвристика
            while (j <= i && !done){ // less prio processes
              queue<Process> tmp1;
              // going through queue
              while(!HQ[j].empty() && !done){
                Process cur1 = HQ[j].front();
                // rewriting to make queue as before
                HQ[j].pop();

                // удаление процесса
                int now = cur1.position;
                cout << "swapping " << cur1.name << endl;
                pushProcToSWAP(cur1);
                tmp1.push(cur1);


            }
              // восстановление
              while(!tmp1.empty()){
                Process c = tmp1.front();
                tmp1.pop(); HQ[j].push(c);
              }


              j++;
          }

          if (isMemEnough(cur)){ //  нашли место после удаления
            // занимаем место в памяти
            for(int t = 0; t < cur.mem; t++){
              RAM[t + cur.position] = 1;
            }
            cur.is_on_swap = false;
            done = true; // завершаем геноцид процессов
            RAM[cur.position] = 2; // отметим старт программы
          }
        }
      }
      tmp.push(cur);
      // remaking queue


    }
    while(!tmp.empty()){
      Process c = tmp.front();
      tmp.pop(); HQ[i].push(c);
    }

  }
}

bool killProcess(Process Fexe){
  bool ret  = false;
  for(int i = 9; i >= 0; i--){
    // buffer queue
      queue<Process> tmp;

      // going through queue
      while(!HQ[i].empty()){
        Process cur = HQ[i].front();
        // rewriting to make queue as before
        HQ[i].pop();
        if (cur.name == Fexe.name && cur.prio == Fexe.prio){
          // очищаем память в РАМе, и не добавляем в чередь заданий
          pushProcToSWAP(cur);
          ret = true;
        } else
          tmp.push(cur);
      }
      // remaking
      while(!tmp.empty()){
        Process c = tmp.front();
        tmp.pop(); HQ[i].push(c);
      }
  }
  return ret;
}

// цвета желтыйи голубой 43 -44
void updStatusMonitor(){

    //для работы с цветом в консоли
    //HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    //15 - цвет фона, 0 - цвет текста
    //SetConsoleTextAttribute(hConsole, (WORD) ((15 << 4) | 0));

    //monitor status
    clock_t myTime = clock() / CLOCKS_PER_SEC, oldTime = myTime;
    //ставим обновление процессов
    int NEED_UPD = 1;
    char exit = 'a';
    cout << "\nPress key for update...\n";
    while (exit != 'q'){
        myTime = clock() / CLOCKS_PER_SEC;
        // каждую пройденную секунду производим обновление параметров
          updateRAM();
            //system("cls");
             //cout << "\033[1;31mbold red text\033[0m\n";
            cout << "MONITOR STATUS\t\tWORKING TIME PROGRAMM: " << myTime << "sec\n";
            cout << "\n\nCAPACITY OF MEMORY - " << MAX_SIZE << " blocks";
            // отрисовка оперативной памяти
            cout << "\nRandom Access Memory:\n";
            cout << " ";
            for (int i = 0; i < 100; i++)
                cout << "-";
            cout << " \n|";
            char flag = 0;
            for (int i = 0; i < 100; i++){
                flag = 0;
                for (int j = 0; j < MAX_SIZE / 100; j++)
                    if (RAM[MAX_SIZE / 100 * i + j] == 1){
                        //SetConsoleTextAttribute(hConsole, (WORD) ((14 << 4) | 8));
                        cout << "\033[1;44m \033[0m";
                        flag = 1;
                        break;
                    }
                    else if (RAM[MAX_SIZE / 100 * i + j] == 2){
                      //SetConsoleTextAttribute(hConsole, (WORD) ((25 << 4) | 8));
                      cout << "\033[1;43m \033[0m";
                      flag = 1;
                      break;
                    }

                if (!flag){
                    //SetConsoleTextAttribute(hConsole, (WORD) ((8 << 4) | 14));
                    cout << " ";
                }
            }
            //SetConsoleTextAttribute(hConsole, (WORD) ((0 << 4) | 15));
            cout << "|\n";
            cout << " ";
            for (int i = 0; i < 100; i++)
                cout << "-";
            cout << " \n";
            // отрисовка всех процессов по приоритету
            //for(int i = 0 ; i < 100; i++) cout << RAM[i]; cout << endl;
            displayProcesses();
            cout << "\n\n\nPress 'q' for exit from monitor status or another key for update...\n";
            oldTime = myTime;

        cin >> exit;
}
}

int main()
{
  //Process *pr;
  //cout << "";
  //for(int i = 0 ; i < 1000; i++) RAM[i]=0;
  createProcess();
  createProcess();
  createProcess();
  //for(int i = 0 ; i < 1000; i++) cout << RAM[i];
  //displayProcesses();
  updStatusMonitor();
  Process *p = new Process("a", 1, 1, 1);
  killProcess(*p);
  updStatusMonitor();
  createProcess();
  updStatusMonitor();
  //createProcess();
  //updateRAM();
  //displayProcesses();
  return 0;
}
