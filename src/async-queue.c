/***************************************************************************
 *            async-queue.c
 *
 *  Feb 8, 2009 7:13:29 PM
 *  Copyright  2009  Goran Sterjov
 *  <goran.sterjov@gmail.com>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with main.c; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */


#include "async-queue.h"
#include "queue.h"
#include "mem.h"
#include <pthread.h>



/*
 * VAsyncQueuePriv:
 *
 * Private structure for #VAsyncQueue.
 */
struct _VAsyncQueuePriv
{
	VQueue *queue;
	
	pthread_mutex_t mutex;
	pthread_cond_t  read;
	pthread_cond_t  write;
};




/**
 * v_async_queue_new:
 *
 * Creates an empty #VAsyncQueue.
 *
 * Returns: a #VAsyncQueue structure.
 */
VAsyncQueue *
v_async_queue_new (unsigned int size)
{
	VAsyncQueue *ret = v_new (VAsyncQueue);
	VAsyncQueuePriv *priv = v_new (VAsyncQueuePriv);
	
	
	/* default values */
	pthread_mutex_init (&priv->mutex, NULL);
	pthread_cond_init  (&priv->read,  NULL);
	pthread_cond_init  (&priv->write, NULL);
	
	priv->queue = v_queue_new (size);
	ret->priv = priv;
	
	
	return ret;
}



/**
 * v_async_queue_free:
 * @queue: a #VAsyncQueue to free.
 *
 * Free's @queue and all its entries.
 * <note>
 *   <para>
 *     If the queue data is dynamically allocated it must first be free'd
 *     manually before v_async_queue_free() is called.
 *   </para>
 * </note>
 */
void
v_async_queue_free (VAsyncQueue *queue)
{
	/* TODO: callback for destroyed nodes */
	
	VAsyncQueuePriv *priv = queue->priv;
	
	
	/* destroy locking components */
	pthread_mutex_destroy (&priv->mutex);
	pthread_cond_destroy  (&priv->read);
	pthread_cond_destroy  (&priv->write);
	
	
	v_queue_free (priv->queue);
	v_free (queue);
}




/**
 * v_async_queue_enqueue:
 * @queue: a #VAsyncQueue.
 * @data: the data to add.
 *
 * Adds @data to the end of @queue.
 */
bool
v_async_queue_enqueue (VAsyncQueue *queue, void *data)
{
	VAsyncQueuePriv *priv = queue->priv;
	
	
	/* lock the queue */
	pthread_mutex_lock (&priv->mutex);
	
	
	/* queue the data */
	bool ret = v_queue_enqueue (priv->queue, data);
	
	
	/* unlock the queue */
	pthread_mutex_unlock (&priv->mutex);
	
	
	return ret;
}



/**
 * v_async_queue_dequeue:
 * @queue: a #VAsyncQueue.
 *
 * Gets the next data from @queue.
 *
 * Returns: the next void* casted data on the queue, %NULL if empty.
 */
void *
v_async_queue_dequeue (VAsyncQueue *queue)
{
	VAsyncQueuePriv *priv = queue->priv;
	
	
	/* lock the queue */
	pthread_mutex_lock (&priv->mutex);
	
	
	/* get the data */
	void *data = v_queue_dequeue (priv->queue);
	
	
	/* unlock the queue */
	pthread_mutex_unlock (&priv->mutex);
	
	
	return data;
}





/**
 * v_async_queue_peek:
 * @queue: a #VAsyncQueue.
 *
 * Gets the next data from @queue without removing it.
 *
 * Returns: the next void* casted data on the queue, %NULL if empty.
 */
void *
v_async_queue_peek (VAsyncQueue *queue)
{
	VAsyncQueuePriv *priv = queue->priv;
	
	
	/* lock the queue */
	pthread_mutex_lock (&priv->mutex);
	
	
	/* get the data */
	void *data = v_queue_peek (priv->queue);
	
	
	/* unlock the queue */
	pthread_mutex_unlock (&priv->mutex);
	
	
	return data;
}




/**
 * v_async_queue_enqueue_wait:
 * @queue: a #VAsyncQueue.
 * @data: the data to add.
 *
 * Adds @data to the end of @queue.
 */
void
v_async_queue_enqueue_wait (VAsyncQueue *queue, void *data)
{
	VAsyncQueuePriv *priv = queue->priv;
	
	
	/* lock the queue */
	pthread_mutex_lock (&priv->mutex);
	
	
	/* queue the data */
	while (v_queue_enqueue (priv->queue, data) == false)
	{
		/* wait until the queue has space */
		pthread_cond_wait (&priv->write, &priv->mutex);
	}

	/* queue has data */
	pthread_cond_signal (&priv->read);
	
	
	/* unlock the queue */
	pthread_mutex_unlock (&priv->mutex);
}




/**
 * v_async_queue_dequeue_wait:
 * @queue: a #VAsyncQueue.
 *
 * Gets the next data from @queue.
 *
 * Returns: the next void* casted data on the queue, %NULL if empty.
 */
void *
v_async_queue_dequeue_wait (VAsyncQueue *queue)
{
	VAsyncQueuePriv *priv = queue->priv;
	
	
	/* lock the queue */
	pthread_mutex_lock (&priv->mutex);
	
	
	/* get the data */
	void *data = v_queue_dequeue (priv->queue);
	
	
	/* no data in the queue */
	while (data == NULL)
	{
		/* wait until the queue has data */
		pthread_cond_wait (&priv->read, &priv->mutex);
		
		/* get the data */
		data = v_queue_dequeue (priv->queue);
	}
	
	
	/* queue has space for data */
	pthread_cond_signal (&priv->write);
	
	
	/* unlock the queue */
	pthread_mutex_unlock (&priv->mutex);
	
	
	return data;
}


