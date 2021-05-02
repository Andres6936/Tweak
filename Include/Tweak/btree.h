/*
 * Flexible B-tree implementation. Supports reference counting for
 * copy-on-write, user-defined node properties, and variable
 * degree.
 * 
 * This file is copyright 2001,2004 Simon Tatham.
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL SIMON TATHAM BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef BTREE_H
#define BTREE_H

#include <stddef.h> /* for offsetof */

#ifndef alignof
#define alignof(typ) ( offsetof(struct { char c; typ t; }, t) )
#endif

typedef struct BinaryTree BinaryTree;
typedef void* ItemType;

typedef int (* cmpfn_t)(void* state, ItemType, ItemType);

typedef ItemType (* copyfn_t)(void* state, ItemType);

typedef void (* freefn_t)(void* state, ItemType);

typedef void (* propmakefn_t)(void* state, ItemType, void* dest);

/* s1 may be NULL (indicating copy s2 into dest). s2 is never NULL. */
typedef void (* propmergefn_t)(void* state, void* s1, void* s2, void* dest);

typedef int (* searchfn_t)(void* tstate, void* sstate, int ntrees,
		void** props, int* counts,
		ItemType* elts, int* is_elt);

enum
{
	BT_REL_EQ, BT_REL_LT, BT_REL_LE, BT_REL_GT, BT_REL_GE
};

BinaryTree* bt_new(cmpfn_t cmp, copyfn_t copy, freefn_t freeelt,
		int propsize, int propalign, propmakefn_t propmake,
		propmergefn_t propmerge, void* state, int mindegree);

void bt_free(BinaryTree* bt);

BinaryTree* bt_clone(BinaryTree* bt);

int bt_count(BinaryTree* bt);

ItemType bt_index(BinaryTree* bt, int index);

ItemType bt_index_w(BinaryTree* bt, int index);

ItemType bt_findrelpos(BinaryTree* bt, ItemType element, cmpfn_t cmp,
		int relation, int* index);

ItemType bt_findrel(BinaryTree* bt, ItemType element, cmpfn_t cmp,
		int relation);

ItemType bt_findpos(BinaryTree* bt, ItemType element, cmpfn_t cmp,
		int* index);

ItemType bt_find(BinaryTree* bt, ItemType element, cmpfn_t cmp);

ItemType bt_propfind(BinaryTree* bt, searchfn_t search, void* sstate,
		int* index);

ItemType bt_replace(BinaryTree* bt, ItemType element, int index);

void bt_addpos(BinaryTree* bt, ItemType element, int pos);

ItemType bt_add(BinaryTree* bt, ItemType element);

ItemType bt_delpos(BinaryTree* bt, int pos);

ItemType bt_del(BinaryTree* bt, ItemType element);

BinaryTree* bt_join(BinaryTree* bt1, BinaryTree* bt2);

BinaryTree* bt_joinr(BinaryTree* bt1, BinaryTree* bt2);

BinaryTree* bt_splitpos(BinaryTree* bt, int index, int before);

BinaryTree* bt_split(BinaryTree* bt, ItemType element, cmpfn_t cmp, int rel);

#endif /* BTREE_H */
