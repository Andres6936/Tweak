#include <Tweak/tweak.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <Tweak/btree.h>

#ifdef TEST_BUFFER
#define BLKMIN 4
#else
#define BLKMIN 512
#endif

#define BLKMAX (2*BLKMIN)

struct file
{
	FILE* fp;
	int refcount;
};

struct buffer
{
	BinaryTree* bt;
};

struct BufferBlock
{
	fileoffset_t len;               /* number of bytes in block, always */
	struct file* file;               /* non-NULL indicates a file block */
	fileoffset_t filepos;           /* only meaningful if fp!=NULL */
	unsigned char* data;           /* only used if fp==NULL */
};

static ItemType BufferBlock_Copy(void* state, void* av)
{
	struct BufferBlock* a = (struct BufferBlock*)av;
	struct BufferBlock* ret;

	if (a->file)
	{
		ret = (struct BufferBlock*)malloc(sizeof(struct BufferBlock));
		ret->data = NULL;
		a->file->refcount++;
	}
	else
	{
		ret = (struct BufferBlock*)malloc(sizeof(struct BufferBlock) + BLKMAX);
		ret->data = (unsigned char*)(ret + 1);
		memcpy(ret->data, a->data, BLKMAX);
	}

	ret->file = a->file;
	ret->filepos = a->filepos;
	ret->len = a->len;

	return ret;
}

static void BufferBlock_Free(void* state, void* av)
{
	struct BufferBlock* a = (struct BufferBlock*)av;

	if (a->file)
	{
		a->file->refcount--;

		if (a->file->refcount == 0)
		{
			fclose(a->file->fp);
			free(a->file);
		}
	}

	free(a);
}

void bufblkpropmake(void* state, ItemType av, void* destv)
{
	struct BufferBlock* a = (struct BufferBlock*)av;
	fileoffset_t* dest = (fileoffset_t*)destv;

	*dest = a->len;
}

/* s1 may be NULL (indicating copy s2 into dest). s2 is never NULL. */
void bufblkpropmerge(void* state, void* s1v, void* s2v, void* destv)
{
	fileoffset_t* s1 = (fileoffset_t*)s1v;
	fileoffset_t* s2 = (fileoffset_t*)s2v;
	fileoffset_t* dest = (fileoffset_t*)destv;
	if (!s1 && !s2) return;           /* don't need to free anything */
	*dest = *s2 + (s1 ? *s1 : 0);
}

static buffer* buf_new_from_bt(BinaryTree* bt)
{
	buffer* buf = (buffer*)malloc(sizeof(buffer));

	buf->bt = bt;

	return buf;
}

static BinaryTree* buf_bt_new(void)
{
	return bt_new(NULL, BufferBlock_Copy, BufferBlock_Free, sizeof(fileoffset_t),
			alignof(fileoffset_t), bufblkpropmake, bufblkpropmerge,
			NULL, 2);
}

extern void buf_free(buffer* buf)
{
	bt_free(buf->bt);
	free(buf);
}

static int bufblksearch(void* tstate, void* sstate, int ntrees,
		void** props, int* counts,
		ItemType* elts, int* is_elt)
{
	fileoffset_t* disttogo = (fileoffset_t*)sstate;
	fileoffset_t distsofar = 0;
	int i;

	for (i = 0; i < ntrees; i++)
	{
		struct BufferBlock* blk;
		fileoffset_t sublen = props[i] ? *(fileoffset_t*)props[i] : 0;

		if ((props[i] && *disttogo < distsofar + sublen) ||
			(*disttogo == distsofar + sublen && i == ntrees - 1))
		{
			*disttogo -= distsofar;
			/*
			 * Descend into this subtree.
			 */
			*is_elt = FALSE;
			return i;
		}

		distsofar += sublen;

		if (i < ntrees - 1)
		{
			blk = (struct BufferBlock*)elts[i];

			if (*disttogo < distsofar + blk->len)
			{
				/*
				 * Select this element.
				 */
				*disttogo -= distsofar;
				*is_elt = TRUE;
				return i;
			}

			distsofar += blk->len;
		}
	}

	assert(!"We should never reach here");
	return 0;                   /* placate gcc */
}

static int buf_bt_find_pos(BinaryTree* bt, fileoffset_t pos, fileoffset_t* poswithin)
{
	int index;

	bt_propfind(bt, bufblksearch, &pos, &index);

	*poswithin = pos;
	return index;
}

/*
 * Convert a file-data block of size at most BUFMAX into a
 * literal-data block. Returns the replacement block (the old one
 * still needs freeing) or NULL if no conversion performed.
 */
static struct BufferBlock* buf_convert_to_literal(struct BufferBlock* blk)
{
	if (blk->file && blk->len <= BLKMAX)
	{
		struct BufferBlock* ret =
				(struct BufferBlock*)malloc(sizeof(struct BufferBlock) + BLKMAX);
		ret->data = (unsigned char*)(ret + 1);
		ret->file = NULL;
		ret->filepos = 0;
		ret->len = blk->len;
		fseeko(blk->file->fp, blk->filepos, SEEK_SET);
		fread(ret->data, blk->len, 1, blk->file->fp);

		return ret;
	}

	return NULL;
}

/*
 * Look at blocks `index' and `index+1' of buf. If they're both
 * literal-data blocks and one of them is undersized, merge or
 * redistribute. Returns 0 if it has not changed the number of
 * blocks, or 1 if it has merged two.
 */
static int buf_bt_cleanup(BinaryTree* bt, int index)
{
	struct BufferBlock* a, * b, * cvt;
	fileoffset_t totallen;
	unsigned char tmpdata[BLKMAX * 2];

	if (index < 0) return 0;

	a = (struct BufferBlock*)bt_index(bt, index);
	b = (struct BufferBlock*)bt_index(bt, index + 1);

	if (a && (cvt = buf_convert_to_literal(a)) != NULL)
	{
		bt_replace(bt, cvt, index);
		BufferBlock_Free(NULL, a);
		a = cvt;
	}

	if (b && (cvt = buf_convert_to_literal(b)) != NULL)
	{
		bt_replace(bt, cvt, index + 1);
		BufferBlock_Free(NULL, b);
		b = cvt;
	}

	if (!a || !b || a->file || b->file) return 0;

	if (a->len >= BLKMIN && b->len >= BLKMIN) return 0;

	assert(a->len <= BLKMAX && b->len <= BLKMAX);

	/* Use bt_index_w to ensure reference count of 1 on both blocks */
	a = (struct BufferBlock*)bt_index_w(bt, index);
	b = (struct BufferBlock*)bt_index_w(bt, index + 1);

	/*
	 * So, we have one block with size at most BLKMIN, and another
	 * with size at most BLKMAX. Combined, their maximum possible
	 * size is in excess of BLKMAX, so we can't guaranteeably merge
	 * them into one. If they won't merge, we instead redistribute
	 * data between them.
	 */
	totallen = a->len + b->len;
	memcpy(tmpdata, a->data, a->len);
	memcpy(tmpdata + a->len, b->data, b->len);

	if (totallen >= BLKMAX)
	{
		/*
		 * Redistribute into two (nearly) equal-sized blocks.
		 */
		a->len = totallen / 2;
		b->len = totallen - a->len;

		memcpy(a->data, tmpdata, a->len);
		memcpy(b->data, tmpdata + a->len, b->len);

		bt_replace(bt, a, index);
		bt_replace(bt, b, index + 1);

		return 0;
	}
	else
	{
		/*
		 * Just merge into one.
		 */
		a->len = totallen;
		memcpy(a->data, tmpdata, a->len);

		bt_replace(bt, a, index);
		free(bt_delpos(bt, index + 1));

		return 1;
	}
}

static int buf_bt_splitpoint(BinaryTree* bt, fileoffset_t pos)
{
	fileoffset_t poswithin;
	int index;
	struct BufferBlock* blk, * newblk;

	index = buf_bt_find_pos(bt, pos, &poswithin);

	if (!poswithin)
		return index;               /* the nice simple case */

	/*
	 * Now split element `index' at position `poswithin'.
	 */
	blk = (struct BufferBlock*)bt_index_w(bt, index);   /* ensure ref count == 1 */
	newblk = (struct BufferBlock*)BufferBlock_Copy(NULL, blk);

	if (!newblk->file)
	{
		memcpy(newblk->data, blk->data + poswithin, blk->len - poswithin);
	}
	else
	{
		newblk->filepos += poswithin;
	}
	blk->len = poswithin;
	bt_replace(bt, blk, index);
	newblk->len -= poswithin;
	bt_addpos(bt, newblk, index + 1);

	buf_bt_cleanup(bt, index + 1);
	index -= buf_bt_cleanup(bt, index - 1);

	return index + 1;
}

static BinaryTree* buf_bt_split(BinaryTree* bt, fileoffset_t pos, int before)
{
	int index = buf_bt_splitpoint(bt, pos);
	return bt_splitpos(bt, index, before);
}

static BinaryTree* buf_bt_join(BinaryTree* a, BinaryTree* b)
{
	int index = bt_count(a) - 1;
	BinaryTree* ret;

	ret = bt_join(a, b);

	buf_bt_cleanup(ret, index);

	return ret;
}

static void buf_insert_bt(buffer* buf, BinaryTree* bt, fileoffset_t pos)
{
	BinaryTree* right = buf_bt_split(buf->bt, pos, FALSE);
	buf->bt = buf_bt_join(buf->bt, bt);
	buf->bt = buf_bt_join(buf->bt, right);
}

static int bufblklensearch(void* tstate, void* sstate, int ntrees,
		void** props, int* counts,
		ItemType* elts, int* is_elt)
{
	fileoffset_t* output = (fileoffset_t*)sstate;
	fileoffset_t size = 0;
	int i;

	for (i = 0; i < ntrees; i++)
	{
		struct BufferBlock* blk;

		if (props[i])
			size += *(fileoffset_t*)props[i];

		if (i < ntrees - 1)
		{
			blk = (struct BufferBlock*)elts[i];

			size += blk->len;
		}
	}

	*output = size;

	/* Actual return value doesn't matter */
	*is_elt = TRUE;
	return 1;
}

static fileoffset_t buf_bt_length(BinaryTree* bt)
{
	fileoffset_t length;

	bt_propfind(bt, bufblklensearch, &length, NULL);

	return length;
}

extern fileoffset_t buf_length(buffer* buf)
{
	return buf_bt_length(buf->bt);
}

extern buffer* buf_new_empty(void)
{
	buffer* buf = (buffer*)malloc(sizeof(buffer));

	buf->bt = buf_bt_new();

	return buf;
}

extern buffer* buf_new_from_file(FILE* fp)
{
	buffer* buf = buf_new_empty();
	struct BufferBlock* blk;
	struct file* file;

	file = (struct file*)malloc(sizeof(struct file));
	file->fp = fp;
	file->refcount = 1;               /* the reference we're about to make */

	blk = (struct BufferBlock*)malloc(sizeof(struct BufferBlock));
	blk->data = NULL;
	blk->file = file;
	blk->filepos = 0;

	fseeko(fp, 0, SEEK_END);
	blk->len = ftello(fp);

	bt_addpos(buf->bt, blk, 0);

	buf_bt_cleanup(buf->bt, 0);

	return buf;
}

extern void buf_fetch_data(buffer* buf, void* vdata, int len, fileoffset_t pos)
{
	int index;
	fileoffset_t poswithin;
	fileoffset_t thislen;
	unsigned char* data = (unsigned char*)vdata;

	index = buf_bt_find_pos(buf->bt, pos, &poswithin);

	while (len > 0)
	{
		struct BufferBlock* blk = (struct BufferBlock*)bt_index(buf->bt, index);

		thislen = blk->len - poswithin;
		if (thislen > len)
			thislen = len;

		if (blk->file)
		{
			fseeko(blk->file->fp, blk->filepos + poswithin, SEEK_SET);
			fread(data, thislen, 1, blk->file->fp);
		}
		else
		{
			memcpy(data, blk->data + poswithin, thislen);
		}

		data += thislen;
		len -= thislen;

		poswithin = 0;

		index++;
	}
}

extern void buf_insert_data(buffer* buf, void* vdata, int len,
		fileoffset_t pos)
{
	BinaryTree* pBinaryTree = buf_bt_new();
	const int ORIGINAL_LENGTH = len;
	unsigned char* data = (unsigned char*)vdata;

	int numberBlocks = len / ((BLKMIN + BLKMAX) / 2);

	if (numberBlocks * BLKMAX < len)
	{
		numberBlocks++;
	}

	const int BLOCK_LENGTH = len / numberBlocks;
	const int EXTRA = len % numberBlocks;

	assert(BLOCK_LENGTH >= BLKMIN || numberBlocks == 1);
	assert(BLOCK_LENGTH <= BLKMAX - (EXTRA != 0));

	for (int i = 0; i < numberBlocks; i++)
	{
		struct BufferBlock* blk;
		int blklen = BLOCK_LENGTH + (i < EXTRA);

		blk = (struct BufferBlock*)malloc(sizeof(struct BufferBlock) + BLKMAX);
		blk->data = (unsigned char*)(blk + 1);
		memcpy(blk->data, data, blklen);
		blk->len = blklen;
		blk->file = NULL;
		blk->filepos = 0;

		data += blklen;
		len -= blklen;

		bt_addpos(pBinaryTree, blk, i);
		assert(ORIGINAL_LENGTH == buf_bt_length(pBinaryTree) + len);
	}

	assert(len == 0);
	assert(ORIGINAL_LENGTH == buf_bt_length(pBinaryTree));

	buf_insert_bt(buf, pBinaryTree, pos);
}

extern void buf_delete(buffer* buf, fileoffset_t len, fileoffset_t pos)
{
	BinaryTree* left = buf_bt_split(buf->bt, pos, TRUE);
	BinaryTree* right = buf_bt_split(buf->bt, len, FALSE);

	bt_free(buf->bt);

	buf->bt = buf_bt_join(left, right);
}

extern void buf_overwrite_data(buffer* buf, void* data, int len,
		fileoffset_t pos)
{
	buf_delete(buf, len, pos);
	buf_insert_data(buf, data, len, pos);
}

extern buffer* buf_cut(buffer* buf, fileoffset_t len, fileoffset_t pos)
{
	BinaryTree* left = buf_bt_split(buf->bt, pos, TRUE);
	BinaryTree* right = buf_bt_split(buf->bt, len, FALSE);
	BinaryTree* ret = buf->bt;

	buf->bt = buf_bt_join(left, right);

	return buf_new_from_bt(ret);
}

extern buffer* buf_copy(buffer* buf, fileoffset_t len, fileoffset_t pos)
{
	BinaryTree* left = buf_bt_split(buf->bt, pos, TRUE);
	BinaryTree* right = buf_bt_split(buf->bt, len, FALSE);
	BinaryTree* ret = bt_clone(buf->bt);

	buf->bt = buf_bt_join(left, buf->bt);
	buf->bt = buf_bt_join(buf->bt, right);

	return buf_new_from_bt(ret);
}

extern void buf_paste(buffer* buf, buffer* cutbuffer, fileoffset_t pos)
{
	BinaryTree* bt = bt_clone(cutbuffer->bt);
	buf_insert_bt(buf, bt, pos);
}

#ifdef TEST_BUFFER
static FILE *debugfp = NULL;
extern void buffer_diagnostic(buffer *buf, char *title)
{
	int i;
	fileoffset_t offset;
	struct BufferBlock *blk;

	if (!debugfp) {
	debugfp = fdopen(3, "w");
	if (!debugfp)
		debugfp = fopen("debug.log", "w");
	}

	if (!buf) {
	fprintf(debugfp, "Buffer [%s] is null\n", title);
	return;
	}

	fprintf(debugfp, "Listing of buffer [%s]:\n", title);
	offset = 0;
	for (i = 0; (blk = (struct BufferBlock *)bt_index(buf->bt, i)) != NULL; i++) {
	fprintf(debugfp, "%016"OFF"x: %p, len =%8"OFF"d,", offset, blk, blk->len);
	if (blk->file) {
		fprintf(debugfp, " file %p pos %8"OFF"d\n", blk->file, blk->filepos);
	} else {
		int j;

		for (j = 0; j < blk->len; j++)
		fprintf(debugfp, " %02x", blk->data[j]);

		fprintf(debugfp, "\n");
	}
	offset += blk->len;
	}
	fprintf(debugfp, "Listing concluded\n\n");

	fflush(debugfp);
}
#endif
