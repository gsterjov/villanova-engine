/***************************************************************************
 *            list.h
 *
 *  Sun Dec 14 13:57:56 2008
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
 
#ifndef V_LIST_H_
#define V_LIST_H_



typedef struct _VList     VList;
typedef struct _VListNode VListNode;



/**
 * VList:
 * @length: the amount of nodes in the list.
 * @first: the first node in the list.
 * @last: the last node in the list.
 *
 * A basic linked list.
 */
struct _VList
{
	unsigned int length;
	
	VListNode *first;
	VListNode *last;
};



/**
 * VListNode:
 * @prev: the previous node on the list.
 * @next: the next node on the list.
 * @data: the data within the list node.
 *
 * A linked list node which stores data and holds a reference to the next and
 * previous nodes on the list.
 */
struct _VListNode
{
	VListNode *prev;
	VListNode *next;
	
	void *data;
};




VList *v_list_new  (void);
void   v_list_free (VList *list);


void v_list_append (VList *list, void *data);
void v_list_remove (VList *list, VListNode *node);


#endif /* V_LIST_H_ */
 
