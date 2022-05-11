/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   utils2.c                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: lsmit <lsmit@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/11 18:42:04 by lsmit         #+#    #+#                 */
/*   Updated: 2022/05/11 18:50:17 by lsmit         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	go_sleep(unsigned long time)
{
	unsigned long	start;

	start = get_time();
	while (get_time() - start < time)
		usleep(100);
}

void	philo_sleep(t_philo *philo)
{
	philo_print(philo, BLU" SLEEPING\n"WHT, false);
	go_sleep(philo->data->time_to_sleep);
}

void	join_threads(pthread_t *threads, int i)
{
	while (i > 0)
	{
		i--;
		pthread_join(threads[i], NULL);
	}
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
		printf("%s%lums %s[%lu] %s", WHT, time - \
		philo->data->start_time, MAG, philo->id, str);
	}
	pthread_mutex_unlock(&(philo->data->write));
}
