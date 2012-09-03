/***************************************************************************
 *            queue.c
 *
 *  Tue Dec 23 17:50:57 2008
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
 
 
#include "queue.h"
#include "mem.h"


typedef struct _VQueueNode VQueueNode;




/*
 * VQueuePriv:
 *
 * Private structure for #VQueue.
 */
struct _VQueuePriv
{
	/* linked queue */
	VQueueNode *first;
	VQueueNode *last;
	
	
	/* circular queue */
	VQueueNode *array;
	
	int head;
	int tail;
	int fill;
};



/*
 * VQueueNode:
 * @next: the next node on the queue.
 * @data: the data within the queue node.
 *
 * A queue node which stores data and a reference to the next node.
 */
struct _VQueueNode
{
	VQueueNode *next;
	void *data;
};





/**
 * v_queue_new:
 *
 * Creates an empty #VQueue.
 *
 * Returns: a #VQueue structure.
 */
VQueue *
v_queue_new (unsigned int size)
{
	VQueue *ret = v_new (VQueue);
	VQueuePriv *priv = v_new (VQueuePriv);
	
	
	/* default values */
	priv->array = NULL;
	ret->length = size;
	ret->priv = priv;
	
	
	/* create circular queue */
	if (size > 0)
	{
		priv->array = v_mallocz (size * sizeof (VQueueNode));
		
		priv->head = 0;
		priv->tail = 0;
		priv->fill = 0;
	}
	
	
	return ret;
}



/**
 * v_queue_free:
 * @queue: a #VQueue to free.
 *
 * Free's @queue and all its entries.
 * <note>
 *   <para>
 *     If the queue data is dynamically allocated it must first be free'd
 *     manually before v_queue_free() is called.
 *   </para>
 * </note>
 */
void
v_queue_free (VQueue *queue)
{
	/* TODO: callback for destroyed nodes */
	
	VQueuePriv *priv = queue->priv;
	
	VQueueNode *node;
	VQueueNode *next;
	
	
	/* linked queue */
	if (priv->array == NULL)
	{
		/* free all nodes */
		for (node = priv->first; node; node = next)
		{
			next = node->next;
			v_free (node);
		}
	}
	
	
	/* circular queue */
	else
		v_free (priv->array);
	
	
	/* free the queue */
	v_free (queue);
}




/**
 * v_queue_enqueue:
 * @queue: a #VQueue.
 * @data: the data to add.
 *
 * Adds @data to the end of @queue.
 */
bool
v_queue_enqueue (VQueue *queue, void *data)
{
	VQueuePriv *priv = queue->priv;
	
	
	/* linked queue */
	if (priv->array == NULL)
	{
		VQueueNode *node = v_new (VQueueNode);
			
		/* set node data */
		node->data = data;
		
		
		/* queue is empty */
		if (priv->first == NULL)
			priv->first = node;
		
		/* connect last node */
		if (priv->last != NULL)
			priv->last->next = node;
		
		
		/* set as last node */
		priv->last = node;
		
		queue->length++;
	}
	
	
	/* circular queue */
	else
	{
		/* queue is full */
		if (priv->fill == queue->length)
			return false;
		
		
		/* increment tail */
		priv->tail++;
		priv->fill++;
		
		
		/* return to the start of the array */
		if (priv->tail == queue->length)
			priv->tail = 0;
		
		
		/* set node data */
		priv->array[priv->tail].data = data;
	}
	
	
	return true;
}



/**
 * v_queue_dequeue:
 * @queue: a #VQueue.
 *
 * Gets the next data from @queue.
 *
 * Returns: the next void* casted data on the queue, %NULL if empty.
 */
void *
v_queue_dequeue (VQueue *queue)
{
	VQueuePriv *priv = queue->priv;
	void *data = NULL;
	
	
	/* linked queue */
	if (priv->array == NULL)
	{
		/* get data at the head of the queue */
		if (priv->first != NULL)
		{
			VQueueNode *first = priv->first;
			
			data = first->data;
			priv->first = first->next;
			
			v_free (first);
			
			queue->length--;
			
			
			/* no more nodes */
			if (priv->first == NULL)
				priv->last = NULL;
		}
	}

	
	/* circular queue */
	else
	{
		/* queue is empty */
		if (priv->fill == 0)
			return NULL;


		/* increment head */
		priv->head++;
		priv->fill--;


		/* return to the start of the array */
		if (priv->head == queue->length)
			priv->head = 0;


		/* get node data */
		data = priv->array[priv->head].data;
	}
	
	
	return data;
}



/**
 * v_queue_peek:
 * @queue: a #VQueue.
 *
 * Gets the next data from @queue without removing it.
 *
 * Returns: the next void* casted data on the queue, %NULL if empty.
 */
void *
v_queue_peek (VQueue *queue)
{
	VQueuePriv *priv = queue->priv;
	void *data = NULL;
	
	
	/* linked queue */
	if (priv->array == NULL)
	{
		/* get first node data */
		if (priv->first != NULL)
			data = priv->first->data;
	}
	
	
	/* circular queue */
	else
	{
		if (priv->fill > 0)
		{
			int head = priv->head + 1;
			
			/* return to the start of the array */
			if (head == queue->length)
				head = 0;
			
			/* get node data at the head */
			data = priv->array[head].data;
		}
	}
	
	
	return data;
}


