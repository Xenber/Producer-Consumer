/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************///
#include "helper.h" 

#define space 1
#define full 2  
int semid;
int in = 0;   
int out = 0;  
int num_of_jobs;
int* buffer;
unsigned int size_of_queue;
pthread_mutex_t mutex;

void *producer (void *id);
void *consumer (void *id);

int main (int argc, char **argv)
{
    if (argc != 5){
        cout<< "1";
        return 0;
    };
    size_of_queue     = check_arg(argv[1]);
    num_of_jobs       = check_arg(argv[2]);
    int num_producers = check_arg(argv[3]);
    int num_consumers = check_arg(argv[4]);

    semid = sem_create(SEM_KEY, 3);
    int sem1 = pthread_mutex_init(&mutex, NULL);
    int sem2 = sem_init(semid, space, size_of_queue); 
    int sem3 = sem_init(semid, full, 0);

    if(semid == -1){

        cerr<<"1"<<semid<<endl;
    }
    else
        cout<<"1"<<semid<<endl<<endl; 

    if(sem1 != 0 || sem2 != 0 || sem3 != 0){

        cerr<<"1"<<endl<<endl;
    }

    srand(time(NULL)); 
    buffer = new int[size_of_queue];
    pthread_t producerid[num_producers];       
    pthread_t consumerid[num_consumers];       
    int producer_id[num_producers];
    int consumer_id[num_consumers]; 

    for(int i = 0; i< num_producers; i++){

	producer_id[i] = i + 1;
	int t = pthread_create (&producerid[i], NULL, producer, &producer_id[i]);
        if(t != 0){

            cerr<<"ERROR: Failure to create producer pthreads. Error value is: ";
            cerr<<t<<endl;
        }
    };

    for(int j = 0; j< num_consumers; j++){

        consumer_id[j] = j + 1;   
        int k =  pthread_create (&consumerid[j], NULL, consumer, &consumer_id[j]);
        if(k != 0){

            cerr<<"ERROR: Failure to create consumer pthreads. Error value is: ";
            cerr<<k<<endl;
        }
    };

    for(int i = 0; i< num_producers; i++){

        int p = pthread_join (producerid[i], NULL);

        if(p != 0){

            cerr<<"ERROR: Failure to join pthreads. Error value is: "<<p<<endl;
            cerr<<" for producers."<<endl;
        }
    };
    for(int j = 0; j< num_consumers; j++){

        int c = pthread_join (consumerid[j], NULL);
        if(c != 0){

            cerr<<"ERROR: Failure to join pthreads. Error value is: "<<c;
            cerr<<" for consumers."<<endl; 
        }
    };
    int semclose = sem_close(semid);
    if(semclose != 0){

        cerr<<"ERROR: Semaphores not closing. Error value is: "<<semclose<<endl;
    }
    return 0;
}

void *producer(void *id) 
{

    int *producer_i = (int*) id;
    for (int i =0; i < num_of_jobs; ++i){ 

        if(sem_timedwait(semid, space, 20) == -1){

            cerr<< "Producer("<< *producer_i <<") Buffer is full and no";
            cerr<<" jobs are being consumed within 20 seconds - TERMINATION"<<endl;

            pthread_exit(0);
        }    
        pthread_mutex_lock(&mutex);  
	buffer[in] = rand()%10 + 1;
        cerr<< "Producer("<< *producer_i << "): Job id (" << in<<")";
        cerr<< " to be executed in "<< buffer[in]<<" seconds. "<<endl;
	in = (in + 1) % size_of_queue;
        sleep(rand() % 5 + 1);        
   
        pthread_mutex_unlock(&mutex); 
        sem_signal(semid, full);   
    }
    cerr<<"Producer("<<*producer_i<<"): No more jobs to produce."<<endl;

    pthread_exit(0);
} 


void *consumer (void *id) 
{  
    int *consumer_id = (int*) id;

    while(1){

        if(sem_timedwait(semid, full, 20) == -1){   //waiting on signal from producer
            cerr<<"Consumer("<< *consumer_id <<") No jobs are being produced within";
            cerr<<" 20 seconds - TERMINATION"<<endl;
            pthread_exit(0);

        }
        pthread_mutex_lock(&mutex); 

        cerr<<"Consumer("<<*consumer_id <<"): Job id ("<< out<<")";
        cerr<<" executing job within "<< buffer[out] <<" seconds."<<endl;
	out = (out + 1) % size_of_queue; 
        pthread_mutex_unlock(&mutex);   
        sem_signal(semid, space);       
        sleep(buffer[out]);          

        cerr<<"Consumer("<<*consumer_id <<"): Job id ("<<out<<") COMPLETED."<<endl; 
    }

    pthread_exit(0);
}  



