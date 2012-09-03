/***************************************************************************
 *            list.c
 *
 *  Sun Dec 14 13:57:31 2008
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
 
 
#include "list.h"
#include "mem.h"



/**
 * v_list_new:
 *
 * Creates an empty #VList.
 *
 * Returns: a #VList structure.
 */
VList *
v_list_new (void)
{
	return v_new (VList);
}



/**
 * v_list_free:
 * @list: a #VList to free.
 *
 * Free's @list and all its entries.
 * <note>
 *   <para>
 *     If the list data is dynamically allocated it must first be free'd
 *     manually before v_list_free() is called.
 *   </para>
 * </note>
 */
void
v_list_free (VList *list)
{
	VListNode *node;
	VListNode *next;
	
	
	/* free all nodes */
	for (node = list->first; node; node = next)
	{
		next = node->next;
		v_free (node);
	}
	
	
	/* free the list */
	v_free (list);
}




/**
 * v_list_append:
 * @list: a #VList.
 * @data: the data to append.
 *
 * Appends @data to the end of @list.
 */
void
v_list_append (VList *list, void *data)
{
	VListNode *node = v_new (VListNode);
	
	
	/* append the node */
	node->prev = NULL;
	node->next = NULL;
	node->data = data;
	
	
	/* list is empty */
	if (list->first == NULL)
		list->first = node;
	
	/* connect last node */
	if (list->last != NULL)
		list->last->next = node;
	
	
	/* set as last node */
	node->prev = list->last;
	list->last = node;
	
	list->length++;
}



/**
 * v_list_remove:
 * @list: a #VList.
 * @node: a #VListNode to remove.
 *
 * Removes @node from @list and free's it.
 */
void
v_list_remove (VList *list, VListNode *node)
{
	/* first list node */
	if (node->prev == NULL)
		list->first = node->next;
	
	else
		node->prev->next = node->next;
	
	
	/* last list node */
	if (node->next == NULL)
		list->last = node->prev;
	
	else
		node->next->prev = node->prev;
	
	
	/* free node */
	v_free (node);
	
	list->length--;
}


