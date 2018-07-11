//Permission is hereby granted by Brigido Rodriguez to use as is without limitations.
#include "../header/octree.h"
#include <stdlib.h>
#include <stdint.h>
#include <memory>
#include <assert.h>

namespace
image_lib
	{
	namespace
		{
		#define ON_INHEAP	1

		typedef struct oct_node_t *oct_node;
		struct oct_node_t
			{
			//sum of all colors represented by this node. 64 bit in case of HUGE image
			uint64_t r, g, b;
			int count, heap_idx;
			uint8_t n_kids, kid_idx, flags, depth;
			oct_node kids[8], parent;
			};

		static oct_node pool = 0;

		struct node_heap
			{
			int alloc;
			int n;
			std::vector<oct_node> buf;
			node_heap(int aa, int nn):
				alloc(aa),
				n(nn)
				{
				buf.resize(1024);
				}
			};

		//cmp function that decides the ordering in the heap. This is how we determine which octree node to fold next, the heart of the algorithm.
		inline int cmp_node(oct_node a, oct_node b)
			{
			if (a->n_kids < b->n_kids) return -1;
			if (a->n_kids > b->n_kids) return 1;
			int ac = a->count >> a->depth;
			int bc = b->count >> b->depth;
			return ac < bc ? -1 : ac > bc;
			}

		void down_heap(std::shared_ptr<node_heap> & h, oct_node p)
			{
			int n = p->heap_idx, m;
			while (1)
				{
				m = n * 2;
				if (m >= h->n) break;
				if (m + 1 < h->n && cmp_node(h->buf[m], h->buf[m + 1]) > 0) m++; 
				if (cmp_node(p, h->buf[m]) <= 0) break;
 
				h->buf[n] = h->buf[m];
				h->buf[n]->heap_idx = n;
				n = m;
				}
			h->buf[n] = p;
			p->heap_idx = n;
			}

		void up_heap(std::shared_ptr<node_heap> & h, oct_node p)
			{
			int n = p->heap_idx;
			oct_node prev;
			while (n > 1)
				{
				prev = h->buf[n / 2];
				if (cmp_node(p, prev) >= 0) break;
 
				h->buf[n] = prev;
				prev->heap_idx = n;
				n /= 2;
				}
			h->buf[n] = p;
			p->heap_idx = n;
			}
 
		void heap_add(std::shared_ptr<node_heap> & h, oct_node p)
			{
			if ((p->flags & ON_INHEAP))
				{
				down_heap(h, p);
				up_heap(h, p);
				return;
				}
			p->flags |= ON_INHEAP;
			if (!h->n) h->n = 1;
			if (h->n >= h->alloc)
				{
				while (h->n >= h->alloc)
					h->alloc += 1024;

				h->buf.resize(h->alloc);
				}
			p->heap_idx = h->n;
			h->buf[h->n++] = p;
			up_heap(h, p);
			}
 
		oct_node pop_heap(std::shared_ptr<node_heap> & h)
			{
			if (h->n <= 1)
				return 0;

			oct_node ret = h->buf[1];
			h->buf[1] = h->buf[--h->n];
			h->buf[h->n] = 0;
			h->buf[1]->heap_idx = 1;
			down_heap(h, h->buf[1]);
			return ret;
			}
 
		oct_node node_new(uint32_t & length, uint8_t idx, uint8_t depth, oct_node p)
			{
			if (length <= 1)
				{
				oct_node p = (oct_node)calloc(sizeof(oct_node_t), 2048);
				p->parent = pool;
				pool = p;
				length = 2047;
				}
			oct_node x = pool + length--;
			x->kid_idx = idx;
			x->depth = depth;
			x->parent = p;
			if (p)
				p->n_kids++;

			return x;
			}
 
		void node_free()
			{
			oct_node p;
			while (pool)
				{
				p = pool->parent;
				free(pool);
				pool = p;
				}
			pool = 0;
			}

		//adding a color triple to octree
		oct_node node_insert(uint32_t & length, oct_node root, uint8_t * pix)
			{
			//OCT_DEPTH: number of significant bits used for tree.  It's probably good enough for most images to use a value of 5.
			//This affects how many nodes eventually end up in the tree and heap, thus smaller values helps with both speed and memory.
			uint8_t const OCT_DEPTH = 8;
			uint8_t i, bit, depth = 0;
			for (bit = 1 << 7; ++depth < OCT_DEPTH; bit >>= 1)
				{
				i = !!(pix[1] & bit) * 4 + !!(pix[0] & bit) * 2 + !!(pix[2] & bit);
				assert(i<8);
				if (!root->kids[i])
					root->kids[i] = node_new(length, i, depth, root);
 
				root = root->kids[i];
				}
			root->r += pix[0];
			root->g += pix[1];
			root->b += pix[2];
			root->count++;
			return root;
			}

		//remove a node in octree and add its count and colors to parent node.
		oct_node node_fold(oct_node p)
			{
			if (p->n_kids)
				abort();

			oct_node q = p->parent;
			q->count += p->count;
			q->r += p->r;
			q->g += p->g;
			q->b += p->b;
			q->n_kids --;
			q->kids[p->kid_idx] = 0;
			return q;
			}

		//traverse the octree just like construction, but this time we replace the pixel color with color stored in the tree node
		void color_replace(std::vector<uint32_t> & pal, oct_node root, uint8_t * pix, uint8_t * out)
			{
			uint8_t i, bit;
			for (bit = 1 << 7; bit; bit >>= 1)
				{
				i = !!(pix[1] & bit) * 4 + !!(pix[0] & bit) * 2 + !!(pix[2] & bit);
				if (!root->kids[i])
					break;

				root = root->kids[i];
				}
			out[0] = (uint8_t)root->r;
			out[1] = (uint8_t)root->g;
			out[2] = (uint8_t)root->b;

			uint32_t add = 0;
			if(pix[3]>127)
				add = ((out[0] << 0) | (out[1] << 8) | (out[2] << 16) | (255 << 24));
			else
				out[3] = add;

			if(add)
				{
				bool found = false;
				uint8_t index = 0;
				for(std::vector<uint32_t>::iterator i=pal.begin(), e=pal.end(); i!=e; index++, i++)
					{
					if(add == *i)
						{
						found = true;
						break;
						}
					}
				assert(index<=256);
				out[3] = index;
				if(!found)
					pal.push_back(add);
				}
			}
		}

	//Building an octree and keep leaf nodes in a bin heap. Afterwards remove first node in heap and fold it
	//into its parent node (which may now be added to heap), until heap contains required number of colors.
	//std::vector<uint32_t> color_quant(std::vector<uint8_t> & pixels, int n_colors)
	std::vector<unsigned char> color_quant(std::vector<unsigned char> & pixels, std::vector<unsigned> & palette, int n_colors)
		{
		int colors = n_colors-1;
		pool = 0;
		uint32_t length = 0;
		std::shared_ptr<node_heap> heap(new node_heap(0, 0));
		oct_node root = node_new(length, 0, 0, 0);
		oct_node got;
		for(std::vector<uint8_t>::const_iterator i=pixels.begin(), e=pixels.end(); i<e; i+=4)
			heap_add(heap, node_insert(length, root, i._Ptr));
 
		while (heap->n > colors + 1)
			heap_add(heap, node_fold(pop_heap(heap)));
 
		double c;
		for (int i = 1; i < heap->n; i++)
			{
			got = heap->buf[i];
			c = got->count;
			got->r = (uint64_t)(got->r / c + .5);
			got->g = (uint64_t)(got->g / c + .5);
			got->b = (uint64_t)(got->b / c + .5);
			}
		std::vector<uint8_t> out(pixels.size(), 0);
		palette.reserve(colors+1);
		palette.push_back(0);
		for(std::vector<uint8_t>::iterator i=pixels.begin(), j=out.begin(), e=pixels.end(); i<e; i+=4, j+=4)
			color_replace(palette, root, i._Ptr, j._Ptr);

		while(palette.size()<=(unsigned)colors)
			palette.push_back(0);

		assert(palette.size()==n_colors);
		node_free();
		return out;
		} 
	}
