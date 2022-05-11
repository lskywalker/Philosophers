/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   algorithm.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lsmit <lsmit@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/11 18:38:24 by lsmit         #+#    #+#                 */
/*   Updated: 2022/05/11 18:45:48 by lsmit         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

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
		if (philo->data->philo_amount == 1)
			break ;
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

bool	forks(t_philo *philo, int left_fork, int right_fork)
{
	if (left_fork == right_fork)
		return (false);
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
