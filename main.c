/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: lsmit <lsmit@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/02 18:40:00 by lsmit         #+#    #+#                 */
/*   Updated: 2022/05/11 18:45:39 by lsmit         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

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
