#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define ORDER 5
#define NUM_LEN 10
#define DATE_LEN 15
#define NAME_LEN 60
#define MAX_MED_IN_SUPP 20
#define TABLE_SIZE 101
#define DATA_FILE "pharmacy_data.txt"

typedef struct Medication_tag MedNode;

/*------------------------------ Data Nodes ------------------------------*/
typedef struct Batch_tag {
    char batchNo[NUM_LEN];
    int expiryDate; // As Batch Tree will be based on expirationdates, so it is INT (YYYYMMDD).
    int stock;
    int totalSales;
    struct Batch_tag* next;  // Linked list of batches, only used in med Tree
    MedNode* medPtr;
} BatchNode;

typedef struct Supplier_tag {
    int supplierID;
    char supplierName[NAME_LEN];
    char contactInfo[NUM_LEN];
    int uniqueMedication;
    float totalCostSupplied;
    // struct Supplier_tag* next; // Linked list of suppliers, only used in med Tree
    int quantities[MAX_MED_IN_SUPP];
    MedNode* MedPtr[MAX_MED_IN_SUPP];
} SuppNode;

typedef struct Medication_tag {
    int medID;
    char medName[NAME_LEN];
    float pricePerUnit;
    int reorderLevel;
    int stockQuantity;
    int totalSales;
    BatchNode* batches;  // Pointer to batch linked list
    SuppNode* suppliers[MAX_MED_IN_SUPP]; // Pointer to supplier
} MedNode;

typedef struct MedName {
    char medName[NAME_LEN];
    int medID;
    struct MedName* next;
} MedName;

/*------------------------------ B Tree Nodes ------------------------------*/
typedef struct BT_MedNode {
    MedNode* keys[ORDER - 1];  
    struct BT_MedNode* children[ORDER]; 
    int noOfKeys;
    bool isLeaf;
} BT_MedNode;

typedef struct BT_BatchNode {  // Batch BTree will be based on expiration dates
    BatchNode* keys[ORDER - 1];   
    struct BT_BatchNode* children[ORDER];
    int noOfKeys;
    bool isLeaf;
} BT_BatchNode;

typedef struct BT_SuppNode {
    SuppNode* keys[ORDER - 1]; 
    struct BT_SuppNode* children[ORDER]; 
    int noOfKeys;
    bool isLeaf;
} BT_SuppNode;

/*------------------------------ B Trees Roots ------------------------------*/
typedef struct BTree {
    BT_MedNode* Med_root;
    BT_BatchNode* Batch_root;
    BT_SuppNode* Supp_root;
} BTree;

/*------------------------------ Helping functions Prototypes ------------------------------*/
void initialise_DB(BTree*);
int check_and_correct_DATE(char[]);
void TraverseFor_StockAlerts (BT_MedNode*, int*);
bool isLeapYear(int);
int daysInMonth(int, int);
int convertToDays(int);
char* format_DATE(int);
void TraverseFor_checkExpiration (BT_BatchNode*, int, int*);
void TraverseFor_check_MedicationBet_DATES(BT_BatchNode*, int, int, int*);
void loadDataFromFile(BTree*);
void saveDataToFile(BTree*);
void saveSuppBTree(FILE*, BT_SuppNode*);
void saveBatchBTree(FILE*, BT_BatchNode*);
void saveMedicationRecursive(FILE*, BT_MedNode*);
void freeBatchList(BatchNode*);
void freeSuppList(SuppNode*);
void freeMedNodes(BT_MedNode*);
void freeBatchBTree(BT_BatchNode*);
void freeSuppBTree(BT_SuppNode*);
void freeMedHashTable();
void freeResources(BTree*);


/*------------------------ Supportive Functions' Prototypes ------------------------*/
unsigned int hashFn(const char*);
void insertIn_Med_Hash(char*, int);
int searchIn_Med_Hash(char*);
void insert_medNode(BT_MedNode**, MedNode*);
BT_MedNode* insertInternal_Med(BT_MedNode*, MedNode*, MedNode**, BT_MedNode**);
void insert_BatchNode(BT_BatchNode**, BatchNode*);
BT_BatchNode* insertInternal_Batch(BT_BatchNode*, BatchNode*, BatchNode**, BT_BatchNode**);
void insert_SuppNode(BT_SuppNode**, SuppNode*);
BT_SuppNode* insertInternal_Supp(BT_SuppNode*, SuppNode*, SuppNode**, BT_SuppNode**);
BT_MedNode* search_MedNode(BT_MedNode*, int);
MedNode* search_MedNode_ByID(BT_MedNode*, int);
BT_SuppNode* search_SuppNode (BT_SuppNode*, int);
SuppNode* search_SuppNode_ByID (BT_SuppNode*, int);

void collectSuppliers(BT_SuppNode*, SuppNode**, int*) ;
int compareUniqueMed(const void*, const void*);
int compareTurnover(const void*, const void*);



BT_SuppNode* createSuppBTreeNode(bool);
void traverseSuppBTree(BT_SuppNode*);
SuppNode* searchSuppBTree(BT_SuppNode*, int);
void splitSuppChild(BT_SuppNode*, int);
void insertSuppNonFull(BT_SuppNode*, SuppNode*);
void insertSupplier(BT_SuppNode**, SuppNode*);
void addSupplierToMedication(BT_SuppNode**, MedNode*);
void updateSupplierInMedication(BT_SuppNode*, MedNode*);
void deleteSupplierFromMedication(BT_SuppNode**, MedNode*);
void searchSupplierInMedication(BT_SuppNode*);


/*------------------------ Main Functions' Definitions ------------------------*/
void addNewMedication(BTree* );
void updateMedication(BTree*);
void searchMedication(BTree*);
void stockAlerts(BTree*);
void checkExpiration(BTree*);
void check_MedicationBet_DATES(BTree*);
void recordSales(BTree*);

void findAll_rounderSupplier(BTree*);
void findTop10LargestTurnover(BTree*);

void supplierManagement(BTree*);

/*------------------------ Some definitions ------------------------*/
extern MedName* MedTable[TABLE_SIZE];
// SuppName* SuppTable[TABLE_SIZE] = {NULL};

#define CREATE_NODE(TYPE, leaf)({\
    TYPE* newNode = (TYPE*)malloc(sizeof(TYPE));\
    newNode->noOfKeys = 0;\
    newNode->isLeaf = leaf;\
    for (int i = 0; i < ORDER; i++)\
        newNode->children[i] = NULL;\
    newNode;\
})

