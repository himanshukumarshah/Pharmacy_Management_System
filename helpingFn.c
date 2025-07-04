#include "prototypes.h"

int compareMedNode(const void* a, const void* b){
    return ((const MedNode*)a)->medID - ((const MedNode*)b)->medID;
}
int compareBatchNode(const void* a, const void* b){
    return ((const BatchNode*)a)->expiryDate - ((const BatchNode*)b)->expiryDate;
}
int compareSupplierNode(const void* a, const void* b){
    return ((const SuppNode*)a)->supplierID - ((const SuppNode*)b)->supplierID;
}
int cmpUniqueMed(const void* a, const void* b){
    return (*(const SuppNode**)b)->uniqueMedication - (*(const SuppNode**)a)->uniqueMedication;
}
int cmpTurnover(const void* a, const void* b){
    return (*(const SuppNode**)b)->totalCostSupplied - (*(const SuppNode**)a)->totalCostSupplied;
}

BTree* createBTree(compareFn cmp){
    BTree* tree = malloc(sizeof(BTree));
    tree->cmp = cmp;
    tree->root = NULL;
    return tree;
}

void initialisation(BT_roots** trees){
    for(int i = 0; i < TABLE_SIZE; i++){
        MedTable[i] = NULL;
    }
    (*trees) = (BT_roots*)malloc(sizeof(BT_roots));
    (*trees)->medTree = createBTree(compareMedNode);
    (*trees)->BatchTree = createBTree(compareBatchNode);
    (*trees)->SupplierTree = createBTree(compareSupplierNode);
}

BTreeNode* createBT_Node(bool leaf){
    BTreeNode* newNode = malloc(sizeof(BTreeNode));
    if (!newNode) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    newNode->isLeaf = leaf;
    newNode->noOfKeys = 0;
    newNode->keys = (void**)calloc((ORDER - 1), sizeof(void*));
    newNode->children = (BTreeNode**)calloc(ORDER, sizeof(BTreeNode*));
    return newNode;
}

MedNode* createMedNode(){
    MedNode* node = (MedNode*)malloc(sizeof(MedNode));
    if (!node) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    node->stockQuantity = 0;
    node->totalSales = 0;
    node->batches = NULL;
    for(int i = 0; i < MAX_MED_IN_SUPP; i++){
        node->suppliers[i] = NULL;
    }
    return node;
}
BatchNode* createBatchNode(){
    BatchNode* node = (BatchNode*)malloc(sizeof(BatchNode));
    if (!node) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    node->stock = 0;
    node->totalSales = 0;
    node->next = NULL;
    node->medPtr = NULL;
    return node;
}
SuppNode* createSuppNode(){
    SuppNode* node = (SuppNode*)malloc(sizeof(SuppNode));
    if (!node) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    node->totalCostSupplied = 0.0;
    node->uniqueMedication = 0;
    for(int i = 0; i < MAX_MED_IN_SUPP; i++){
        node->quantities[i] = 0;
        node->MedPtr[i] = NULL;
    }
    return node;
}

int check_and_correct_DATE(char date[]){
    if (strlen(date) != 10 || date[2] != '-' || date[5] != '-') {
        printf("\nInvalid date format! Use DD-MM-YYYY");
        return -1;
    }
    
    int day, month, year;
    if (sscanf(date, "%d-%d-%d", &day, &month, &year) != 3) {
        printf("\nInvalid date components!");
        return -1;
    }

    if (month < 1 || month > 12 || day < 1 || day > daysInMonth(month, year)) {
        printf("\nInvalid date values!");
        return -1;
    }

    return year * 10000 + month * 100 + day;
}

bool isLeapYear(int year) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return true;  
    }
    return false;
}
int daysInMonth(int month, int year){
    int dayInMonth[12] = {31,((isLeapYear(year)) ? 29 : 28 ) ,31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return dayInMonth [month - 1];
}
int convertToDays(int dt){
    int day = dt % 100;
    dt /= 100;
    int month = dt % 100;
    dt /= 100;
    int year = dt;
    
    int noOfDays = day;
    int yr = 2010;
    int mn = 1;
    while (yr < year){
        if (isLeapYear(yr)) noOfDays += 366;
        else noOfDays += 365;
        yr++;
    }
    while(mn < month){
        noOfDays += daysInMonth(mn, year);
        mn++;
    }
    return noOfDays;
}

char* format_DATE(int dt){
    int i;
    char* date = (char*)malloc(DATE_LEN * sizeof(char));
    if (!date) {
        printf("Memory allocation failed!");
        exit(1);
    }
    date[2] = date[5] = '-';
    for (i = 1; i >= 0; i--){
        date[i] = (dt % 10) + '0';
        dt /= 10;
    }
    for (i = 4; i >= 3; i--){
        date[i] = (dt % 10) + '0';
        dt /= 10;
    }
    for (i = 9; i >= 6; i--){
        date[i] = (dt % 10) + '0';
        dt /= 10;
    }
    date[10] = '\0';
    return date;
}

void print_Medication(MedNode* mNode){
    if (mNode){
        printf("\nMedication found!\n");
        printf("\n[ID] Name: [%d] %s", mNode->medID, mNode->medName);
        printf("\nPrice per unit: rs %.2f",mNode->pricePerUnit);
        printf("\nQuantity in stock: %d", mNode->stockQuantity);
        printf("\nTotal Sales: %d", mNode->totalSales);
        
        printf("\n\nBatch Details: ");
        BatchNode* btemp = mNode->batches;
        while(btemp){
            printf("\n\tBatch Number: %s", btemp->batchNo);
            printf("\n\tExpiration Date: %s", format_DATE(btemp->expiryDate));
            printf("\n\tQuantity in stock: %d", btemp->stock);
            printf("\n\tTotal Sales: %d", btemp->totalSales);
            btemp = btemp->next;
            printf("\n");
        }
        printf("\nSupplier Details: ");
        for (int i = 0; i < MAX_MED_IN_SUPP && mNode->suppliers[i] != NULL; i++){
            printf("\n\tSupplier Name: [%d] %s", mNode->suppliers[i]->supplierID, mNode->suppliers[i]->supplierName);
            printf("\n\tContact: %s", mNode->suppliers[i]->contactInfo);
            for(int j = 0; j < MAX_MED_IN_SUPP && mNode->suppliers[i]->MedPtr[j] != NULL; j++){
                if(mNode->suppliers[i]->MedPtr[j]->medID == mNode->medID){
                    printf("\n\tQuantity supplied: %d", mNode->suppliers[i]->quantities[j]);
                    break;
                }
            }
            printf("\n");
        }
    }
    else printf("Medication NOT found!");
}

/*--------------------- File Handling ---------------------*/

void saveDataToFile(BT_roots* Tree) {
    FILE *fp = fopen(DATA_FILE, "w");
    if (!fp) {
        printf("\nError opening file.");
        return;
    }

    fprintf(fp, "#MEDICATION_BTREE\n");
    saveMedicationRecursive(fp, Tree->medTree->root);

    fprintf(fp, "#SUPPLIER_BTREE\n");
    saveSuppBTree(fp, Tree->SupplierTree->root);

    fclose(fp);

    printf("\nAll data saved successfully to %s.", DATA_FILE);
}

void saveMedicationRecursive(FILE* fp, BTreeNode* node) {
    if (!node) return;

    int i;
    for (i = 0; i < node->noOfKeys; i++) {
        if (!node->isLeaf) saveMedicationRecursive(fp, node->children[i]);

        MedNode* med = (MedNode*)node->keys[i];
        fprintf(fp, "MEDICATION %d, %s, %.2f, %d, %d, %d\n", med->medID, med->medName, med->pricePerUnit, med->reorderLevel, med->stockQuantity, med->totalSales);

        BatchNode* batch = med->batches;
        while (batch != NULL) {
            fprintf(fp, "BATCH %s, %d, %d, %d\n", batch->batchNo, batch->expiryDate, batch->stock, batch->totalSales);
            batch = batch->next;
        }
        fprintf(fp, "END_MEDICATION\n");
    }

    if (!node->isLeaf) saveMedicationRecursive(fp, node->children[i]);
}

void saveSuppBTree(FILE* fp, BTreeNode* node) {
    if (!node) return;

    int i;
    for (i = 0; i < node->noOfKeys; i++){
        if (!node->isLeaf) saveSuppBTree(fp, node->children[i]);

        SuppNode* supp = (SuppNode*)node->keys[i];

        fprintf(fp, "SUPPLIER_TREE_NODE %d, %s, %s, %d, %.2f", supp->supplierID, supp->supplierName, supp->contactInfo, supp->uniqueMedication,  supp->totalCostSupplied);
        for (int j = 0; j < MAX_MED_IN_SUPP && supp->MedPtr[j] != NULL; j++){
            fprintf(fp, ", %d %d", supp->MedPtr[j]->medID, supp->quantities[j]);
        }
        fprintf(fp, "\n");
    }
    if (!node->isLeaf) saveSuppBTree(fp, node->children[i]);
}

void loadDataFromFile(BT_roots* Tree) {
    FILE* fp = fopen(DATA_FILE, "r");
    if (!fp){
        printf("Error opening file.\n");
        return;
    }

    char line[256];
    MedNode* med = NULL;

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = 0; 

        if (strcmp(line, "#MEDICATION_BTREE") == 0 || strcmp(line, "#SUPPLIER_BTREE") == 0) {
            continue;
        }

        if (strncmp(line, "MEDICATION", 10) == 0) {
            med = createMedNode();
            if (sscanf(line + 11, "%d, %[^,], %f, %d, %d, %d", &med->medID, med->medName, &med->pricePerUnit, &med->reorderLevel, &med->stockQuantity, &med->totalSales) != 6) {
                printf("\nError reading MEDICATION line: %s", line);
                continue;
            }

            insertIn_Med_Hash(med->medName, med->medID);
            BT_insert(Tree->medTree, med);
        }

        else if (strncmp(line, "BATCH", 5) == 0) {
            if (!med) {
                printf("\nWarning: BATCH found before any MEDICATION. Skipping.");
                continue;
            }

            BatchNode* newBatch = createBatchNode();
            if (sscanf(line + 6, "%[^,], %d, %d, %d", newBatch->batchNo, &newBatch->expiryDate, &newBatch->stock, &newBatch->totalSales) != 4) {
                printf("\nError reading BATCH line: %s", line);
                continue;
            }

            newBatch->medPtr = med;
            newBatch->next = med->batches;
            med->batches = newBatch;

            BT_insert(Tree->BatchTree, newBatch);
        }
        else if (strncmp(line, "SUPPLIER_TREE_NODE", strlen("SUPPLIER_TREE_NODE")) == 0){
            SuppNode* supp = createSuppNode();
            
            char tempLine[256];
            strcpy(tempLine, line + strlen("SUPPLIER_TREE_NODE"));

            int readCount = sscanf(tempLine, "%d, %[^,], %[^,], %d, %f", &supp->supplierID, supp->supplierName,supp->contactInfo, &supp->uniqueMedication, &supp->totalCostSupplied);

            if (readCount != 5) {
                printf("\nError reading supplier info: %s", line);
                continue;
            }

            char* medStart = strchr(tempLine, ','); 
            for (int i = 0; i < 4; i++) {
                medStart = strchr(medStart + 1, ',');
            }

            if (medStart) {
                medStart++; 
                int medID, quantity, i = 0;
                while (sscanf(medStart, " %d %d", &medID, &quantity) == 2) {
                    MedNode tmpMed;
                    tmpMed.medID = medID;
                    MedNode* mptr = search_ByID(Tree->medTree, &tmpMed);

                    if (!mptr) {
                        printf("\nMedID %d not found while linking supplier %d", medID, supp->supplierID);
                        continue;
                    }

                    int j;
                    for (j = 0; j < MAX_MED_IN_SUPP && mptr->suppliers[j]; j++);
                    if (j < MAX_MED_IN_SUPP)
                        mptr->suppliers[j] = supp;
                    else
                        printf("\nSupplier list of medication %d is full.", mptr->medID);

                    supp->MedPtr[i] = mptr;
                    supp->quantities[i] = quantity;
                    i++;

                    medStart = strchr(medStart, ',');
                    if (!medStart) break;
                    medStart++;
                }
            }

            BT_insert(Tree->SupplierTree, supp);
        }

        else if (strcmp(line, "END_MEDICATION") == 0) {
            med = NULL;
        }
    }

    fclose(fp);
    printf("\nAll data loaded successfully from %s.\n", DATA_FILE);
}


void freeResources(BT_roots* Trees) {
    if (Trees == NULL) return;
    freeMedTree(Trees->medTree->root);
    freeBatchTree(Trees->BatchTree->root);
    freeSuppTree(Trees->SupplierTree->root);
    freeMedHashTable();
    free(Trees->BatchTree);
    free(Trees->medTree);
    free(Trees->SupplierTree);
    free(Trees);
}

void freeMedTree(BTreeNode* node) {
    if (!node) return;
    
    int i;
    for (i = 0; i < node->noOfKeys; i++) {
        if (!node->isLeaf)
            freeMedTree(node->children[i]);
        free(node->keys[i]);
    }
    if (!node->isLeaf)
        freeMedTree(node->children[i]);
    
    free(node);
}

void freeBatchTree(BTreeNode* node) {
    if (!node) return;
    
    int i;
    for (i = 0; i < node->noOfKeys; i++) {
        if (!node->isLeaf)
            freeBatchTree(node->children[i]);
        free(node->keys[i]);
    }
    if (!node->isLeaf)
        freeBatchTree(node->children[i]);
    
    free(node);
}

void freeSuppTree(BTreeNode* node) {
    if (!node) return;
    
    int i;
    for (i = 0; i < node->noOfKeys; i++) {
        if (!node->isLeaf)
            freeSuppTree(node->children[i]);
        free(node->keys[i]);
    }
    if (!node->isLeaf)
        freeSuppTree(node->children[i]);
    
    free(node);
}

void freeMedHashTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        MedHash* current = MedTable[i]; 
        while (current) {
            MedHash* temp = current;
            current = current->next;
            free(temp);
        }
    }
}

void freeBatchList(BatchNode* head) {
    BatchNode* current = head;
    while (current) {
        BatchNode* temp = current;
        current = current->next;
        free(temp);
    }
}

void toLowerStr(char* dest, const char* src) {
    int i = 0;
    while (src[i]) {
        dest[i] = tolower((unsigned char)src[i]);
        i++;
    }
    dest[i] = '\0';
}