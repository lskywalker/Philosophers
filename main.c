/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: lsmit <lsmit@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/02 18:40:00 by lsmit         #+#    #+#                 */
/*   Updated: 2022/05/03 22:58:20 by lsmit         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

unsigned long	get_time(void)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return((current_time.tv_sec * 1000) + (current_time.tv_usec / 1000));
}

void	del_mutex(pthread_mutex_t *mutex)
{
	
}

int		mutex_init(t_data *data, unsigned int i)
{
	data->forks = malloc(sizeof(pthread_mutex_t) * data->philo_amount);
	if (!data->forks)
		return (1);
	while (i < data->philo_amount)
	{
		if (pthread_mutex_init(&(data->forks[i]), NULL))
		{
			while (i > 0)
			{
				i--;
				if (pthread_mutex_destroy(&data->forks[i]))
					pthread_mutex_unlock(&data->forks[i]);
				pthread_mutexattr_destroy(&data->forks[i]);
			}
			free(data->forks);
			return (1);
		}
		i++;
	}
	i = 0;
	if (pthread_mutex_init(&(data->write), NULL))
	{
		while (i < data->philo_amount)
		{
			if (pthread_mutex_destroy(&data->forks[i]))
				pthread_mutex_unlock(&data->forks[i]);
			pthread_mutexattr_destroy(&data->forks[i]);
			i++;
		}
		free(data->forks);
		return (1);
	}
	if (pthread_mutex_init(&(data->check_dead), NULL))
	{
		while (i < data->philo_amount)
		{
			if (pthread_mutex_destroy(&data->forks[i]))
				pthread_mutex_unlock(&data->forks[i]);
			pthread_mutexattr_destroy(&data->forks[i]);
			i++;
		}
		free(data->forks);
		if (pthread_mutex_destroy(&data->write))
				pthread_mutex_unlock(&data->write);
		pthread_mutexattr_destroy(&data->write);
		return (1);
	}
	return (0);
}

int		init_data(char **argv, t_data *data)
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
}

int		parser(char **argv)
{
	int	i;
	int j;

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

int		philo_init(t_philo *philo, int id, t_data *data)
{
	philo->id = id;
	philo->times_eaten = 0;
	philo->last_eaten = get_time();
	philo->left_fork = id + 1;
	philo->right_fork = id;
	philo->done = false;
	philo->data = data;
	if (philo->left_fork ==  data->philo_amount)
		philo->left_fork = 0;
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
	if (check_dead(philo->data) == true || state == true)
	{
		time = get_time();
		ft_putnbr(time - philo->data->start_time);
		ft_putnbr(philo->id);
		write(1, str, ft_strlen(str));
	}
	pthread_mutex_unlock(&(philo->data->write));
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// HIER BEN JE BEZIG

void	philo_start(void *phil)
{
	t_philo	*philo;
	
	philo = (t_philo*)phil;
	philo_print(philo, "APPEARED", false);
	usleep(philo->id % 2 * 100);
	while (1)
	{
		philo_print(philo, "THINKING", false);
		if (check_dead(philo->data) == true)
			break ;
		// philo_eat(philo);
		if (check_dead(philo->data) == true)
			break ;
		if (philo->times_eaten > 0 && philo->times_eaten == philo->data->must_eat)
		{
			philo_print(philo, "DONE EATING", false);
			philo->data->done_eating++;
			philo->done = true;
			break ;
		}
		// philo_sleep(philo);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


void	monitoring(t_philo *philo)
{
	unsigned int	i;
	unsigned long	time;
	
	i = 0;
	while (i < philo->data->philo_amount)
	{
		pthread_mutex_lock(&(philo[i].eat));
		time = get_time();
		if (time - philo[i].last_eaten >= philo->data->time_to_die && philo[i].done == false)
		{
			pthread_mutex_lock(&philo->data->check_dead);
			philo->data->dead = true;
			pthread_mutex_unlock(&philo->data->check_dead);
			philo_print(&(philo[i]), "DIED", true);
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
		if (pthread_create(&(threads[i]), NULL, (void*)philo_start, &(philo[i])))
			return (join_threads(threads, i));
		i++;
	}
	monitoring(philo);
	join_threads(threads, data->philo_amount);
}

int		main(int argc, char **argv)
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
		set_table(&data, threads, philo);
	}
	else
		printf("Invalid input!\n");
	return (0);
}
