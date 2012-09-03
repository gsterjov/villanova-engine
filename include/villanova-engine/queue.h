/***************************************************************************
 *            queue.h
 *
 *  Tue Dec 23 17:30:35 2008
 *  Copyright  2008  Goran Sterjov
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
 
#ifndef V_QUEUE_H_
#define V_QUEUE_H_


#include <stdbool.h>


typedef struct _VQueue     VQueue;
typedef struct _VQueuePriv VQueuePriv;



/**
 * VQueue:
 * @length: the amount of nodes in the queue.
 * @first: the first node in the queue.
 * @last: the last node in the queue.
 *
 * A basic queue using a linked list implementation.
 */
struct _VQueue
{
	unsigned int length;
	
	/*< private >*/
	VQueuePriv *priv;
};




VQueue *v_queue_new  (unsigned int size);
void    v_queue_free (VQueue *queue);


bool  v_queue_enqueue (VQueue *queue, void *data);
void *v_queue_dequeue (VQueue *queue);
void *v_queue_peek    (VQueue *queue);



#endif /* V_QUEUE_H_ */
 
