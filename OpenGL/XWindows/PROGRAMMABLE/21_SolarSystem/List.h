#ifndef _LIST_H
#define _LIST_H

#include "vmath.h"	 //for mat4 variable

#define DATA_NOT_FOUND		-1
#define LIST_EMPTY			-2
#define LST_SUCCESS			 1
#define LST_FAILURE			 0
#define LST_TRUE			 1
#define LST_FALSE			 0

struct stNode; // Forward Declaration of Structure Node
typedef struct stNode		node_t;
typedef node_t				list_t;
typedef vmath::mat4			data_t;
typedef int					len_t;
typedef int					flag_t;
typedef int					result_t;
typedef int					bool_t;
//typedef int					bool;  // got "invalid combinaton of type specifiers"


struct stNode
{
	data_t data;
	struct stNode *pPrev, *pNext;
};


/// Interface Routienes Start

list_t*		CreateList(void);
result_t	InsertAtBegining(list_t *pList,data_t new_data);
result_t	InsertAtEnd(list_t *pList,data_t new_data);
result_t	InsertBeforeData(list_t *pList, data_t exsisting_data,data_t new_data);
result_t	InsertAfterData(list_t *pList, data_t existing_data, data_t new_data);
result_t	DeleteBegining(list_t *pList);
result_t	DeleteEnd(list_t *pList);
result_t	DeleteData(list_t *pList,data_t existing_data);


result_t	ExamineBegining(list_t *pList, data_t *pData);
result_t	ExamineEnd(list_t *pList, data_t *pData);
result_t	ExamineAndDeleteBegining(list_t *pList,data_t *pData); //**
result_t	ExamineAndDeleteEnd( list_t *pList, data_t *pData);

result_t	FindData(list_t *pList, data_t pData);
void		DisplayList(list_t *pList);
inline bool_t		IsEmpty(list_t *pList);
len_t		LengthList(list_t *pList);


data_t*		ToArray(list_t *pList,len_t *pLength);
list_t*		ToList(data_t *p_arr, len_t len);


result_t	DestroyList(list_t **ppList);
/// Interface Routienes End

/// List Auxillary Routines Start
static void		g_Insert(node_t *pBeg,node_t *pMid,node_t *pEnd);
static void		g_Delete(node_t *pNode);
static node_t*	SearchNode(list_t *pList, data_t search_data);
static node_t*	GetNode(data_t new_data);
/// List Auxillary Routines End

/// Auxillary Routines Start
static void*	XCalloc(int nr_Elements, int iSizePerElement);
/// Auxillary Routines End

#endif // !_LIST_H
