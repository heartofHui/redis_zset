/*************************************************************************
    > File Name:rediszset.h
    > Author: chenhui
    > Mail: chenhui@17paipai.cn
    > Created Time: 2017年11月08日 10:45:38
 ******************************************************************/
#ifndef DEF_CL_REDIS_ZSET_H
#define DEF_CL_REDIS_ZSET_H

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include "skiplist.h"
using namespace std;

class ClZset
{
private:
	SkipList skpl;
	std::map<int64_t,int64_t> MemScoreMap;
public:
	/*
	 * args
	 * 		mem:data
	 * 		score:sort by score
	 * return:
	 * 		0:success other:fail
	 * 		*/
	int zAdd(const int64_t& mem, const int64_t& score);
	int zScore(const int64_t& mem,int64_t& score);
	int64_t zCount();
	/*is start score or end score in zset range
	 *
	 * return:
	 * 		0:success other:fail*/
	int zsetIsInRange(const int64_t& startScore, const int64_t& endScore);
	void zRange(const int64_t& startRank,const int64_t& endRank,std::vector<int64_t>& resList,const int64_t& limitCount=-1);
	void zRevRange(const int64_t& startRank,const int64_t& endRank,std::vector<int64_t>& resList,const int64_t& limitCount=-1);
	void zRangeByScore(const int64_t& startScore, const int64_t& endScore, std::vector<int64_t>& resList,const int64_t& limitCount=-1);

	int zrem(const int64_t& mem);
	void zRemRangeByRank(const int64_t& startRank,const int64_t& endRank,const int64_t& limitCount=-1);
	void zRemRangeByScore(const int64_t& startScore, const int64_t& endScore, const int64_t& limitCount = -1);
	void printzsetlist();
};

void ClZset::zRange(const int64_t& startRank,const int64_t& endRank,std::vector<int64_t>& resList,const int64_t& limitCount)
{
	skpl.getRangeByRank(startRank,endRank,resList,limitCount);
}

void ClZset::zRevRange(const int64_t& startRank,const int64_t& endRank,std::vector<int64_t>& resList,const int64_t& limitCount)
{
	skpl.getRangeByRank(startRank,endRank,resList,limitCount, true);
}

int64_t ClZset::zCount()
{
	return skpl.length;
}

int ClZset::zsetIsInRange(const int64_t& startScore, const int64_t& endScore)
{
	if(startScore > endScore)
	{
		return 1;
	}
}

int ClZset::zAdd(const int64_t& mem, const int64_t& score)
{
	std::map<int64_t,int64_t>::iterator it = MemScoreMap.find(mem);
	if(it != MemScoreMap.end())
	{
		skpl.skdelete(mem,it->second);
		MemScoreMap.erase(it);
	}
	skpl.insert(mem,score);
	MemScoreMap[mem] = score;
	return 0;
}

int ClZset::zScore(const int64_t& mem,int64_t& score)
{
	score = 0;
	if(MemScoreMap.find(mem) != MemScoreMap.end())
	{
		score = MemScoreMap[mem];
		return 0;
	}
	else
	{
		return 1;
	}
}

int ClZset::zrem(const int64_t& mem)
{
	std::map<int64_t,int64_t>::iterator it = MemScoreMap.find(mem);
	if(it != MemScoreMap.end())
	{
		skpl.skdelete(mem, it->second);
		MemScoreMap.erase(it);
	}
	return 0;
}

void ClZset::zRangeByScore(const int64_t& start, const int64_t& end, std::vector<int64_t>& resList, const int64_t& limitCount)
{
	skpl.getRangeByScore(start,end,resList,limitCount);
}

void ClZset::zRemRangeByRank(const int64_t& startRank,const int64_t& endRank,const int64_t& limitCount)
{
	std::vector<int64_t> remList;
	skpl.deleteRangeByRank(startRank,endRank,remList,limitCount);
	for(std::vector<int64_t>::iterator it = remList.begin();it!=remList.end();it++)
	{
		std::map<int64_t,int64_t>::iterator itDic = MemScoreMap.find(*it);
		if (itDic != MemScoreMap.end())
		{
#ifdef CODE_TEST_LOG
			cout<<"zremrangebyrank :"<<*it<<endl;
#endif
			MemScoreMap.erase(itDic);
		}
	}
}

void ClZset::zRemRangeByScore(const int64_t& startScore, const int64_t& endScore, const int64_t& limitCount)
{
	std::vector<int64_t> remList;
	skpl.deleteRangeByScore(startScore,endScore,remList,limitCount);
	for(std::vector<int64_t>::iterator it = remList.begin();it!=remList.end();it++)
	{
		std::map<int64_t,int64_t>::iterator itDic = MemScoreMap.find(*it);
		if (itDic != MemScoreMap.end())
		{
#ifdef CODE_TEST_LOG
			cout<<"zRemRangeByScore :"<<*it<<endl;
#endif
			MemScoreMap.erase(itDic);
		}
	}
}

void ClZset::printzsetlist()
{
	skpl.printList();
}

#endif
