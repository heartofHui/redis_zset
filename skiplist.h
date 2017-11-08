/*************************************************************************
    > File Name:skiplist.h
    > Author: chenhui
    > Mail: chenhui@17paipai.cn
    > Created Time: 2017年11月08日 10:45:38
 ******************************************************************/
#ifndef DEF_CL_SKIP_LIST_H
#define DEF_CL_SKIP_LIST_H

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include "../Includes/times.h"
#define MAXLEVEL 12 //
#define ZSKIPLIST_P 0.25
#define CODE_TEST_LOG true
using namespace std;

class SKNode
{
public:
	int64_t member;
	int64_t score;
	SKNode* backend;

	class SKNodeLevel
	{
	public:
		SKNode* forward;
		int32_t span;//到写一个节点需要跨越的节点数
	};
	SKNodeLevel level[MAXLEVEL];
//	SKNode()
//	{
//		member = 0;
//		score = 0;
//		for (int i = 0; i < MAXLEVEL; i++)
//		{
//			level[i].forward = NULL;
//			level[i].span = 0;
//		}
//	}
};
//skip list
class SkipList
{
public:
	SKNode *header; /* header */
	SKNode *tail;/* tail */
	int listLevel; /* current max level */
	int length;/* current lenght */
public:
	int insert(int64_t mem, int64_t score);
	SKNode* search(int64_t mem, int64_t key);

	/*
	 * func:查找score第一次出现的位置
	 * */
	SKNode* searchLeftFirstLocation(const int64_t& score);
	void getRangeByScore(const int64_t& start,const int64_t& end, std::vector<int64_t>& resList, const int64_t& limitCount=-1);
	SKNode* getNodeByRank(const int64_t& rank);

	/* func:获取两个排名之间的数据集合
	 * arg:
	 * 	start:开始排名
	 * 	end:结束排名
	 * 	resList:结果集合
	 * 	limitCount:返回最大结果数
	 * 	reverse:逆序，从大到小
	 * 	*/

	void getRangeByRank(const int64_t& start,const int64_t& end, std::vector<int64_t>& resList, const int64_t& limitCount=-1,const bool reverse=false);
	bool skdelete(int64_t mem, int64_t key);
	void skdeletenode(SKNode *x,SKNode **update);
	void printList();
	int getInsertLevel();
	int zslRandomLevel();
	void deleteRangeByRank(const int64_t& start,const int64_t& end, std::vector<int64_t>& resList, const int64_t& limitCount=-1);
	void deleteRangeByScore(const int64_t& start,const int64_t& end, std::vector<int64_t>& resList, const int64_t& limitCount=-1);
public:
	SkipList()
	{
		cout<<"SkipList()"<<endl;
		header = new SKNode;
		listLevel = 1;
		header->score = -INT_MAX;
		SKNode* end = new SKNode;
		SKNode* first = new SKNode;
		first->score = -INT_MAX;
		end->score = INT_MAX;
		for (int i = 0; i < MAXLEVEL; i++)
		{
			header->level[i].forward = first;
			header->level[i].span = 1;
			header->level[i].forward->level[i].forward = end;
			header->level[i].forward->level[i].span = 1;
		}
		end->backend = first;
		first->backend = header;
		tail = end;
		length = 2;
		printList();
	}
	~SkipList()
	{
		cout<<"~SkipList()"<<endl;
		delete header;
	}
};

void SkipList::deleteRangeByRank(const int64_t& start,const int64_t& end, std::vector<int64_t>& resList, const int64_t& limitCount)
{
	if(start>end)
	{
		return;
	}
	int64_t tmpStart = (start < 0) ? (length + start-2) : start;
	int64_t tmpEnd = (end < 0) ? (length + end-2) : end;
	SKNode *update[MAXLEVEL];
	SKNode* current = header->level[0].forward;
	SKNode* tmp;
	//tmp = current = getNodeByRank(tmpStart+1);
	int64_t through = -1;
	for (int i = listLevel - 1; i >= 0; i--)
	{
		while(current->level[i].forward->score!=INT_MAX && (through+current->level[i].span) < tmpStart)
		{
			through += current->level[i].span;
			current = current->level[i].forward;
		}
//		if(through == tmpStart)
//		{
//			return current;
//		}
		update[i] = current;
	}
	tmp = current = current->level[0].forward;

	int64_t index = 0;
	int64_t numPre = (tmpEnd - tmpStart);
	while(current && current->score != INT_MAX && (index <= numPre) && (limitCount==-1 || (limitCount != -1 && index < limitCount)))
	{
		resList.push_back(current->member);
		current = current->level[0].forward;
		skdeletenode(tmp, update);
		delete tmp;
		tmp = current;
		index++;
	}
}

void SkipList::deleteRangeByScore(const int64_t& start,const int64_t& end, std::vector<int64_t>& resList, const int64_t& limitCount)
{
	if (start > end)
	{
		return;
	}
	SKNode* update[MAXLEVEL];
	SKNode* current=header;
	SKNode* tmp;
	int i = listLevel - 1;
	for (; i >= 0; i--)
	{
		while (current->level[i].forward->score != INT_MAX
				&& (start > current->level[i].forward->score)) //key大于下一个数据的值。转到本层下一个元素
		{
#ifdef CODE_TEST_LOG
			cout<<" find, from score="<< current->level[i].forward->score<<" this floor to next "<<endl;
#endif
			current = current->level[i].forward;
		}
		if(i != 0)
		{
#ifdef CODE_TEST_LOG
			cout<<" find, from score="<< current->score <<" to next floor "<<endl;
#endif
		}
		update[i] = current;
		//否则i--，转到下一层
	}

	tmp = current = current->level[0].forward;
	int num = 1;
	while(current->score>=start && current->score<=end && i <= limitCount)
	{
		resList.push_back(current->member);
		skdeletenode(current, update);
		current = current->level[0].forward;
		delete tmp;
		tmp = current;
		num++;
	}
}

int SkipList::zslRandomLevel(void) {
    int level = 1;
    while ((random()&0xFFFF) < (ZSKIPLIST_P * 0xFFFF))
        level += 1;
    return (level<MAXLEVEL) ? level : MAXLEVEL;
}


int SkipList::getInsertLevel()
{
	static int i = 1;
	i++;
	int32_t sed = getTimeMsec() % 10000 + i;
	srand(sed);
	int upcount = 0;
	for (int i = 0; i < MAXLEVEL; i++)
	{
		int num = rand() % 10;
		if (num < 3)
		{
			upcount++;
		}
	}
	return upcount > 1 ? upcount : 1;
}

SKNode* SkipList::search(int64_t mem, int64_t score)
{
	SKNode* current;// = new SKNode;
	current = header;
	int i = listLevel - 1;
	for (; i >= 0; i--)
	{
		while (current->level[i].forward->score != INT_MAX
				&& (score > current->level[i].forward->score
				|| (score == current->level[i].forward->score && mem != current->level[i].forward->member))) //key大于下一个数据的值。转到本层下一个元素
		{
#ifdef CODE_TEST_LOG
			cout<<" find, from score="<< current->level[i].forward->score<<" this floor to next "<<endl;
#endif
			current = current->level[i].forward;
		}
		if(i != 0)
		{
#ifdef CODE_TEST_LOG
			cout<<" find, from "<< current->score <<" to next floor "<<endl;
#endif
		}
		//否则i--，转到下一层
	}
	current = current->level[0].forward;
	if (current != NULL && current->score == score)
	{
#ifdef CODE_TEST_LOG
		cout << "find" << score << endl;
#endif
		return current;
	}
	return NULL;
}

SKNode* SkipList::searchLeftFirstLocation(const int64_t& score)
{
    SKNode* current;// = new SKNode;
	current = header;
	int i = listLevel - 1;
	for (; i >= 0; i--)
	{
		while (current->level[i].forward->score != INT_MAX
				&& (score > current->level[i].forward->score)) //key大于下一个数据的值。转到本层下一个元素
		{
#ifdef CODE_TEST_LOG
			cout<<" find, from score="<< current->level[i].forward->score<<" this floor to next "<<endl;
#endif
			current = current->level[i].forward;
		}
		if(i != 0)
		{
#ifdef CODE_TEST_LOG
			cout<<" find, from score="<< current->score <<" to next floor "<<endl;
#endif
		}
		//否则i--，转到下一层
	}
	current = current->level[0].forward;
	if (current != NULL && current->score >= score)
	{
#ifdef CODE_TEST_LOG
		cout << "find" << score << endl;
#endif
		return current;
	}
	return NULL;
}

SKNode* SkipList::getNodeByRank(const int64_t& rank)
{
	int64_t through = -1;//因为前面有个辅助节点，所以初始值为-1
	SKNode* current = header;
	for (int i = listLevel - 1; i >= 0; i--)
	{
		while(current->level[i].forward->score!=INT_MAX && (through+current->level[i].span)<=rank)
		{
			through += current->level[i].span;
			current = current->level[i].forward;
		}
		if(through==rank)
		{
			return current;
		}
	}
	printf("not found rank:%llu\n",rank);
	return NULL;
}

void SkipList::getRangeByRank(const int64_t& start,const int64_t& end, std::vector<int64_t>& resList, const int64_t& limitCount, const bool reverse)
{
	if(!reverse && start > end)
	{
		return;
	}
	if (reverse && start < end)
	{
		return;
	}
	int64_t tmpStart = (start < 0) ? (length + start-2) : start;
	int64_t tmpEnd = (end < 0) ? (length + end-2) : end;

	SKNode* current;
	current = getNodeByRank(tmpStart+1);
	int64_t index = 0;
	int64_t numPre = reverse ? (tmpStart - tmpEnd) : (tmpEnd - tmpStart);
	while(current && current->score != INT_MAX && (index <= numPre) && (limitCount==-1 || (limitCount != -1 && index < limitCount)))
	{
		resList.push_back(current->member);
		if(reverse)
		{
			current = current->backend;
		}
		else
		{
			current = current->level[0].forward;
		}
		index++;
	}
}

void SkipList::getRangeByScore(const int64_t& start,const int64_t& end, std::vector<int64_t>& resList, const int64_t& limitCount)
{
	if(start > end)
	{
		return ;
	}
	SKNode* leftFirst;
	leftFirst = searchLeftFirstLocation(start);
	if(leftFirst == NULL)
	{
		return ;
	}
	while (leftFirst != NULL && leftFirst->score <= end && (limitCount==-1 || (limitCount!=-1 && resList.size()<limitCount)))
	{
		resList.push_back(leftFirst->member);
		leftFirst = leftFirst->level[0].forward;
	}
}

int SkipList::insert(int64_t mem, int64_t score)
{
#ifdef CODE_TEST_LOG
	printf("---------insert mem:%d,score:%d----------",mem,score);
#endif
	int level = zslRandomLevel();
	SKNode* node = new SKNode;
	node->member = mem;
	node->score = score;

	SKNode *update[MAXLEVEL];
	unsigned int rank[MAXLEVEL];
	SKNode* current;
	SKNode* last;

	current = last = header;
	int i = listLevel - 1;
	for (; i >= 0; i--)
	{
		rank[i] = (i == listLevel - 1) ? 0 : rank[i + 1];
		while (current->level[i].forward->score != INT_MAX &&
			    (score > current->level[i].forward->score ||
				(score == current->level[i].forward->score && mem > current->level[i].forward->member))) //score大于下一个元素的score，转到本层下一个元素
		{
			rank[i] += current->level[i].span;
			current = current->level[i].forward;
		}
		update[i] = current; //保存每一层位置上的最后指针的前驱
	}
	last = current->level[0].forward;
	if (last != NULL && last->score == score)
	{
		level = 1;
	}
	if (level > listLevel) //更新层数
	{
		for (int i = listLevel; i < level; i++)
		{
			rank[i] = 1;//这个地方因为增加了-INT_MAX和INT_MAX两个节点，所以赋值为1
			update[i] = header->level[i].forward;
			update[i]->level[i].span = length-1;//这个地方因为增加了-INT_MAX和INT_MAX两个节点，所以长度减1
		}
		listLevel = level;
	}

	for (int k = 0; k < level; k++)
	{
		node->level[k].forward = update[k]->level[k].forward;
		update[k]->level[k].forward = node;

		node->level[k].span = update[k]->level[k].span - (rank[0]-rank[k]);
		update[k]->level[k].span = (rank[0]-rank[k]) + 1;//update[k]->level[k].span表示前驱到新节点的距离
	}
	for(int k=level;k<listLevel;k++)//把没有更新到的层长度增加1
	{
		update[k]->level[k].span++;
	}
	//更新backend指针
	node->backend = update[0];
	if(node->level[0].forward)
	{
		node->level[0].forward->backend = node;
	}

	length++;
	return 1;
}

void SkipList::skdeletenode(SKNode *x,SKNode **update)
{
	for (int i = 0; i < listLevel; i++)
	{
		if(update[i]->level[i].forward == x)
		{
			update[i]->level[i].forward = x->level[i].forward;
			update[i]->level[i].span += x->level[i].span-1;
		}
		else
		{
			update[i]->level[i].span--;
		}
	}
	x->level[0].forward->backend = x->backend;
	while(listLevel>1 && header->level[listLevel-1].forward->level[listLevel-1].forward->score==INT_MAX)
	{
#ifdef CODE_TEST_LOG
		printf("listLevel--\n");
#endif
		listLevel--;
	}
	length--;
}

/*
 * key: delete key
 * return:false:fail , true:success*/
bool SkipList::skdelete(int64_t mem, int64_t score)
{
	SKNode *update[MAXLEVEL];
	SKNode* current;
	current = header;
	int i = listLevel - 1;
	for (; i >= 0; i--)
	{
		while (current->level[i].forward->score != INT_MAX
				&& (score > current->level[i].forward->score
				|| (score == current->level[i].forward->score && mem > current->level[i].forward->member))) //key大于下一个数据的值。转到本层下一个元素
		{
#ifdef CODE_TEST_LOG
			cout<<" find, from score="<< current->level[i].forward->score<<" this floor to next "<<endl;
#endif
			current = current->level[i].forward;
		}
		if(i != 0)
		{
#ifdef CODE_TEST_LOG
			cout<<" find, from "<< current->score <<" to next floor "<<endl;
#endif
		}
		update[i] = current;
		//否则i--，转到下一层
	}
	current = current->level[0].forward;
	if (current != NULL && current->score == score && current->member == mem)
	{
#ifdef CODE_TEST_LOG
		cout << "delete " <<mem<<":"<< score << endl;
#endif
		skdeletenode(current, update);
		delete current;
	}
	return true;
}

void SkipList::printList()
{
	SKNode* current = header;
	for (int i = listLevel - 1; i >= 0; i--)
	{
		current = header->level[i].forward;
		cout << "level " << i << "................................" << endl;
		while (current->level[i].forward) //
		{
			cout << " " <<current->member<<":"<< current->score<<"@"<<current->level[i].span;
			current = current->level[i].forward;
		}
		cout << " " <<current->member<<":"<< current->score<<"@"<<current->level[i].span << endl;
	}
}

#endif
