/***************************************************************************
 *            async-queue.h
 *
 *  Feb 8, 2009 7:07:59 PM
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

#ifndef V_ASYNC_QUEUE_H_
#define V_ASYNC_QUEUE_H_


#include <stdbool.h>


typedef struct _VAsyncQueue     VAsyncQueue;
typedef struct _VAsyncQueuePriv VAsyncQueuePriv;



/**
 * VAsyncQueue:
 * @length: the amount of nodes in the queue.
 * @first: the first node in the queue.
 * @last: the last node in the queue.
 *
 * A basic queue using a linked list implementation.
 */
struct _VAsyncQueue
{
	/*< private >*/
	VAsyncQueuePriv *priv;
};




VAsyncQueue *v_async_queue_new  (unsigned int size);
void         v_async_queue_free (VAsyncQueue *queue);


bool  v_async_queue_enqueue (VAsyncQueue *queue, void *data);
void *v_async_queue_dequeue (VAsyncQueue *queue);
void *v_async_queue_peek    (VAsyncQueue *queue);


void  v_async_queue_enqueue_wait (VAsyncQueue *queue, void *data);
void *v_async_queue_dequeue_wait (VAsyncQueue *queue);



#endif /* V_ASYNC_QUEUE_H_ */
