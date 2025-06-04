#include "structures.h"

void initialise_DB(BTree* Trees){
    Trees->Med_root = NULL;
    Trees->Batch_root = NULL;
    Trees->Supp_root = NULL;
}
/*--------------------- File Handling ---------------------*/
//Save to File

void saveMedicationRecursive(FILE *fp, BT_MedNode *node) {
    if (node == NULL) return;

    int i;
    for (i = 0; i < node->noOfKeys; i++) {
        if (!node->isLeaf) {
            saveMedicationRecursive(fp, node->children[i]);
        }

        MedNode *m = node->keys[i];
        fprintf(fp, "MEDICATION %d %s %.2f %d %d %d\n",
                m->medID, m->medName, m->pricePerUnit, m->reorderLevel,
                m->stockQuantity, m->totalSales);

        BatchNode *batch = m->batches;
        while (batch != NULL) {
            fprintf(fp, "BATCH %s %d %d %d\n", batch->batchNo, batch->expiryDate,
                    batch->stock, batch->totalSales);
            batch = batch->next;
        }

        SuppNode *supp = m->suppliers;
        while (supp != NULL) {
            fprintf(fp, "SUPPLIER %d %s %s %d %d %.2f\n",
                    supp->supplierID, supp->supplierName, supp->contactInfo,
                    supp->quantitySupplied, supp->uniqueMedication,
                    supp->totalCostSupplied);
            supp = supp->next;
        }

        fprintf(fp, "END_MEDICATION\n");
    }

    if (!node->isLeaf) {
        saveMedicationRecursive(fp, node->children[i]);
    }
}

void saveBatchBTree(FILE* fp, BT_BatchNode* Node){
    if (Node == NULL) return;

    int i;
    for (i = 0; i < Node->noOfKeys; i++){
        if (!Node->isLeaf)
            saveBatchBTree(fp, Node->children[i]);

        fprintf(fp, "Batch_Tree_Node: %s %d %d %d %d\n", Node->keys[i]->batchNo, Node->keys[i]->expiryDate, Node->keys[i]->stock, Node->keys[i]->totalSales, Node->keys[i]->medPtr->medID);
    }
    if (!Node->isLeaf)
        saveBatchBTree(fp, Node->children[i]);
}
void saveSuppBTree(FILE* fp, BT_SuppNode* Node){
    if (Node == NULL) return;

    int i;
    for (i = 0; i < Node->noOfKeys; i++){
        if (!Node->isLeaf)
            saveSuppBTree(fp, Node->children[i]);
        SuppNode* s = Node->keys[i];
        fprintf(fp, "Supplier_Tree_Node: %d %s %s %d %d %.2f", s->supplierID, s->supplierName, s->contactInfo, s->quantitySupplied, s->uniqueMedication, s->totalCostSupplied);
        for (int j = 0; j <s->uniqueMedication; j++){
            fprintf(fp, " %d", s->MedPtr[j]->medID);
        }
        fprintf(fp, "\n");
    }
    if (!Node->isLeaf)
        saveSuppBTree(fp, Node->children[i]);
}

void saveDataToFile(BTree* Tree) {
    FILE *fp = fopen(DATA_FILE, "w");
    if (fp == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }
    fprintf(fp, "#MEDICATION_BTREE\n");
    saveMedicationRecursive(fp, Tree->Med_root);
    fprintf(fp, "#BATCH_BTREE\n");
    saveBatchBTree(fp, Tree->Batch_root);
    fprintf(fp, "#SUPPLIER_BTREE\n");
    saveSuppBTree(fp, Tree->Supp_root);

    fclose(fp);
    printf("All data saved successfully to pharmacy_data.txt\n");
}

//Load from file
void loadDataFromFile(BTree* Tree){
    FILE* fp = fopen(DATA_FILE, "r");
    if (fp == NULL){
        printf("Error opening file for reading.\n");
        return;
    }

    char type[25];
    char line[256];
    MedNode* currentMed = NULL;

    while(fscanf(fp, "%s", type) != EOF){
        if (strcmp(type, "#MEDICATION_BTREE") == 0 || strcmp(type, "#BATCH_BTREE") == 0 || strcmp(type, "#SUPPLIER_BTREE") == 0) {
            fgets(line, sizeof(line), fp);
            continue;
        }
        else if (strcmp(type, "MEDICATION") == 0){
            int medID, reorderLevel, stockQuantity, totalSales;
            float pricePerUnit;
            char medName[NAME_LEN];
            fscanf(fp, "%d %s %f %d %d %d", &medID, medName, &pricePerUnit, &reorderLevel, &stockQuantity, &totalSales);

            currentMed = (MedNode*)malloc(sizeof(MedNode));
            currentMed->medID = medID;
            strcpy(currentMed->medName, medName);
            currentMed->pricePerUnit = pricePerUnit;
            currentMed->reorderLevel = reorderLevel;
            currentMed->stockQuantity = stockQuantity;
            currentMed->totalSales = totalSales;
            currentMed->batches = NULL;
            currentMed->suppliers = NULL;

            insert_medNode((&Tree->Med_root), currentMed);
            insertIn_Med_Hash(currentMed->medName, currentMed->medID);
        }
        else if (strcmp(type, "BATCH") == 0){
            if (currentMed == NULL) continue;

            BatchNode* newBatch = (BatchNode*)malloc(sizeof(BatchNode));
            fscanf(fp, "%s %d %d %d", newBatch->batchNo, &newBatch->expiryDate, &newBatch->stock, &newBatch->totalSales);
            newBatch->medPtr = currentMed;
            newBatch->next = currentMed->batches;
            currentMed->batches = newBatch;

            insert_BatchNode((&Tree->Batch_root), newBatch);
        }
        else if (strcmp(type, "SUPPLIER") == 0){
            if (currentMed == NULL) continue;
        
            SuppNode* newSupp = (SuppNode*)malloc(sizeof(SuppNode));
            int numMed;
            fscanf(fp, "%d %s %s %d %d %f%n", 
                   &newSupp->supplierID, newSupp->supplierName, newSupp->contactInfo,
                   &newSupp->quantitySupplied, &newSupp->uniqueMedication,
                   &newSupp->totalCostSupplied, &numMed);
        
            // Read associated medications
            memset(newSupp->MedPtr, 0, sizeof(newSupp->MedPtr));
            int max_med = (newSupp->uniqueMedication < MAX_MED_IN_SUPP) ? 
                          newSupp->uniqueMedication : MAX_MED_IN_SUPP;
            
            for (int j = 0; j < max_med; j++) {
                int medID;
                if (fscanf(fp, "%d", &medID) != 1) break;
                newSupp->MedPtr[j] = search_MedNode_ByID(Tree->Med_root, medID);
            }
            
            // Link to current medication
            newSupp->next = currentMed->suppliers;
            currentMed->suppliers = newSupp;
            printf("insert Supplierd %d\n", newSupp->supplierID);
            insert_SuppNode((&Tree->Supp_root), newSupp);
        }
        else if (strcmp(type, "END_MEDICATION") == 0){
            currentMed = NULL; 
        }
        else {
            fgets(line, sizeof(line), fp);
        }
    }

    fclose(fp);
    printf("All data loaded successfully from pharmacy_data.txt\n");
}


void freeBatchList(BatchNode* head) {
    BatchNode* current = head;
    while (current) {
        BatchNode* temp = current;
        current = current->next;
        free(temp);
    }
}

void freeSuppList(SuppNode* head) {
    SuppNode* current = head;
    while (current) {
        SuppNode* temp = current;
        current = current->next;
        free(temp);
    }
}

void freeMedNodes(BT_MedNode* node) {
    if (node == NULL) return;
    
    int i;
    for (i = 0; i <= node->noOfKeys; i++) {
        freeMedNodes(node->children[i]);
    }
    
    for (i = 0; i < node->noOfKeys; i++) {
        MedNode* med = node->keys[i];
        if (med) {
            freeBatchList(med->batches);
            freeSuppList(med->suppliers);
            free(med);
        }
    }
    
    free(node);
}

void freeBatchBTree(BT_BatchNode* node) {
    if (node == NULL) return;
    
    int i;
    for (i = 0; i <= node->noOfKeys; i++) {
        freeBatchBTree(node->children[i]);
    }
    free(node);
}

void freeSuppBTree(BT_SuppNode* node) {
    if (node == NULL) return;
    
    int i;
    for (i = 0; i <= node->noOfKeys; i++) {
        freeSuppBTree(node->children[i]);
    }
    free(node);
}

void freeMedHashTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        MedName* current = MedTable[i]; 
        while (current) {
            MedName* temp = current;
            current = current->next;
            free(temp);
        }
    }
}

void freeResources(BTree* Tree) {
    if (Tree == NULL) return;
    freeMedNodes(Tree->Med_root);
    freeBatchBTree(Tree->Batch_root);
    freeSuppBTree(Tree->Supp_root);
    freeMedHashTable();
    free(Tree);
}

/*--------------------- Other Helping Functions ---------------------*/
int check_and_correct_DATE(char date[]){
    if (strlen(date) != 10 || date[2] != '-' || date[5] != '-') {
        printf("Invalid date format! Use DD-MM-YYYY\n");
        return -1;
    }
    
    int day, month, year;
    if (sscanf(date, "%d-%d-%d", &day, &month, &year) != 3) {
        printf("Invalid date components!\n");
        return -1;
    }

    if (month < 1 || month > 12 || day < 1 || day > daysInMonth(month, year)) {
        printf("Invalid date values!\n");
        return -1;
    }

    return year * 10000 + month * 100 + day;
}

void TraverseFor_StockAlerts (BT_MedNode* Node, int *found){
    if (Node != NULL){
        int i;
        for (i = 0;  i < Node->noOfKeys; i++){
            if(!Node->isLeaf) TraverseFor_StockAlerts(Node->children[i], found);
            if (Node->keys[i]->stockQuantity <= Node->keys[i]->reorderLevel){
                printf("%d, %s, %d\n", Node->keys[i]->medID, Node->keys[i]->medName, Node->keys[i]->stockQuantity);
                *found = 1;
            }
        }
        if(!Node->isLeaf) TraverseFor_StockAlerts(Node->children[i], found);
    }
    return;
}


bool isLeapYear(int year) {
    bool isLeap = false;
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        isLeap = true;  
    }
    return isLeap;
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
        if (isLeapYear(yr))
            noOfDays += 366;
        else
            noOfDays += 365;

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
    if (date == NULL) {
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
    date[DATE_LEN - 1] = '\0';
    return date;
}

void TraverseFor_checkExpiration (BT_BatchNode* Node, int todayDate, int *found){
    if (Node != NULL){
        int i = 0;
        while (i < Node->noOfKeys && (convertToDays(Node->keys[i]->expiryDate) - convertToDays(todayDate)) <= 30) i++;
        int j;
        for (j = 0; j < i; j++){
            if (!Node->isLeaf) TraverseFor_checkExpiration(Node->children[j], todayDate, found);
            if ((convertToDays(Node->keys[j]->expiryDate) - convertToDays(todayDate)) <= 30){
                *found = 1;
                printf("\n%d, %s, %d, %s ", Node->keys[j]->medPtr->medID,Node->keys[j]->batchNo, Node->keys[j]->stock, format_DATE (Node->keys[j]->expiryDate));
            }
        }
        if (!Node->isLeaf) TraverseFor_checkExpiration(Node->children[j], todayDate, found);
    }   
}
/*
void TraverseFor_check_MedicationBet_DATES(BT_BatchNode* Node, int dt1, int dt2, int *found){
    if (Node != NULL){
        int i = 0, start, end, flag1 = 0;
        while (i < Node->noOfKeys && !flag1) {
            if (Node->keys[i]->expiryDate > dt1 && Node->keys[i]->expiryDate < dt2){
                start = i;
                flag1 = 1;
            }
            else i++;
        }
        int flag2 = 0;
        while (i < Node->noOfKeys && !flag2) {
            if (Node->keys[i]->expiryDate > dt1 && Node->keys[i]->expiryDate < dt2) i++;
            else {
                end = i;
                flag2 = 1;
            }
        }
        if (!flag1){
            if (Node->keys[0]->expiryDate > dt2) start = 0;
            else if (Node->keys[Node->noOfKeys - 1]->expiryDate < dt1) start = Node->noOfKeys;
            else start = 0;
        }
        if (!flag2){
            if (Node->keys[0]->expiryDate > dt2) end = 0;
            else if (Node->keys[Node->noOfKeys - 1]->expiryDate < dt1) end = Node->noOfKeys;
            else end = Node->noOfKeys;
        }

        // req keys: start to end - 1
        // req child: start to end

        int j;
        for (j = start; j <= end; j++){
            if (!Node->isLeaf) TraverseFor_check_MedicationBet_DATES(Node->children[j], dt1, dt2, found);
            if (Node->keys[i]->expiryDate > dt1 && Node->keys[i]->expiryDate < dt2 && j <= end - 1){
                *found = 1;
                printf("\n%d, %s, %d, %s ", Node->keys[j]->medPtr->medID,Node->keys[j]->batchNo, Node->keys[j]->stock, format_DATE (->keys[j]->expiryDate));
            }
        }
        if (!Node->isLeaf && end == Node->noOfKeys) TraverseFor_check_MedicationBet_DATES(Node->children[j], dt1, dt2, found);
    } 
}*/

void TraverseFor_check_MedicationBet_DATES(BT_BatchNode* Node, int dt1, int dt2, int *found) {
    if (Node != NULL){
        int i = 0;
        while (i < Node->noOfKeys && Node->keys[i]->expiryDate < dt1) i++;
        for (; i < Node->noOfKeys && Node->keys[i]->expiryDate <= dt2; i++) {
            if (!Node->isLeaf)
                TraverseFor_check_MedicationBet_DATES(Node->children[i], dt1, dt2, found);
            if (Node->keys[i]->expiryDate >= dt1 && Node->keys[i]->expiryDate <= dt2) {
                *found = 1;
                printf("\n%d, %s, %d, %s ", Node->keys[i]->medPtr->medID, 
                       Node->keys[i]->batchNo, Node->keys[i]->stock, 
                       format_DATE(Node->keys[i]->expiryDate));
            }
        }
        if (!Node->isLeaf)
            TraverseFor_check_MedicationBet_DATES(Node->children[i], dt1, dt2, found);
    }
}

