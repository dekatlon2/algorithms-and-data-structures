#include <cstdio>
#include <algorithm>
using namespace std;

struct Task {
    int id;
    double time;
    bool interrupted = false;
};

void sortTasks(Task* arr, int n);

class TaskArray {
    private:
        Task* start;
        int count;
        int limit;
        Task* sorted;
        bool dirty;

        void grow(){
            if (count == limit){
                int newLimit;
                if (limit == 0){
                    newLimit = limit + 10;
                }
                else {
                    newLimit = limit * 2;
                }
                Task* tmp = new Task[newLimit];
                for (int i = 0; i < count; i++){
                    tmp[i] = start[i];
                }
                delete[] start;
                start = tmp;
                limit = newLimit;
            }
        }
    public:
        TaskArray() : start(nullptr), count(0), limit(0), sorted(nullptr), dirty(true) {}

        TaskArray(const TaskArray&) = delete;
        TaskArray& operator=(const TaskArray&) = delete;
        ~TaskArray(){
            delete[] start;
            delete[] sorted;
        }

        void add(const Task& t){
            grow();
            start[count] = t;
            count++;
            dirty = true;
        }

        int getCount() const{
            return count;
        }
        const Task& get(int idx) const{
            return start[idx];
        }

        void insert(int k, const Task& t){
            grow();
            for (int i = count; i > k - 1; i--){
                start[i] = start[i-1];
            }
            start[k-1] = t;
            count++;
            dirty = true;
        }

        void erase(int i){
            int idx = -1;
            for (int j = 0; j < count; j++){
                if (start[j].id == i){
                    idx = j;
                    break;
                }
            }
            if (idx == -1){
                return;
            }
            for (int j = idx; j < count - 1; j++){
                start[j] = start[j+1];
            }
            dirty = true;
            count--;
        }

        const Task* getSortedLS(){
            if (dirty){
                delete[] sorted;
                sorted = new Task[count];
                for (int i = 0; i < count; i++) {
                    sorted[i] = start[i];
                }
                sortTasks(sorted, count);
                dirty = false;
            }
            return sorted;
        }

        const Task* data() const{
            return start;
        }

};

void printResult(double* load, TaskArray* mach, int m){
    double Cmax = load[0];
    for (int i = 0; i < m; i++){
        if (load[i] > Cmax){
            Cmax = load[i];
        }
    }

    double sigmaC = 0;
    for (int i = 0; i < m; i++){
        for (int j = 0; j < mach[i].getCount(); j++){
            if (!mach[i].get(j).interrupted){
                sigmaC += mach[i].get(j).time;
            }
        }
    }

    printf("Cmax: %g\n", Cmax);
    printf("sigmaC: %g\n", sigmaC);
    for (int i = 0; i < m; i++){
        printf("M%d:", i + 1);
        for (int j = 0; j < mach[i].getCount(); j++){
            Task p = mach[i].get(j);
            if (p.interrupted){
                printf("( P%d = %g )", p.id, p.time);
            }
            else {
                printf("( C%d = %g )", p.id, p.time);
            }
        }
        printf("\n");
    }
}

void assignTasks(const Task* arr, int n, int m){
    double* load = new double[m];
    for (int i = 0; i < m; i++){
        load[i] = 0.0;
    }
    TaskArray* mach = new TaskArray[m];
    for (int i = 0; i < n; i++){
        Task t = arr[i];
        int minId = 0;
        double minVal = load[0];
        for (int j = 0; j < m; j++){
            if (load[j] < minVal){
                minId = j;
                minVal = load[j];
            }
        }
        double C = load[minId] + t.time;
        Task pair;
        pair.id = t.id;
        pair.time = C;
        mach[minId].add(pair);
        load[minId] = C;
    }

    printResult(load, mach, m);

    delete[] load;
    delete[] mach;
}

void B(const TaskArray& tasks, int m){
    assignTasks(tasks.data(), tasks.getCount(), m);
}

void sortTasks(Task* arr, int n){
    if (n <= 1) return;
    for (int i = 0; i < n - 1; i++){
        for (int j = 0; j < n - 1 - i; j++){
            bool swap;
            if (arr[j].time != arr[j+1].time){
                swap = (arr[j].time < arr[j+1].time);
            }
            else {
                swap = (arr[j].id < arr[j+1].id);
            }
            if (swap){
                Task tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
        }
    }
}

void L(TaskArray& tasks, int m){
    const Task* arr = tasks.getSortedLS();
    assignTasks(arr, tasks.getCount(), m);
}

void S(TaskArray& tasks, int m){
    const Task* arr = tasks.getSortedLS();
    int n = tasks.getCount();
    double* load = new double[m];
    for (int i = 0; i < m; i++) {
        load[i] = 0.0;
    }
    TaskArray* mach = new TaskArray[m];
    for (int j = n - 1; j >= 0; j--){
        int i = j % m;
        Task t = arr[j];
        double C = load[i] + t.time;
        Task pair;
        pair.id = t.id;
        pair.time = C;
        mach[i].add(pair);
        load[i] = C;
    }

    printResult(load, mach, m);
    delete[] load;
    delete[] mach;
}

void M(const TaskArray& tasks, int m){
    int n = tasks.getCount();
    const Task* arr = tasks.data();
    double sumT = 0.0;
    double maxT = 0.0;
    for (int i = 0; i < n; i++){
        double taskLength = arr[i].time;
        sumT += taskLength;
        if (taskLength > maxT){
            maxT = taskLength;
        }
    }
    double Cmax = sumT / m;
    if (maxT > Cmax){
        Cmax = maxT;
    }

    double* load = new double[m];
    for (int i = 0; i < m; i++){
        load[i] = 0.0;
    }
    TaskArray* mach = new TaskArray[m];

    Task* copy = new Task[n];
    for (int i = 0; i < n; i++){
        copy[i] = tasks.get(i);
    }

    int j = 0;
    bool nextIsCont = false;
    for (int i = 0; i < m; i++){
        while (j < n && load[i] + copy[j].time <= Cmax){
            Task pair;
            pair.id = copy[j].id;
            pair.time = load[i] + copy[j].time;
            pair.interrupted = nextIsCont;
            mach[i].add(pair);
            load[i] += copy[j].time;
            j++;
            nextIsCont = false;
        }
        if (j < n){
            double frag = Cmax - load[i];
            if (frag > 0){
                Task pair;
                pair.id = copy[j].id;
                pair.time = Cmax;
                pair.interrupted = false;
                mach[i].add(pair);
                load[i] = Cmax;
                copy[j].time -= frag;
                nextIsCont = true;
            }
        }
    }

    double sigmaC = 0.0;
    for (int i = 0; i < m; i++){
        for (int k = 0; k < mach[i].getCount(); k++){
            if (!mach[i].get(k).interrupted){
                sigmaC += mach[i].get(k).time;
            }
        }
    }

    printf("Cmax: %g\n", Cmax);
    printf("sigmaC: %g\n", sigmaC);
    for (int i = 0; i < m; i++){
        printf("M%d:", i + 1);
        for (int k = 0; k < mach[i].getCount(); k++){
            Task p = mach[i].get(k);
            if (p.interrupted){
                printf("( P%d = %g )", p.id, p.time);
            }
            else {
                printf("( C%d = %g )", p.id, p.time);
            }
        }
        printf("\n");
    }

    delete[] copy;
    delete[] load;
    delete[] mach;
}

int main(){

    int n;
    scanf("%d", &n);
    int nextId = n + 1;

    TaskArray tasks;

    for (int i = 1; i <= n; i++){
        Task t;
        t.id = i;
        scanf("%lf", &t.time);
        tasks.add(t);
    }

    char sign;
    while (scanf(" %c", &sign) == 1){
        if (sign == 'B'){
            int m;
            scanf("%d", &m);
            B(tasks, m);
        }
        else if (sign == '+'){
            int k;
            double p;
            scanf("%d %lf", &k, &p);
            Task t;
            t.id = nextId;
            t.time = p;
            tasks.insert(k, t);
            nextId++;
        }
        else if (sign == '-'){
            int i;
            scanf("%d", &i);
            tasks.erase(i);
        }
        else if (sign == 'L'){
            int m;
            scanf("%d", &m);
            L(tasks, m);
        }
        else if (sign == 'S'){
            int m;
            scanf("%d", &m);
            S(tasks, m);
        }
        else if (sign == 'M'){
            int m;
            scanf("%d", &m);
            M(tasks, m);
        }
    }

    return 0;
}