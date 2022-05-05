/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: lsmit <lsmit@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/02 18:40:00 by lsmit         #+#    #+#                 */
/*   Updated: 2022/05/05 21:06:04 by lsmit         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

unsigned long	get_time(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return ((current_time.tv_sec * 1000) + (current_time.tv_usec / 1000));
}

void	mutex_destroyer(pthread_mutex_t *mutex)
{
	if (pthread_mutex_destroy(mutex))
		pthread_mutex_unlock(mutex);
	pthread_mutex_destroy(mutex);
}

void	mutex_destroyall(pthread_mutex_t *forks, int i)
{
	while (i > 0)
	{
		i--;
		mutex_destroyer(&forks[i]);
	}
	free(forks);
}

int	mutex_init(t_data *data, unsigned int i)
{
	data->forks = malloc(sizeof(pthread_mutex_t) * data->philo_amount);
	if (!data->forks)
		return (1);
	while (i < data->philo_amount)
	{
		if (pthread_mutex_init(&(data->forks[i]), NULL))
		{
			mutex_destroyall(data->forks, i);
			return (1);
		}
		i++;
	}
	i = 0;
	if (pthread_mutex_init(&(data->write), NULL))
	{
		mutex_destroyall(data->forks, data->philo_amount);
		return (1);
	}
	if (pthread_mutex_init(&(data->check_dead), NULL))
	{
		mutex_destroyall(data->forks, data->philo_amount);
		mutex_destroyer(&data->write);
		return (1);
	}
	return (0);
}

int	init_data(char **argv, t_data *data)
{
	data->philo_amount = ft_atoi(argv[1]);
	data->time_to_die = ft_atoi(argv[2]);
	data->time_to_eat = ft_atoi(argv[3]);
	data->time_to_sleep = ft_atoi(argv[4]);
	if (argv[5])
		data->must_eat = ft_atoi(argv[5]);
	else
		data->must_eat = 0;
	data->start_time = get_time();
	data->done_eating = 0;
	data->dead = false;
	if (mutex_init(data, 0))
		return (1);
	return (0);
}

int	parser(char **argv)
{
	int	i;
	int	j;

	i = 1;
	j = 0;
	while (argv[i])
	{
		j = 0;
		if (ft_atoi(argv[i]) == 0)
			return (0);
		while (argv[i][j])
		{
			if (!ft_isdigit(argv[i][j]))
				return (0);
			j++;
		}
		i++;
	}
	return (1);
}

void	join_threads(pthread_t *threads, int i)
{
	while (i > 0)
	{
		i--;
		pthread_join(threads[i], NULL);
	}
}

int	philo_init(t_philo *philo, int id, t_data *data)
{
	philo->id = id;
	philo->times_eaten = 0;
	philo->last_eaten = get_time();
	philo->left_fork = id + 1;
	if (philo->left_fork == data->philo_amount)
		philo->left_fork = 0;
	philo->right_fork = id;
	philo->done = false;
	philo->data = data;
	if (pthread_mutex_init(&(philo->eat), NULL))
		return (1);
	return (0);
}

bool	check_dead(t_data *data)
{
	pthread_mutex_lock(&(data->check_dead));
	if (data->dead == true)
	{
		pthread_mutex_unlock(&(data->check_dead));
		return (true);
	}
	pthread_mutex_unlock(&(data->check_dead));
	return (false);
}

void	philo_print(t_philo *philo, char *str, bool state)
{
	unsigned long	time;

	pthread_mutex_lock(&(philo->data->write));
	if (check_dead(philo->data) == false || state == true)
	{
		time = get_time();
		printf("%s%lums %s[%lu] %s",WHT, time - \
		philo->data->start_time, MAG, philo->id, str);
	}
	pthread_mutex_unlock(&(philo->data->write));
}

bool	forks(t_philo *philo, int left_fork, int right_fork)
{
	pthread_mutex_lock(&(philo->data->forks[right_fork]));
	if (check_dead(philo->data) == true)
	{
		pthread_mutex_unlock(&(philo->data->forks[right_fork]));
		return (false);
	}
	philo_print(philo, GRN" PICKING UP FORK\n"WHT, false);
	pthread_mutex_lock(&(philo->data->forks[left_fork]));
	if (check_dead(philo->data) == true)
	{
		pthread_mutex_unlock(&(philo->data->forks[left_fork]));
		pthread_mutex_unlock(&(philo->data->forks[right_fork]));
		return (false);
	}
	philo_print(philo, GRN" PICKING UP FORK\n"WHT, false);
	return (true);
}

void	go_sleep(unsigned long time)
{
	unsigned long	start;

	start = get_time();
	while (get_time() - start < time)
		usleep(100);
}

void	philo_eat(t_philo *philo)
{
	if (forks(philo, philo->left_fork, philo->right_fork) == false)
		return ;
	pthread_mutex_lock(&(philo->eat));
	philo_print(philo, YEL" EATING\n"WHT, false);
	philo->times_eaten++;
	philo->last_eaten = get_time();
	pthread_mutex_unlock(&(philo->eat));
	go_sleep(philo->data->time_to_eat);
	pthread_mutex_unlock(&(philo->data->forks[philo->right_fork]));
	pthread_mutex_unlock(&(philo->data->forks[philo->left_fork]));
}

void	philo_sleep(t_philo *philo)
{
	philo_print(philo, BLU" SLEEPING\n"WHT, false);
	go_sleep(philo->data->time_to_sleep);
}

void	philo_start(void *phil)
{
	t_philo	*philo;

	philo = (t_philo *)phil;
	philo_print(philo, " APPEARED\n", false);
	usleep(philo->id % 2 * 100);
	while (1)
	{
		philo_print(philo, CYN" THINKING\n"WHT, false);
		if (check_dead(philo->data) == true)
			break ;
		philo_eat(philo);
		if (check_dead(philo->data) == true)
			break ;
		if (philo->times_eaten > 0 && \
		philo->times_eaten == philo->data->must_eat)
		{
			philo->data->done_eating++;
			philo->done = true;
			break ;
		}
		philo_sleep(philo);
	}
}

void	monitoring(t_philo *philo)
{
	unsigned int	i;
	unsigned long	time;

	i = 0;
	while (i < philo->data->philo_amount)
	{
		pthread_mutex_lock(&(philo[i].eat));
		time = get_time();
		if (time - philo[i].last_eaten >= philo->data->time_to_die && \
		philo[i].done == false)
		{
			pthread_mutex_lock(&philo->data->check_dead);
			philo->data->dead = true;
			pthread_mutex_unlock(&philo->data->check_dead);
			philo_print(&(philo[i]), RED" DIED\n"WHT, true);
			pthread_mutex_unlock(&(philo[i].eat));
			break ;
		}
		pthread_mutex_unlock(&(philo[i].eat));
		if (philo->data->done_eating == philo->data->philo_amount)
			break ;
		i++;
		if (i == philo->data->philo_amount)
			i = 0;
	}
}

void	set_table(t_data *data, pthread_t *threads, t_philo *philo)
{
	unsigned int	i;

	i = 0;
	while (i < data->philo_amount)
	{
		if (philo_init(&(philo[i]), i, data))
			return (join_threads(threads, i));
		if (pthread_create(&(threads[i]), NULL, (void *)philo_start, \
		&(philo[i])))
			return (join_threads(threads, i));
		i++;
	}
	monitoring(philo);
	join_threads(threads, data->philo_amount);
}

int	main(int argc, char **argv)
{	
	t_data		data;
	pthread_t	*threads;
	t_philo		*philo;

	if (argc < 5 || argc > 6)
	{
		printf("Not the right amount of arguments\n");
		return (0);
	}
	if (parser(argv))
	{
		if (init_data(argv, &data))
			return (1);
		threads = malloc(sizeof(pthread_t) * data.philo_amount);
		philo = malloc(sizeof(t_philo) * data.philo_amount);
		if (!threads || !philo)
			return (1);
		set_table(&data, threads, philo);
	}
	else
		printf("Invalid input!\n");
	return (0);
}
