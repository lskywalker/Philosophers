/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   philosophers.h                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: lsmit <lsmit@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/05/02 18:44:59 by lsmit         #+#    #+#                 */
/*   Updated: 2022/05/05 20:36:36 by lsmit         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <stdlib.h>
# include <unistd.h>
# include <pthread.h>
# include <stdio.h>
# include <sys/time.h>
# include <stdbool.h>

# define NRM  "\x1B[0m"
# define RED  "\x1B[31m"
# define GRN  "\x1B[32m"
# define YEL  "\x1B[33m"
# define BLU  "\x1B[34m"
# define MAG  "\x1B[35m"
# define CYN  "\x1B[36m"
# define WHT  "\x1B[37m"

typedef struct s_data
{
	size_t			philo_amount;
	size_t			time_to_die;
	size_t			time_to_eat;
	size_t			time_to_sleep;
	size_t			must_eat;
	size_t			done_eating;
	size_t			start_time;
	bool			dead;
	pthread_mutex_t	check_dead;
	pthread_mutex_t	write;
	pthread_mutex_t	*forks;
}				t_data;

typedef struct s_philo
{
	size_t			id;
	size_t			times_eaten;
	size_t			last_eaten;
	size_t			left_fork;
	size_t			right_fork;
	bool			done;
	pthread_mutex_t	eat;
	t_data			*data;
}				t_philo;

// LIBFT
void	ft_putnbr(unsigned long n);
int		ft_atoi(char *str);
int		ft_isdigit(int c);
int		ft_strlen(char *str);

#endif
