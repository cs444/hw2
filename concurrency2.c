#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lockdisplay;
pthread_mutex_t forks[5];


struct philosopher
{
	char philo_name[20];
	
	int seat_num;
};

void thinking(struct philosopher *ph)
{
	pthread_mutex_lock(&lockdisplay);
	printf("%s is thinking.\n", ph->philo_name);
	pthread_mutex_unlock(&lockdisplay);
	//thinks for 1-20 seconds.
	sleep(rand()%20+1);
}

void geting_forks(struct philosopher *ph)
{
	int leftfork;
	int rightfork;
	if (ph->seat_num+1 == 5) {
		leftfork = 0;
		rightfork = ph->seat_num;
	}
	else {
		leftfork = ph->seat_num;
		rightfork = ph->seat_num+1;
	}
	pthread_mutex_lock(&forks[leftfork]);
	pthread_mutex_lock(&lockdisplay);
	printf("%s attempts to get no. %d fork\n", ph-> philo_name,leftfork);
	pthread_mutex_unlock(&lockdisplay);
	
	pthread_mutex_lock(&forks[rightfork]);
	pthread_mutex_lock(&lockdisplay);
	printf("%s attempts to get no. %d fork \n", ph-> philo_name,rightfork);
	pthread_mutex_unlock(&lockdisplay);
	
}

void eating(struct philosopher *ph)
{
	pthread_mutex_lock(&lockdisplay);
	printf("%s is eating.\n", ph->philo_name);
	pthread_mutex_unlock(&lockdisplay);
	//eats for 2 - 9 seconds.
	sleep(rand()%8+2);
	
	pthread_mutex_lock(&lockdisplay);
	printf("%s eating done, then will start to thinking.\n", ph->philo_name);
	pthread_mutex_unlock(&lockdisplay);
}

void puting_forks(struct philosopher *ph)
{
	int leftfork;
	int rightfork;
	if (ph->seat_num+1 == 5) {
		leftfork = 0;
		rightfork = ph->seat_num;
	}
	else {
		leftfork = ph->seat_num;
		rightfork = ph->seat_num+1;
	}
	pthread_mutex_lock(&lockdisplay);
	printf("%s puts down the forks\n", ph-> philo_name);
	pthread_mutex_unlock(&lockdisplay);
	pthread_mutex_unlock(&forks[leftfork]);
	pthread_mutex_unlock(&forks[rightfork]);
}

void *philosopher_do(void *phil)
{
	struct philosopher *ph = (struct philosopher *)phil;

	while(1) {
		thinking(ph);
		geting_forks(ph);
		eating(ph);
		puting_forks(ph);

	}
}


void init_mutex_and_thread( struct philosopher * philosophers){
	
	int i;
	pthread_mutex_init(&lockdisplay, NULL);
	for (i = 0; i < 5; i++) {
		pthread_mutex_init(&forks[i], NULL);
	}
	
	pthread_t threads[5];
	for (i = 0; i < 5; i++) {
		pthread_create(&threads[i], NULL, philosopher_do, &philosophers[i]);
	}
	
	for (i = 0; i < 5; i++) {
		pthread_join(threads[i],NULL);
	}


}

void init_struct(struct philosopher * philosophers){
	strcpy(philosophers[0].philo_name, "Aristotle");
	strcpy(philosophers[1].philo_name, "Plato");
	strcpy(philosophers[2].philo_name, "Pythagoras");
	strcpy(philosophers[3].philo_name, "Heraclitus");
	strcpy(philosophers[4].philo_name, "Socrates");



printf("the order that five philosopherss: 0 Arstotle 1 Plato 2 pythagoras 3 Heraclitus 4 Socrates 0 \n  ");

	int i;
	for (i = 0; i < 5; i++) {
		philosophers[i].seat_num = i;
	}



}

int main()
{
	

	struct philosopher *philosophers;
	philosophers = malloc(sizeof(struct philosopher)*5);
	

      init_struct(philosophers);

	init_mutex_and_thread( philosophers );


	return 0;
}
