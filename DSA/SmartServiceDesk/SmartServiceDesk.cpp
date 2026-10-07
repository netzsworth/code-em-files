#include <bits/stdc++.h>
using namespace std;
using Time = long long;
struct Customer
{
    string id, name, serviceType;
    int arrivalTime = 0, processingTime = 1, waitingWeight = 1;
};
struct Ticket
{
    Customer customer;
    bool served = false;
    Time start = -1, end = -1;
};
class LinkedQueue
{
    struct Node
    {
        string id;
        Node *next;
    };
    Node *front = nullptr, *rear = nullptr;
    size_t count = 0;

public:
    LinkedQueue() = default;
    LinkedQueue(const LinkedQueue &) = delete;
    LinkedQueue &operator=(const LinkedQueue &) = delete;
    ~LinkedQueue() { clear(); }
    bool empty() const { return front == nullptr; }
    size_t size() const { return count; }
    void enqueue(const string &id)
    {
        Node *p = new Node{id, nullptr};
        if (rear)
            rear->next = p;
        else
            front = p;
        rear = p;
        ++count;
    }
    bool dequeue(string &id)
    {
        if (empty())
            return false;
        Node *p = front;
        id = p->id;
        front = p->next;
        delete p;
        if (!front)
            rear = nullptr;
        --count;
        return true;
    }
    string peek() const { return front ? front->id : ""; }
    vector<string> ids() const
    {
        vector<string> result;
        for (Node *p = front; p; p = p->next)
            result.push_back(p->id);
        return result;
    }
    int position(const string &id) const
    {
        int i = 0;
        for (Node *p = front; p; p = p->next, ++i)
            if (p->id == id)
                return i;
        return -1;
    }
    bool remove(const string &id)
    {
        Node *previous = nullptr;
        for (Node *p = front; p; previous = p, p = p->next)
        {
            if (p->id != id)
                continue;
            if (previous)
                previous->next = p->next;
            else
                front = p->next;
            if (rear == p)
                rear = previous;
            delete p;
            --count;
            return true;
        }
        return false;
    }
    void insert(size_t index, const string &id)
    {
        if (index > count)
            throw logic_error("Vi tri queue khong hop le");
        if (index == count)
        {
            enqueue(id);
            return;
        }
        Node *p = new Node{id, nullptr};
        if (index == 0)
        {
            p->next = front;
            front = p;
        }
        else
        {
            Node *before = front;
            for (size_t i = 1; i < index; ++i)
                before = before->next;
            p->next = before->next;
            before->next = p;
        }
        ++count;
    }
    void clear()
    {
        string id;
        while (dequeue(id))
        {
        }
    }
};
enum class ActionType
{
    ADD,
    DELETE,
    UPDATE,
    SERVE,
    SORT
};
struct Action
{
    ActionType type = ActionType::ADD;
    Ticket oldTicket;
    string id;
    size_t listPosition = 0;
    int queuePosition = -1;
    Time oldClock = 0;
    vector<string> oldOrder; // Chi SORT can luu thu tu cu, O(n).
};
class ActionStack
{
    struct Node
    {
        Action data;
        Node *next;
    };
    Node *top = nullptr;

public:
    ActionStack() = default;
    ActionStack(const ActionStack &) = delete;
    ActionStack &operator=(const ActionStack &) = delete;
    ~ActionStack() { clear(); }
    void push(Action a) { top = new Node{move(a), top}; }
    bool pop(Action &a)
    {
        if (!top)
            return false;
        Node *p = top;
        a = move(p->data);
        top = p->next;
        delete p;
        return true;
    }
    void clear()
    {
        Action a;
        while (pop(a))
        {
        }
    }
};
class ServiceDesk
{
    vector<Ticket> tickets;
    LinkedQueue waiting;
    ActionStack history;
    Time clock = 0;
    int index(const string &id) const
    {
        for (size_t i = 0; i < tickets.size(); ++i)
            if (tickets[i].customer.id == id)
                return static_cast<int>(i);
        return -1;
    }

public:
    static bool valid(const Customer &c)
    {
        return !c.id.empty() && !c.name.empty() && !c.serviceType.empty() && c.arrivalTime >= 0 && c.processingTime > 0 && c.waitingWeight > 0;
    }
    const Ticket *find(const string &id) const
    {
        int i = index(id);
        return i < 0 ? nullptr : &tickets[i];
    }
    vector<string> queueIds() const { return waiting.ids(); }
    Time currentTime() const { return clock; }
    void reset()
    {
        history.clear();
        waiting.clear();
        tickets.clear();
        clock = 0;
    }
    bool add(const Customer &c)
    {
        if (!valid(c) || find(c.id))
            return false;
        Action a;
        a.type = ActionType::ADD;
        a.id = c.id;
        tickets.push_back(Ticket{c, false, -1, -1});
        waiting.enqueue(c.id);
        history.push(move(a));
        return true;
    }
    bool erase(const string &id)
    {
        int i = index(id);
        if (i < 0)
            return false;
        // Bao cao lich su phuc vu duoc giu lai; chi xoa khach con cho.
        if (tickets[i].served)
            return false;
        Action a;
        a.type = ActionType::DELETE;
        a.id = id;
        a.oldTicket = tickets[i];
        a.listPosition = i;
        a.queuePosition = waiting.position(id);
        waiting.remove(id);
        tickets.erase(tickets.begin() + i);
        history.push(move(a));
        return true;
    }
    bool update(const string &id, const Customer &c)
    {
        int i = index(id);
        if (i < 0 || tickets[i].served || c.id != id || !valid(c))
            return false;
        Action a;
        a.type = ActionType::UPDATE;
        a.id = id;
        a.oldTicket = tickets[i];
        tickets[i].customer = c;
        history.push(move(a));
        return true;
    }
    bool serve(Ticket &result)
    {
        if (waiting.empty())
            return false;
        int i = index(waiting.peek());
        if (i < 0)
            throw logic_error("Queue va List khong dong bo");
        Action a;
        a.type = ActionType::SERVE;
        a.id = waiting.peek();
        a.oldTicket = tickets[i];
        a.oldClock = clock;
        string id;
        waiting.dequeue(id);
        Ticket &t = tickets[i];
        t.start = max(clock, Time(t.customer.arrivalTime));
        t.end = t.start + t.customer.processingTime;
        t.served = true;
        clock = t.end;
        result = t;
        history.push(move(a));
        return true;
    }
    void sortList()
    {
        Action a;
        a.type = ActionType::SORT;
        for (const Ticket &t : tickets)
            a.oldOrder.push_back(t.customer.id);
        stable_sort(tickets.begin(), tickets.end(), [](const Ticket &x, const Ticket &y)
                    { return x.customer.id < y.customer.id; });
        history.push(move(a));
    }
    bool undo()
    {
        Action a;
        if (!history.pop(a))
            return false;
        int i = index(a.id);
        switch (a.type)
        {
        case ActionType::ADD:
            waiting.remove(a.id);
            tickets.erase(tickets.begin() + i);
            break;
        case ActionType::DELETE:
            tickets.insert(tickets.begin() + a.listPosition, a.oldTicket);
            if (a.queuePosition >= 0)
                waiting.insert(a.queuePosition, a.id);
            break;
        case ActionType::UPDATE:
            tickets[i] = a.oldTicket;
            break;
        case ActionType::SERVE:
            tickets[i] = a.oldTicket;
            waiting.insert(0, a.id);
            clock = a.oldClock;
            break;
        case ActionType::SORT:
        {
            // Gan vi tri cu roi sort: O(n^2) de tim vi tri + O(n log n).
            vector<pair<size_t, Ticket>> ordered;
            for (const Ticket &t : tickets)
            {
                auto p = std::find(a.oldOrder.begin(), a.oldOrder.end(), t.customer.id);
                ordered.push_back({size_t(p - a.oldOrder.begin()), t});
            }
            sort(ordered.begin(), ordered.end(), [](const auto &x, const auto &y)
                 { return x.first < y.first; });
            for (size_t j = 0; j < tickets.size(); ++j)
                tickets[j] = move(ordered[j].second);
            break;
        }
        }
        return true;
    }
    static void printTicket(const Ticket &t)
    {
        const Customer &c = t.customer;
        cout << c.id << " | " << c.name << " | " << c.serviceType
             << " | den=" << c.arrivalTime << " | p=" << c.processingTime
             << " | w=" << c.waitingWeight << " | " << (t.served ? "DA PHUC VU" : "DANG CHO");
        if (t.served)
            cout << " | start=" << t.start << " | C=" << t.end
                 << " | W=" << t.start - c.arrivalTime << " | w*W=" << 1LL * c.waitingWeight * (t.start - c.arrivalTime);
        cout << '\n';
    }
    void printRemaining() const { printQueue("Remaining: "); }
    void printQueue(const string &label = "Queue: ") const
    {
        auto ids = waiting.ids();
        cout << label;
        if (ids.empty())
            cout << "(rong)";
        for (size_t i = 0; i < ids.size(); ++i)
            cout << (i ? " -> " : "") << ids[i];
        cout << '\n';
    }
    void printList() const
    {
        if (tickets.empty())
            cout << "Danh sach rong.\n";
        for (const Ticket &t : tickets)
            printTicket(t);
    }
    void report() const
    {
        size_t served = 0;
        long double totalW = 0, totalCost = 0, totalWC = 0;
        cout << "\n===== BAO CAO =====\nDong ho: " << clock << " phut\n";
        for (const Ticket &t : tickets)
            if (t.served)
            {
                ++served;
                Time w = t.start - t.customer.arrivalTime;
                totalW += w;
                totalCost += (long double)t.customer.waitingWeight * w;
                totalWC += (long double)t.customer.waitingWeight * t.end;
                printTicket(t);
            }
        cout << "Tong khach: " << tickets.size() << " | Da phuc vu: " << served
             << " | Con cho: " << waiting.size() << '\n';
        cout << "Tong thoi gian cho: " << totalW << " | Trung binh: " << (served ? totalW / served : 0) << '\n'
             << "Tong chi phi cho w*W: " << totalCost << " | Tong w*C: " << totalWC << '\n';
        printQueue();
        for (const string &id : waiting.ids())
        {
            const Ticket &t = *find(id);
            printTicket(t);
            cout << "  Da cho tai dong ho hien tai: " << max(Time(0), clock - t.customer.arrivalTime) << " phut\n";
        }
    }
    vector<Customer> pending() const
    {
        vector<Customer> result;
        for (const string &id : waiting.ids())
            result.push_back(find(id)->customer);
        return result;
    }
    static long double schedule(const vector<Customer> &order, Time initial, bool print)
    {
        Time time = initial;
        long double wc = 0, ww = 0;
        for (const Customer &c : order)
        {
            Time start = max(time, Time(c.arrivalTime));
            time = start + c.processingTime;
            wc += (long double)c.waitingWeight * time;
            ww += (long double)c.waitingWeight * (start - c.arrivalTime);
            if (print)
                cout << c.id << " | w/p=" << double(c.waitingWeight) / c.processingTime
                     << " | start=" << start << " | C=" << time << " | W=" << start - c.arrivalTime << '\n';
        }
        if (print)
            cout << "Tong w*C=" << wc << " | Tong w*W=" << ww << '\n';
        return wc;
    }
    static void ratioSort(vector<Customer> &v)
    {
        stable_sort(v.begin(), v.end(), [](const Customer &a, const Customer &b)
                    { return 1LL * a.waitingWeight * b.processingTime > 1LL * b.waitingWeight * a.processingTime; });
    }
    void compare() const
    {
        auto v = pending();
        if (v.empty())
        {
            cout << "Khong co ticket dang cho.\n";
            return;
        }
        cout << "\nFIFO (bat dau tai " << clock << "):\n";
        long double fifo = schedule(v, clock, true);
        ratioSort(v);
        cout << "\nThu tu w/p giam dan:\n";
        long double sorted = schedule(v, clock, true);
        cout << "Chenh lech FIFO - w/p (sum w*C): " << fifo - sorted << '\n';
    }
};
string readText(const string &prompt)
{
    string s;
    while (true)
    {
        cout << prompt;
        if (!getline(cin, s))
            throw runtime_error("EOF");
        if (s.find_first_not_of(" \t\r") != string::npos)
            return s;
        cout << "Khong duoc de trong.\n";
    }
}
int readInt(const string &prompt, int minimum = 0)
{
    while (true)
    {
        string s = readText(prompt);
        istringstream in(s);
        int value;
        char extra;
        if ((in >> value) && !(in >> extra) && value >= minimum)
            return value;
        cout << "Nhap so nguyen >= " << minimum << ".\n";
    }
}
Customer readCustomer(const string &fixedId = "")
{
    Customer c;
    c.id = fixedId.empty() ? readText("Ma: ") : fixedId;
    c.name = readText("Ho ten: ");
    c.serviceType = readText("Dich vu: ");
    c.arrivalTime = readInt("Thoi gian den (phut): ");
    c.processingTime = readInt("Thoi gian xu ly p (>0): ", 1);
    c.waitingWeight = readInt("Trong so w (>0): ", 1);
    return c;
}
int main()
{
    ServiceDesk desk;
    try
    {
        while (true)
        {
            cout << "\n===== SMART SERVICE DESK =====\n"
                 << "1. Add customer\n"
                 << "2. Serve next customer\n"
                 << "3. Search / Update customer\n"
                 << "4. Undo last action\n"
                 << "5. Print report\n"
                 << "0. Exit\n";
            int choice = readInt("Chon: ");
            if (choice == 0)
                break;
            switch (choice)
            {
            case 1:
                cout << (desk.add(readCustomer()) ? "Da them khach.\n" : "Ma khach da ton tai hoac du lieu khong hop le.\n");
                desk.printQueue();
                break;
            case 2:
            {
                Ticket t;
                if (desk.serve(t))
                {
                    cout << "Served: " << t.customer.id << " | waitingCost = "
                         << 1LL * t.customer.waitingWeight * t.end << '\n';
                    desk.printRemaining();
                }
                else
                    cout << "Hang doi rong, khong co khach de phuc vu.\n";
                break;
            }
            case 3:
            {
                cout << "1. Tim kiem\n2. Cap nhat\n3. Xoa khach dang cho\n"
                     << "4. Sap xep danh sach theo ma\n5. Tao moi danh sach\n0. Quay lai\n";
                int operation = readInt("Chon thao tac: ");
                if (operation == 0)
                    break;
                if (operation == 4)
                {
                    desk.sortList();
                    desk.printList();
                    break;
                }
                if (operation == 5)
                {
                    if (readInt("Xoa du lieu va lich su? (1=co, 0=khong): ") == 1)
                    {
                        desk.reset();
                        cout << "Da tao moi danh sach.\n";
                    }
                    break;
                }
                if (operation < 1 || operation > 3)
                {
                    cout << "Lua chon khong hop le.\n";
                    break;
                }
                string id = readText("Ma khach: ");
                const Ticket *t = desk.find(id);
                if (!t)
                {
                    cout << "Khong tim thay khach.\n";
                    break;
                }
                if (operation == 1)
                    ServiceDesk::printTicket(*t);
                else if (operation == 2)
                {
                    if (t->served)
                    {
                        cout << "Khach da phuc vu, khong the cap nhat.\n";
                        break;
                    }
                    cout << (desk.update(id, readCustomer(id)) ? "Da cap nhat.\n" : "Cap nhat that bai.\n");
                }
                else
                    cout << (desk.erase(id) ? "Da xoa khach.\n" : "Khach da phuc vu, khong the xoa.\n");
                break;
            }
            case 4:
                cout << (desk.undo() ? "Da hoan tac thao tac gan nhat.\n" : "Khong co thao tac de hoan tac.\n");
                desk.printQueue();
                break;
            case 5:
                desk.report();
                desk.compare();
                break;
            default:
                cout << "Lua chon khong hop le.\n";
            }
        }
    }
    catch (const runtime_error &e)
    {
        if (string(e.what()) != "EOF")
        {
            cerr << e.what() << '\n';
            return 1;
        }
    }
    return 0;
}
