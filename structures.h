#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define ORDER 5
#define NUM_LEN 11
#define DATE_LEN 12
#define NAME_LEN 60
#define MAX_MED_IN_SUPP 20
#define TABLE_SIZE 101
#define DATA_FILE "pharma.txt"

typedef struct MedNode MedNode;
typedef int (*compareFn)(const void*, const void*);

/*------------------------------ Data Nodes ------------------------------*/
typedef struct BatchNode {
    int expiryDate; // As Batch Tree will be based on expirationdates, so it is INT (YYYYMMDD).
    char batchNo[NUM_LEN];
    int stock;
    int totalSales;
    struct BatchNode* next;  // Linked list of batches, only used in med Tree
    MedNode* medPtr;
} BatchNode;

typedef struct SuppNode {
    int supplierID;
    char supplierName[NAME_LEN];
    char contactInfo[NUM_LEN];
    int uniqueMedication;
    float totalCostSupplied;
    int quantities[MAX_MED_IN_SUPP];
    MedNode* MedPtr[MAX_MED_IN_SUPP];
} SuppNode;

typedef struct MedNode {
    int medID;
    char medName[NAME_LEN];
    float pricePerUnit;
    int reorderLevel;
    int stockQuantity;
    int totalSales;
    BatchNode* batches;  // Pointer to batch linked list
    SuppNode* suppliers[MAX_MED_IN_SUPP]; // Pointer to supplier
} MedNode;

typedef struct MedHash {
    char medName[NAME_LEN];
    int medID;
    struct MedHash* next;
} MedHash;

/*------------------------------ B-Tree ------------------------------*/

typedef struct BTreeNode{
    void** keys;
    struct BTreeNode** children;
    int noOfKeys;
    bool isLeaf;
} BTreeNode;

typedef struct BTree {
    BTreeNode* root;
    compareFn cmp;
} BTree;

typedef struct BT_roots{
    BTree* medTree;
    BTree* BatchTree;
    BTree* SupplierTree;
} BT_roots;


extern MedHash* MedTable[TABLE_SIZE];