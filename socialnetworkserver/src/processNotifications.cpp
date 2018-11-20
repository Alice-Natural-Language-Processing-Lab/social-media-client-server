/*
 * processNotifications.cpp
 *
 *  Created on: Nov 18, 2018
 *      Author: pournami
 */
#include <pthread.h>
#include "func_lib.h"

extern pthread_cond_t notify_cond;
extern pthread_mutex_t notify_mutex;

#define DEBUG	printf

void processNotification()
{
	DEBUG("Notification Thread created\n");
	/*
	int sock_fd, read, sock_write;
	Notification notificiation;
	struct packet notify;
	pthread_mutex_lock(&notify_mutex);

	while (1)
	{
		pthread_cond_wait(&notify_cond, &notify_mutex);
		notification = getNotifications();
		while (notification.next())
		{
			memset(&notify, 0, sizeof(notify));
			sock_fd = notification.sendNotification(notify);
			if (sock_fd < 0)
			{
				printf("Error (sendNotification): Notification sending failed\n");
				break;
			}
			sock_write = write_socket(sock_fd, notify);
			if (sock_write < 0)
			{
				printf("Error (write_socket): Notification sending failed (skipping to next notification)\n");
				continue;
			}
			read = notification.markRead();
			if (read < 0)
			{
				printf("Error (markRead): Notification sending failed\n");
				break;
			}
		}
	}
	pthread_mutex_unlock(&notify_mutex);
	*/
	return;
}
