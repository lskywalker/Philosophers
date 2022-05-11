/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   init.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: lsmit <lsmit@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/11 18:32:02 by lsmit         #+#    #+#                 */
/*   Updated: 2022/05/11 18:33:49 by lsmit         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

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
	if (data->philo_amount > 200)
	{
		printf("%sERROR:\t%sToo many philosophers\n", RED, WHT);
		return (1);
	}
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
