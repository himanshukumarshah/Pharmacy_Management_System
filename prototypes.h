#ifndef PROTOTYPES_H
#define PROTOTYPES_H

#include "structures.h"

/*------------------------ Main Functions' Definitions ------------------------*/
void addNewMedication(BT_roots* );
void updateMedication(BT_roots*);
void searchMedication(BT_roots*);
void deleteMedication(BT_roots*);
void stockAlerts(BT_roots*);
void checkExpirationDates(BT_roots*);
void MedBetweenDates(BT_roots*);
void recordSales(BT_roots*);
void supplierManagement(BT_roots*);
void FindAllRounderSupplier(BTree*);
void FindLargestTuroverSupplier(BTree*);

/*------------------------ Supportive Functions' Prototypes ------------------------*/
void BT_insert(BTree*, void*);
BTreeNode* BT_insertInternal(BTree*, BTreeNode*, void*, void**, BTreeNode**);
void insertIn_Med_Hash(char*, int);
unsigned int MedKey(const char*);
int searchIn_Med_Hash(char*);
void* search_ByID(BTree*, void*);
BatchNode* search_BatchNode_ByBatchNo(BTreeNode*, char*);
void traverse_StockAlerts(BTreeNode*, int*);
void traverse_ExpirationDates(BTreeNode*, int, int*);
void rangeSearch_Dates(BTreeNode*, int, int, int*);

void BT_Delete(BTree*, void*);
void deleteKey(BTree*, BTreeNode*, void*);
void removeFromLeaf(BTreeNode*, int);
void removeFromNonLeaf(BTree*, BTreeNode*, int);
void* getPred(BTreeNode*, int);
void* getSucc(BTreeNode*, int);
void mergeNodes(BTreeNode*, int);
void fill(BTreeNode*, int);
void borrowFromPrev(BTreeNode*, int);
void borrowFromNext(BTreeNode*, int);
void collectSuppliers(BTreeNode*, SuppNode**, int*);

void addSupplier(BT_roots*);
void updateSupplier(BTree*);
void deleteSupplier(BTree*);
void searchSupplier(BTree*);

/*------------------------------ Helping functions Prototypes ------------------------------*/
int compareMedNode(const void*, const void*);
int compareBatchNode(const void*, const void*);
int compareSupplierNode(const void*, const void*);
int cmpUniqueMed(const void*, const void*);
int cmpTurnover(const void*, const void*);
BTree* createBTree(compareFn);
void initialisation(BT_roots**);
MedNode* createMedNode();
BatchNode* createBatchNode();
SuppNode* createSuppNode();
BTreeNode* createBT_Node(bool);
int check_and_correct_DATE(char date[]);
bool isLeapYear(int);
int daysInMonth(int, int);
int convertToDays(int);
char* format_DATE(int);
void print_Medication(MedNode*);
void freeBatchList(BatchNode*);
void toLowerStr(char*, const char*);

void saveDataToFile(BT_roots*);
void saveMedicationRecursive(FILE*, BTreeNode*);
void saveSuppBTree(FILE*, BTreeNode*) ;
void loadDataFromFile(BT_roots*);

void freeResources(BT_roots*);
void freeMedTree(BTreeNode*);
void freeBatchTree(BTreeNode*);
void freeSuppTree(BTreeNode*);
void freeMedHashTable();

#endif