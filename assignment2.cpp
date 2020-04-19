/*
	CSCI203 Assignment 2 - Shop simulator
	Filename: assignment2.cpp
	By Ivana Ozakovic

*/


//Heap functions referece: 
//http://www.eecs.wsu.edu/~ananth/CptS223/Lectures/heaps.pdf

//Circular queue functions reference:
//http://www.geeksforgeeks.org/circular-queue-set-1-introduction-array-implementation/

#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

//*******************************************//
//**************	CONSTANTS	*************//
//*******************************************//

const int MAX_SERVERS = 20;                 //maximum number of servers
const int MAX_CUSTOMERS = 1000;             //maximum number of customers that can be queued
const int MAX_EVENTS = 1000;                //maximum number of events


//*******************************************//
//**************	GLOBALS 	*************//
//*******************************************//

//-----------    STRUCTURES      ------------//

//SERVER STRUCTRE
struct Server
{

    double endTime;                 //end time of serving the last customer
    double totalServiceTime;        //total duration of serving customers
    double efficiency;              //server efficiency
    bool isIdle;                    //bool to check if server is busy or idle

    int nCustomersServed;           //number of customers served by server

    //constructor - initialize server to idle and other variables to 0
    Server(): isIdle(true), nCustomersServed(0), endTime(0.0), totalServiceTime(0.0) {}

    //function to serve customer on the server - returns end of service time
    double serveCustomer(double serviceTime, double currentTime)
    { 
        //total time taken to serve a customer = (customer service time) * (server efficiency)
        double totalTime = serviceTime * efficiency; 
        //server end time = current simulation time + total service time for a customer
        endTime = currentTime + totalTime; 
        //increment number of customers served by this server
        nCustomersServed++; 
        //set server to idle
        isIdle = false;
        //add total time taken to serve a customer to total service time of a server
        totalServiceTime += totalTime;

        //return end of server's service time
        return endTime; 
    };
    
};

//EVENT STRUCTRE
struct Event
{
    enum                            //event type enum for 2 types of events:
    {
        ARRIVAL,                    // 1) Customer arriving at the shop
        CHECKOUT                    // 2) Customer completing payment at the server 
    } type;                         
                     

    double time;                    //time of event
    double serviceTime;             //customer service time

    int serverIndex;                //index of a server serving the customer in case of checkout event

    //initialize event time variables to 0
    Event(): time(0.0), serviceTime(0.0) {}
 
};

//CUSTOMER STRUCTRE
struct Customer
{
    double arrivalTime;             //customer arrival time
    double serviceTime;             //customer base service time
    
};

//------------    ARRAYS      -------------//
//SERVERS ARRAY
Server servers[MAX_SERVERS];

//------------    CLASSES      -------------//

//SERVER QUEUE CLASS
class ServerQueue
{
public:
    //constructor - initializes servers with data read in from file
    ServerQueue(ifstream &ins)
    {
        //initialize heap size
        currentHeapSize = 0;
        //read in number of servers
        ins >> servers_n;

        //read in server's efficiency
        for (int i = 0; i < servers_n; ++i)
        {
            ins >> servers[i].efficiency;
            heapInsertServer(i);
        }
        
        //initialize total cutomers served
        totalCustomersServed = 0;   
        //initialize number of idle servers to number of total servers         
        idleServers_n = servers_n;          
    };

    //heap insert function to add idle server index to heap (array containing idle server index)
    void heapInsertServer(int serverIndex)
    {
        if(currentHeapSize == MAX_SERVERS)
        {
            cerr << "Error: Cannot add any more servers to heap." << endl;
            exit(1);
        }

        //sift up
        int hole = ++currentHeapSize;

        for(; hole > 1 && servers[serverIndex].efficiency < servers[serverHeap[hole / 2]].efficiency; hole /= 2)
            serverHeap[hole] = serverHeap[hole / 2];

        serverHeap[hole] = serverIndex;
    };

    //heap min delete function for deleting occupied server index from heap (array containing idle server index)
    void heapDeleteFastestServer()
    {
        if(currentHeapSize == 0)
        {
            cerr << "Error: No serves to delete from heap." << endl;
            exit(1);
        }

        serverHeap[1] = serverHeap[currentHeapSize--];
        siftDownServer(serverHeap, currentHeapSize, 1);
    };
    //heap sift down function
    void siftDownServer(int array[], int currentSize, int hole)
    {
        int child;
        int tmp = array[hole];

        for(; hole * 2 <= currentSize; hole = child)
        {
            //left child
            child = hole * 2;
            
            //right child
            if(child != currentSize && servers[array[child + 1]].efficiency < servers[array[child]].efficiency)
                ++child;
            
            //child to swap based on servers efficiency
            if(servers[array[child]].efficiency < servers[tmp].efficiency)
                array[hole] = array[child];
            else
                break;
        }
        //insert element in free spot
        array[hole] = tmp;
    };


    //serve next customer at the most efficient idle server
    double serveNextCustomer(double serviceTime, double currentTime, int &serverIndex)
    { 
        serverIndex = serverHeap[1];    // find the most efficient idle server

        //serve a customer and get finish time
        double endTime = servers[serverIndex].serveCustomer(serviceTime, currentTime);
        
        heapDeleteFastestServer();      //delete busy server index from heap
        
        idleServers_n--;                //decrement number of idle servers
        
        return endTime;                 //return the time when serving customer completed
    };
    
    //remove served customer from server
    void removeServedCustomer(int serverIndex)
    { 
        //remove customer from server (served)
        servers[serverIndex].isIdle = true;
        //insert idle server index to heap
        heapInsertServer(serverIndex);
        //incerement number of idle servers
        idleServers_n++;
        //increment total customers served
        totalCustomersServed++; 

    };

    //returns true if all servers busy
    bool allBusy(){ return idleServers_n == 0; };

    //returns number of servers
    int getNumberOfServers() { return servers_n; };

    //returns number of total customers served
    int getTotalCustomersServed() { return totalCustomersServed; };


private:
    int totalCustomersServed;               //number of total customers served by all servers        
    int servers_n;                          //number of servers
    int idleServers_n;                      //number of idle servers

    //heap variables used to get fastest idle server
    int serverHeap[MAX_SERVERS];            //storing server index on heap for getting fastest idle one
    int currentHeapSize;                    //current server heap size


};

//*******************************************//
//******    Functions declaration    ********//
//*******************************************//

//returs true if read customer data from file
bool readInCustomer(ifstream &ins, Customer &customer);
//-------------- EVENT HEAP FUNCTIONS -------------//
// The following three algorithms were referenced from: http://www.eecs.wsu.edu/~ananth/CptS223/Lectures/heaps.pdf
void heapInsertEvent(Event array[], Event x, int &currentSize, const int HEAP_SIZE);
void heapDeleteMinEvent(Event array[], int &currentSize);
void siftDown(Event array[], int currentSize, int hole);


//EVENT QUEUE CLASS - HEAP
class EventQueue
{
public:
    //constructor
    EventQueue(){ events_n = 0; };

    //add new arrival event
    void addArrivalEvent(double eventTime, double serviceTime)
    { 
        Event temp;

        temp.type = Event::ARRIVAL;
        temp.time = eventTime;
        temp.serviceTime = serviceTime;
        temp.serverIndex = -1;

        heapInsertEvent(events, temp, events_n, MAX_EVENTS);

    };

    //add new checkout event
    void addCheckoutEvent(double eventTime, int serverIndex)
    {
        Event temp;

        temp.type = Event::CHECKOUT;
        temp.time = eventTime;
        temp.serviceTime = 0;

        temp.serverIndex = serverIndex;

        heapInsertEvent(events, temp, events_n, MAX_EVENTS);

    };

    //get next event 
    Event getNextEvent()
    {
        //1st element in the event queue (heap) is the next event
        Event next_event = events[1];       

        heapDeleteMinEvent(events, events_n);    

        return next_event;
    };

    //returns true if there are any events
    bool isNotEmpty() { return events_n > 0; };
private:
    Event events[MAX_EVENTS];               //events array
    int events_n;                           //number of events
    
};

//CUSTOMER CIRCULAR QUEUE CLASS
class CustomerQueue
{
public:
    //constructor
    CustomerQueue()
    {
        customers_n = 0;
        //initialize circular queue variables
        front = 0;
        rear = 0; 
        //initialize stats variables
        total_customers_n = 0; 
        max_queue_length = 0;
        total_queue_time = 0;             
        total_queue_length = 0;
        total_time = 0;

    };

    //------------- CIRCULAR QUEUE FUNCTIONS -------------//
    //function returns true if there is no customers in the queue
    bool isEmpty(void) { return customers_n == 0; }
    //function returns true if maximum number of customers reached
    bool isFull(void)  { return customers_n == MAX_CUSTOMERS; } 

    //function adds customer to queue - returns true if customer added sucessfully
    bool enqueue(Customer customer)
    {
        //if queue full
        if((front == 0 && rear == MAX_CUSTOMERS - 1) || (front == rear + 1))
        {
            cerr << "Error: Customer queue full. " << endl;
            return false;
        }
        else
        {
            if(rear == MAX_CUSTOMERS - 1 && front != 0)
                rear = -1;

            customers[rear++] = customer;
            //incerement current number of customers
            ++customers_n;
            //increment total number of customers
            ++total_customers_n;

            if(front == -1)
                front = 0;
        }

        return true;
    }
    //function removes customer from the queue - returns deleted customer, otherwise returns null customer
    Customer dequeue(double currentTime)
    {
        //if queue empty
        if(front == -1 && rear == -1)   
        {
            cerr << "Error: No customers in the queue." << endl;
        }
        else
        {
            //dequeue customer
            Customer ret = customers[front++];

            if(front == MAX_CUSTOMERS)
                front = 0;

            if(front - 1 == rear)
                front = rear = -1;

            //decrement current number of customers
            --customers_n;

            //add total queue time
            total_queue_time += currentTime - ret.arrivalTime;

            return ret;
        }

        return Customer();    //return null customer
    }

    //------------- STATS FUNCTIONS -------------//
    //function to calculate statistical information of the customer queue
    void calculateStatistics(double time)
    {
        //total stats
        total_queue_length += customers_n * time;
        total_time += time;

        //check if current queue length maximum length of queue
        if (customers_n > max_queue_length)
            max_queue_length = customers_n;   
        
    };

    //returns maximum queue length
    int getMaxLength(){ return max_queue_length; };
    //retutns average queue length
    double getAverageLength() { return total_queue_length/total_time; };
    //returns average time spent in the queue 
    double getAverageQueuedTime() { return total_queue_time/total_customers_n; };

private:
    Customer customers[MAX_CUSTOMERS];       //customers array
    //circular queue variables
    int customers_n;                         //current number of customers in queue
    int front, rear;

    //variables for statistical information
    double total_customers_n; 
    int max_queue_length;
    double total_queue_time;             
    double total_queue_length;
    double total_time;
    
};


//*******************************************//
//**********	Main function	*************//
//*******************************************//
int main(int argc, char *argv[])
{ 

	ifstream ins;									//input stream
    char filename[32];								//variable for storing filename
    
    cout << "|---------------- SHOP SIMULATION -----------------|\n" << endl;

    cout << "Enter an input filename: ";
    cin.getline(filename, sizeof filename);			//get filename
 
 	ins.open(filename);								//open file

    if(!ins.good())									//if cannot open the file, print error message
    {
        cerr << "Unable to open the file. Program will now terminate." << endl;
        return 1;
    }

    //Initialize queues
    ServerQueue serverQ(ins);
    CustomerQueue customerQ;
    EventQueue eventQ;

    //Customer variable to read in next customer arrival and service time from file
    Customer customer;
    //read in 1st customer
    readInCustomer(ins, customer);

    //add event for 1st customer's arrival
    eventQ.addArrivalEvent(customer.arrivalTime, customer.serviceTime);
    //variables for current time in simulation and start time of simulation initialized to 1st customer time arrival
    double currentTime, startTime = customer.arrivalTime;  

    //variable to store time after previous event ended - used for calculating statistical information
    double previousTime = -1;
    //variable to store service time for next event in event queue
    double serviceTime = 0;

    //while event queue is not empty
    while(eventQ.isNotEmpty())
    {

        //get next event from event queue
        Event nextEvent = eventQ.getNextEvent(); 
        //update current and service time based on next event
        currentTime = nextEvent.time;
        serviceTime = nextEvent.serviceTime;

        //calculate customer queue statistical information
        if (previousTime >= 0) 
            customerQ.calculateStatistics(currentTime - previousTime);
        //set previous time equal to current time after calculating statistical information
        previousTime = currentTime;
        

        //if next event is customer arrival
        if (nextEvent.type == Event::ARRIVAL)
        {
            //set customer's data
            customer.arrivalTime = currentTime;
            customer.serviceTime = serviceTime;
            //enqueue customer
            customerQ.enqueue(customer);

            //read in next customer and add arrival event if not end of file
            if (readInCustomer(ins, customer) && !ins.eof())
            {
                    //add arrival event to event queue
                    eventQ.addArrivalEvent(customer.arrivalTime, customer.serviceTime);
            }

        }
        //else next event is customer checkout
        else
        {
            //remove sereved customer from server
            serverQ.removeServedCustomer(nextEvent.serverIndex);
        }
        
        //move customers from customer queue to most efficient idle server
        if (!customerQ.isEmpty() && !serverQ.allBusy())
        { 
            //store server index
            int serverIndex; 
            //get served customer and remove from queue
            Customer served = customerQ.dequeue(currentTime);

            //add finish time to event queue
            double finishTime = serverQ.serveNextCustomer(served.serviceTime, currentTime, serverIndex);
            //add checkout event
            eventQ.addCheckoutEvent(finishTime, serverIndex);

        }
            
    }

    //close file
    ins.close();									


    //------------- PRINT STATISTICAL INFORMATION --------------//

    cout << "|---------------- STATISTICAL INFORMATION -----------------|\n" << endl;
    
    cout << "Number of customers served: " << serverQ.getTotalCustomersServed() << endl;
    cout << "Time last customer was finished being served: " << currentTime << endl;                                                                                                                                                                    
    cout << "Greatest length reached in the queue: " << customerQ.getMaxLength() << endl;
    cout << "Average length of the queue: " << customerQ.getAverageLength() << endl;
    cout << "Average time spent in the queue: " << customerQ.getAverageQueuedTime() << endl;
    cout << endl;
    
    cout << setw(5) << "Server" << setw(15) << "Efficiency" << setw(20) << "Customers Served"  << setw(15) << "Idle Time" << endl;

    double serverIdleTime;

    //print stats for each server
    for (int i = 0; i < serverQ.getNumberOfServers(); i++)
    {
        serverIdleTime = (currentTime - startTime) - servers[i].totalServiceTime;       //calculate idle time for each server

        cout << setw(5) << (i+1) << setw(15) << servers[i].efficiency << setw(20) << servers[i].nCustomersServed << setw(15) << setprecision(7) << serverIdleTime << endl; 
    }
    cout << endl;

	return 0;
}

//*******************************************//
//*******    Function definitions    ********//
//*******************************************//

//returs true if read customer data from file
bool readInCustomer(ifstream &ins, Customer &customer)
{
    //if file cannot be read return false
    if(ins.fail())
        return false;

    //read in customer arrival time and service time from file
    ins >> customer.arrivalTime  >> customer.serviceTime;

    return true;
}

//------------- EVENT HEAP FUNCTIONS --------------//
// The following three algorithms were referenced from: http://www.eecs.wsu.edu/~ananth/CptS223/Lectures/heaps.pdf
//heap insert function for event queue
void heapInsertEvent(Event array[], Event x, int &currentSize, const int HEAP_SIZE)
{
    if(currentSize == HEAP_SIZE)
    {
        cerr << "Error: Event queue full." << endl;
        exit(1);
    }

    //sift up
    int hole = ++currentSize;

    for(; hole > 1 && x.time < array[hole / 2].time; hole /= 2)
        array[hole] = array[hole / 2];

    array[hole] = x;
}
//heap delete minimum event function for event queue
void heapDeleteMinEvent(Event array[], int &currentSize)
{
    if(currentSize == 0)
    {
        cerr << "Error: No events to delete." << endl;
        exit(1);
    }

    array[1] = array[currentSize--];
    siftDown(array, currentSize, 1);
}
//heap sift down function event heap for event queue
void siftDown(Event array[], int currentSize, int hole)
{
    int child;
    Event tmp = array[hole];

    for(; hole * 2 <= currentSize; hole = child)
    {
        //left child
        child = hole * 2;
        
        //right child
        if(child != currentSize && array[child + 1].time < array[child].time)
            ++child;
        
        //child to swap with
        if(array[child].time < tmp.time)
            array[hole] = array[child];
        else
            break;
    }
    //insert element in free spot
    array[hole] = tmp;
}




