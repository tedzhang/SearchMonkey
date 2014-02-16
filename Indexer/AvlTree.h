
#pragma  once 

#include "../include/MemoryPool/BaseAllocator.h"

typedef int		(*AtCmp)( void* p1, void* p2);
/*----------------------------------------------------------------------*
 * Tree									*
 *----------------------------------------------------------------------*/
# define AVLTreeNoCopyKeys      0
# define AVLTreeCopyKeys        1
# define AVLTreeNoKeySize       0

# define AVLTreeNoCompFun       ((void*)0)
# define AVLTreeNullData        ((void*)0)

enum
{
	AVLPreWalk=0, AVLInWalk=1, AVLPostWalk=2
};

enum
{
	LeftHigh=-1, EqualHigh=0, RightHigh=1
};

# define PARENT(t)	(t)->an_parent
# define LEFT(t)	(t)->an_left
# define RIGHT(t)	(t)->an_right
# define LEAF(n)	(!LEFT((n)) && !RIGHT((n)))
# define ORPHAN(n)	(!PARENT((n)))
# define NodeIsLeftChild(x)  ((x)&&PARENT((x))&&(x)==LEFT(PARENT((x))))
# define NodeIsRightChild(x) ((x)&&PARENT((x))&&(x)==RIGHT(PARENT((x))))


# define CUTOFF(n)	(n)->an_right=(n)->an_left=(n)->an_parent=NULL

enum
{
	LEFTCHILD=-1, RIGHTCHILD=1
};



template<class KEY, class DATA>
class CAvlNode:public CBaseAlloc
{
public:
	CAvlNode()
	{
		an_left=NULL;
		an_parent=NULL;
		an_right=NULL;
		
		m_nHigh=EqualHigh;
		m_pData=NULL;
		m_pKey=NULL;
	};
	~CAvlNode()
	{
		if( m_pKey != NULL )
		{
			delete m_pKey;
		}
		if( m_pData != NULL )
		{
			delete m_pData;
		}
	};

	inline KEY*	getKey( )
	{
		return m_pKey;
	};
	inline DATA* getData()
	{
		return m_pData;
	};

	//删除当前节点的值
	inline void	avlCut()
	{
		if( m_pData != NULL )
		{
			delete m_pData;
		}
		m_pData=NULL;
	};

public:
	KEY*	m_pKey;			//关键字
	DATA*	m_pData;	    // What is stored in the tree 
	int		m_nHigh;	    // One of left, equal, right
    CAvlNode*	an_parent;	//父节点
    CAvlNode*	an_left;	//左节点
    CAvlNode*	an_right;	//右节点

};

template<class KEY, class DATA>
class CAvlTree
{
public:
	CAvlTree(AtCmp pFunc)
	{
		m_keyComp= pFunc;
		m_pRoot= NULL;
		m_pCurrent=NULL;
		m_nNodes=0;
	};
	~CAvlTree()
	{
		avl_close_node(m_pRoot);
	};
private:
    int			m_nNodes;		//节点个数
	AtCmp		m_keyComp;		//比较函数

    CAvlNode<KEY,DATA>*	m_pRoot;		//根节点
    CAvlNode<KEY,DATA>*	m_pCurrent;		//当前节点

public:
	//添加一个节点信息
	int	addNode(KEY*& pKey, DATA*& pData)
	{
		CAvlNode<KEY,DATA>*	new_root=NULL;
		int 		grew = 0;

		new_root = avl_insert( m_pRoot, pKey, pData, &grew);
		if (!new_root)
		{
			return -1;
		}
		pKey=NULL;
		pData=NULL;
		m_pRoot= new_root;
		m_nNodes++;
		return 0;
	};

	//获取一个节点信息
	inline DATA* getNode(KEY* pKey)
	{
		m_pCurrent= avl_find_node( m_pRoot,pKey);
		if( m_pCurrent == NULL )
		{
			return NULL;
		}
		return m_pCurrent->m_pData;
	};

	inline KEY* getNodeKey(KEY* pKey)
	{
		m_pCurrent= avl_find_node(m_pRoot, pKey );
		if( m_pCurrent == NULL )
		{
			return NULL;
		}
		return m_pCurrent->m_pKey;
	};

	//删除一个节点信息
	inline int	rmNode(KEY* pKey)
	{
		CAvlNode<KEY,DATA>* pNode= avl_find_node(m_pRoot, pKey);
		if( pNode != NULL )
		{
			return avlRemoveNode(pNode);
		}
		return -1;
	};
	//获取当前节点数据,关键字信息
	inline int	getCurrentNode( KEY*& pKey, DATA*& pData)
	{
		if( m_pCurrent != NULL )
		{
			pKey=	m_pCurrent->m_pKey;
			pData=	m_pCurrent->m_pData;
			return 0;
		}
		return -1;
	};
	//删除当前节点, Flag= 0. Current=Prev, 其他 Current=Next
	inline int rmCurrentNode(int nFlag)
	{
		if( m_pCurrent != NULL )
		{
			CAvlNode<KEY, DATA>* pNode= m_pCurrent;
			if( nFlag )
			{
				m_pCurrent= avlNextNode(m_pCurrent);
			}
			else
			{
				m_pCurrent= avlPrevNode(m_pCurrent);
			}
			return avlRemoveNode(pNode);
		}
		return -1;
	}
	//获取当前节点数据信息
	inline DATA* getCurrentData()
	{
		if( m_pCurrent != NULL )
		{
			return 	m_pCurrent->m_pData;
		}
		return NULL;
	};
	//设置当间数据节点信息
	inline int setCurrentData(DATA* pData)
	{
		if( m_pCurrent != NULL )
		{
			delete m_pCurrent->m_pData;
			m_pCurrent->m_pData=pData;
			return 0;
		}
		return -1;
	};
	//得到最小的数据节点信息
	inline int getMinNode(KEY*& pKey, DATA*& pData)
	{
		CAvlNode<KEY,DATA>* pNode=	avl_minimum_node(m_pRoot);
		if( pNode != NULL )
		{
			pKey=	pNode->m_pKey;
			pData=	pNode->m_pData;
			return 0;
		}
		return -1;
	};

	//得到最大的数据节点信息
	inline int getMaxNode(KEY*& pKey, DATA*& pData)
	{
		CAvlNode<KEY,DATA>* pNode=	avl_maximum_node(m_pRoot );
		if( pNode != NULL )
		{
			pKey=	pNode->m_pKey;
			pData=	pNode->m_pData;
			return 0;
		}
		return -1;
	};

	//获取当前节点的前在节点信息
	inline int getPrevNode(KEY*& pKey, DATA*& pData)
	{
		CAvlNode<KEY,DATA>* pNode=	avlPrevNode( m_pCurrent );
		if( pNode != NULL )
		{
			pKey=	pNode->m_pKey;
			pData=	pNode->m_pData;
			return 0;
		}
		return -1;
	};

	//获取当前节点的后续节点信息
	inline int	getNextNode(KEY*& pKey, DATA*& pData)
	{
		CAvlNode<KEY,DATA>* pNode=	avlNextNode( m_pCurrent );
		if( pNode != NULL )
		{
			pKey=	pNode->m_pKey;
			pData=	pNode->m_pData;
			return 0;
		}
		return -1;
	}
	
	//得到高度
	inline int	getHigh( )
	{
		return avlRealTreeHeight( m_pRoot );
	};

	//得到节点个数
	inline int	getNodeCount( )
	{
		return m_nNodes;
	};

	//检查AVL树是否正确
	inline int	avlCheck()
	{
		return avl_check_node( m_pRoot,0 );
	}


private:
	//递归调用的方法
	inline void avl_close_node( CAvlNode<KEY,DATA>* pNode )
	{
		if( !pNode )
		{
			return;
		}

		if ( LEFT(pNode) )			/* Go into right subtree	*/
		{
			avl_close_node( LEFT(pNode) );
		}
		if ( RIGHT(pNode) )
		{
			avl_close_node( RIGHT(pNode) );
		}
		delete pNode;
		return;
	};
	inline void avl_close_nodedata(CAvlNode<KEY,DATA>* pNode)
	{
		if( !pNode )
		{
			return;
		}

		if (LEFT(pNode))			/* Go into right subtree	*/
		{
			avl_close_nodedata( LEFT(pNode) );
		}
		if (RIGHT(pNode))
		{
			avl_close_nodedata( RIGHT(pNode) );
		}
		if( pNode->m_pData != NULL )
		{
			delete pNode->m_pData;
		}
		pNode->m_pData=NULL;
		return;
	};

	//插入到节点
	inline CAvlNode<KEY,DATA>* avl_insert( CAvlNode<KEY,DATA> *pNode, KEY* pKey, DATA *pData, int *grew)
	{
		CAvlNode<KEY,DATA> * tmp;
		if (!pNode)
		{
			pNode = new CAvlNode<KEY,DATA>;
			if ( pNode == NULL )
			{ 
				return NULL; 
			}
			pNode->m_pKey=		pKey;
			pNode->m_pData =	pData;
			*grew = 1;
			m_pCurrent = pNode;
			return pNode;
		}
		int nCmp=	m_keyComp(pKey, pNode->m_pKey );

		if (nCmp < 0)	    /* Node key is smaller than m_pRoot */
		{
			tmp = avl_insert( LEFT(pNode), pKey, pData, grew);
			if (!tmp)
			{ 
				return NULL; 
			}

			/* Mark the parent of the new left node */
			LEFT(pNode) = tmp;
			PARENT(LEFT(pNode)) = pNode;

			if (*grew)	   /* Left tree grew in size */
			{
				//需要调整树的结构
				if (pNode->m_nHigh == LeftHigh)
				{ 
					/* Root needs to be left-balanced */
					if (! (pNode = avl_left_balance(pNode)) )
					{ 
						return NULL; 
					}
					*grew = 0;
				}
				else if (pNode->m_nHigh == EqualHigh)
				{
					pNode->m_nHigh = LeftHigh;
				}
				else
				{
					/* Root was RightHigh, since insertion happened in the
					* left subtree this means this tree did not grow in
					* size (of course the left subtree did grow in size)
					* */
					pNode->m_nHigh = EqualHigh;
					*grew = 0;
				}
			}
			return pNode;
		}
		else if (nCmp > 0)
		{
			tmp = avl_insert( RIGHT(pNode), pKey, pData,grew);
			if (!tmp)
			{ 
				return NULL; 
			}

			RIGHT(pNode) = tmp;
			PARENT(RIGHT(pNode)) = pNode;
			if (*grew)
			{
				if (pNode->m_nHigh == LeftHigh)
				{
					pNode->m_nHigh = EqualHigh;
					*grew = 0;
				}
				else if (pNode->m_nHigh == EqualHigh)
				{
					pNode->m_nHigh = RightHigh;
				}
				//需要调整树的结构
				else
				{
					if (! (pNode = avl_right_balance(pNode)) )
					{ 
						return NULL; 
					}
					*grew = 0;
				}
			}
			return pNode;
		}

		//重复键值
		return NULL;
	};

	//查找节点
	inline CAvlNode<KEY,DATA>*	avl_find_node(CAvlNode<KEY,DATA>* pNode, KEY* pKey)
	{
		if (!pNode)
		{ 
			return NULL; 
		}
		int nCmp= m_keyComp(pKey, pNode->m_pKey );
		if( nCmp < 0 )
		{
			return avl_find_node( LEFT(pNode), pKey);
		}
		else if( nCmp > 0 )
		{
			return avl_find_node( RIGHT(pNode), pKey);
		}
		else
		{
			return pNode;
		}
		return NULL;
	};


	//某一个最小节点
	inline CAvlNode<KEY,DATA>*	avl_minimum_node(CAvlNode<KEY,DATA>* pNode)
	{
		if (!pNode)
		{ 
			return NULL; 
		}

		while (LEFT(pNode))
		{
			pNode = LEFT(pNode);
		}
		return (m_pCurrent = pNode );
	};


	//某一个最大节点
	inline CAvlNode<KEY,DATA>*	avl_maximum_node(CAvlNode<KEY,DATA>* pNode)
	{
		if (!pNode)
		{ 
			return NULL; 
		}

		while (RIGHT(pNode))
		{
			pNode = RIGHT(pNode);
		}

		return pNode;
	};

	//节点的最大高度
	inline int			avlRealTreeHeight(CAvlNode<KEY,DATA>* pNode)
	{
		int n = 0, l = 0, r = 0;

		if (!pNode)
		{
			return 0;
		}
		if (pNode)
		{
			n++;
		}
		if (LEFT(pNode))
		{
			l = avlRealTreeHeight( LEFT(pNode));
		}
		if (RIGHT(pNode))
		{
			r = avlRealTreeHeight(RIGHT(pNode));
		}
		//挑选左右节点中最大的一个
		n += l > r ? l : r;
		return n;
	};

	//左旋
	inline static CAvlNode<KEY,DATA>*	avl_left_rotate(CAvlNode<KEY,DATA>* pNode)
	{
		CAvlNode<KEY,DATA> * node;
		if (!pNode || !RIGHT(pNode) )
		{ 
			return NULL;
		}
		node = RIGHT(pNode);
		RIGHT(pNode) = LEFT(node);
		if (LEFT(node))
		{
			PARENT(LEFT(node)) = pNode;
		}
		LEFT(node) = pNode;
		PARENT(node) = PARENT(pNode);
		PARENT(pNode) = node;
		return node;
	};

	//右旋
	inline static CAvlNode<KEY,DATA>*	avl_right_rotate(CAvlNode<KEY,DATA>* pNode)
	{
		CAvlNode<KEY,DATA> * node;

		if (!pNode || ! LEFT(pNode))
		{ 
			return NULL; 
		}

		node = LEFT(pNode);

		LEFT(pNode) = RIGHT(node);

		if (RIGHT(node))
		{
			PARENT(RIGHT(node)) = pNode;
		}

		RIGHT(node) = pNode;

		PARENT(node) = PARENT(pNode);
		PARENT(pNode) = node;

		return node;
	};

	//右边平衡
	inline static CAvlNode<KEY,DATA>*	avl_right_balance(CAvlNode<KEY,DATA>* pNode)
	{
		CAvlNode<KEY,DATA> * right;

		if (!pNode || !RIGHT(pNode))
		{ 
			return NULL; 
		}

		right = RIGHT(pNode);

		if (right->m_nHigh == RightHigh)
		{
			pNode->m_nHigh = EqualHigh;
			right->m_nHigh= EqualHigh;

			return avl_left_rotate(pNode);
		}
		else if (right->m_nHigh == LeftHigh)
		{
			CAvlNode<KEY,DATA> * w = LEFT(right);

			if (w->m_nHigh    == LeftHigh)
			{
				w->m_nHigh    = EqualHigh;
				pNode->m_nHigh = EqualHigh;
				right->m_nHigh= RightHigh;
			}
			else if (w->m_nHigh == RightHigh)
			{
				w->m_nHigh    = EqualHigh;
				pNode->m_nHigh = LeftHigh;
				right->m_nHigh= EqualHigh;
			}
			else
			{
				pNode->m_nHigh = EqualHigh;
				right->m_nHigh= EqualHigh;
			}

			RIGHT(pNode) = avl_right_rotate(right);
			w = avl_left_rotate(pNode);
			//根节点
			return w;
		}
		return pNode;
	};

	//左边平衡
	inline static CAvlNode<KEY,DATA>*	avl_left_balance(CAvlNode<KEY,DATA>* pNode)
	{
		CAvlNode<KEY,DATA>* left;

		if (!pNode || !LEFT(pNode))
		{ 
			return NULL; 
		}

		left = LEFT(pNode);

		if (left->m_nHigh == LeftHigh)
		{
			left->m_nHigh = EqualHigh;
			pNode->m_nHigh = EqualHigh;

			return avl_right_rotate(pNode);
		}
		else if (left->m_nHigh == RightHigh)
		{
			CAvlNode<KEY,DATA> * w = RIGHT(left);

			if (w->m_nHigh    == LeftHigh)
			{
				w->m_nHigh    = EqualHigh;
				pNode->m_nHigh = RightHigh;
				left->m_nHigh = EqualHigh;
			}
			else if (w->m_nHigh == RightHigh)
			{
				w->m_nHigh    = EqualHigh;
				pNode->m_nHigh = EqualHigh;
				left->m_nHigh = LeftHigh;
			}
			else
			{
				pNode->m_nHigh = EqualHigh;
				left->m_nHigh = EqualHigh;
			}

			LEFT(pNode) = avl_left_rotate(left);
			w = avl_right_rotate(pNode);

			// w has become the new m_pRoot 
			return w;
		}

		return pNode;
	};

	//删除一个节点
	inline CAvlNode<KEY, DATA>*	avl_rem_fix_up_right(CAvlNode<KEY, DATA> * p)
	{
		CAvlNode<KEY, DATA> * 	pp;
		int		l_child,	r_child;

		if (!p)
		{ 
			return p; 
		}

		pp 	    = PARENT(p);
		l_child = NodeIsLeftChild(p);
		r_child = NodeIsRightChild(p);

		switch (p->m_nHigh)
		{
		case LeftHigh:		/* Case 1 : shorter */
			{
				p->m_nHigh = EqualHigh;
				if (l_child) 	  
				{
					return avl_rem_fix_up_right(pp);
				}
				else if (r_child) 
				{
					return avl_rem_fix_up_left (pp);
				}
				else
				{
					return p;
				}
			}
			break;

		case EqualHigh:		/* Case 2 : not shorter */
			{
				p->m_nHigh = RightHigh;
				return p;
			}
			break;

		case RightHigh:		/* Case 3 */
			{
				CAvlNode<KEY, DATA> * q = RIGHT(p);
				if (!q) 
				{ 
					return NULL; 
				}

				if (q->m_nHigh == EqualHigh) /* Case 3a : not shorter */
				{
					//					DBG(printf("Case 3a\n"));

					p->m_nHigh = RightHigh;
					q->m_nHigh =  LeftHigh;

					p = avl_left_rotate(p);

					if (l_child)	
					{
						LEFT(pp) = p;
					}
					else if (r_child)	
					{
						RIGHT(pp)= p;
					}
					return p;
				}
				else if (q->m_nHigh == RightHigh) /* Case 3b : shorter */
				{
					p->m_nHigh = EqualHigh;
					q->m_nHigh = EqualHigh;

					p = avl_left_rotate(p);

					if (l_child)
					{
						LEFT(pp) = p;
						return avl_rem_fix_up_right(pp);
					}
					else if (r_child)
					{
						RIGHT(pp)= p;
						return avl_rem_fix_up_left (pp);
					}
					return p;
				}
				else		/* Case 3c : shorter */
				{
					CAvlNode<KEY, DATA> * r = LEFT(q);

					//					DBG(printf("Case 3c\n"));
					if (!r) 
					{ 
						return NULL; 
					}

					switch (r->m_nHigh)
					{
					case LeftHigh:
						{
							p->m_nHigh = EqualHigh;
							q->m_nHigh = RightHigh;
						}
						break;

					case EqualHigh:
						{
							p->m_nHigh = EqualHigh;
							q->m_nHigh = EqualHigh;
						}
						break;

					case RightHigh:
						{
							p->m_nHigh =  LeftHigh;
							q->m_nHigh = EqualHigh;
						}
						break;
					default:
						{
							break;
						}
					}

					r->m_nHigh = EqualHigh;

					r = avl_right_rotate(q);
					RIGHT(p) = r;
					r =  avl_left_rotate(p);

					if (l_child)
					{
						LEFT(pp) = r;
					}
					else if (r_child)
					{
						RIGHT(pp)= r;
					}
					else
					{
					}

					if (NodeIsLeftChild(r))
					{
						LEFT(PARENT(r)) = r;
						return avl_rem_fix_up_right(PARENT(r));
					}
					else if (NodeIsRightChild(r))
					{
						RIGHT(PARENT(r))= r;
						return avl_rem_fix_up_left (PARENT(r));
					}
					return r;
				}
			}
			break;
		default:
			{
			}
			break;
		}


		return NULL;
	}


	inline CAvlNode<KEY, DATA>*	avl_rem_fix_up_left(CAvlNode<KEY, DATA> * p)
	{
		CAvlNode<KEY, DATA> * 	pp;
		int		l_child,r_child;

		if (!p)
			return p;

		pp 	    = PARENT(p);
		l_child = NodeIsLeftChild(p);
		r_child = NodeIsRightChild(p);


		switch (p->m_nHigh)
		{
		case RightHigh:		/* Case 1 : shorter */
			{
				p->m_nHigh = EqualHigh;
				if (l_child) 	 
				{
					return avl_rem_fix_up_right(pp);
				}
				else if (r_child) 
				{
					return avl_rem_fix_up_left (pp);
				}
			}
			return p;

		case EqualHigh:		/* Case 2 : not shorter */
			{
				p->m_nHigh = LeftHigh;
			}
			return p;

		case LeftHigh:		/* Case 3 */
			{
				CAvlNode<KEY, DATA> * q = LEFT(p);

				if (!q) 
				{ 
					return NULL; 
				}

				if (q->m_nHigh == EqualHigh) /* Case 3a : not shorter */
				{
					p->m_nHigh =  LeftHigh;
					q->m_nHigh = RightHigh;

					p = avl_right_rotate(p);

					if (l_child)
					{
						LEFT(pp) = p;
					}
					else if (r_child)
					{
						RIGHT(pp)= p;
					}
					else
					{

					}
					return p;
				}
				else if (q->m_nHigh == LeftHigh) /* Case 3b : shorter */
				{
					CAvlNode<KEY, DATA> * parent = PARENT(p);
					p->m_nHigh = EqualHigh;
					q->m_nHigh = EqualHigh;

					p = avl_right_rotate(p);

					if (l_child)
					{
						LEFT(parent) = p;
						return avl_rem_fix_up_right(parent);
					}
					else if (r_child)
					{
						RIGHT(parent)= p;
						return avl_rem_fix_up_left (parent);
					}
					return p;
				}
				else		/* Case 3c : shorter */
				{
					CAvlNode<KEY, DATA> * r = RIGHT(q);

					if (!r) 
					{ 
						return NULL; 
					}

					switch (r->m_nHigh)
					{
					case LeftHigh:
						{
							p->m_nHigh = RightHigh;
							q->m_nHigh = EqualHigh;
						}
						break;

					case EqualHigh:
						{
							p->m_nHigh = EqualHigh;
							q->m_nHigh = EqualHigh;
						}
						break;

					case RightHigh:
						{
							p->m_nHigh = EqualHigh;
							q->m_nHigh =  LeftHigh;
						}
						break;
					default:
						{

						}
						break;
					}

					r->m_nHigh = EqualHigh;

					LEFT(p) = avl_left_rotate( q);
					r 	    = avl_right_rotate(p);

					/* Update parent pointer */

					if (l_child)	
					{
						LEFT(pp) = r;
					}
					else if (r_child)	
					{
						RIGHT(pp)= r;
					}

					if (NodeIsLeftChild(r))
					{
						LEFT(PARENT(r)) = r;
						return avl_rem_fix_up_right(PARENT(r));
					}
					else if (NodeIsRightChild(r))
					{
						RIGHT(PARENT(r))= r;
						return avl_rem_fix_up_left (PARENT(r));
					}
					else
					{

					}
					/* r is now the new root of the tree */
					return r;
				}
			}
		default:
			{

			}
			break;
		}

		return NULL;
	}



	inline int avl_remove_node(CAvlNode<KEY,DATA>* pNode)
	{
		CAvlNode<KEY,DATA> * parent;
		int	      l_child,r_child;
		CAvlNode<KEY,DATA> * pTreeRoot=	m_pRoot;


		if (!pNode)
		{ 
			return -1; 
		}

# define BIND(node) 	\
	if (l_child) \
		{\
		LEFT(parent) = (node);\
		avl_rem_fix_up_right(parent);\
		}\
		else if (r_child)\
		{\
		RIGHT(parent)= (node);\
		parent = avl_rem_fix_up_left(parent);\
		}\
		else			/* Node has no parent nor children */\
		{\
		printf("DEL: m_pRoot node\n");\
		pTreeRoot = (node);\
		}

		/* Case 0: node has both a LEFT and a RIGHT child */
		if (LEFT(pNode) && RIGHT(pNode))
		{
			CAvlNode<KEY,DATA> * pred = avlPrevNode(pNode);
			if (!pred)
			{ 
				printf("AVL TREE INTERNAL ERROR\n");
				return -1; 
			}
			if (avl_copy_nodes(pNode,pred))
			{ 
				return -1; 
			}

			/* Replace node's data by prev's data in the tree */
			pNode = pred;
		}
		//删除后续节点

		l_child = NodeIsLeftChild(pNode);
		r_child = NodeIsRightChild(pNode);
		parent  = PARENT(pNode);

		/* Case 1: node is a LEAF */
		//叶子节点
		if (LEAF(pNode))
		{
			BIND(NULL);
			CUTOFF(pNode);
		}
		//右节点为空
		else if (LEFT(pNode) && !RIGHT(pNode))
		{	
			/* Case 2: node has only a LEFT child */

			PARENT(LEFT(pNode)) = parent;
			BIND( LEFT(pNode));
			CUTOFF(pNode);
		}
		//左节点为空
		else if (RIGHT(pNode) && !LEFT(pNode))
		{
			/* Case 3: node has only a RIGHT child */
			PARENT(RIGHT(pNode)) = parent;
			BIND(  RIGHT(pNode));
			CUTOFF(pNode);
		}
		else
		{ 
			printf("AVL TREE INTERNAL ERROR\n");
			return -1; 
		}

		/* if avlRemoveFix...() went all the way up to the m_pRoot, and
		* rotated it out of position, then it will have a parent. Go up
		* the tree until we find the new real m_pRoot and return it */

		if (pTreeRoot)				/* If any nodes left in tree	*/
		{
			while (PARENT(pTreeRoot))
				pTreeRoot = PARENT(pTreeRoot);
		}
		m_pRoot = pTreeRoot;

		//释放内存空间
		delete pNode;
		return 0;
	};

	//查找一个前序节点
	inline CAvlNode<KEY,DATA>*	avlPrevNode( CAvlNode<KEY,DATA>* node)
	{
		if (LEFT(node))
		{
			return m_pCurrent = avl_maximum_node(LEFT(node));
		}
		//对于父亲节点不为空和或者是本省为该节点的左节点的情况
		while (PARENT(node) && node == LEFT(PARENT(node)))
		{
			node = PARENT(node);
		}

		return m_pCurrent = PARENT(node);
	};

	//获取后续节点
	inline CAvlNode<KEY,DATA>*	avlNextNode( CAvlNode<KEY,DATA>* node )
	{
		if (RIGHT(node))
		{
			return (m_pCurrent = avl_minimum_node( RIGHT(node)) );
		}

		while (PARENT(node) && node == RIGHT(PARENT(node)))
		{
			node = PARENT(node);
		}

		return (m_pCurrent = PARENT(node) );
	};

	//检查节点的完整性
	inline int	avl_check_node(CAvlNode<KEY,DATA> *node,int depth)
	{
		int left_h = -1,right_h = -1;
		int has_left = 0;
		int has_right= 0;

		if (LEAF(node))
			return 0;

		if (LEFT(node))
		{
			has_left = 1;
			left_h = avl_check_node( LEFT(node),depth + 1);
		}
		if (RIGHT(node))
		{
			has_right = 1;
			right_h= avl_check_node(RIGHT(node),depth + 1);
		}

		if (!has_left || !has_right)
		{
			if ( (has_left  && left_h  > 0) || 
				(has_right && right_h > 0) )
			{
				const char * which = has_left ? " left " : " right ";

				printf("Error in tree, %s child only has height"
					" of more than 1!\n",which);
				return 0;
			}
		}
		if ( (left_h < right_h && right_h - left_h > 1)	||
			(left_h > right_h && left_h - right_h > 1) )
		{
			printf("ERROR in the tree, depth = %d\n",depth);
			printf("left height = %d, right height = %d\n",
				left_h,right_h);
			return 0;
		}
		return 1 + (left_h > right_h ? left_h : right_h);
	};


	//拷贝节点,修改了内存的问题
	inline int	avl_copy_nodes(CAvlNode<KEY,DATA>* pDest,CAvlNode<KEY,DATA>* pSrc)
	{
		//删除目标节点数据,黄斌 2007-3-28
		if( pDest->m_pKey != NULL )
		{
			delete pDest->m_pKey;
		}
		if( pDest->m_pData != NULL )
		{
			delete pDest->m_pData;
		}
		pDest->m_pData = pSrc->m_pData;
		pDest->m_pKey = pSrc->m_pKey;

		//将源节点的指针置为空
		pSrc->m_pData=NULL;
		pSrc->m_pKey=NULL;
		return 0;
	};

	inline int	avlRemoveNode(CAvlNode<KEY,DATA>* node)
	{
		if (!node)
		{ 
			return -1; 
		}	    /* No such node */

		if (m_pCurrent == node)
		{
			m_pCurrent = NULL;
		}

		if (avl_remove_node(node))
		{
	#ifdef _DEBUG
			printf("avlRemoveNode(): UNFORSEEN ERROR OCCURRED\n");
	#endif
			return -1;			    /* A nasty error ?? */
		}
		m_nNodes--;

		return 0;
	};
};



//测试代码

/*
#include "MemPool.h"


void* MallocMem(int nSize);
void FreeMem(void* p, int nSize);

class C1
{
public:
C1()
{
n1=0;
n2=0;
memset(szTemp, 0, 100);
};
~C1()
{
n1=0;
n2=0;
};
char szTemp[100];

#ifdef _MEM_POOL_
inline void* operator new(unsigned int nSize)
{
void* p= MallocMem(nSize);
return p;
};
inline void operator delete(void* p)
{
FreeMem(p, sizeof(C1) );
return;
}
#endif

private:
int n1;
int n2;

};

class CINT
{
public:
CINT()
{
m_i=100;
};
~CINT()
{
m_i=0;
};

#ifdef _MEM_POOL_
inline void* operator new(unsigned int nSize)
{
void* p= MallocMem(nSize);
return p;
};
inline void operator delete(void* p)
{
FreeMem(p, sizeof(CINT) );
return;
}
#endif

//private:
unsigned int m_i;
};


typedef CAvlTree<CINT, C1> CTestTree;

CMemPool* g_pPool1=NULL;
CMemPool* g_pPool2=NULL;

void* MallocMem(int nSize)
{
if( nSize == sizeof(CINT))
{
return g_pPool1->mallocMemBuf();
}
else if( nSize == sizeof(C1))
{
return g_pPool2->mallocMemBuf();
}
else
{
return malloc(nSize);
}
}

void FreeMem(void* p, int nSize)
{
if( nSize == sizeof(CINT))
{
g_pPool1->freeMemBuf(p);
}
else if( nSize == sizeof(C1))
{
g_pPool2->freeMemBuf(p);
}
else
{
free(p);
}
}

int CmpFunc(void* p1, void* p2 )
{
CINT* pKey1=(CINT*)p1;
CINT* pKey2=(CINT*)p2;
unsigned int nCmp;
if( pKey1->m_i < pKey2->m_i)
{
nCmp =-1;
}
else if( pKey1->m_i > pKey2->m_i )
{
nCmp=1;
}
else
{
nCmp=0;
}
return nCmp;
}

#define M10		100000
#define G1		10000000

#include <Time.h>
int main(int argc, char* argv[])
{
unsigned int nData=0;
srand( (unsigned)time( NULL ) );
CMemPool* m_pPool1=NULL;
CMemPool* m_pPool2=NULL;
CTestTree* m_pTestTree= new CTestTree(CmpFunc);

g_pPool1= new CMemPool(sizeof(CINT) , 3000 );
g_pPool2= new CMemPool( sizeof(C1) , 3000 );
CINT*	pKey=NULL;
C1*		pData=NULL;	


unsigned int nLoopCount=0;
//顺序添加数据
int nStart=GetTickCount();
int nAddCount=0;
while(nLoopCount <  M10 )
{
nLoopCount++;
pKey=new CINT();
pData=new C1();
pKey->m_i=	nLoopCount;
sprintf( pData->szTemp, "%d", pKey->m_i );
if( m_pTestTree->addNode(pKey, pData) )
{
printf( "AddNode Error\n");
}
else
{
nAddCount++;
}
}
printf( "AddCount=%d, TIME=%d\n", nAddCount, GetTickCount()- nStart);

//随机添加数据
nStart= GetTickCount();
nLoopCount=0;
nAddCount=0;
nData=0;
while( nLoopCount < M10 )
{
nLoopCount++;
nData= rand() * rand() * rand() * rand() * GetTickCount() + GetTickCount();
pKey=new CINT();
pData=new C1;
pKey->m_i=	nData;
sprintf( pData->szTemp, "%d", pKey->m_i );
if( m_pTestTree->addNode(pKey, pData) )
{
printf( "AddNode Error, Key=%d\n", pKey->m_i);
delete pKey;
delete pData;
}
else
{
nAddCount++;
}
}
printf( "AddCount=%d, TIME=%d\n", nAddCount, GetTickCount()- nStart);

//检查AVL树的平衡性
nStart= GetTickCount();
m_pTestTree->avlCheck();
printf( "Check time=%d\n", GetTickCount()-nStart );

//查询
nStart= GetTickCount();
nLoopCount=0;
pKey=new CINT();
pData=NULL;
int nGetCount=0;
while(nLoopCount < G1 )
{
//查找节点
nLoopCount++;

pKey->m_i= nLoopCount;
pData= m_pTestTree->getNode(pKey);
if( pData != NULL )
{
if( atoi(pData->szTemp) != pKey->m_i )
{
printf( "Get Data Error, nData=%d\n", nData);
}
}
else
{
nGetCount++;
}
}
delete pKey;
printf( "GetCount=%d, Time=%d\n", nGetCount, GetTickCount()- nStart);


//检查AVL的正确性
nStart=GetTickCount();
int nCheckCount=0;
if( m_pTestTree->getMinNode(pKey, pData) )
{
printf ("Get Min Node Error\n");
}
CINT* pKey1=NULL;
C1* pData1= NULL;
while( m_pTestTree->getNextNode(pKey1, pData1)== 0 )
{
if( pKey->m_i > pKey1->m_i )
{
printf( "AVL seq error, key1=%d, key2=%d\n", pKey->m_i, pKey1->m_i);
}
pKey= pKey1;
pData= pData1;
nCheckCount++;
}
printf( "CheckCount=%d, time=%d\n", nCheckCount, GetTickCount()- nStart);


//随机删除数据
nStart= GetTickCount();
pKey=new CINT();
int nDelCount=0;
nLoopCount=0;
while( nLoopCount < M10 )
{
nLoopCount++;
nData= rand();
nData= nData* GetTickCount();
pKey->m_i=	nData;
if( m_pTestTree->rmNode(pKey) )
{
//printf( "AddNode Error\n");
}
else
{
nDelCount++;
}
}
printf ( "DelCount=%d, time=%d\n", nDelCount, GetTickCount()- nStart);

//全部删除数据
nStart= GetTickCount();
nDelCount=0;
nLoopCount=0;
while( nLoopCount < M10 )
{
nLoopCount++;
pKey->m_i= nLoopCount;
if( m_pTestTree->rmNode(pKey ) )
{

}
else
{
nDelCount++;
}
}
delete pKey;
printf( "DelCount=%d, Time=%d\n", nDelCount, GetTickCount() - nStart);

delete m_pTestTree;


m_pTestTree=NULL;

delete g_pPool1;
delete g_pPool2;
printf( "Time=%d\n", GetTickCount() - nStart);


Sleep(1000000);

return 0;
}

*/



#endif // !defined(AFX_AVLTREE_H__E0C83055_C174_4C76_AF28_5B1B81ADE748__INCLUDED_)



